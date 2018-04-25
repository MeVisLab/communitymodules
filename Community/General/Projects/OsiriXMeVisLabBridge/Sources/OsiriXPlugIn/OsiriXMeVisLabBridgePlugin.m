/*
 *  OsiriXMeVisLabBridgePlugin.m
 *  OsiriXMeVisLabBridgePlugin
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import "OsiriXMeVisLabBridgePlugin.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmissing-selector-name"
#pragma GCC diagnostic ignored "-Wduplicate-method-match"
#import <OsiriXAPI/browserController.h>
#import <OsiriXAPI/Notifications.h>
#import <OsiriXAPI/DicomDatabase.h>
#pragma GCC diagnostic pop

#import "mlMeVisLabServicesProtocol.h"
#import "ToolPopUpButton.h"
#import "IndeterminatedProgressController.h"

#define OFFER_MEVISLAB_EXAMPLE 1

#ifdef OFFER_MEVISLAB_EXAMPLE
# define TITLE_PREPARE_MEVISLAB  @"Prepare a MeVisLab target"
#endif

@interface DicomDatabase (OSIRIX_PRIVATE)
+(DicomDatabase*)activeLocalDatabase;
-(NSString*)incomingDirPath;
@end

@implementation OsiriXMeVisLabBridgePlugin

- (void) showHelp
{
  [[NSWorkspace sharedWorkspace] openFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"OsiriXMeVisLabBridge" ofType:@"pdf"]];
}

- (void) initPlugin
{
  // Vending this plugin's services
  if ((incomingConnection = [[NSConnection alloc] init])) {
    [incomingConnection setRootObject:self];
    if ([incomingConnection registerName:@"MLOsiriXServices"] == YES) {
      NSLog(@"Successfully registered MLOsiriXServices");
    }
    else {
      NSLog(@"Could not register MLOsiriXServices");
    }
  }
  
  // Watch for XML-RPC messages
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(OsiriXXMLRPCMessage:)
                                               name:OsirixXMLRPCMessageNotification
                                             object:nil];

  // Watch the termination of OsiriX
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(OsiriXWillTerminate:)
                                               name:NSApplicationWillTerminateNotification
                                             object:nil];
  
  // Watch addition of toolbar items
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(OsiriXWillAddToolbarItem:)
                                               name:NSToolbarWillAddItemNotification
                                             object:nil];
}

- (void) dealloc
{
  [self finalizePlugin];
  
  [super dealloc];
}

- (void) OsiriXWillTerminate: (NSNotification *)notification
{
  [self finalizePlugin];
}

- (void) finalizePlugin
{
  NSLog(@"Finalizing MeVisLabBridge plugin.");

  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:NSToolbarWillAddItemNotification
                                                object:nil];
  
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:NSApplicationWillTerminateNotification
                                                object:nil];
  
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:OsirixXMLRPCMessageNotification
                                                object:nil];
  
  [self connectionCleanup];
  
  // Don't know why invalidation of the receiving port is necessary, the documentation is a bit vague here
  // and implies invalidation of the connection is all that is required. However, the service name is not
  // unregistered unless the receiving port iteself gets invalidated...
  [[incomingConnection receivePort] invalidate];
  // Send NSConnectionDidDieNotification to connected observers
  [incomingConnection invalidate];
  [incomingConnection release];
  incomingConnection = nil;
  
  [targets release];
  targets = nil;
}

- (int) protocolVersion
{
  return MLOsiriXServices_ProtocolVersion;
}

- (NSString *) servicesProviderName
{
  return NSStringFromClass([self class]);
}

- (NSUInteger) servicesProviderVersion
{
  return [[[NSBundle bundleForClass:[self class]] objectForInfoDictionaryKey:@"CFBundleVersion"] unsignedIntegerValue];
}

- (NSString *) servicesProviderAppName
{
  return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
}

- (NSString *) servicesProviderAppVersion
{
  return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
}

- (NSString *) servicesProviderAppId
{
  return [[NSBundle mainBundle] bundleIdentifier];
}

- (BOOL) removeLocalDatabaseFolder:(NSString *)folderPath
{
  NSMutableArray *dbArray = [[[[NSUserDefaults standardUserDefaults] arrayForKey:@"localDatabasePaths"] mutableCopy] autorelease];
  if (dbArray == nil)
    return NO;

  NSMutableArray *discardedDicts = [NSMutableArray array];

  for (NSDictionary *dict in dbArray) {
    if ([[dict valueForKey:@"Path"] isEqualToString:folderPath]) {
      [discardedDicts addObject:dict];
    }
  }

  [dbArray removeObjectsInArray:discardedDicts];

  BOOL modified = [discardedDicts count] > 0;
  if (modified) {
    [[NSUserDefaults standardUserDefaults] setObject:dbArray forKey:@"localDatabasePaths"];
    [[NSUserDefaults standardUserDefaults] synchronize];
  }

  return modified;
}

- (OBTargetIdentifier) registerTargetWithName:(NSString *)name status:(MLOSStatus *)status
{
  if (!targets) {
    targets = [NSMutableDictionary new];
  }
  
  // Ensure unique target identifier and menu item title
  unsigned int number = 1;
  OBTargetIdentifier targetIdentifierValue = 0;
  NSNumber *targetIdentifierObject = nil;
  do {
    if (number > 1) {
      name = [NSString stringWithFormat:@"%@ %u", name, number];
    }
    targetIdentifierValue = (OBTargetIdentifier)[name hash];
    [targetIdentifierObject release];
    targetIdentifierObject = [[NSNumber alloc] initWithUnsignedLongLong:targetIdentifierValue];
    ++number;
  } while ([targets objectForKey:targetIdentifierObject]);
  
  [targets setObject:name forKey:targetIdentifierObject];
  [[targetPopUp cell] addItemWithTitle:name];
  [[[targetPopUp cell] itemWithTitle:name] setRepresentedObject:targetIdentifierObject];
  [targetIdentifierObject release];

  if ([targets count] == 1) {
#ifdef OFFER_MEVISLAB_EXAMPLE
    [[targetPopUp cell] removeItemWithTitle:TITLE_PREPARE_MEVISLAB];
#endif
    
    [self indicateTargetsAvailable:YES];
  }
  
  NSLog(@"Successfully registered target: '%@'", name);
  
  if (status) {
    *status = MLOSSuccess;
  }
  return targetIdentifierValue;
}

- (MLOSStatus) unregisterTargetWithIdentifier:(OBTargetIdentifier)identifier
{
  NSNumber *targetIdentifierObject = [NSNumber numberWithUnsignedLongLong:identifier];
  
  if (!targets || ![targets objectForKey:targetIdentifierObject]) {
    NSLog(@"Could not unregister target with identifier: '%@'. Unknown identifier", targetIdentifierObject);
    
    return MLOSUnkownTargetError;
  }
  else {
    NSString *name = [targets objectForKey:targetIdentifierObject];
    
    [[targetPopUp cell] removeItemWithTitle:name];
    [targets removeObjectForKey:targetIdentifierObject];
    
    if ([targets count] == 0) {
#ifdef OFFER_MEVISLAB_EXAMPLE
      [[targetPopUp cell] addItemWithTitle:TITLE_PREPARE_MEVISLAB];
#endif

      [self indicateTargetsAvailable:NO];
    }
    
    NSLog(@"Successfully unregistered target: '%@'", name);
    
    return MLOSSuccess;
  }
}

- (void) connectionCleanup
{
  if (proxy) {
    NSLog(@"Cleaning up connection with valid proxy.");

    NSConnection *outgoingConnection = [proxy connectionForProxy];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:NSConnectionDidDieNotification
                                                  object:nil];
    [outgoingConnection invalidate];
    [proxy release];
    proxy = nil;
  }
}

- (BOOL) connectAndReturnError: (NSError **)error
{
  // Check for existing connection
  if (proxy != nil) {
    return YES;
  }
  
  // Look for service on local host only
  proxy = [[NSConnection rootProxyForConnectionWithRegisteredName:@"MeVisLabLoadDICOM" host:nil] retain];
  
  if (proxy == nil) {
    NSLog(@"Could not connect to MeVisLabLoadDICOM");

    if (error != NULL) {
      *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                   code:-1
                               userInfo:[NSDictionary dictionaryWithObjectsAndKeys:
                                         @"Could not connect to the MeVisLabLoadDICOM service.", NSLocalizedDescriptionKey,
                                         @"Did you include an OsiriXBridge module into your MeVisLab network?", NSLocalizedRecoverySuggestionErrorKey,
                                         nil]];
    }
    
    [self connectionCleanup];
  }
  else {
    NSLog(@"Established connection to MeVisLabLoadDICOM");
    
    // Set connection timeouts
    NSConnection *outgoingConnection = [proxy connectionForProxy];
    [outgoingConnection setRequestTimeout:5];
    [outgoingConnection setReplyTimeout:300];
    
    // Notification observer for a lost connection
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(connectionDidDie:)
                                                 name:NSConnectionDidDieNotification
                                               object:outgoingConnection];
    
    // Set protocol for the remote object
    [proxy setProtocolForProxy:@protocol(MLMeVisLabLoadDICOMServices)];
    
    if ([proxy respondsToSelector:@selector(protocolVersion)]) {
      if ([proxy protocolVersion] != MLMeVisLabLoadDICOMServices_ProtocolVersion) {
        NSLog (@"Communication protocol version mismatch. Expected MeVisLabLoadDICOM protocol version %d, but got version %d", MLMeVisLabLoadDICOMServices_ProtocolVersion, [proxy protocolVersion]);
      
        if (error != NULL) {
          *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                       code:-3
                                   userInfo:[NSDictionary dictionaryWithObjectsAndKeys:
                                             [NSString stringWithFormat:@"Communication protocol version mismatch. Expected MeVisLabLoadDICOM protocol version %d, but got version %d.", MLMeVisLabLoadDICOMServices_ProtocolVersion, [proxy protocolVersion]], NSLocalizedDescriptionKey,
                                             @"Please ensure you've installed the OsiriXMeVisLabBridge plugin that shipped with your current MeVisLab version.", NSLocalizedRecoverySuggestionErrorKey,
                                             nil]];
        }
        
        [self connectionCleanup];
      }
    }
  }

  return (proxy != nil);
}

- (void) connectionDidDie: (NSNotification *)notification
{
  NSLog(@"MeVisLabLoadDICOM connection died");
  
  [self connectionCleanup];
}

- (NSString *) osirixIncomingFolderPath
{
  // The images put into this folder will be imported to the database automatically.
  // OsiriX will check that folder every few seconds.

  if ([DicomDatabase respondsToSelector:@selector(activeLocalDatabase)]) {
    DicomDatabase *dataBase = [DicomDatabase activeLocalDatabase];
    if ([dataBase respondsToSelector:@selector(incomingDirPath)]) {
      return [dataBase incomingDirPath];
    }
  }

  NSString *documentsDirectory = [[BrowserController currentBrowser] documentsDirectory];
  return [documentsDirectory stringByAppendingPathComponent:@"/INCOMING.noindex/"];
}

- (ViewerController *) viewerControllerDisplayingImageSeriesWithFilename:(NSString *)filename
{
  NSArray *viewers = [self viewerControllersList];
  for (ViewerController *viewer in viewers) {
    NSArray *pixList = [viewer pixList];
    for (DCMPix *curPix in pixList) {
      if ([[curPix srcFile] isEqualToString:filename]) {
        return viewer;
      }
    }
  }
  return nil;
}

- (NSArray *) fileListOfAnySerieInCurrentDatabaseSelection
{
  NSMutableArray *fileList = nil;
  
  NSArray *currentSelection = [[BrowserController currentBrowser] databaseSelection];
  if ([currentSelection count] > 0) {
    // Just evaluate the first selected item
    id selection = [currentSelection objectAtIndex:0];
    NSSet *images = nil;
    
    if ([[[selection entity] name] isEqualToString:@"Study"]) 
      images = [[[selection valueForKey:@"series"] anyObject] valueForKey:@"images"];
    else
      images = [selection valueForKey:@"images"];
    
    fileList = [NSMutableArray arrayWithCapacity:[images count]];
    
    for (id item in images) {
      [fileList addObject:[item valueForKey:@"completePath"]];
    }
  }
  
  return fileList;
}

- (NSArray *) fileListOfCurrentDatabaseSelection
{
  NSMutableArray *fileList = nil;
  
  NSArray *currentSelection = [[BrowserController currentBrowser] databaseSelection];
  for (id selectedItem in currentSelection) {
    NSSet *images = nil;
    
    if ([[[selectedItem entity] name] isEqualToString:@"Study"]) {
      NSMutableSet *imageSeries = [NSMutableSet setWithCapacity:500];
      for (id serie in [selectedItem valueForKey:@"series"]) {
        [imageSeries unionSet:[serie valueForKey:@"images"]];
      }
      images = imageSeries;
    } else {
      images = [selectedItem valueForKey:@"images"];
    }
    
    if (! fileList) {
      fileList = [NSMutableArray arrayWithCapacity:[images count]];
    }
    for (id item in images) {
      [fileList addObject:[item valueForKey:@"completePath"]];
    }
  }
  
  return fileList;
}

- (NSArray *) fileListOfCurrentViewerSeries
{
  NSArray *pixList = [viewerController pixList];
  
  NSMutableArray *fileList = [NSMutableArray arrayWithCapacity:[pixList count]];
  
  for (DCMPix *curPix in pixList) {
    [fileList addObject:[curPix srcFile]];
  }
  
  return fileList;
}

- (NSDictionary *) seriesInfoOfViewerDisplayingImageSeriesWithFilename: (NSString *)filename
{
  ViewerController *controller = [self viewerControllerDisplayingImageSeriesWithFilename:filename];
  if (! controller) return nil;
  
  NSArray *pixList = [controller pixList];
  NSMutableArray *fileList = [NSMutableArray arrayWithCapacity:[pixList count]];
  
  for (DCMPix *curPix in pixList) {
    [fileList addObject:[curPix srcFile]];
  }
  
  DCMView *view = [controller imageView];
  NSNumber *curImage = [NSNumber numberWithInt:[view curImage]];
  NSString *curImageFilename = [[view curDCM] srcFile];
  NSNumber *curWL = [NSNumber numberWithFloat:[view curWL]];
  NSNumber *curWW = [NSNumber numberWithFloat:[view curWW]];
  
  return [NSDictionary dictionaryWithObjectsAndKeys:fileList, @"series",
                                                    curImage, @"curImage",
                                                    curImageFilename, @"curImageFilename",
                                                    curWL, @"curImageWL",
                                                    curWW, @"curImageWW",
          nil];
}

- (BOOL) sendSeriesWithFilename: (NSString *)filename toMeVisLabTargetWithIdentifier: (OBTargetIdentifier)identifier error: (NSError **)error
{
  BOOL successful = FALSE;
  
  if ([self connectAndReturnError:error]) {
    NSDictionary *info = [self seriesInfoOfViewerDisplayingImageSeriesWithFilename:filename];
    if (info) {
      
      @try {
        // send filenames to MeVisLab
        [proxy loadImagesWithFilenames:[info objectForKey:@"series"] forTargetWithIdentifier:identifier];
        
        successful = TRUE;
      }
      @catch (NSException *e) {
        NSLog (@"Exception while sending series:\nname: %@\nreason: %@", [e name], [e reason]);
        
        if (error != NULL) {
          *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                       code:-2
                                   userInfo:[NSDictionary dictionaryWithObjectsAndKeys:
                                             [NSString stringWithFormat:@"Exception while sending series:\nname: %@\nreason: %@", [e name], [e reason]], NSLocalizedDescriptionKey,
                                             @"Please send the series again. If the error persists restart MeVisLab and/or OsiriX.", NSLocalizedRecoverySuggestionErrorKey,
                                             nil]];
        }
      }
    }
  }
  
  return successful;
}

- (BOOL) sendCurrentSeriesToMeVisLabTargetWithName: (NSString *)name error: (NSError **)error
{
  OBTargetIdentifier targetIdentifierValue = (OBTargetIdentifier)[name hash];
  NSNumber *targetIdentifierObject = [NSNumber numberWithUnsignedLongLong:targetIdentifierValue];
  
  if ([targets objectForKey:targetIdentifierObject]) {
    return [self sendCurrentSeriesToMeVisLabTargetWithIdentifier:targetIdentifierValue
                                                           error:error];
  }
  
  if (error != NULL) {
    *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                 code:-4
                             userInfo:[NSDictionary dictionaryWithObjectsAndKeys:
                                       [NSString stringWithFormat:@"Unknown target '%@'", name], NSLocalizedDescriptionKey,
                                       @"Please verify the target names assigned to the OsirixBridge modules.", NSLocalizedRecoverySuggestionErrorKey,
                                       nil]];
  }
  return FALSE;
}

- (BOOL) sendCurrentSeriesToMeVisLabTargetWithIdentifier: (OBTargetIdentifier)identifier error: (NSError **)error
{
  BOOL successful = FALSE;
  
  if ([self connectAndReturnError:error]) {
    NSArray *fileList = [self fileListOfCurrentDatabaseSelection];
    
    @try {
      // send filenames to MeVisLab
      [proxy loadImagesWithFilenames:fileList forTargetWithIdentifier:identifier];
      
      successful = TRUE;
    }
    @catch (NSException *e) {
      NSLog (@"Exception while sending series:\nname: %@\nreason: %@", [e name], [e reason]);
      
      if (error != NULL) {
        *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                     code:-2
                                 userInfo:[NSDictionary dictionaryWithObjectsAndKeys:
                                           [NSString stringWithFormat:@"Exception while sending series:\nname: %@\nreason: %@", [e name], [e reason]], NSLocalizedDescriptionKey,
                                           @"Please send the series again. If the error persists restart MeVisLab and/or OsiriX.", NSLocalizedRecoverySuggestionErrorKey,
                                           nil]];
      }
    }
  }
  
  return successful;
}

- (void) OsiriXWillAddToolbarItem: (NSNotification *)notification
{
  NSToolbarItem *item = [[notification userInfo] objectForKey: @"item"];
  if ([[item label] isEqualToString:@"Send to MeVisLab"]) {
    [self setupToolbarItem:item];
  }
}

- (void) setMenus
{
  // Customize toolbar item
  NSToolbar *toolbar = [[[BrowserController currentBrowser] window] toolbar];
  for (NSToolbarItem *item in [toolbar items]) {
    if ([[item label] isEqualToString:@"Send to MeVisLab"]) {
      [self setupToolbarItem:item];
      break;
    }
  }
}

- (void) setupToolbarItem: (NSToolbarItem *)item
{
  if (targetPopUp) return;
  
  targetPopUp = [[[ToolPopUpButton alloc] initWithFrame:([[item toolbar] sizeMode] == NSToolbarSizeModeSmall) ? NSMakeRect(0,0,24,24) : NSMakeRect(0,0,32,32)] autorelease];
  [targetPopUp setIconImage:[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"SendToMeVisLab-notarget" ofType:@"tiff"]] autorelease]];
#ifdef OFFER_MEVISLAB_EXAMPLE
  [targetPopUp setArrowImage:[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"ArrowPointingDown" ofType:@"tiff"]] autorelease]];
#endif
  [targetPopUp setTarget:self];
  [targetPopUp setAction:@selector(toolMenuAction:)];
  [targetPopUp setToolTip:@"Sends the selected series to MeVisLab. Currently, there are no receiving modules known to OsiriX."];
  
  NSMenu *menu = [targetPopUp menu];
  [menu insertItem:[[NSMenuItem new] autorelease] atIndex:0];
  
#ifdef OFFER_MEVISLAB_EXAMPLE
  [[targetPopUp cell] addItemWithTitle:TITLE_PREPARE_MEVISLAB];
#endif
  
  [item setView:targetPopUp];
  
  if ([self connectAndReturnError:NULL]) {
    [proxy clientIsInitialized];
  }
}

- (void) indicateTargetsAvailable: (BOOL)flag
{
  if (flag) {
    [targetPopUp setIconImage :[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"SendToMeVisLab-gray" ofType:@"tiff"]] autorelease]];
#ifndef OFFER_MEVISLAB_EXAMPLE
    [targetPopUp setArrowImage:[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"ArrowPointingDown" ofType:@"tiff"]] autorelease]];
#endif
    [targetPopUp setToolTip:@"Sends the selected series to MeVisLab."];
  }
  else {
    [targetPopUp setIconImage :[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"SendToMeVisLab-notarget" ofType:@"tiff"]] autorelease]];
#ifndef OFFER_MEVISLAB_EXAMPLE
    [targetPopUp setArrowImage:[[NSImage new] autorelease]];
#endif
    [targetPopUp setToolTip:@"Sends the selected series to MeVisLab. Currently, there are no receiving modules known to OsiriX."];
  }
}

- (void) toolMenuAction: (id)sender
{
  if (! [self isOsiriXVersionSupported])
    return;
  
  NSMenuItem *item = [sender selectedItem];
  NSString *targetName = [item title];
  NSNumber *targetIdentifierObject = [item representedObject];
  OBTargetIdentifier targetIdentifierValue = [targetIdentifierObject unsignedLongLongValue];

  NSLog(@"Target '%@' selected as receiver for the series.", targetName);
  
  if (targets && [targets objectForKey:targetIdentifierObject]) {
    [targetPopUp setIconImage:[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"SendToMeVisLab-red" ofType:@"tiff"]] autorelease]];
    // process current run loop for a single iteration to update display
    [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate date]];
    
    IndeterminatedProgressController *waitWindow = [IndeterminatedProgressController new];
    [waitWindow setMessage:@"Sending series to MeVisLab..."];
    [waitWindow setInfo:[NSString stringWithFormat:@"(%@)", targetName]];
    [waitWindow showWindow:nil];

    NSError *error = nil;
    BOOL successful = [self sendCurrentSeriesToMeVisLabTargetWithIdentifier:targetIdentifierValue error:&error];
    
    [waitWindow close];
    [waitWindow release];
    
    [targetPopUp setIconImage:[[[NSImage alloc] initWithContentsOfFile:[[NSBundle bundleForClass:[self class]] pathForResource:@"SendToMeVisLab-gray" ofType:@"tiff"]] autorelease]];
    
    if (! successful) {
      [[NSAlert alertWithError:error] runModal];
    }
  }
#ifdef OFFER_MEVISLAB_EXAMPLE
  else if ([targetName isEqualToString:TITLE_PREPARE_MEVISLAB]) {
    NSURL *networkFileURL = [NSURL fileURLWithPath:[[NSBundle bundleForClass:[self class]] pathForResource:@"OsiriXBridge" ofType:@"mlab"]];
    if ([[NSWorkspace sharedWorkspace] openURLs:[NSArray arrayWithObject:networkFileURL]
                        withAppBundleIdentifier:@"de.mevis.MeVisLab"
                                        options:NSWorkspaceLaunchWithoutActivation
                 additionalEventParamDescriptor:nil
                              launchIdentifiers:NULL] == NO) {

      NSAlert *alert = [[NSAlert new] autorelease];
      [alert setMessageText:@"The MeVisLab example network could not be loaded with MeVisLab."];
      [alert setInformativeText:@"The plugin contains an example network that can be used with MeVisLab to setup a target for OsiriX."];
      [alert addButtonWithTitle:@"Close"];
      [alert addButtonWithTitle:@"Show network file"];
      [alert addButtonWithTitle:@"Help"];

      NSModalResponse response = [alert runModal];

      if (response == NSAlertSecondButtonReturn) {
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[NSArray arrayWithObject:networkFileURL]];
      }
      else if (response == NSAlertThirdButtonReturn) {
        [self showHelp];
      }
    }
  }
#endif
}

- (long) filterImage: (NSString *)menuName
{
  if (! [self isOsiriXVersionSupported])
    return 0;
  
  NSAlert *alert = [[NSAlert new] autorelease];
  [alert setMessageText:@"The OsiriXMeVisLabBridge uses its toolbar icon for control. Control via the Plugins menu is not supported."];
  [alert setInformativeText:@"The toolbar icon can be added by selecting Customize Toolbar... from this dialog or the Format menu."];
  [alert addButtonWithTitle:@"Close"];
  [alert addButtonWithTitle:@"Customize Toolbar..."];
  [alert addButtonWithTitle:@"Help"];

  NSModalResponse response = [alert runModal];

  if (response == NSAlertSecondButtonReturn) {
    NSToolbar *toolbar = [[[BrowserController currentBrowser] window] toolbar];
    if ([toolbar customizationPaletteIsRunning] == NO) {
      [toolbar runCustomizationPalette:nil];
    }
  }
  else if (response == NSAlertThirdButtonReturn) {
    [self showHelp];
  }

  // Return 0 in all cases since the user has already been informed about errors
  return 0;
}

- (void) OsiriXXMLRPCMessage: (NSNotification *)notification
{
  @try {
    NSMutableDictionary *httpServerMessage = [notification object];
    
    if ([[httpServerMessage valueForKey:@"MethodName"] isEqualToString:@"SendSelectedSeriesToMeVisLab"]) {
      NSError *error = nil;
      
      NSXMLDocument *doc = [httpServerMessage valueForKey:@"NSXMLDocument"];
      NSString *encoding = [doc characterEncoding];
      NSArray *keys = [doc nodesForXPath:@"methodCall/params//member/name" error:&error];
      NSArray *values = [doc nodesForXPath:@"methodCall/params//member/value" error:&error];
      if (1 == [keys count] || 1 == [values count]) {
        NSMutableDictionary *paramDict = [NSMutableDictionary dictionary];
        int i = 0;
        for (i = 0; i < [keys count]; ++i) {
          id value;
          if([encoding isEqualToString:@"UTF-8"] == NO && [[[values objectAtIndex:i] objectValue] isKindOfClass:[NSString class]]) {
            value = [(NSString*)CFXMLCreateStringByUnescapingEntities(NULL, (CFStringRef)[[values objectAtIndex:i] objectValue], NULL) autorelease];
          } else {
            value = [[values objectAtIndex:i] objectValue];
          }
          [paramDict setValue:value forKey:[[keys objectAtIndex:i] objectValue]];
        }
        
        error = nil;
        NSInteger errorValue = 0;
        if (! [self sendCurrentSeriesToMeVisLabTargetWithName:[paramDict valueForKey:@"target"] error:&error]) {
          errorValue = [error code];
        }

#if __LP64__ || NS_BUILD_32_LIKE_64
# define PrintfFormatString_NSInteger "%ld"
#else
# define PrintfFormatString_NSInteger "%d"
#endif
        
        // Done, send response to sender
        NSString *xml = [NSString stringWithFormat:@"<?xml version=\"1.0\"?><methodResponse><params><param><value><struct><member><name>error</name><value>" PrintfFormatString_NSInteger "</value></member></struct></value></param></params></methodResponse>", errorValue];
        NSXMLDocument *doc = [[[NSXMLDocument alloc] initWithXMLString:xml options:NSXMLNodeOptionsNone error:&error] autorelease];
        [httpServerMessage setValue:doc forKey:@"NSXMLDocumentResponse"];
        [httpServerMessage setValue:[NSNumber numberWithBool:YES] forKey:@"Processed"];
      }
      else {
        NSLog(@"SendSelectedSeriesToMeVisLab requires exactly one target name as argument.");
      }
    }
    else if ([[httpServerMessage valueForKey:@"MethodName"] isEqualToString:@"CloseDB"]) {
      NSError *error = nil;

      NSXMLDocument *doc = [httpServerMessage valueForKey:@"NSXMLDocument"];
      NSString *encoding = [doc characterEncoding];
      NSArray *keys = [doc nodesForXPath:@"methodCall/params//member/name" error:&error];
      NSArray *values = [doc nodesForXPath:@"methodCall/params//member/value" error:&error];
      if (1 == [keys count] || 1 == [values count]) {
        NSMutableDictionary *paramDict = [NSMutableDictionary dictionary];
        int i = 0;
        for (i = 0; i < [keys count]; ++i) {
          id value;
          if([encoding isEqualToString:@"UTF-8"] == NO && [[[values objectAtIndex:i] objectValue] isKindOfClass:[NSString class]]) {
            value = [(NSString*)CFXMLCreateStringByUnescapingEntities(NULL, (CFStringRef)[[values objectAtIndex:i] objectValue], NULL) autorelease];
          } else {
            value = [[values objectAtIndex:i] objectValue];
          }
          [paramDict setValue:value forKey:[[keys objectAtIndex:i] objectValue]];
        }

        NSInteger errorValue = 0;
        if (! [self removeLocalDatabaseFolder:[paramDict valueForKey:@"path"]]) {
          errorValue = -1;
        }

#if __LP64__ || NS_BUILD_32_LIKE_64
# define PrintfFormatString_NSInteger "%ld"
#else
# define PrintfFormatString_NSInteger "%d"
#endif

        // Done, send response to sender
        NSString *xml = [NSString stringWithFormat:@"<?xml version=\"1.0\"?><methodResponse><params><param><value><struct><member><name>error</name><value>" PrintfFormatString_NSInteger "</value></member></struct></value></param></params></methodResponse>", errorValue];
        NSXMLDocument *doc = [[[NSXMLDocument alloc] initWithXMLString:xml options:NSXMLNodeOptionsNone error:&error] autorelease];
        [httpServerMessage setValue:doc forKey:@"NSXMLDocumentResponse"];
        [httpServerMessage setValue:[NSNumber numberWithBool:YES] forKey:@"Processed"];
      }
      else {
        NSLog(@"CloseDB requires exactly one folder path as argument.");
      }
    }
    
    NSLog(@"%@", [httpServerMessage description]);
  }
  @catch (NSException *e) {
    NSLog(@"Exception while processing XML-RPC:\nname: %@\nreason: %@", [e name], [e reason]);
  }
}

- (BOOL) isOsiriXVersionSupported
{
  if ([[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"] intValue] < 4649) {
    NSAlert *alert = [[NSAlert new] autorelease];
    [alert setMessageText:@"The OsiriX application you are running is out of date."];
    [alert setInformativeText:@"OsiriX 3.3 is necessary for this plugin to execute."];
    [alert addButtonWithTitle:@"Close"];
    [alert addButtonWithTitle:@"Help"];

    NSModalResponse response = [alert runModal];

    if (response == NSAlertSecondButtonReturn) {
      [self showHelp];
    }
    
    return NO;
  }
  return YES;
}

@end
