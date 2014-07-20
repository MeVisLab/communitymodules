//
//  MevisProxyForOsiriX.h
//  _MLOsiriXImpoter_debug
//
//  Created by chuwa on 6/9/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "mlOsiriXImporter.h"
#import "MeVisOsiriXProxyProtocol.h"

@interface MevisProxyForOsiriX : NSObject
{
  ml::OsiriXImporter* importerML;
  NSConnection *outgoingConnection; 
  NSMutableArray* sharedMemoryArray;
  id osirixMevisPluginProxy;
}
- (id) initWithImporterML:(ml::OsiriXImporter*) aML;
- (void) connectionDidDie: (NSNotification *)notification;
-(void)releaseAllImages;
- (void*) createSharedVolume:(long)size:(NSString*)memidstr;
-(void) releaseSharedVolume:(NSMutableDictionary*)apiece;
-(void) disconnectProxy;
-(BOOL) getImagesFromOsiriX;
-(BOOL) setImage:(NSDictionary*)aImageDict ForIndex:(int)imgindex;
-(BOOL) tryBuildConnectionWithOsiriX;
-(BOOL) isConnectedWithOsiriX;
@end
