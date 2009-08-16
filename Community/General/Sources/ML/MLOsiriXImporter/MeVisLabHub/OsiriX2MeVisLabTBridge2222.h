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
	MeVisHubWindowController* windowController;
	NSConnection *incomingConnection; 
	NSString* incomingConnectionRegisteredName;
	SharedImagesManager* imagesManager;
	id meVisLabImporterProxy;
	id meVisLabExporterProxy;
	BOOL ifNeedUpdateInputImageFromOsiriX;
}
- (id) initWithWindow:(MeVisHubWindowController*)window;;
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname;
- (BOOL)connectToExporterMLNamedAs:(NSString*)registeredName;
- (BOOL)connectToImporterMLNamedAs:(NSString*)registeredName;
@end
