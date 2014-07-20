//
//  MeVisLab2OsiriXTBridge.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MeVisLab2OsiriXTBridge.h"
#import <arpa/inet.h>

@implementation MeVisLab2OsiriXTBridge

#pragma mark-
#pragma mark functions to start build the message bridge
- (id) initWithIncommingConnectionName:(NSString*)aname 
{
  self = [super init];
  if (self)
  {
    imagesManager=[[SharedImagesManager alloc] initWithIDStringPrefix:@"MeVis"];
    //outgoingConnectionRegisteredName=[bname copy];//outgoing name is not initialize any more, it will wait OsiriX call first.
    outgoingConnectionRegisteredName=nil;
    ifSupportMemorySharing=NO;
    // Vending services
    NSSocketPort* port1=[[NSSocketPort alloc] init];

    if ((incomingConnection = [[NSConnection alloc] initWithReceivePort:port1 sendPort:nil])){
      [incomingConnection setDelegate:self];
      [incomingConnection setRootObject:self];
      [incomingConnection setRequestTimeout:5];
      [incomingConnection setReplyTimeout:30];
      if ([[NSSocketPortNameServer sharedInstance] registerPort:port1 name:aname] == YES) {
        std::cout << "Successfully registere service as " << [aname  UTF8String] << std::endl;
        incomingConnectionRegisteredName=[aname copy];
      }
      else {
        std::cout << "Cound not register service as "  << [aname  UTF8String] << std::endl;
      }
    }
    [port1 release];
  }
  return self;
}
-(void)prepareToDealloc
{
  if(remoteObjectProxy)
  {
    NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
    {
      NSString* operation=[NSString stringWithString:@"Breakup"];
      NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
      NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
      
      [anoperation setObject:operation forKey:@"Operation"];
      [anoperation setObject:parameters forKey:@"Parameters"];
      [anoperation setObject:relatedImages forKey:@"RelatedImages"];
    }
    
    [remoteObjectProxy setOperation:anoperation];
    
  }
  [remoteConnection invalidate];
  [[NSNotificationCenter defaultCenter] removeObserver: self];
  [[incomingConnection receivePort] invalidate];//this will work to unregister the NSConnection
  [[incomingConnection sendPort] invalidate];
  [incomingConnection setRootObject:nil];
  [incomingConnection registerName:nil];//this will not work as suggestion it alway return NO
  [incomingConnection invalidate];
  [incomingConnection release];

  if(incomingConnectionRegisteredName)
  {
    [[NSSocketPortNameServer sharedInstance] removePortForName:incomingConnectionRegisteredName];
  }
  incomingConnection = nil;
}
-(void)dealloc
{
  [imagesManager release];
    
  [super dealloc];
}
- (void) setImporterML:(ml::OsiriXImporter*) aML
{
  importerML=aML;
}

- (void) setExporterML:(ml::OsiriXExporter*) aML
{
  exporterML=aML;
}
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname
{
  if ([[NSSocketPortNameServer sharedInstance] registerPort:[incomingConnection receivePort] name:aname]  == YES) {
    std::cout << "Successfully registere service as " << [aname  UTF8String] << std::endl;
    if(incomingConnectionRegisteredName)
    {
      [[NSSocketPortNameServer sharedInstance] removePortForName:incomingConnectionRegisteredName];
    }
    incomingConnectionRegisteredName=[aname copy];
    return YES;
  }
  else {
    std::cout << "Cound not register service as "  << [aname  UTF8String] << std::endl;
    return NO;
  }
}
- (BOOL)connectToRemoteObjectRegisteredAs:(NSString*)registeredName 
{
  if(!registeredName)
    return NO;
  NSSocketPort  *port = (NSSocketPort *)[[NSSocketPortNameServer sharedInstance] 
                       portForName:registeredName
                       host:@"*"];  
  remoteConnection=[NSConnection connectionWithReceivePort:nil sendPort:port];
  if(remoteObjectProxy)
    [remoteObjectProxy release];
  remoteObjectProxy = [[remoteConnection rootProxy] retain];
  
  
  if(remoteObjectProxy)
  {
  

    //autoReleasePool = [[NSAutoreleasePool alloc] init];
    outgoingConnectionRegisteredName=[registeredName copy];
    [remoteObjectProxy setProtocolForProxy:@protocol(MeVisOsiriXProxyProtocol)];
    
    NSConnection *outgoingConnection = [remoteObjectProxy connectionForProxy];

    // Notification observer for a lost connection
    [[NSNotificationCenter defaultCenter] addObserver:self
                         selector:@selector(connectionDidDie:)
                           name:NSConnectionDidDieNotification
                           object:outgoingConnection];
    [[NSNotificationCenter defaultCenter] addObserver:self
                         selector:@selector(connectionDidDie:)
                           name:NSPortTimeoutException
                           object:outgoingConnection];
    NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
    {
      NSString* operation=[NSString stringWithString:@"CallBack"];
      NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
      {
        [parameters setObject:incomingConnectionRegisteredName forKey:@"RegisteredName"];
      }
      NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
    
      [anoperation setObject:operation forKey:@"Operation"];
      [anoperation setObject:parameters forKey:@"Parameters"];
      [anoperation setObject:relatedImages forKey:@"RelatedImages"];
    }
    
    [remoteObjectProxy setOperation:anoperation];
    
  }
  else
  {
    std::cout << "Cound not connect to OsiriX!"  << std::endl;
    return NO;
  }
  
  return YES;
  

}

- (void) connectionDidDie: (NSNotification *)notification
{
  //This will never be called if NSSocketPort is used to connect to server.
  if(remoteObjectProxy)
  {
    [remoteObjectProxy release];
    remoteObjectProxy=nil;
//    [autoReleasePool release];
  }
  remoteConnection=nil;
  
}
#pragma mark-
#pragma mark functions to interprete the operation request from OsiriX.
- (void)setOperation:(NSDictionary *)request
{
  NSString* operation=[request objectForKey:@"Operation"];
  if(operation)
  {
    if([operation isEqualToString:@"SendMeImage"])
    {
      NSMutableDictionary* parameters=[request objectForKey:@"Parameters"];
      NSString* description=[parameters objectForKey:@"ImageDescription"];
      [self prepareImageForUpperBridgeFromExporter:description];
    }
    else  if([operation isEqualToString:@"CallBack"])
    {
      NSMutableDictionary* parameters=[request objectForKey:@"Parameters"];
      NSString* registeredName=[parameters objectForKey:@"RegisteredName"];
      NSNumber* supportSharedMem=[parameters objectForKey:@"SupportSharedMem"];
      ifSupportMemorySharing=[supportSharedMem boolValue];
      if(remoteObjectProxy)
      {
        [remoteObjectProxy release];
        remoteObjectProxy=nil;
      }
      outgoingConnectionRegisteredName=[registeredName copy];
//      if(!remoteObjectProxy)
//        [self connectToRemoteObjectRegisteredAs:registeredName];
    }
    else  if([operation isEqualToString:@"Breakup"])
    {

      [remoteConnection invalidate];
      outgoingConnectionRegisteredName=nil;
      
    }
    else  if([operation isEqualToString:@"ParameterUpdate"])
    {
      NSDictionary* parameters=[request objectForKey:@"Parameters"];
      [self passingOnNotificationsToImporter:parameters];
    }
  }
  return;
}
//- (NSDictionary*)getImage:(NSString*)description
//{
//  NSLog([NSString stringWithFormat:@"MeVisLab: OsiriX is asking for:%@",  description] );
//  return [self prepareImageForUpperBridgeFromExporter:description];
//}
- (void)setImage:(NSDictionary*)anImage ForDescription:(NSString*) description
{
  if(anImage)
  {
    NSMutableDictionary*anNewImage=[anImage mutableCopy];
    [imagesManager creatASharedImage:anNewImage ForDescription:description SupportSharedMem:ifSupportMemorySharing];
    NSLog([NSString stringWithFormat:@"MeVisLab: %@ received",  description] );
    [anNewImage release];
    
  }
  
}
#pragma mark-
#pragma mark functions when works as a bridge between Importer and OsiriX.
-(NSDictionary*)getImageFromLowerBridge:(NSString*)description
{
  if(remoteObjectProxy==nil)
  {
    if(![self connectToRemoteObjectRegisteredAs:outgoingConnectionRegisteredName])
    {
      //std::cout << "Cound not connect to OsiriX!"  << std::endl;
      return nil;
    }
  }
  NSLog([NSString stringWithFormat:@"MeVisLab: MeVisLab is asking for:%@",  description] );
  //NSDictionary* anImage=[remoteObjectProxy getImage:description];
  NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
  {
    NSString* operation=[NSString stringWithString:@"SendMeImage"];
    NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
    {
      [parameters setObject:description forKey:@"ImageDescription"];
    }
    NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
    
    [anoperation setObject:operation forKey:@"Operation"];
    [anoperation setObject:parameters forKey:@"Parameters"];
    [anoperation setObject:relatedImages forKey:@"RelatedImages"];
  }
  
  [remoteObjectProxy setOperation:anoperation];
  
  return [imagesManager getImageForDescription: description];
}
-(void)passingOnNotificationsToImporter:(NSDictionary*)parameters
{
  NSLog([NSString stringWithFormat:@"MeVisLab: OsiriX is notifying :%@",  @"parameter changed"] );
  importerML->updateParameters(parameters);
}
#pragma mark-
#pragma mark functions when works as a bridge between Exporter and OsiriX
-(NSDictionary*)prepareImageForUpperBridgeFromExporter:(NSString*)description
{
  NSMutableDictionary* tempImage=[[NSMutableDictionary alloc] initWithCapacity:0];
  [tempImage setObject:description forKey:@"Description"];
  if([description isEqualToString:@"OutputImage0"])
  {
    if(exporterML->calcInputImageProps(tempImage, 0))
    {
      [tempImage setObject:[NSString stringWithString:@"float"] forKey:@"ImageType"];
      int dimension[4]={0,0,0,0};
      int i;
      NSArray* dimensionarray=[tempImage objectForKey:@"Dimension"];
      for(i=0;i<4;i++)
        dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
      long size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(float);
      [tempImage setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
      if(size<=0)
        return nil;
    }
    else
      return nil;
  }
  else if([description isEqualToString:@"OutputImage1"])
  {
    if(exporterML->calcInputImageProps(tempImage, 1))
    {
      [tempImage setObject:[NSString stringWithString:@"char"] forKey:@"ImageType"];
      int dimension[4]={0,0,0,0};
      int i;
      NSArray* dimensionarray=[tempImage objectForKey:@"Dimension"];
      for(i=0;i<4;i++)
        dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
      long size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(char);
      [tempImage setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
      if(size<=0)
        return nil;
    }
    else
      return nil;

  }
  else if([description isEqualToString:@"OutputImage2"])
  {
//    if (exporterML->prepareCSOForOsiriX(tempImage))
//    {
//    //savedImage ???????
//    }
//    else
      return nil;
  }
  NSMutableDictionary* savedImage=[imagesManager getImageForDescription: description];
  if(![description isEqualToString:@"OutputImage2"])
  {
    if(savedImage&&[[savedImage objectForKey:@"MemSize"] longValue]==[[tempImage objectForKey:@"MemSize"] longValue])
    {
        [savedImage setObject:[tempImage objectForKey:@"Dimension"] forKey:@"Dimension"];
        [savedImage setObject:[tempImage objectForKey:@"Spacing"] forKey:@"Spacing"];
        [savedImage setObject:[tempImage objectForKey:@"MatrixToPatientCo"] forKey:@"MatrixToPatientCo"];
        [savedImage setObject:[tempImage objectForKey:@"Maximum"] forKey:@"Maximum"];
        [savedImage setObject:[tempImage objectForKey:@"Minimum"] forKey:@"Minimum"];

    }
    else
    {
      [savedImage removeObjectForKey:@"Data"];
      if(![imagesManager creatASharedImage:tempImage ForDescription:description SupportSharedMem:ifSupportMemorySharing])
      {
        //here the error message should be sent back to osirix
        [imagesManager removeImage:savedImage];
      }
                     
    }
  }
  [tempImage release];
  NSMutableDictionary* resultImage=[imagesManager getImageForDescription: description];
  NSMutableDictionary* returnImage=nil;
  if(resultImage)
  {
    if([description isEqualToString:@"OutputImage0"])
    {
      exporterML->calcInSubImage(resultImage, 0);
    }
    else if([description isEqualToString:@"OutputImage1"])
    {
      exporterML->calcInSubImage(resultImage, 1);
    }

    
    
    returnImage=[resultImage mutableCopy];
    if(ifSupportMemorySharing)
      [returnImage removeObjectForKey:@"Data"];

    [remoteObjectProxy setImage: returnImage ForDescription: description];
    [returnImage release];

  }
  
  NSLog([NSString stringWithFormat:@"MeVisLab: MeVisLab is sending:%@",  description] );
  return [imagesManager getImageForDescription: description];
}
-(void)passingOnNotificationsToUpperBridge:(NSDictionary*)parameters
{
  if(remoteObjectProxy==nil)
  {
    if(![self connectToRemoteObjectRegisteredAs:outgoingConnectionRegisteredName])
    {
      //NSLog(@"Cound not connect to MeVisLab Exporter!");
      return;
    }
  }
  NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
  {
    NSString* operation=[NSString stringWithString:@"ParameterUpdate"];
    
    NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
    
    [anoperation setObject:operation forKey:@"Operation"];
    [anoperation setObject:parameters forKey:@"Parameters"];
    [anoperation setObject:relatedImages forKey:@"RelatedImages"];
  }
  
  [remoteObjectProxy setOperation:anoperation];
}

- (BOOL)initializeAnImageForSharing:(NSMutableDictionary*)anImage
{
  return [imagesManager creatASharedImage:anImage ForDescription:[anImage objectForKey:@"Description"] SupportSharedMem:ifSupportMemorySharing];
}

@end
