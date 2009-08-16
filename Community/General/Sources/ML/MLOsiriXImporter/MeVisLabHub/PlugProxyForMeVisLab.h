//
//  PlugProxyForMeVisLab.h
//  MeVisLabHub
//
//  Created by chuwa on 8/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MeVisOsiriXConnectionProtocols.h"


@interface PlugProxyForMeVisLab : NSObject  <MeVisOsiriXProxyProtocol>
{
	ViewerController* osrixViewController;
	NSConnection *incomingConnection; 
	NSMutableArray* sharedMemoryArray;
	id MeVisLabExporterProxy;
}
- (id)initWithViewer:(ViewerController*)viewer;
- (void)releaseAllImages;
- (void) releaseSharedVolume:(NSMutableDictionary*)apiece;
- (void*) createSharedVolume:(long)size:(NSString*)memidstr;

@end
