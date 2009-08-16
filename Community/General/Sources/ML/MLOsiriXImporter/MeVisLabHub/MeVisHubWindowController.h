//
//  MeVisHubWindowController.h
//  MeVisLabHub
//
//  Created by chuwa on 7/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ConfigCoreDataDelegate.h"
#import "PluginFilter.h"
#import "OsiriX2MeVisLabTBridge.h"
#import "CMIVDCMView.h"

@interface MeVisHubWindowController : NSWindowController {
	IBOutlet NSWindow* configWindow;
	IBOutlet NSWindow* warningWindow;
	IBOutlet NSSlider* inputImageSlider;
	IBOutlet NSSlider* outputImageSlider;
	IBOutlet CMIVDCMView* inputImageView;
	IBOutlet CMIVDCMView* outputImageView;
	IBOutlet ConfigCoreDataDelegate* configDelgate;
	IBOutlet NSArrayController* operationController;
	IBOutlet NSTextField* para0NameTextField;
	IBOutlet NSTextField* para1NameTextField;
	IBOutlet NSTextField* para2NameTextField;
	IBOutlet NSTextField* para3NameTextField;
	IBOutlet NSTextField* para4NameTextField;
	IBOutlet NSTextField* para5NameTextField;
	IBOutlet NSSlider* para0Slider;
	IBOutlet NSSlider* para1Slider;
	IBOutlet NSSlider* para2Slider;
	IBOutlet NSSlider* para3Slider;
	IBOutlet NSSlider* para4Slider;
	IBOutlet NSSlider* para5Slider;
	IBOutlet NSTextField* para0ValueTextField;
	IBOutlet NSTextField* para1ValueTextField;
	IBOutlet NSTextField* para2ValueTextField;
	IBOutlet NSTextField* para3ValueTextField;
	IBOutlet NSTextField* para4ValueTextField;
	IBOutlet NSTextField* para5ValueTextField;
	IBOutlet NSMatrix* toolsBar;
	NSArray* seedingToolsConfigList;
	NSMutableArray* resultROIListCorrespondingToSeedList;
	NSArray* parametersConfigList;
	NSString* inputImageSeriesUID;
	NSTask*  meVisLabTask;
	ViewerController* osrixViewController;
	OsiriX2MeVisLabTBridge* bridgeExportToMeVisLab;
	OsiriX2MeVisLabTBridge* bridgeImportFromMeVisLab;
	BOOL isInitializing;
	int CurrentToolID;
	NSManagedObject* currentSeedingTool;
	NSDictionary* outputMaskResult;
	int currentSychronizeMode;
}
-(id)initFromNIB;
-(void)setWindowController:(ViewerController*)viewer;
-(void)showMainWindow:(NSString*)menuStr;
-(void)showConfigWindow;
-(void)runCommandLineForOperation:(NSString*)commandLineStr;
-(IBAction)endWarningWindow:(id)sender;
-(void)showParameterControls:(NSManagedObject*) configDict;
-(BOOL)prepareInputImages;
-(BOOL)initializeParameters:(NSManagedObject*)configDict;
-(BOOL)prepareOutputImages;
-(IBAction)scrollImageSynchronically:(id)sender;
-(IBAction)changeParameter:(id)sender;

- (IBAction)changCurrentTool:(id)sender;
- (void) roiAdded: (NSNotification*) note;
- (void) roiChanged: (NSNotification*) note;
- (void) roiRemoved: (NSNotification*) note;
- (NSColor*)colorByGivenName:(NSString*)name;
- (void)initializeToolsBar:(NSManagedObject*)configDict;
- (IBAction)updateOutputImage:(id)sender;
- (void)notifyMeVisLabSeedsChanged;
- (IBAction)closeCurrentWindow:(id)sender;
-(int)checkSynchronizeMode;
-(void)updateMasksForOutputView:(int)index;
- (void)initializeOutputViewROI;
@end
