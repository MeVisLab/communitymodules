//
//  OsiriX2MeVisLabTBridge.h
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "SharedImagesManager.h"
#import "MeVisOsiriXProxyProtocol.h"
@class MeVisHubWindowController;

@interface OsiriX2MeVisLabTBridge : NSObject <MeVisOsiriXProxyProtocol>
{
	//common parts for OsiriX2MeVisLab and MeVisLab2OsiriX
	NSConnection *incomingConnection; 
	NSString* incomingConnectionRegisteredName;
	NSString* outgoingConnectionRegisteredName;
	SharedImagesManager* imagesManager;
	id remoteObjectProxy;
	
	
	//different parts
	MeVisHubWindowController* windowController;
	
	
}
//common parts for OsiriX2MeVisLab and MeVisLab2OsiriX
- (id) initWithIncommingConnectionName:(NSString*)aname OutgoingConnection:(NSString*)bname;
- (void)prepareToDealloc;
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname;
- (BOOL)connectToRemoteObjectRegisteredAs:(NSString*)registeredName;
- (void) connectionDidDie: (NSNotification *)notification;
- (BOOL)initializeAnImageForSharing:(NSMutableDictionary*)anImage;
- (NSDictionary*)getImageFromLowerBridge:(NSString*)description;
- (void)passingOnNotificationsToUpperBridge:(NSDictionary*)parameters;


//different parts
- (void) setWindowController:(MeVisHubWindowController*)window;
- (NSDictionary*)prepareImageForUpperBridgeFromOsiriX:(NSString*)description;
- (void) passingOnNotificationsToWindowController:(NSDictionary*)parameters;
@end
