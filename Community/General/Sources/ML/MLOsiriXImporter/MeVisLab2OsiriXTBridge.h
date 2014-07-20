//
//  MeVisLab2OsiriXTBridge.h
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "mlOsiriXImporter.h"
#import "mlOsiriXExporter.h"
#import "SharedImagesManager.h"
#import "MeVisOsiriXProxyProtocol.h"

@interface MeVisLab2OsiriXTBridge : NSObject <MeVisOsiriXProxyProtocol>
{
  //common parts for OsiriX2MeVisLab and MeVisLab2OsiriX
  NSConnection *incomingConnection; 
  NSString* incomingConnectionRegisteredName;

  NSString* outgoingConnectionRegisteredName;

  SharedImagesManager* imagesManager;
  id remoteObjectProxy;
  NSConnection* remoteConnection;
  BOOL ifSupportMemorySharing;
  NSAutoreleasePool *autoReleasePool;
  //different parts
  ml::OsiriXImporter* importerML;
  ml::OsiriXExporter* exporterML;
}
//common parts for OsiriX2MeVisLab and MeVisLab2OsiriX
- (id) initWithIncommingConnectionName:(NSString*)aname;
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname;
- (BOOL)connectToRemoteObjectRegisteredAs:(NSString*)registeredName;
- (void) connectionDidDie: (NSNotification *)notification;
- (BOOL)initializeAnImageForSharing:(NSMutableDictionary*)anImage;
- (NSDictionary*)getImageFromLowerBridge:(NSString*)description;
- (void) passingOnNotificationsToUpperBridge:(NSDictionary*)parameters;
- (void)prepareToDealloc;
  //different parts
- (void) setImporterML:(ml::OsiriXImporter*) aML;
- (void) setExporterML:(ml::OsiriXExporter*) aML;
- (void) passingOnNotificationsToImporter:(NSDictionary*)parameters;
- (NSDictionary*)prepareImageForUpperBridgeFromExporter:(NSString*)description;

@end
