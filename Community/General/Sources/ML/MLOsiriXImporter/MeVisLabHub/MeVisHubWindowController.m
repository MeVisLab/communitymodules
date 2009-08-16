//
//  MeVisHubWindowController.m
//  MeVisLabHub
//
//  Created by chuwa on 7/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MeVisHubWindowController.h"
#define SYNCMODE_ALLSAME 1
#define SYNCMODE_SAMEORIENTATION 2
#define SYNCMODE_NOSUPPORT 0
@implementation MeVisHubWindowController
-(id)initFromNIB
{
	self = [super initWithWindowNibName:@"MeVisHubGUI"];
	[self loadWindow];
	return self;
}
-(void)setWindowController:(ViewerController*)viewer
{
	osrixViewController=viewer;
}
-(void)dealloc
{
	[meVisLabTask release];
	[super dealloc];
}
-(void)showConfigWindow
{
	[configWindow makeKeyAndOrderFront:self];
	[configWindow display];
}
-(void)showMainWindow:(NSString*)menuStr
{
	//read the configuration		
	NSManagedObject* operationConfigDict=[configDelgate getOperationForMenuItem:menuStr];
	if(operationConfigDict==nil)
	{
		NSRunAlertPanel(@"Failed", @"Failed to run this function, the link to this function is broken." ,nil, nil, nil);
		return;
	}
	[operationController setSelectedObjects:[NSArray arrayWithObject:operationConfigDict]];
	
	NSString* operationName=[operationConfigDict valueForKey:@"OperationName"];
	
	
	bridgeExportToMeVisLab=[[OsiriX2MeVisLabTBridge alloc] initWithIncommingConnectionName:@"MeVisHub:Exporter" OutgoingConnection:[[NSString stringWithString:@"OsiriXImporter:"] stringByAppendingString:operationName]];
	bridgeImportFromMeVisLab=[[OsiriX2MeVisLabTBridge alloc] initWithIncommingConnectionName:@"MeVisHub:Importer" OutgoingConnection:[[NSString stringWithString:@"OsiriXExporter:"] stringByAppendingString:operationName]];
	

	if([self prepareInputImages])
	{
		//try to connect to MeVisLab
		NSString* operationName=[NSString stringWithString:@"OsiriXImporter:"];
		operationName=[operationName stringByAppendingString:[operationConfigDict valueForKey:@"OperationName"]];
		if(![bridgeExportToMeVisLab connectToRemoteObjectRegisteredAs:operationName])
		{
			NSString* commandLineString = [operationConfigDict valueForKey:@"CommandLineStr"];
			[self runCommandLineForOperation:commandLineString];
			int sleepTime=0;
			do{
				sleep(3);
				sleepTime+=3;
			}while(sleepTime<=30 && ([bridgeExportToMeVisLab connectToRemoteObjectRegisteredAs:operationName] == NO));
			if( sleepTime>30 )
			{
				NSRunAlertPanel(@"Failed", @"Failed to run this function, the link to this function is broken." ,nil, nil, nil);
				[self windowWillClose:[NSNotification notificationWithName:NSWindowWillCloseNotification object:[self window]]]; 
				return;
			}
			//				
		}
		

		NSNumber* needshowMeVisLabGUI = [operationConfigDict valueForKey:@"IfShowMevisWindow"];
		if(![needshowMeVisLabGUI boolValue])
		{
			NSRect screenrect=[[[osrixViewController window] screen] visibleFrame];
			[[self window]setFrame:screenrect display:NO animate:NO];
			[[self window] setLevel:NSFloatingWindowLevel];
			[[self window] setDelegate:self];
			[self showWindow:self];
			[self initializeToolsBar:operationConfigDict];
			
			if(![self initializeParameters:operationConfigDict])
			{
				NSRunAlertPanel(@"Failed", @"Failed to intialize parameters in MeVisLab" ,nil, nil, nil);
			}
			if(![self prepareOutputImages])
			{
				NSRunAlertPanel(@"Failed", @"Failed to run this function, the connection was established, but cannot create output Images. Check MeVisLab log for more information" ,nil, nil, nil);
				return;
			}
			
			//[[NSApplication sharedApplication] runModalForWindow:[self window]];
		}
		else
		{
			
			[warningWindow makeKeyAndOrderFront:self];
			[warningWindow display];
			//[[NSApplication sharedApplication] runModalForWindow:warningWindow]; 
			
			NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
			[parameters setObject:[NSNumber numberWithBool:YES] forKey:@"ShowMeVisLab"];
			[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
			
			[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(applicationWillResignActive:) name:NSApplicationWillResignActiveNotification object:nil];
			[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(applicationWillBecomeActive:) name:NSApplicationWillBecomeActiveNotification object:nil];
			 
		}
		
		
	}
	else
	{
		NSRunAlertPanel(@"Failed", @"Failed to create shared memory, you may run out of memory. Use the VOI cutter plugin to reduce the volume and try again." ,nil, nil, nil);
	}
	
	
	
	return;
	
}
- (void)applicationWillResignActive:(NSNotification *)aNotification
{
	[warningWindow setLevel:NSNormalWindowLevel];
}
- (void)applicationWillBecomeActive:(NSNotification *)aNotification
{
	[warningWindow setLevel:NSFloatingWindowLevel];
}
- (void)windowWillClose:(NSNotification *)notification
{
	//cut connection
	//release shared memory here
	NSWindow* resignedWindow=[notification object];
	if(resignedWindow==[self window] || resignedWindow==warningWindow)
	{
				
		//seeding tools

		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"roiChange"object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"addROI"object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"removeROI"object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSApplicationWillResignActiveNotification object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSApplicationWillBecomeActiveNotification object:nil];

		
		[seedingToolsConfigList release];
		seedingToolsConfigList=nil;
		[resultROIListCorrespondingToSeedList release];
		resultROIListCorrespondingToSeedList=nil;
		
		[bridgeExportToMeVisLab prepareToDealloc];
		[bridgeExportToMeVisLab release];
		bridgeExportToMeVisLab=nil;
		[bridgeImportFromMeVisLab prepareToDealloc];
		[bridgeImportFromMeVisLab release];
		bridgeImportFromMeVisLab=nil;
		if(resignedWindow==[self window])
		{
			[inputImageView setDrawing:NO];
			[inputImageView setDCM:[NSArray array]:[NSArray array] :[NSArray array] :0 :'i' :NO];
			[outputImageView setDrawing:NO];
			[outputImageView setDCM:[NSArray array]:[NSArray array] :[NSArray array] :0 :'i' :NO];

		}
		[parametersConfigList release];
		parametersConfigList=nil;
		
		
	}
}
-(void)runCommandLineForOperation:(NSString*)commandLineStr
{

	NSMutableArray* commandLineComponents=[[commandLineStr componentsSeparatedByString:@" "] mutableCopy];
	if([commandLineComponents count]==0)
		return;
	NSString* lunchPath=[commandLineComponents objectAtIndex:0];
	NSString* pluginResourcePath=[configDelgate pluginResourcePath];
	pluginResourcePath=[pluginResourcePath stringByAppendingString:@"/"];
	[commandLineComponents removeObjectAtIndex:0];
	if([lunchPath length]==0)
	{
		if([commandLineComponents count]==0)
			return;
		lunchPath=[commandLineComponents objectAtIndex:0];
		[commandLineComponents removeObjectAtIndex:0];
	}
	NSMutableArray* argumentsArray=[NSMutableArray arrayWithCapacity:[commandLineComponents count]];
	for(NSString* argument in commandLineComponents)
	{
		argument=[argument stringByReplacingOccurrencesOfString:@"$pluginfolder$" withString:pluginResourcePath];
		[argumentsArray addObject:argument];
	}
	meVisLabTask = [[NSTask alloc] init];
	
	[meVisLabTask setArguments: argumentsArray];
	
	if( [[NSFileManager defaultManager] fileExistsAtPath: lunchPath])
	{
		[meVisLabTask setLaunchPath: lunchPath];
		[meVisLabTask launch];
		//[theTask waitUntilExit];
	}
	
//	[theTask release];
}
-(void)showParameterControls:(NSManagedObject*) configDict
{
	
}
-(IBAction)endWarningWindow:(id)sender
{

	[warningWindow performClose:sender];

}
-(BOOL)prepareInputImages
{

	int dimension[4]={0,0,0,0};
	float spacing[4]={1.0,1.0,1.0,1.0};
	float transformmatrix[16]={ 1,0,0,0,
								0,1,0,0,
								0,0,1,0,
								0,0,0,1};
	float maxvalue=1, minvalue=0;

	
	// Input Image 1: the image in the OsiriX Viewer
	NSMutableDictionary* inputImage0=[NSMutableDictionary dictionaryWithCapacity:0];

	[inputImage0 setObject:[NSString stringWithString:@"InputImage0"] forKey:@"Description"];
	[inputImage0 setObject:[NSString stringWithString:@"float"] forKey:@"ImageType"];
	
	NSArray				*pixList = [osrixViewController pixList];
	
	float* volumePtr=[osrixViewController volumePtr:0];
	DCMPix* curPix = [pixList objectAtIndex: 0];
	NSManagedObject	*curImage = [[osrixViewController fileList] objectAtIndex:0];
	inputImageSeriesUID=[curImage valueForKeyPath: @"series.seriesInstanceUID"];
	
	
	dimension[0] = [curPix pwidth];
	dimension[1] = [curPix pheight];
	dimension[2] = [pixList count];	
	dimension[3] = [osrixViewController maxMovieIndex];
	spacing[0]=[curPix pixelSpacingX];
	spacing[1]=[curPix pixelSpacingY];
	float sliceThickness = [curPix sliceInterval];   
	if( sliceThickness == 0)
	{
		NSLog(@"Slice interval = slice thickness!");
		sliceThickness = [curPix sliceThickness];
	}	
	spacing[2]=sliceThickness;
	
	int i;
	float vectors[9];
	[curPix orientation:vectors];
	for(i=0;i<3;i++)
	{
		transformmatrix[0+i]=vectors[i*3]*spacing[i];
		transformmatrix[4+i]=vectors[i*3+1]*spacing[i];
		transformmatrix[8+i]=vectors[i*3+2]*spacing[i];
		transformmatrix[12+i]=0;
	}
	transformmatrix[3]=[curPix originX];
	transformmatrix[7]=[curPix originY];
	transformmatrix[11]=[curPix originZ];
	transformmatrix[15]=1;
	

	

	maxvalue = [curPix maxValueOfSeries];
	minvalue = [curPix minValueOfSeries];
	


	NSMutableArray* dimensionarray=[NSMutableArray arrayWithCapacity:4];
	for(i=0;i<4;i++)
		[dimensionarray addObject:[NSNumber numberWithInt:dimension[i]]];
	[inputImage0 setObject:dimensionarray forKey:@"Dimension"];
	
	
	NSMutableArray* spacingarray=[NSMutableArray arrayWithCapacity:4];
	for(i=0;i<4;i++)
		[spacingarray addObject:[NSNumber numberWithFloat:spacing[i]]];
	[inputImage0 setObject:spacingarray forKey:@"Spacing"];
	
	NSMutableArray* matrix=[NSMutableArray arrayWithCapacity:16];;
	for(i=0;i<16;i++)
		[matrix addObject:[NSNumber numberWithFloat:transformmatrix[i]]];
	[inputImage0 setObject:matrix forKey:@"MatrixToPatientCo"];
	
	[inputImage0 setObject:[NSNumber numberWithFloat:maxvalue] forKey:@"Maximum"];
	[inputImage0 setObject:[NSNumber numberWithFloat:minvalue] forKey:@"Minimum"];
	
	long size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(float);
	NSData* volumeData=[NSData dataWithBytesNoCopy:volumePtr length:size freeWhenDone:NO];
	[inputImage0 setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
	[inputImage0 setObject:volumeData forKey:@"Data"];

	if(![bridgeExportToMeVisLab initializeAnImageForSharing:inputImage0])
		return NO;
	//Update Input View
	
	NSData	*newData = [inputImage0 objectForKey:@"Data"];
	if(newData)
	{
		float* newDataPtr=(float*)[newData bytes];
		NSMutableArray	*newPixList = [NSMutableArray arrayWithCapacity: 0];
		NSMutableArray	*newDcmList = [NSMutableArray arrayWithCapacity: 0];
		NSMutableArray  *newROIList = [NSMutableArray arrayWithCapacity: 0];
		pixList = [osrixViewController pixList];
		curPix = [pixList objectAtIndex: 0];

		unsigned j;
		for( j = 0; j < [pixList count]; j++)
		{
			curPix = [pixList objectAtIndex: j];
			DCMPix	*copyPix = [curPix copy];
			[copyPix setfImage: (float*) (newDataPtr + j* dimension[0]* dimension[1])];
			[newPixList addObject: copyPix];
			[copyPix release];
			[newDcmList addObject: [[osrixViewController fileList] objectAtIndex: j]];
			[newROIList addObject:[NSMutableArray arrayWithCapacity:0]];
	
		}
		[inputImageView setStringID: @"MeVisHubInput"];
		[inputImageView setDCM:newPixList :newDcmList :newROIList :0 :'i' :YES];
		[inputImageView setDrawing:YES];
		[inputImageSlider setNumberOfTickMarks:[pixList count]];
		[inputImageSlider setMaxValue:[pixList count]-1];
		[inputImageSlider setIntValue:[pixList count]/2];
		[inputImageView setIndexWithReset: [pixList count]/2 :YES];
		[inputImageView setOrigin: NSMakePoint(0,0)];
		[inputImageView setCurrentTool:tWL];
		[inputImageView scaleToFit];	
		
		
		[[osrixViewController window] performClose:self];
		// Close the window to release more memory for rest steps.  This is not necessary. However it will become more complicated if the viewer will still running when the plugin closed, because the shared memory should be released but the plugin.

		
		//[osrixViewController replaceSeriesWith:newPixList :newDcmList :newData];
    
		
		
		
		
	}
	//InputMask1
	NSArray* roiList=[osrixViewController roiList];
	//check if contains Brush ROI
	BOOL brushROIfound=NO;
	NSMutableArray* roiNames=[NSMutableArray arrayWithCapacity:0];
	unsigned j;
	for(i=0;i<(signed)[roiList count];i++)
		for(j=0;j<[[roiList objectAtIndex:i] count];j++)
		{
			ROI* aROI=[[roiList objectAtIndex:i] objectAtIndex:j];
			if([aROI type]==tPlain)
			{
				brushROIfound=YES;
				BOOL isInNameList=NO;
				for(NSString* aname in roiNames)
				{
					if([aname isEqualToString:[aROI name]])
					   isInNameList=YES;
				}
				if(!isInNameList)
					[roiNames addObject:[aROI name]];
					   
			}
			
		}
	if(brushROIfound)
	{
		NSMutableDictionary* inputMask0=[NSMutableDictionary dictionaryWithCapacity:0];
		
		[inputMask0 setObject:[NSString stringWithString:@"InputImage1"] forKey:@"Description"];
		[inputMask0 setObject:[NSString stringWithString:@"char"] forKey:@"ImageType"];
		
		NSMutableArray* dimensionarray=[NSMutableArray arrayWithCapacity:4];
		for(i=0;i<4;i++)
			[dimensionarray addObject:[NSNumber numberWithInt:dimension[i]]];
		[inputMask0 setObject:dimensionarray forKey:@"Dimension"];
		
		
		NSMutableArray* spacingarray=[NSMutableArray arrayWithCapacity:4];
		for(i=0;i<4;i++)
			[spacingarray addObject:[NSNumber numberWithFloat:spacing[i]]];
		[inputMask0 setObject:spacingarray forKey:@"Spacing"];
		
		NSMutableArray* matrix=[NSMutableArray arrayWithCapacity:16];;
		for(i=0;i<16;i++)
			[matrix addObject:[NSNumber numberWithFloat:transformmatrix[i]]];
		[inputMask0 setObject:matrix forKey:@"MatrixToPatientCo"];
		
		[inputMask0 setObject:[NSNumber numberWithInt:[roiNames count]] forKey:@"Maximum"];
		[inputMask0 setObject:[NSNumber numberWithInt:0] forKey:@"Minimum"];
		
		size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(char);
		[inputMask0 setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
		if(![bridgeExportToMeVisLab initializeAnImageForSharing:inputMask0])
			return NO;
		NSData	*newData = [inputMask0 objectForKey:@"Data"];
		if(newData)
		{
			unsigned char* newDataPtr=(unsigned char*)[newData bytes];
			memset(newDataPtr,0x00,size);
			unsigned short tagValue;
			//set tag value
			int x,y,x1,x2,y1,y2,textureWidth;
			unsigned char * texture;
			unsigned k;
			for(i=0;i<[roiList count];i++)
				for(j=0;j<[[roiList objectAtIndex:i] count];j++)
				{
					ROI* tempROI = [[roiList objectAtIndex: i] objectAtIndex:j];
					
					if([tempROI type]==tPlain)
						for(k=0;k<[roiNames count];k++)
							if ([[tempROI name] isEqualToString:[roiNames objectAtIndex: k]]==YES)
							{
								x1 = [tempROI textureUpLeftCornerX];
								y1 = [tempROI textureUpLeftCornerY];
								x2 = [tempROI textureDownRightCornerX];
								y2 = [tempROI textureDownRightCornerY];
								textureWidth = [tempROI textureWidth];
								texture = [tempROI textureBuffer];
								tagValue = k+1;
								for(y=y1;y<=y2;y++)
									for(x=x1;x<=x2;x++)
									{
										if(*(texture+(y-y1)*textureWidth+x-x1))
											*(newDataPtr+dimension[0]*dimension[1]*i+dimension[0]*y+x)+=tagValue;
										
									}

							}
					
					
				}
			
		}
		
	}

	//InputImage2
	//InputMask2
	
	
	return YES;
	
}
-(BOOL)initializeParameters:(NSManagedObject*)configDict
{
	isInitializing=YES;
	if(parametersConfigList)
		[parametersConfigList release];
	parametersConfigList = [[[configDict valueForKey:@"Parameters"] allObjects] retain];
	
	[para0NameTextField setHidden:YES];
	[para0ValueTextField setHidden:YES];
	[para0Slider setHidden:YES];
	[para1NameTextField setHidden:YES];
	[para1ValueTextField setHidden:YES];
	[para1Slider setHidden:YES];
	[para2NameTextField setHidden:YES];
	[para2ValueTextField setHidden:YES];
	[para2Slider setHidden:YES];
	[para3NameTextField setHidden:YES];
	[para3ValueTextField setHidden:YES];
	[para3Slider setHidden:YES];
	[para4NameTextField setHidden:YES];
	[para4ValueTextField setHidden:YES];
	[para4Slider setHidden:YES];
	[para5NameTextField setHidden:YES];
	[para5ValueTextField setHidden:YES];
	[para5Slider setHidden:YES];
	unsigned i=0;
	
	for(NSManagedObject* aparameter in parametersConfigList)
	{
		if(i<6)
		{
			switch (i) {
				case 0:
					[para0NameTextField setHidden:NO];
					[para0ValueTextField setHidden:NO];
					[para0Slider setHidden:NO];
					[para0NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					NSNumber* maxValue=[aparameter valueForKey:@"MaxValue"];
					[para0Slider setMaxValue:[maxValue doubleValue]];
					NSNumber* minValue=[aparameter valueForKey:@"MinValue"];
					[para0Slider setMinValue:[minValue doubleValue]];
					NSNumber* defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para0Slider setDoubleValue:[defaultValue doubleValue]];
					[para0ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para0Slider setContinuous:YES];
					}
					else
					{
						[para0Slider setContinuous:NO];
					}
						
					break;
				case 1:
					[para1NameTextField setHidden:NO];
					[para1ValueTextField setHidden:NO];
					[para1Slider setHidden:NO];
					[para1NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					maxValue=[aparameter valueForKey:@"MaxValue"];
					[para1Slider setMaxValue:[maxValue doubleValue]];
					minValue=[aparameter valueForKey:@"MinValue"];
					[para1Slider setMinValue:[minValue doubleValue]];
					defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para1Slider setDoubleValue:[defaultValue doubleValue]];
					[para1ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para1Slider setContinuous:YES];
					}
					else
					{
						[para1Slider setContinuous:NO];
					}
					
					break;
				case 2:
					[para2NameTextField setHidden:NO];
					[para2ValueTextField setHidden:NO];
					[para2Slider setHidden:NO];
					[para2NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					maxValue=[aparameter valueForKey:@"MaxValue"];
					[para2Slider setMaxValue:[maxValue doubleValue]];
					minValue=[aparameter valueForKey:@"MinValue"];
					[para2Slider setMinValue:[minValue doubleValue]];
					defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para2Slider setDoubleValue:[defaultValue doubleValue]];
					[para2ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para2Slider setContinuous:YES];
					}
					else
					{
						[para2Slider setContinuous:NO];
					}
					
					break;
				case 3:
					[para3NameTextField setHidden:NO];
					[para3ValueTextField setHidden:NO];
					[para3Slider setHidden:NO];
					[para3NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					maxValue=[aparameter valueForKey:@"MaxValue"];
					[para3Slider setMaxValue:[maxValue doubleValue]];
					minValue=[aparameter valueForKey:@"MinValue"];
					[para3Slider setMinValue:[minValue doubleValue]];
					defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para3Slider setDoubleValue:[defaultValue doubleValue]];
					[para3ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para3Slider setContinuous:YES];
					}
					else
					{
						[para3Slider setContinuous:NO];
					}
					
					break;
				case 4:
					[para4NameTextField setHidden:NO];
					[para4ValueTextField setHidden:NO];
					[para4Slider setHidden:NO];
					[para4NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					maxValue=[aparameter valueForKey:@"MaxValue"];
					[para4Slider setMaxValue:[maxValue doubleValue]];
					minValue=[aparameter valueForKey:@"MinValue"];
					[para4Slider setMinValue:[minValue doubleValue]];
					defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para4Slider setDoubleValue:[defaultValue doubleValue]];
					[para4ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para4Slider setContinuous:YES];
					}
					else
					{
						[para4Slider setContinuous:NO];
					}
					
					break;
				case 5:
					[para5NameTextField setHidden:NO];
					[para5ValueTextField setHidden:NO];
					[para5Slider setHidden:NO];
					[para5NameTextField setStringValue:[aparameter valueForKey:@"ParaName"]];
					maxValue=[aparameter valueForKey:@"MaxValue"];
					[para5Slider setMaxValue:[maxValue doubleValue]];
					minValue=[aparameter valueForKey:@"MinValue"];
					[para5Slider setMinValue:[minValue doubleValue]];
					defaultValue=[aparameter valueForKey:@"DefaultValue"];
					[para5Slider setDoubleValue:[defaultValue doubleValue]];
					[para5ValueTextField setDoubleValue:[defaultValue doubleValue]];
					if([[aparameter valueForKey:@"UpdateInRealTime"] boolValue]==YES)
					{
						[para5Slider setContinuous:YES];
					}
					else
					{
						[para5Slider setContinuous:NO];
					}
					
					break;


				default:
					break;
			}
		}
		i++;
	}
	
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	if(![para0NameTextField isHidden])
	{
		[parameters setObject:[para0ValueTextField stringValue] forKey:[para0NameTextField stringValue]];
	}
	if(![para1NameTextField isHidden])
	{
		[parameters setObject:[para1ValueTextField stringValue] forKey:[para1NameTextField stringValue]];
	}
	if(![para2NameTextField isHidden])
	{
		[parameters setObject:[para2ValueTextField stringValue] forKey:[para2NameTextField stringValue]];
	}
	if(![para3NameTextField isHidden])
	{
		[parameters setObject:[para3ValueTextField stringValue] forKey:[para3NameTextField stringValue]];
	}
	if(![para4NameTextField isHidden])
	{
		[parameters setObject:[para4ValueTextField stringValue] forKey:[para4NameTextField stringValue]];
	}
	if(![para5NameTextField isHidden])
	{
		[parameters setObject:[para5ValueTextField stringValue] forKey:[para5NameTextField stringValue]];
	}
	[parameters setObject:[NSNumber numberWithBool:YES] forKey:@"ImageUpdated"];
	[parameters setObject:inputImageSeriesUID forKey:@"SeriesUID"];
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
	isInitializing=NO;	
	return YES;
	
}
-(void)initializeToolsBar:(NSManagedObject*)configDict
{
	if(seedingToolsConfigList)
		[seedingToolsConfigList release];

	while([toolsBar numberOfColumns]>5)
		[toolsBar removeColumn:5];

	[toolsBar setPrototype:[toolsBar cellAtRow:0 column:1]];
	[toolsBar selectCellAtRow:0 column:0];
	if([[configDict valueForKey:@"IfSupportSeedsPlanting"] boolValue]==NO)
		return;
		
	seedingToolsConfigList = [[[configDict valueForKey:@"ObjectsList"] allObjects] retain];
	for(NSManagedObject* aSeedTool in seedingToolsConfigList)
	{
		[toolsBar addColumn];
		[toolsBar sizeToCells];
		NSButtonCell *cell = [toolsBar cellAtRow: 0 column:[toolsBar numberOfColumns]-1];
		[cell setRepresentedObject: aSeedTool];
		NSString* toolsName=[aSeedTool valueForKey:@"ToolsToMark"];
		NSImage* iconImage=[NSImage imageNamed:toolsName];
		[cell setImage:iconImage];
		[cell setImagePosition: NSImageAbove];
		
	//	NSColor *txtColor = [self colorByGivenName:[aSeedTool valueForKey:@"ObjectColor"]];
//		NSFont *txtFont = [NSFont boldSystemFontOfSize:14];
//		NSDictionary *txtDict = [NSDictionary dictionaryWithObjectsAndKeys:
//								 txtFont, NSFontAttributeName, txtColor, NSForegroundColorAttributeName, nil];
//		NSAttributedString *attrStr = [[[NSAttributedString alloc] initWithString:[aSeedTool valueForKey:@"ObjectName"] attributes:txtDict] autorelease];
//		[cell setAttributedStringValue:attrStr];
//
		
		[cell setTitle:[aSeedTool valueForKey:@"ObjectName"]];
		[cell setTag:1001];
		[cell setTarget:self];
		[cell setAction:@selector(changCurrentTool:)];
		[cell setState: NSOffState];
		
	}
	if([seedingToolsConfigList count])
	{
		NSNotificationCenter *nc;
		nc = [NSNotificationCenter defaultCenter];
		[nc addObserver: self selector: @selector(roiChanged:) name:@"roiChange" object:nil];
		[nc addObserver: self selector: @selector(roiAdded:) name:@"addROI" object:nil];
		[nc addObserver: self selector: @selector(roiRemoved:) name:@"removeROI" object:nil];
	}
	[toolsBar selectCellAtRow:0 column:0];
	return;
	
	
}

-(NSColor*)colorByGivenName:(NSString*)name
{
	if([name isEqualToString:@"Blue"])
		return [NSColor blueColor];
	if([name isEqualToString:@"Brown"])
		return [NSColor brownColor];
	if([name isEqualToString:@"Cyan"])
		return [NSColor cyanColor];
	if([name isEqualToString:@"Green"])
		return [NSColor greenColor];
	if([name isEqualToString:@"Magenta"])
		return [NSColor magentaColor];
	if([name isEqualToString:@"Orange"])
		return [NSColor orangeColor];
	if([name isEqualToString:@"Purple"])
		return [NSColor purpleColor];
	if([name isEqualToString:@"Red"])
		return [NSColor redColor];
	if([name isEqualToString:@"Yellow"])
		return [NSColor yellowColor];
	return [NSColor whiteColor];
}
- (void) roiAdded: (NSNotification*) note
{
	id sender =[note object];
	
	
	if( sender&&(CurrentToolID==1001))
	{
		if ([sender isEqual:inputImageView])
		{
			
			ROI * roi = [[note userInfo] objectForKey:@"ROI"];
			if(roi)
			{
				int roitype =[roi type];
				if(roitype==t2DPoint)
				{
					
					[roi setName: [currentSeedingTool valueForKey:@"ObjectName"]];
					RGBColor c;
					NSColor* currentSeedColor=[self colorByGivenName:[currentSeedingTool valueForKey:@"ObjectColor"]];
					CGFloat r, g, b;
					[currentSeedColor getRed:&r green:&g blue:&b alpha:0L];
					
					
					
					c.red =(short unsigned int) (r * 65535.);
					c.green =(short unsigned int)( g * 65535.);
					c.blue = (short unsigned int)(b * 65535.);
					
					[roi setColor:c];
					
					
					if([[currentSeedingTool valueForKey:@"UpdateInRealtime"] boolValue])
					{
						[self notifyMeVisLabSeedsChanged];
						[self prepareOutputImages];
					}
				}
			}
		}
	}
}

- (void) roiChanged: (NSNotification*) note
{
	id sender =[note object];
	
	
	if( sender&&(CurrentToolID==1001))
	{
		if ([sender isEqual:inputImageView])
		{
			
			ROI * roi = [[note userInfo] objectForKey:@"ROI"];
			if(roi)
			{
				int roitype =[roi type];
				if(roitype!=t3Dpoint)
				{
					
					if([[currentSeedingTool valueForKey:@"UpdateInRealtime"] boolValue])
					{
						[self notifyMeVisLabSeedsChanged];
						[self prepareOutputImages];
					}
				}
			}
		}
	}
	
}
- (void) roiRemoved: (NSNotification*) note
{
	id sender =[note object];
	
	
	if( sender&&(CurrentToolID==1001))
	{
		if ([sender isEqual:inputImageView])
		{
			
			ROI * roi = [[note userInfo] objectForKey:@"ROI"];
			if(roi)
			{
				int roitype =[roi type];
				if(roitype!=t3Dpoint)
				{
					
					if([[currentSeedingTool valueForKey:@"UpdateInRealtime"] boolValue])
					{
						[self notifyMeVisLabSeedsChanged];
						[self prepareOutputImages];
					}
				}
			}
		}
	}
	
}
//- (void)prepareNewResultROI:(int)index
//{
//	if(resultROIListCorrespondingToSeedList)
//		[resultROIListCorrespondingToSeedList release];
//	ROI* tempROI;
//	RGBColor	color;
//	NSString *roiName;
//	unsigned char *textureBuffer=(unsigned char *) malloc(sizeof(unsigned char)*imageSize);
//	*textureBuffer=0xff;
//	*(textureBuffer+imageSize-1)=0xff;
//	
//	tempROI = [choosenSeedsArray objectAtIndex:index];
//	roiName = [tempROI name];
//	
//	
//	color= [tempROI rgbcolor];
//	
//	DCMPix* curPix;	
//	curPix = [resultPixList objectAtIndex: 0];
//	
//	ROI *newROI=[[ROI alloc] initWithTexture:textureBuffer textWidth:imageWidth textHeight:imageHeight textName:roiName positionX:0 positionY:0 spacingX:[curPix pixelSpacingX] spacingY:[curPix pixelSpacingY]  imageOrigin:NSMakePoint( [curPix originX], [curPix originY])];
//	
//	//	[newROI reduceTextureIfPossible];
//	
//	
//	
//	[newROI setColor:color];
//	
//	[resultPrivateROIList insertObject:newROI atIndex:index];
//	[newROI release];
//	
//	free(textureBuffer);
//	
//}
-(BOOL)prepareOutputImages
{
	int dimension[4]={0,0,0,0};
	float spacing[4]={1.0,1.0,1.0,1.0};
	float transformmatrix[16]={ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
	0,0,0,1};
	NSMutableArray  *inputPixList=[inputImageView dcmPixList];
	NSArray  *inputDcmList=[inputImageView dcmFilesList];
	NSMutableArray	*newPixList=[NSMutableArray arrayWithCapacity: 0];
	NSMutableArray	*newDcmList=[NSMutableArray arrayWithCapacity: 0];
	NSMutableArray	*newROIList=[NSMutableArray arrayWithCapacity: 0];
	BOOL ifOutputImage1Available=NO;
	BOOL ifOutputMask1Available=NO;
	
	NSDictionary* outputImage1=[bridgeImportFromMeVisLab getImageFromLowerBridge:@"OutputImage0"];
	if(outputImage1)
	{
		int i;
		NSString* imgtypestr=[outputImage1 objectForKey:@"ImageType"];
		if([imgtypestr isEqualToString:@"float"])
		{

			NSArray* dimensionarray=[outputImage1 objectForKey:@"Dimension"];
			for(i=0;i<4;i++)
				dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
			
			NSArray* spacingarray=[outputImage1 objectForKey:@"Spacing"];
			for(i=0;i<4;i++)
				spacing[i]=[[spacingarray objectAtIndex:i] floatValue];
			
			NSArray* matrix=[outputImage1 objectForKey:@"MatrixToPatientCo"];
			for(i=0;i<16;i++)
				transformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
			float vector[9];
			for(i=0;i<3;i++)
			{
				vector[i]=transformmatrix[i*4]/spacing[0];
				vector[i+3]=transformmatrix[i*4+1]/spacing[1];
				vector[i+6]=transformmatrix[i*4+2]/spacing[2];
			}
			NSData	*newData = [outputImage1 objectForKey:@"Data"];
			if(newData)
			{
				ifOutputImage1Available=YES;
				float* newDataPtr=(float*)[newData bytes];
				for( i = 0 ; i < dimension[2]; i ++)
				{
					DCMPix	*copyPix = [[inputPixList objectAtIndex: 0] copy];
					float origin[3];
					int j;
					for(j=0;j<3;j++)
						origin[j]=i*transformmatrix[j*4+2]+transformmatrix[j*4+3];
					
					[copyPix setPwidth: dimension[0]];
					[copyPix setPheight: dimension[1]];
					[copyPix setPixelSpacingX:spacing[0]];
					[copyPix setPixelSpacingY:spacing[1]];
					[copyPix setOrigin:origin];
					[copyPix setOrientation: vector];
					[copyPix setfImage: (float*) (newDataPtr + dimension[0]* dimension[1]* i)];
					[copyPix setTot: dimension[2]];
					[copyPix setFrameNo: 0];
					[copyPix setID: i];
					
					[newPixList addObject: copyPix];
					[copyPix release];
					[newDcmList addObject: [inputDcmList objectAtIndex: 0]];
				}
			}
			
		}


	}
	if(ifOutputImage1Available==NO)
	{
		[newPixList addObjectsFromArray:inputPixList];
		[newDcmList addObjectsFromArray:inputDcmList];
		
		
	}
	unsigned j;
	for( j = 0; j < [newPixList count]; j++)
	{
		[newROIList addObject:[NSMutableArray arrayWithCapacity:0]];
	}
	
	[outputImageView setStringID: @"MeVisHubOutput"];
	[outputImageView setDCM:newPixList :newDcmList :newROIList :0 :'i' :YES];
	[outputImageView setDrawing:YES];
	[outputImageSlider setNumberOfTickMarks:[newPixList count]];
	[outputImageSlider setMaxValue:[newPixList count]-1];
	[outputImageSlider setIntValue:[newPixList count]/2];
	[outputImageView setIndexWithReset: [newPixList count]/2 :YES];
	[outputImageView setOrigin: NSMakePoint(0,0)];
	[outputImageView setCurrentTool:tWL];
	[outputImageView scaleToFit];	
	
	
	//Output Mask 1
	NSDictionary* outputMask1=[bridgeImportFromMeVisLab getImageFromLowerBridge:@"OutputImage1"];
	outputMaskResult=outputMask1;
	if(outputMask1)
	{
		ifOutputMask1Available=YES;
		[self initializeOutputViewROI];
	}

	currentSychronizeMode=[self checkSynchronizeMode];
	[outputImageSlider setIntValue:0];
	[self scrollImageSynchronically:outputImageSlider];
	
	return (ifOutputMask1Available||ifOutputImage1Available);
}
-(IBAction)scrollImageSynchronically:(id)sender
{
	if(sender==inputImageSlider)
	{
		[inputImageView setIndex: [sender intValue]];
		if(currentSychronizeMode==SYNCMODE_ALLSAME)
		{
			[outputImageView setIndex: [sender intValue]];
			[outputImageSlider setIntValue:[sender intValue]];
		}
		
	}
	else if (sender==outputImageSlider)
	{
		[outputImageView setIndex: [sender intValue]];
		if(currentSychronizeMode==SYNCMODE_ALLSAME)
		{
			[inputImageView setIndex: [sender intValue]];
			[inputImageSlider setIntValue:[sender intValue]];
		}

	}
	if(outputMaskResult)
		[self updateMasksForOutputView:[sender intValue]];
}
- (void)initializeOutputViewROI
{
	if(!resultROIListCorrespondingToSeedList)
		resultROIListCorrespondingToSeedList=[[NSMutableArray alloc] initWithCapacity:0];
	[resultROIListCorrespondingToSeedList removeAllObjects];
	DCMPix* curPix;	
	curPix = [[outputImageView dcmPixList] objectAtIndex: 0];
	int dimension[2];
	dimension[0] = [curPix pwidth];
	dimension[1] = [curPix pheight];	
	int imageSize=dimension[0]*dimension[1];
	

	RGBColor	c;
	NSString *roiName;
	unsigned char *textureBuffer=(unsigned char *) malloc(sizeof(unsigned char)*imageSize);
	*textureBuffer=0xff;
	*(textureBuffer+imageSize-1)=0xff;
	unsigned i;
	for(i=0;i<[seedingToolsConfigList count];i++)
	{
		roiName=[[seedingToolsConfigList objectAtIndex:i] valueForKey:@"ObjectName"];
		
		NSColor* currentSeedColor=[self colorByGivenName:[[seedingToolsConfigList objectAtIndex:i] valueForKey:@"ObjectColor"]];
		CGFloat r, g, b;
		[currentSeedColor getRed:&r green:&g blue:&b alpha:0L];
		
		
		
		c.red =(short unsigned int) (r * 65535.);
		c.green =(short unsigned int)( g * 65535.);
		c.blue = (short unsigned int)(b * 65535.);

		
		ROI *newROI=[[ROI alloc] initWithTexture:textureBuffer textWidth:dimension[0] textHeight:dimension[1] textName:roiName positionX:0 positionY:0 spacingX:[curPix pixelSpacingX] spacingY:[curPix pixelSpacingY]  imageOrigin:NSMakePoint( [curPix originX], [curPix originY])];
		
		[newROI setColor:c];
		
		[resultROIListCorrespondingToSeedList addObject:newROI];
		[newROI release];
		
	}
	//for other results that are not in the seeding list
	{
		roiName=[NSString stringWithString:@"other"];
		
		NSColor* currentSeedColor=[NSColor blueColor];
		CGFloat r, g, b;
		[currentSeedColor getRed:&r green:&g blue:&b alpha:0L];
		
		
		
		c.red =(short unsigned int) (r * 65535.);
		c.green =(short unsigned int)( g * 65535.);
		c.blue = (short unsigned int)(b * 65535.);
		
		
		ROI *newROI=[[ROI alloc] initWithTexture:textureBuffer textWidth:dimension[0] textHeight:dimension[1] textName:roiName positionX:0 positionY:0 spacingX:[curPix pixelSpacingX] spacingY:[curPix pixelSpacingY]  imageOrigin:NSMakePoint( [curPix originX], [curPix originY])];
		
		[newROI setColor:c];
		
		[resultROIListCorrespondingToSeedList addObject:newROI];
		[newROI release];
		
	}
	free(textureBuffer);
	
}

-(void)updateMasksForOutputView:(int)index
{
	if(!resultROIListCorrespondingToSeedList ||[resultROIListCorrespondingToSeedList count]==0)
		return;
	float dimension[4],spacing[4],transformmatrix[16],vector[9];
	unsigned int i;
	NSArray* dimensionarray=[outputMaskResult objectForKey:@"Dimension"];
	for(i=0;i<4;i++)
		dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
	
	NSArray* spacingarray=[outputMaskResult objectForKey:@"Spacing"];
	for(i=0;i<4;i++)
		spacing[i]=[[spacingarray objectAtIndex:i] floatValue];
	
	NSArray* matrix=[outputMaskResult objectForKey:@"MatrixToPatientCo"];
	for(i=0;i<16;i++)
		transformmatrix[i]=[[matrix objectAtIndex:i] floatValue];

	for(i=0;i<3;i++)
	{
		vector[i]=transformmatrix[i*4]/spacing[0];
		vector[i+3]=transformmatrix[i*4+1]/spacing[1];
		vector[i+6]=transformmatrix[i*4+2]/spacing[2];
	}
	NSData	*newData = [outputMaskResult objectForKey:@"Data"];
	if(newData)
	{
		unsigned char* maskDataPtr=(unsigned char*)[newData bytes];
		if(currentSychronizeMode==SYNCMODE_ALLSAME)
		{
			int imageSize=dimension[0]*dimension[1];
			
			ROI* tempROI;
			
			int colorIndex,j;
			
			[[[outputImageView dcmRoiList] objectAtIndex: index] removeAllObjects];
			
			unsigned char *textureBuffer;
			i=0;
			for(;i< [seedingToolsConfigList count];i++)
			{

				//ROI in the seeding list
				colorIndex = i+1;
				tempROI = [resultROIListCorrespondingToSeedList objectAtIndex: i];
				textureBuffer = [tempROI textureBuffer];
				memset(textureBuffer,0,sizeof(unsigned char)*imageSize);			
				for(j=0;j<imageSize;j++)
					if(*(maskDataPtr+index*imageSize+j)==colorIndex)
						*(textureBuffer+j) = 0xff;
				*textureBuffer=0xff;
				*(textureBuffer+imageSize-1)=0xff;
				[[[outputImageView dcmRoiList] objectAtIndex: index] addObject: tempROI ];
				
				
			}
			{
				//ROI in the seeding list
				colorIndex = i;
				tempROI = [resultROIListCorrespondingToSeedList objectAtIndex: i];
				textureBuffer = [tempROI textureBuffer];
				memset(textureBuffer,0,sizeof(unsigned char)*imageSize);			
				for(j=0;j<imageSize;j++)
					if(*(maskDataPtr+index*imageSize+j)>colorIndex)
						*(textureBuffer+j) = 0xff;
				*textureBuffer=0xff;
				*(textureBuffer+imageSize-1)=0xff;
				[[[outputImageView dcmRoiList] objectAtIndex: index] addObject: tempROI ];
				
				
			}

		}
		
	}
	
}
-(int)checkSynchronizeMode
{
	DCMPix* inImg=[[inputImageView dcmPixList] objectAtIndex:0];
	DCMPix* outImg=[[outputImageView dcmPixList] objectAtIndex:0];
	float vectorIn[9],vectorOut[9],originIn[3],originOut[3],spacingIn[3],spacingOut[3];
	[inImg orientation:vectorIn];
	[outImg orientation:vectorOut];
	spacingIn[0]=[inImg pixelSpacingX];
	spacingIn[1]=[inImg pixelSpacingY];
	spacingIn[2] = [inImg sliceInterval];   
	if( spacingIn[2] == 0)
	{
		spacingIn[2] = [inImg sliceThickness];
	}	
	
	
	spacingOut[0] = [outImg pixelSpacingX];
	spacingOut[1] = [outImg pixelSpacingY];
	spacingOut[2] = [outImg sliceInterval];   
	if( spacingOut[2] == 0)
	{
		spacingOut[2] = [outImg sliceThickness];
	}	
	
	originIn[0]=[inImg originX];
	originIn[1]=[inImg originY];
	originIn[2]=[inImg originZ];
	
	
	originOut[0]=[outImg originX];
	originOut[1]=[outImg originY];
	originOut[2]=[outImg originZ];
	
	int i;
	for(i=0;i<9;i++)
		if(vectorIn[i]!=vectorOut[i])
		{
			return SYNCMODE_NOSUPPORT;
		}
	if([[inputImageView dcmPixList] count]!=[[outputImageView dcmPixList] count])
		return SYNCMODE_SAMEORIENTATION;
	for(i=0;i<3;i++)
		if(originIn[i]!=originOut[i])
		{
			return SYNCMODE_SAMEORIENTATION;
		}
	for(i=0;i<3;i++)
		if(originIn[i]!=originOut[i])
		{
			return SYNCMODE_SAMEORIENTATION;
		}
	
	for(i=0;i<3;i++)
		if(spacingIn[i]!=spacingOut[i])
		{
			return SYNCMODE_SAMEORIENTATION;
		}
	
	return SYNCMODE_ALLSAME;

}
-(IBAction)changeParameter:(id)sender
{
	if(isInitializing)
		return;
	int tag=[sender tag];
	NSManagedObject* aparameterConfig=[parametersConfigList objectAtIndex:tag];
	BOOL isInteger=NO;
	if([[aparameterConfig valueForKey:@"Type"] isEqualToString:@"int"])
		isInteger=YES;
	double value;
	NSString* paraName;
	switch (tag) {
		case 0:
			if(isInteger)
			{
				[para0Slider setIntValue:[sender intValue]];
				[para0ValueTextField setIntValue:[sender intValue]];
				value=[para0ValueTextField intValue];
			}
			else
			{
				[para0Slider setDoubleValue:[sender doubleValue]];
				[para0ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para0ValueTextField doubleValue];
			}
			paraName=[para0NameTextField stringValue];
			break;
		case 1:
			if(isInteger)
			{
				[para1Slider setIntValue:[sender intValue]];
				[para1ValueTextField setIntValue:[sender intValue]];
				value=[para1ValueTextField intValue];
			}
			else
			{
				[para1Slider setDoubleValue:[sender doubleValue]];
				[para1ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para1ValueTextField doubleValue];
			}
			paraName=[para1NameTextField stringValue];
			break;
		case 2:
			if(isInteger)
			{
				[para2Slider setIntValue:[sender intValue]];
				[para2ValueTextField setIntValue:[sender intValue]];
				value=[para2ValueTextField intValue];
			}
			else
			{
				[para2Slider setDoubleValue:[sender doubleValue]];
				[para2ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para2ValueTextField doubleValue];
			}
			paraName=[para2NameTextField stringValue];
			break;
		case 3:
			if(isInteger)
			{
				[para3Slider setIntValue:[sender intValue]];
				[para3ValueTextField setIntValue:[sender intValue]];
				value=[para3ValueTextField intValue];
			}
			else
			{
				[para3Slider setDoubleValue:[sender doubleValue]];
				[para3ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para3ValueTextField doubleValue];
			}
			paraName=[para3NameTextField stringValue];
			break;
		case 4:
			if(isInteger)
			{
				[para4Slider setIntValue:[sender intValue]];
				[para4ValueTextField setIntValue:[sender intValue]];
				value=[para4ValueTextField intValue];
			}
			else
			{
				[para4Slider setDoubleValue:[sender doubleValue]];
				[para4ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para4ValueTextField doubleValue];
			}
			paraName=[para4NameTextField stringValue];
			break;
		case 5:
			if(isInteger)
			{
				[para5Slider setIntValue:[sender intValue]];
				[para5ValueTextField setIntValue:[sender intValue]];
				value=[para5ValueTextField intValue];
			}
			else
			{
				[para5Slider setDoubleValue:[sender doubleValue]];
				[para5ValueTextField setDoubleValue:[sender doubleValue]];
				value=[para5ValueTextField doubleValue];
			}
			paraName=[para5NameTextField stringValue];
			break;


		default:
			break;
	}
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	[parameters setObject:[NSNumber numberWithDouble:value] forKey:paraName];
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
	if(![self prepareOutputImages])
	{
		NSRunAlertPanel(@"Failed", @"Cannot create output Images. Check MeVisLab log for more information" ,nil, nil, nil);
	}
	
}
- (IBAction)changCurrentTool:(id)sender
{
	int tag=[[sender selectedCell] tag];
	CurrentToolID=tag;
	if(tag>=0&&tag<4)
	{
		[inputImageView setCurrentTool: tag];
		[outputImageView setCurrentTool: tag];
		
		
	}
	else if(tag==4)
	{
		[inputImageView setCurrentTool: tMesure];
		[outputImageView setCurrentTool: tMesure];

//		[[NSUserDefaults standardUserDefaults] setBool:NO forKey: @"ROITEXTNAMEONLY"];
//		[[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"ROITEXTIFSELECTED"];
	}
	else if(tag==1001)
	{
		currentSeedingTool=[[sender selectedCell] representedObject];
		NSString* toolName=[currentSeedingTool valueForKey:@"ToolsToMark"];
		if([toolName isEqualToString:@"Point"])
		{
			[inputImageView setCurrentTool:t2DPoint];
			[outputImageView setCurrentTool:tWL];
		}
		else
		{
			NSRunAlertPanel(@"NOT SUPPORT", @"This type tools is not supported in this version!" ,nil, nil, nil);

		}
	}
	
}
- (void)prepareInputMarkerImage
{
	NSMutableDictionary* inputImage5=[NSMutableDictionary dictionaryWithCapacity:0];
	
	[inputImage5 setObject:[NSString stringWithString:@"InputImage4"] forKey:@"Description"];
	[inputImage5 setObject:[NSString stringWithString:@"overlay"] forKey:@"ImageType"];
	[inputImage5 setObject:[NSString stringWithString:@"points"] forKey:@"OverlayType"];
	NSArray* roiList=[inputImageView dcmRoiList];
	
	NSMutableArray* markerPoints=[NSMutableArray arrayWithCapacity:0];
	unsigned i,j,k;
	for(i=0;i<[roiList count];i++)
		for(j=0;j<[[roiList objectAtIndex:i] count];j++)
		{
			ROI* aROI=[[roiList objectAtIndex:i] objectAtIndex:j];
			if([aROI type]==t2DPoint)
			{
				NSString* roiName=[aROI name];
				for(k=0;k<[seedingToolsConfigList count];k++)
					if([roiName isEqualToString:[[seedingToolsConfigList objectAtIndex:k] valueForKey:@"ObjectName"]])
					{
						NSRect roiRect=[aROI rect];
						NSDictionary* a3DPoint=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:roiRect.origin.x],@"x",[NSNumber numberWithInt:roiRect.origin.y],@"y",[NSNumber numberWithInt:i],@"z",[NSNumber numberWithInt:k+1],@"value",nil];
						[markerPoints addObject:a3DPoint];
						break;
					}
				
			}
			
		}
	[inputImage5 setObject:markerPoints forKey:@"Points"];
	[bridgeExportToMeVisLab initializeAnImageForSharing:inputImage5];
	
}
- (void)notifyMeVisLabSeedsChanged
{
	[self prepareInputMarkerImage];
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	[parameters setObject:[NSNumber numberWithBool:YES] forKey:@"MarkerUpdated"];
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
}
- (IBAction)updateOutputImage:(id)sender;
{
	[self notifyMeVisLabSeedsChanged];
	[self prepareOutputImages];
}
- (IBAction)closeCurrentWindow:(id)sender
{
	[[self window] setDelegate:self];
	[[self window] performClose:sender];
}
@end
