/*
 *  mlMeVisLabLoadDICOM.m
 *  MLOsiriXBridge
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import "mlMeVisLabLoadDICOM.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

static MeVisLabLoadDICOM *__mlabLoadDicom = nil;

@implementation MeVisLabLoadDICOM : NSObject

@synthesize slicesFileList;

- (id) init
{
  NSAssert (__mlabLoadDicom == nil, @"[MeVisLabLoadDICOM -init] called more than once");
  
  if ((self = [super init])) {
    // Vending services
    if ((incomingConnection = [[NSConnection alloc] init])) {
      [incomingConnection setRootObject:self];
      if ([incomingConnection registerName:@"MeVisLabLoadDICOM"] == YES) {
        NSLog(@"Successfully registered MeVisLabLoadDICOM service");
      }
      else {
        NSLog(@"Cound not register MeVisLabLoadDICOM service");
      }
    }
  }
  return self;
}

+ (MeVisLabLoadDICOM *) sharedManager
{
  if (__mlabLoadDicom == nil) {
    __mlabLoadDicom = [[super allocWithZone:NULL] init];
  }
  return __mlabLoadDicom;
}

- (id) retainManager
{
  ++managerRetainCount;
  return self;
}

- (void) releaseManager
{
  if (--managerRetainCount == 0) {
    [super release];
  }
}

+ (id) allocWithZone:(NSZone *)zone
{
#pragma unused ( zone )
  return [[self sharedManager] retain];
}

- (id) copyWithZone:(NSZone *)zone
{
#pragma unused ( zone )
  return self;
}

- (id) retain
{
  return self;
}

- (NSUInteger) retainCount
{
  // Denotes an object that cannot be released
  return NSUIntegerMax;
}

- (oneway void) release
{
  // Do nothing
}

- (id) autorelease
{
  return self;
}

- (void) dealloc
{
  [slicesFileList release];
  
  // Don't know why invalidation of the receiving port is necessary, the documentation is a bit vague here
  // and implies invalidation of the connection is all that is required. However, the service name is not
  // unregistered unless the receiving port iteself gets invalidated...
  [[incomingConnection receivePort] invalidate];
  // Send NSConnectionDidDieNotification to connected observers
  [incomingConnection invalidate];
  if ([incomingConnection isValid]) {
    NSLog(@"Couldn't unregister MeVisLabLoadDICOM service");
  } else {
    NSLog(@"Unregistered MeVisLabLoadDICOM service");
  }
  [incomingConnection release];
  incomingConnection = nil;
  
  __mlabLoadDicom = nil;
  
  [super dealloc];
}

- (int) protocolVersion
{
  return MeVisLabLoadDICOM_ProtocolVersion;
}

- (NSString *) servicesProviderName
{
  return NSStringFromClass([self class]);
}

- (NSUInteger) servicesProviderVersion
{
  return 1;
}

- (void) clientIsInitialized
{
  [[NSNotificationCenter defaultCenter] postNotificationName:@"ClientInitialized"
                                                      object:self];
}

- (void) loadImagesWithFilenames:(in bycopy NSArray *)images forTargetWithIdentifier:(in OBTargetIdentifier)identifier
{
  [slicesFileList release];
  slicesFileList = [images retain];
  
  [[NSNotificationCenter defaultCenter] postNotificationName:@"DICOMImagesReceived"
                                                      object:self
                                                    userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedLongLong:identifier] forKey:@"TargetIdentifier"]];
}

@end
