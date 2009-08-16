//
//  ImporterNotificationHandler.h
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "mlOsiriXImporter.h"

@interface ImporterNotificationHandler : NSObject {
	ml::OsiriXImporter* importerML;
}
- (id) initWithImporterML:(ml::OsiriXImporter*) aML;
- (void) registerSelfToNSNotificationCenter;
- (void) exporterIsAdded: (NSNotification *)notification;
@end
