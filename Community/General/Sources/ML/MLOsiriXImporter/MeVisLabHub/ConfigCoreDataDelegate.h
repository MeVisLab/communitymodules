//
//  ConfigCoreDataDelegate.h
//  MeVisLabHub
//
//  Created by chuwa on 7/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ConfigCoreDataDelegate : NSObject {
    IBOutlet NSWindow *window;
	BOOL needUpdateMenuItems;
    IBOutlet NSArrayController* operationController;
    NSPersistentStoreCoordinator *persistentStoreCoordinator;
    NSManagedObjectModel *managedObjectModel;
    NSManagedObjectContext *managedObjectContext;
}
- (NSString *)pluginResourcePath;
- (NSPersistentStoreCoordinator *)persistentStoreCoordinator;
- (NSManagedObjectModel *)managedObjectModel;
- (NSManagedObjectContext *)managedObjectContext;
- (NSMutableArray*)menuItemFromOpernationsList;
- (IBAction)saveAction:sender;
- (NSManagedObject*)getOperationForMenuItem:(NSString*)menuStr;
- (IBAction)createCommandLine:(id)sender;

@end
