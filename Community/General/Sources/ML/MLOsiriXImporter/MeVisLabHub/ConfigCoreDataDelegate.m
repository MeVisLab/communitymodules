//
//  ConfigCoreDataDelegate.m
//  MeVisLabHub
//
//  Created by chuwa on 7/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ConfigCoreDataDelegate.h"


@implementation ConfigCoreDataDelegate

/**
 Returns the support folder for the application, used to store the Core Data
 store file.  This code uses a folder named "coredatetest2" for
 the content, either in the NSApplicationSupportDirectory location or (if the
 former cannot be found), the system's temporary directory.
 */

- (NSString *)pluginResourcePath {
	
   // NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
//    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : NSTemporaryDirectory();[basePath stringByAppendingPathComponent:@"MeVisHub"]
	NSBundle* myBundle = [NSBundle bundleWithIdentifier:@"se.liu.cmiv.MeVisLabHub"];
	NSString *basePath = [myBundle resourcePath];
	
    return basePath;
}


/**
 Creates, retains, and returns the managed object model for the application 
 by merging all of the models found in the application bundle.
 */

- (NSManagedObjectModel *)managedObjectModel {
	
    if (managedObjectModel != nil) {
        return managedObjectModel;
    }
	NSBundle* myBundle = [NSBundle bundleWithIdentifier:@"se.liu.cmiv.MeVisLabHub"];
	NSArray* bundlesArray=[NSArray arrayWithObject:myBundle];
    managedObjectModel = [[NSManagedObjectModel mergedModelFromBundles:bundlesArray] retain];    
    return managedObjectModel;
}


/**
 Returns the persistent store coordinator for the application.  This 
 implementation will create and return a coordinator, having added the 
 store for the application to it.  (The folder for the store is created, 
 if necessary.)
 */

- (NSPersistentStoreCoordinator *) persistentStoreCoordinator {
	
    if (persistentStoreCoordinator != nil) {
        return persistentStoreCoordinator;
    }
	
    NSFileManager *fileManager;
    NSString *pluginResourcePath  = nil;
    NSURL *url;
    NSError *error;
    
    fileManager = [NSFileManager defaultManager];
    pluginResourcePath  = [self pluginResourcePath ];
    if ( ![fileManager fileExistsAtPath:pluginResourcePath  isDirectory:NULL] ) {
        [fileManager createDirectoryAtPath:pluginResourcePath  attributes:nil];
    }
    
    url = [NSURL fileURLWithPath: [pluginResourcePath  stringByAppendingPathComponent: @"MeVisLabHubConfigs.xml"]];
    persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel: [self managedObjectModel]];
    if (![persistentStoreCoordinator addPersistentStoreWithType:NSXMLStoreType configuration:nil URL:url options:nil error:&error]){
        [[NSApplication sharedApplication] presentError:error];
    }    
	
    return persistentStoreCoordinator;
}


/**
 Returns the managed object context for the application (which is already
 bound to the persistent store coordinator for the application.) 
 */

- (NSManagedObjectContext *) managedObjectContext {
	
    if (managedObjectContext != nil) {
        return managedObjectContext;
    }
	
    NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
    if (coordinator != nil) {
        managedObjectContext = [[NSManagedObjectContext alloc] init];
        [managedObjectContext setPersistentStoreCoordinator: coordinator];
    }
    
    return managedObjectContext;
}


/**
 Returns the NSUndoManager for the application.  In this case, the manager
 returned is that of the managed object context for the application.
 */

- (NSUndoManager *)windowWillReturnUndoManager:(NSWindow *)window {
    return [[self managedObjectContext] undoManager];
}


/**
 Performs the save action for the application, which is to send the save:
 message to the application's managed object context.  Any encountered errors
 are presented to the user.
 */

- (IBAction) saveAction:(id)sender {
	if([sender tag])//from cancel button
	{
		NSUndoManager * undoManager=[[self managedObjectContext] undoManager];
		while([undoManager canUndo])[undoManager undo];
	}
	
	[window performClose:sender];
 }


/**
 Implementation of the applicationShouldTerminate: method, used here to
 handle the saving of changes in the application managed object context
 before the application terminates.
 */

- (BOOL)windowShouldClose:(id)window
{
	
    NSError *error;
    int reply = YES;
	needUpdateMenuItems=NO;
    
    if (managedObjectContext != nil) {
        if ([managedObjectContext commitEditing]) {
			if([managedObjectContext hasChanges])
			{
				needUpdateMenuItems=YES;
				if (![managedObjectContext save:&error]) {
					
					// This error handling simply presents error information in a panel with an 
					// "Ok" button, which does not include any attempt at error recovery (meaning, 
					// attempting to fix the error.)  As a result, this implementation will 
					// present the information to the user and then follow up with a panel asking 
					// if the user wishes to "Quit Anyway", without saving the changes.
					
					// Typically, this process should be altered to include application-specific 
					// recovery steps.  
					
					BOOL errorResult = [[NSApplication sharedApplication] presentError:error];
					
					if (errorResult == YES) {
						reply = NO;
					} 
					
					else {
						
						int alertReturn = NSRunAlertPanel(nil, @"Could not save changes while quitting. Quit anyway?" , @"Quit anyway", @"Cancel", nil);
						if (alertReturn == NSAlertAlternateReturn) {
							reply = NO;	
						}
					}
				}
				else
				{
					NSUndoManager * undoManager=[[self managedObjectContext] undoManager];
					[undoManager removeAllActions];
				}
			}
        } 
        
        else {
            reply = NO;
        }
    }
    
    return reply;
}

- (void)windowWillClose:(NSNotification *)notification
{
	if(window==[notification object]&&needUpdateMenuItems)
	{
		NSBundle* myBundle = [NSBundle bundleWithIdentifier:@"se.liu.cmiv.MeVisLabHub"];
		NSString* infoPlistPath=[myBundle resourcePath];
		infoPlistPath=[infoPlistPath stringByDeletingLastPathComponent];
		infoPlistPath=[infoPlistPath stringByAppendingString:@"/Info.plist"];

		NSMutableDictionary* infoPlistDict=[NSMutableDictionary dictionaryWithContentsOfFile: infoPlistPath];
		NSMutableArray* newMenuItems=[self menuItemFromOpernationsList];
		
		[newMenuItems addObject:[NSString stringWithString:@"Configuration"]];
		[newMenuItems addObject:[NSString stringWithString:@"Help"]];
		[infoPlistDict setObject:newMenuItems forKey:@"MenuTitles"];
		BOOL ifsussess=[infoPlistDict writeToFile:infoPlistPath atomically:YES];
		if(!ifsussess)
			NSRunAlertPanel(@"Failed to Save", @"Failed to change the menu items in plugin. Please check if you have the right to write the Info.plist file in the plugin packege." ,nil, nil, nil);

	}
}
/**
 Implementation of dealloc, to release the retained variables.
 */

- (void) dealloc {
	
    [managedObjectContext release], managedObjectContext = nil;
    [persistentStoreCoordinator release], persistentStoreCoordinator = nil;
    [managedObjectModel release], managedObjectModel = nil;
    [super dealloc];
}

-(NSManagedObject*)getOperationForMenuItem:(NSString*)menuStr
{
	NSManagedObjectContext *moc = [self managedObjectContext];
	
	NSEntityDescription *entityDescription = [NSEntityDescription
											  
											  entityForName:@"Operations" inManagedObjectContext:moc];
	
	NSFetchRequest *request = [[[NSFetchRequest alloc] init] autorelease];
	
	[request setEntity:entityDescription];
	
	
	
	// Set example predicate and sort orderings...

	
	NSPredicate *predicate = [NSPredicate predicateWithFormat:
							  
							  @"OperationName == %@", menuStr];
	
	[request setPredicate:predicate];
	
	
	
//	NSSortDescriptor *sortDescriptor = [[NSSortDescriptor alloc]
//										
//										initWithKey:@"firstName" ascending:YES];
//	
//	[request setSortDescriptors:[NSArray arrayWithObject:sortDescriptor]];
//	
//	[sortDescriptor release];
	
	
	
	NSError *error = nil;
	
	NSArray *array = [moc executeFetchRequest:request error:&error];
	
	if (array == nil)
		
	{
		
		[[NSApplication sharedApplication] presentError:error];
		return nil;
		
	}
	if([array count])
		return [array objectAtIndex:0];
	return nil;
}
-(NSMutableArray*)menuItemFromOpernationsList
{
	NSManagedObjectContext *moc = [self managedObjectContext];
	
	NSEntityDescription *entityDescription = [NSEntityDescription
											  
											  entityForName:@"Operations" inManagedObjectContext:moc];
	
	NSFetchRequest *request = [[[NSFetchRequest alloc] init] autorelease];
	
	[request setEntity:entityDescription];
	
	
	[request setPredicate: [NSPredicate predicateWithValue: YES]];
	NSError *error = nil;
	NSMutableArray* menuItemsArray=[NSMutableArray arrayWithCapacity:0];
	NSArray *operationsarray = [moc executeFetchRequest:request error:&error];
	
	if (operationsarray == nil)
		
	{
		
		[[NSApplication sharedApplication] presentError:error];

		
	}
	else
	{
	
	
		for(NSManagedObject* aoperationconfig in operationsarray)
		{
			
			[menuItemsArray addObject:[aoperationconfig valueForKey:@"OperationName"]];
			
		}

	}
	
	return menuItemsArray;
	
}

- (IBAction)createCommandLine:(id)sender
{
	int                 result;
    NSOpenPanel         *oPanel = [NSOpenPanel openPanel];
	
    
	[oPanel setCanSelectHiddenExtension:YES];
	[oPanel setRequiredFileType:@"mlab"];
    [oPanel setAllowsMultipleSelection:NO];
    [oPanel setCanChooseDirectories:NO];
    
    result = [oPanel runModalForDirectory:0L file:nil types:nil];
    
    if (result == NSOKButton) 
    {
		NSString* path;
		path=[[oPanel filenames] objectAtIndex: 0];
		NSString* cachedMlabFilePath=[self pluginResourcePath];
		cachedMlabFilePath=[cachedMlabFilePath stringByAppendingString:@"/"];
		cachedMlabFilePath=[cachedMlabFilePath stringByAppendingString:[path lastPathComponent]];
		if([[NSFileManager defaultManager] fileExistsAtPath:cachedMlabFilePath])
			[[NSFileManager defaultManager] removeFileAtPath:cachedMlabFilePath handler:nil];
		BOOL isCopied=[[NSFileManager defaultManager] copyPath:path toPath:cachedMlabFilePath	handler:nil];
		NSString* commandline;
		if(isCopied)
		{
			commandline=[NSString stringWithFormat:@"/Applications/MeVisLab.app/Contents/MacOS/MeVisLab -quick -nowelcome -unique $pluginfolder$%@", [path lastPathComponent]];
		}
		else
		{
			NSRunAlertPanel(@"Failed", @"Failed to copy the mlab file into plugin package, you may not have the right to write in the plugin package. The commandline may not work if the original mlab file is removed from its path." ,nil, nil, nil);
			commandline=[NSString stringWithFormat:@"/Applications/MeVisLab.app/Contents/MacOS/MeVisLab -quick -nowelcome -unique %@", path];
		}
		[[operationController selection] setValue:commandline forKey:@"CommandLineStr"];
	}	
	
	
	
	
}
@end
