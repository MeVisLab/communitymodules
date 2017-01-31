//
//  mlOsiriXBridge_p.mm
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

#import "mlOsiriXBridge_p.h"
#import "mlMeVisLabLoadDICOM.h"
#import "mlEatDicom.h"
#import <mlOsiriXServices.h>
#import <macObjectiveAutoreleasePool.h>
#import <mlStringLineMultiField.h>

// MARK: BridgeNotificationHandler

@interface BridgeNotificationHandler : NSObject
{
  ml::OsiriXBridgePrivate *obj;
}
- (id) initWithObject: (ml::OsiriXBridgePrivate *)obj;
@end

@implementation BridgeNotificationHandler : NSObject
- (id) initWithObject: (ml::OsiriXBridgePrivate *)o
{
  if ((self = [super init])) {
    obj = o;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(clientIsInitialized)
                                                 name:@"ClientInitialized"
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(handleDICOMDataset:)
                                                 name:@"DICOMImagesReceived"
                                               object:nil];
  }
  return self;
}
- (void) dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];
  [super dealloc];
}
- (void) clientIsInitialized
{
  obj->registerWithClient();
}
- (void) handleDICOMDataset: (NSNotification *)notification
{
  OBTargetIdentifier targetIdentifier = [[[notification userInfo] objectForKey:@"TargetIdentifier"] unsignedLongLongValue];
  
  if (obj->targetIdentifier() == targetIdentifier) {
    obj->handleDICOMDataset();
  }
}
@end


// MARK: OsiriXBridgePrivate

ML_START_NAMESPACE

OsiriXBridgePrivate::OsiriXBridgePrivate(Module *o) : obj(o), notifyHandler(nil), eatDicom(nil), _targetIdentifier(0)
{
  macos::ObjectiveAutoreleasePool pool;
  
  notifyHandler = [[BridgeNotificationHandler alloc] initWithObject:this];
  
  loadDICOM = [[MeVisLabLoadDICOM sharedManager] retainManager];

  osirixServices = [[MLOsiriXServices alloc] init];
  registerWithClient();
}

OsiriXBridgePrivate::~OsiriXBridgePrivate()
{
  macos::ObjectiveAutoreleasePool pool;
  
  unRegisterFromClient();
  [osirixServices release];
  osirixServices = nil;
  
  [eatDicom release];
  eatDicom = nil;
  
  [loadDICOM releaseManager];
  loadDICOM = nil;
  
  [notifyHandler release];
  notifyHandler = nil;
}

void
OsiriXBridgePrivate::registerWithClient(const std::string &name)
{
  unRegisterFromClient();
  
  @try {
    MLOSStatus status = MLOSSuccess;
    
    _targetIdentifier = [osirixServices registerTargetWithName:[NSString stringWithUTF8String:name.c_str()] status:&status];
    
    if (MLOSTargetExistsError == status) {
      std::cerr << "A target with name: '" << name << "' already exists. Registration failed." << std::endl;
    }
  }
  @catch (NSException *e) {
    NSLog (@"Exception in OsiriXBridgePrivate::registerWithClient():\nname: %@\nreason: %@", [e name], [e reason]);
  }
  @catch (id e) {
    NSLog (@"Unknown exception in OsiriXBridgePrivate::registerWithClient()");
  }
}

void
OsiriXBridgePrivate::unRegisterFromClient()
{
  @try {
    if (_targetIdentifier != 0) {
      [osirixServices unregisterTargetWithIdentifier:_targetIdentifier];
      
      _targetIdentifier = 0;
    }
  }
  @catch (NSException *e) {
    NSLog (@"Exception in OsiriXBridgePrivate::unRegisterFromClient():\nname: %@\nreason: %@", [e name], [e reason]);
  }
  @catch (id e) {
    NSLog (@"Unknown exception in OsiriXBridgePrivate::unRegisterFromClient()");
  }
}

void
OsiriXBridgePrivate::registerWithClient()
{
  _targetName = obj->getField("alternativeTargetName")->getStringValue();
  if (_targetName.empty()) {
    _targetName = obj->getField("instanceName")->getStringValue();
  }
  registerWithClient(_targetName);
}

void
OsiriXBridgePrivate::handleDICOMDataset()
{
  macos::ObjectiveAutoreleasePool pool;
  
  Field *field = obj->getField("osirixIncomingDir");
  if (field) {
    field->setStringValue(osirixIncomingFolder());
  }
  field = obj->getField("clientAppBundleId");
  if (field) {
    field->setStringValue(clientAppBundleId());
  }
  
  field = obj->getField("useEatDicom");
  if (field && static_cast<BoolField *>(field)->getBoolValue()) {
    if (eatDicom == nil) {
      eatDicom = [EatDicom new];
    }
    field = obj->getField("dataStoreDir");
    if (field) {
      [eatDicom setDICOMStoreFolder:[NSString stringWithCString:field->getStringValue().c_str()
                                                       encoding:NSUTF8StringEncoding]];
    }
    field = obj->getField("eatDicomOptions");
    if (field) {
      [eatDicom setEatDicomOptions:[NSString stringWithCString:field->getStringValue().c_str()
                                                      encoding:NSUTF8StringEncoding]];
    }
    
    // Fire up eatDicom
    NSArray *slicesList = [[loadDICOM slicesFileList] retain];
    if ([eatDicom importImages:slicesList]) {
      NSString *filename = [eatDicom DICOMFilename];
      if (filename) {
        std::string dicomFilename = [filename UTF8String];
        
        std::cout << _targetName << ": handleDICOMDataset( " << dicomFilename.c_str() << " )" << std::endl;
        
        field = obj->getField("filename");
        if (field) {
          field->setStringValue(dicomFilename);
        }
      }
    }
    [slicesList release];
    
    field = obj->getField("eatDicomOutput");
    if (field && [eatDicom eatDicomOutput]) {
      field->setStringValue([[eatDicom eatDicomOutput] UTF8String]);
    }
  }
    
  NSArray *slicesList = [[loadDICOM slicesFileList] retain];
  if (slicesList) {
    std::vector<std::string> slicesStdVec;
    slicesStdVec.reserve([slicesList count]);
    for (NSString *slice in slicesList) {
      slicesStdVec.push_back([slice UTF8String]);
    }
    [slicesList release];
    
    std::cout << _targetName << ": handleDICOMDataset( " << slicesStdVec.size() << ((slicesStdVec.size()>1) ? " slices )" : " slice )") << std::endl;
    
    StringLineMultiField *mField = static_cast<StringLineMultiField *> (obj->getField("slicesFileList"));
    if (mField) {
      mField->setMultiField(slicesStdVec);
    }
  }
}

std::string
OsiriXBridgePrivate::osirixIncomingFolder()
{
  macos::ObjectiveAutoreleasePool pool;
  
  NSString *path = nil;
  @try {
    path = [osirixServices osirixIncomingFolderPath];
  }
  @catch (NSException *e) {
    NSLog (@"Exception in OsiriXBridgePrivate::osirixIncomingFolder():\nname: %@\nreason: %@", [e name], [e reason]);
    
    path = nil;
  }
  @catch (id e) {
    NSLog (@"Unknown exception in OsiriXBridgePrivate::osirixIncomingFolder()");
    
    path = nil;
  }
  
  return ((path) ? std::string([path UTF8String]) : std::string());
}

std::string
OsiriXBridgePrivate::clientAppBundleId()
{
  macos::ObjectiveAutoreleasePool pool;

  NSString *appId = nil;
  @try {
    appId = [osirixServices servicesProviderAppId];
  }
  @catch (NSException *e) {
    NSLog (@"Exception in OsiriXBridgePrivate::clientAppBundleId():\nname: %@\nreason: %@", [e name], [e reason]);

    appId = nil;
  }
  @catch (id e) {
    NSLog (@"Unknown exception in OsiriXBridgePrivate::clientAppBundleId()");

    appId = nil;
  }

  return ((appId) ? std::string([appId UTF8String]) : std::string());
}

ML_END_NAMESPACE
