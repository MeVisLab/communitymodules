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
	ifSupportMemorySharing=[[operationConfigDict valueForKey:@"IfSupportMemorySharing"] boolValue];
	NSString* osiriXImporterName=[[NSString stringWithString:@"OsiriXImporter:"] stringByAppendingString:operationName];
	
	bridgeExportToMeVisLab=[[OsiriX2MeVisLabTBridge alloc] initWithIncommingConnectionName:@"MeVisHub:Exporter" SupportSharedMem:ifSupportMemorySharing];
	[bridgeExportToMeVisLab setWindowController:self];
	
	NSString* osiriXExporterName=[[NSString stringWithString:@"OsiriXExporter:"] stringByAppendingString:operationName];
	bridgeImportFromMeVisLab=[[OsiriX2MeVisLabTBridge alloc] initWithIncommingConnectionName:@"MeVisHub:Importer" SupportSharedMem:ifSupportMemorySharing];
	[bridgeImportFromMeVisLab setWindowController:self];
	
	BOOL isRemoteOperation=NO;

	if([self prepareInputImages])
	{
		//try to connect to MeVisLab
		NSString* commandLineString = [operationConfigDict valueForKey:@"CommandLineStr"];
		if([commandLineString hasPrefix:@"Remote"])
		{
			isRemoteOperation=YES;
			if([bridgeExportToMeVisLab connectToRemoteObjectRegisteredAs:osiriXImporterName]==NO)
				NSRunAlertPanel(@"Failed", @"Failed to run this function, Please make sure the server is running." ,nil, nil, nil);

		}
		else if(![bridgeExportToMeVisLab connectToRemoteObjectRegisteredAs:osiriXImporterName])
		{

				
			[self runCommandLineForOperation:commandLineString];
			int sleepTime=0;
			do{
				sleep(3);
				sleepTime+=3;
			}while(sleepTime<=30 && ([bridgeExportToMeVisLab connectToRemoteObjectRegisteredAs:osiriXImporterName] == NO));
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
			[bridgeImportFromMeVisLab connectToRemoteObjectRegisteredAs:osiriXExporterName];
			NSRect screenrect=[[[osrixViewController window] screen] visibleFrame];
			[[self window]setFrame:screenrect display:NO animate:NO];
			[[self window] setLevel:NSFloatingWindowLevel];
			[[self window] setDelegate:self];
			[inputImageView becomeFirstResponder];
			[self showWindow:self];
			[self initializeToolsBar:operationConfigDict];
			
			if(![self initializeParameters:operationConfigDict])
			{
				NSRunAlertPanel(@"Failed", @"Failed to intialize parameters in MeVisLab" ,nil, nil, nil);
			}
		//	if(![self prepareOutputImages])
//			{
//				NSRunAlertPanel(@"Failed", @"Failed to run this function, the connection was established, but cannot create output Images. Check MeVisLab log for more information" ,nil, nil, nil);
//				return;
//			}
			
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
	
	float maxvalue=1, minvalue=0;

	
	// Input Image 0: the image in the OsiriX Viewer
	NSMutableDictionary* inputImage0=[[NSMutableDictionary alloc] initWithCapacity:0];

	[inputImage0 setObject:[NSString stringWithString:@"InputImage0"] forKey:@"Description"];
	[inputImage0 setObject:[NSString stringWithString:@"float"] forKey:@"ImageType"];
	
	NSArray				*pixList = [osrixViewController pixList];
	

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
	for(i=0;i<16;i++)
		transformmatrix[i]=0;
	transformmatrix[0]=1;
	transformmatrix[5]=1;
	transformmatrix[10]=1;
	transformmatrix[15]=1;
//	transformmatrix[16]={ 1,0,0,0,
//						  0,1,0,0,
//						  0,0,1,0,
//						  0,0,0,1};
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
	NSData* volumeData=[osrixViewController volumeData];
	[inputImage0 setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
	[inputImage0 setObject:volumeData forKey:@"Data"];

	if(![bridgeExportToMeVisLab initializeAnImageForSharing:inputImage0])
		return NO;
	[inputImage0 release];
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
	//InputMask 0
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
		NSMutableDictionary* inputMask0=[[NSMutableDictionary alloc] initWithCapacity:0];
		
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
		[inputMask0 release];
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

	
	//
	//
	//InputImage2: the image in the OsiriX Viewer
	ViewerController*  secondOsrixViewController = [osrixViewController blendingController];
	
	if(secondOsrixViewController==nil)
		return YES;
	
	NSMutableDictionary* inputImage1=[[NSMutableDictionary alloc] initWithCapacity:0];
	
	[inputImage1 setObject:[NSString stringWithString:@"InputImage2"] forKey:@"Description"];
	[inputImage1 setObject:[NSString stringWithString:@"float"] forKey:@"ImageType"];
	
	pixList = [secondOsrixViewController pixList];
	

	curPix = [pixList objectAtIndex: 0];
	curImage = [[secondOsrixViewController fileList] objectAtIndex:0];
	//inputImageSeriesUID=[curImage valueForKeyPath: @"series.seriesInstanceUID"];//input series uid is only need for inputimage0, this uid is sent to MeVislab as a UID for saved intermediate results.
	
	
	dimension[0] = [curPix pwidth];
	dimension[1] = [curPix pheight];
	dimension[2] = [pixList count];	
	dimension[3] = [secondOsrixViewController maxMovieIndex];
	spacing[0]=[curPix pixelSpacingX];
	spacing[1]=[curPix pixelSpacingY];
	sliceThickness = [curPix sliceInterval];   
	if( sliceThickness == 0)
	{
		NSLog(@"Slice interval = slice thickness!");
		sliceThickness = [curPix sliceThickness];
	}	
	spacing[2]=sliceThickness;
	

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
	
	
	
	[dimensionarray removeAllObjects];
	for(i=0;i<4;i++)
		[dimensionarray addObject:[NSNumber numberWithInt:dimension[i]]];
	[inputImage1 setObject:dimensionarray forKey:@"Dimension"];
	
	
	[spacingarray removeAllObjects];
	for(i=0;i<4;i++)
		[spacingarray addObject:[NSNumber numberWithFloat:spacing[i]]];
	[inputImage1 setObject:spacingarray forKey:@"Spacing"];
	
	[matrix removeAllObjects];;
	for(i=0;i<16;i++)
		[matrix addObject:[NSNumber numberWithFloat:transformmatrix[i]]];
	[inputImage1 setObject:matrix forKey:@"MatrixToPatientCo"];
	
	[inputImage1 setObject:[NSNumber numberWithFloat:maxvalue] forKey:@"Maximum"];
	[inputImage1 setObject:[NSNumber numberWithFloat:minvalue] forKey:@"Minimum"];
	
	size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(float);
	volumeData=[secondOsrixViewController volumeData];
	[inputImage1 setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
	[inputImage1 setObject:volumeData forKey:@"Data"];
	
	if(![bridgeExportToMeVisLab initializeAnImageForSharing:inputImage1])
		return NO;
	[inputImage1 release];
	//Update Input View
	
//	newData = [inputImage1 objectForKey:@"Data"];
//	if(newData)
//	{
//		float* newDataPtr=(float*)[newData bytes];
//		NSMutableArray	*newPixList = [NSMutableArray arrayWithCapacity: 0];
//		NSMutableArray	*newDcmList = [NSMutableArray arrayWithCapacity: 0];
//		NSMutableArray  *newROIList = [NSMutableArray arrayWithCapacity: 0];
//		pixList = [secondOsrixViewController pixList];
//		curPix = [pixList objectAtIndex: 0];
//		
//		unsigned j;
//		for( j = 0; j < [pixList count]; j++)
//		{
//			curPix = [pixList objectAtIndex: j];
//			DCMPix	*copyPix = [curPix copy];
//			[copyPix setfImage: (float*) (newDataPtr + j* dimension[0]* dimension[1])];
//			[newPixList addObject: copyPix];
//			[copyPix release];
//			[newDcmList addObject: [[secondOsrixViewController fileList] objectAtIndex: j]];
//			[newROIList addObject:[NSMutableArray arrayWithCapacity:0]];
//			
//		}
//		[inputImageView setStringID: @"MeVisHubInput"];
//		[inputImageView setDCM:newPixList :newDcmList :newROIList :0 :'i' :YES];
//		[inputImageView setDrawing:YES];
//		[inputImageSlider setNumberOfTickMarks:[pixList count]];
//		[inputImageSlider setMaxValue:[pixList count]-1];
//		[inputImageSlider setIntValue:[pixList count]/2];
//		[inputImageView setIndexWithReset: [pixList count]/2 :YES];
//		[inputImageView setOrigin: NSMakePoint(0,0)];
//		[inputImageView setCurrentTool:tWL];
//		[inputImageView scaleToFit];	
//		
//		
		[[secondOsrixViewController window] performClose:self];
		// Close the window to release more memory for rest steps.  This is not necessary. However it will become more complicated if the viewer will still running when the plugin closed, because the shared memory should be released but the plugin.
		
//		
//		//[secondOsrixViewController replaceSeriesWith:newPixList :newDcmList :newData];
//		
//		
//		
//		
//		
//	}
	//InputMask2
	roiList=[secondOsrixViewController roiList];
	//check if contains Brush ROI
	brushROIfound=NO;
	[roiNames removeAllObjects];

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
		NSMutableDictionary* inputMask1=[[NSMutableDictionary alloc] initWithCapacity:0];
		
		[inputMask1 setObject:[NSString stringWithString:@"InputImage3"] forKey:@"Description"];
		[inputMask1 setObject:[NSString stringWithString:@"char"] forKey:@"ImageType"];
		
		NSMutableArray* dimensionarray=[NSMutableArray arrayWithCapacity:4];
		for(i=0;i<4;i++)
			[dimensionarray addObject:[NSNumber numberWithInt:dimension[i]]];
		[inputMask1 setObject:dimensionarray forKey:@"Dimension"];
		
		
		NSMutableArray* spacingarray=[NSMutableArray arrayWithCapacity:4];
		for(i=0;i<4;i++)
			[spacingarray addObject:[NSNumber numberWithFloat:spacing[i]]];
		[inputMask1 setObject:spacingarray forKey:@"Spacing"];
		
		NSMutableArray* matrix=[NSMutableArray arrayWithCapacity:16];;
		for(i=0;i<16;i++)
			[matrix addObject:[NSNumber numberWithFloat:transformmatrix[i]]];
		[inputMask1 setObject:matrix forKey:@"MatrixToPatientCo"];
		
		[inputMask1 setObject:[NSNumber numberWithInt:[roiNames count]] forKey:@"Maximum"];
		[inputMask1 setObject:[NSNumber numberWithInt:0] forKey:@"Minimum"];
		
		size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(char);
		[inputMask1 setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
		if(![bridgeExportToMeVisLab initializeAnImageForSharing:inputMask1])
			return NO;
		[inputMask1	release];
		NSData	*newData = [inputMask1 objectForKey:@"Data"];
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
				updatingROI=YES;

				[roi setName: [currentSeedingTool valueForKey:@"ObjectName"]];
				RGBColor c;
				NSColor* currentSeedColor=[self colorByGivenName:[currentSeedingTool valueForKey:@"ObjectColor"]];
				CGFloat r, g, b;
				[currentSeedColor getRed:&r green:&g blue:&b alpha:0L];
				
				
				
				c.red =(short unsigned int) (r * 65535.);
				c.green =(short unsigned int)( g * 65535.);
				c.blue = (short unsigned int)(b * 65535.);
				
				[roi setColor:c];
				
	//			if([[currentSeedingTool valueForKey:@"UpdateInRealtime"] boolValue])
//				{
//					int roitype =[roi type];
//					
//					if(roitype==t2DPoint)
//						[self notifyMeVisLabSeedsChanged];
//					else if(roitype==tROI)
//						[self notifyMeVisLabCSOChanged];
//					//[self prepareOutputImages];
//				}


				updatingROI=NO;
			}
		}
	}
}

- (void) roiChanged: (NSNotification*) note
{
	id sender =[note object];
	
	if(updatingROI)
		return;
	if( sender&&(CurrentToolID==1001))
	{
		if ([sender isKindOfClass:[ROI class]])
		{
			
			ROI * roi = sender;
			NSArray* curroilist=[[inputImageView dcmRoiList] objectAtIndex:[inputImageView curImage]];
			if(roi&&[curroilist containsObject:roi])
			{
				
				if([[roi name ] isEqualToString:[currentSeedingTool valueForKey:@"ObjectName"]]&&[[currentSeedingTool valueForKey:@"UpdateInRealtime"] boolValue])
				{
					int roitype =[roi type];
					updatingROI=YES;
					NSLog(@"OsriX: ROI changed");
					if(roitype==t2DPoint)
						[self notifyMeVisLabSeedsChanged];
					else if(roitype==tROI)
						[self notifyMeVisLabCSOChanged];
					[self prepareOutputImages];
					NSLog(@"OsriX: Output updated after ROI changed");
					updatingROI=NO;
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
	int i;
	for(i=0;i<16;i++)
		outputTransformmatrix[i]=0;
	outputTransformmatrix[0]=1;
	outputTransformmatrix[5]=1;
	outputTransformmatrix[10]=1;
	outputTransformmatrix[15]=1;
	
//	outputTransformmatrix[16]={ 1,0,0,0,
//								0,1,0,0,
//								0,0,1,0,
//								0,0,0,1};
	NSMutableArray  *inputPixList=[inputImageView dcmPixList];
	NSArray  *inputDcmList=[inputImageView dcmFilesList];
	NSMutableArray	*newPixList=[NSMutableArray arrayWithCapacity: 0];
	NSMutableArray	*newDcmList=[NSMutableArray arrayWithCapacity: 0];
	NSMutableArray	*newROIList=[NSMutableArray arrayWithCapacity: 0];
	BOOL ifOutputImage1Available=NO;
	ifOutputMask1Available=NO;
	
	NSDictionary* outputImage1=[bridgeImportFromMeVisLab getImageFromLowerBridge:@"OutputImage0"];
	if(outputImage1)
	{

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
				outputTransformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
			float vector[9];
			for(i=0;i<3;i++)
			{
				vector[i]=outputTransformmatrix[i*4]/spacing[0];
				vector[i+3]=outputTransformmatrix[i*4+1]/spacing[1];
				vector[i+6]=outputTransformmatrix[i*4+2]/spacing[2];
			}
			NSData	*newData = [outputImage1 objectForKey:@"Data"];
			if(newData&&[newData length])
			{
				ifOutputImage1Available=YES;
				float* newDataPtr=(float*)[newData bytes];
				for( i = 0 ; i < dimension[2]; i ++)
				{
					DCMPix	*copyPix = [[inputPixList objectAtIndex: 0] copy];
					float origin[3];
					int j;
					for(j=0;j<3;j++)
						origin[j]=i*outputTransformmatrix[j*4+2]+outputTransformmatrix[j*4+3];
					
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
		NSDictionary* outputImage1=[bridgeExportToMeVisLab prepareImageForUpperBridgeFromOsiriX:@"InputImage0"];
		if(outputImage1)
		{
			
				
				NSArray* dimensionarray=[outputImage1 objectForKey:@"Dimension"];
				for(i=0;i<4;i++)
					dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
				
				NSArray* spacingarray=[outputImage1 objectForKey:@"Spacing"];
				for(i=0;i<4;i++)
					spacing[i]=[[spacingarray objectAtIndex:i] floatValue];
				
				NSArray* matrix=[outputImage1 objectForKey:@"MatrixToPatientCo"];
				for(i=0;i<16;i++)
					outputTransformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
		}
		
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
	[self initializeOutputViewBrushROI];
	if(outputMask1)
	{
		ifOutputMask1Available=YES;
		NSArray* dimensionarray=[outputMaskResult objectForKey:@"Dimension"];

		for(i=0;i<4;i++)
			if(dimension[i]!=[[dimensionarray objectAtIndex:i] intValue])
			{
								ifOutputMask1Available=NO;
			}

		
		NSArray* matrix=[outputMaskResult objectForKey:@"MatrixToPatientCo"];
		for(i=0;i<16;i++)
			if(outputTransformmatrix[i]!=[[matrix objectAtIndex:i] floatValue])
			{
							ifOutputMask1Available=NO;
			}
		if(ifOutputMask1Available==NO)
			NSRunAlertPanel(@"Failed", @"The mask's dimension is not equal to the output images' dimension." ,nil, nil, nil);

		
		
	}


	currentSychronizeMode=[self checkSynchronizeMode];
	//Synchronize the two view
	[inputImageSlider setIntValue:[inputImageView curImage]];
	[outputImageSlider setIntValue:[inputImageView curImage]];
	[outputImageView setIndex: [inputImageView curImage]];

	BOOL ifOuputCSOAvailable=NO;
	NSDictionary* outputCSO1=[bridgeImportFromMeVisLab getImageFromLowerBridge:@"OutputImage2"];
	if(outputCSO1)
		
	{
		ifOuputCSOAvailable=YES;
		[self updateOutputViewCSOROI:[outputCSO1 objectForKey:@"OverlayObjects"]];
	}
	[self scrollImageSynchronically:inputImageSlider];
	
	return (ifOutputMask1Available||ifOutputImage1Available||ifOuputCSOAvailable);
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
- (void)updateOutputViewCSOROI:(NSDictionary*)overlayObjects
{
	float invertOutputTraslateMatrix[16];
	[self invert4X4Matrix:invertOutputTraslateMatrix :outputTransformmatrix];
	NSArray* outputViewROIList=[outputImageView dcmRoiList];
	unsigned i;
	//clean up all the old ROIs
	for(i=0;i<[outputViewROIList count];i++)
		[[outputViewROIList objectAtIndex:i] removeAllObjects];
	//add new ROI objects to roi list
	for(NSDictionary* anOverlayObject in overlayObjects)
	{
		int roitype=tOPolygon;
		if([anOverlayObject objectForKey:@"IsColsed"])
			roitype=tCPolygon;
		
		NSArray* points3D=[anOverlayObject objectForKey:@"Points"];
		NSMutableArray* points2D=[NSMutableArray arrayWithCapacity:[points3D count]];
		NSMutableArray* pointsZCoors=[NSMutableArray arrayWithCapacity:[points3D count]];
		float meanzcoor=0.0;
		for(NSDictionary* apoint3D in points3D)
		{
			float x,y,z,x0,y0,z0;
			x0=[[apoint3D objectForKey:@"x"] floatValue];
			y0=[[apoint3D objectForKey:@"y"] floatValue];
			z0=[[apoint3D objectForKey:@"z"] floatValue];
			x = invertOutputTraslateMatrix[0]*x0+invertOutputTraslateMatrix[1]*y0+invertOutputTraslateMatrix[2]*z0+invertOutputTraslateMatrix[3];
			y = invertOutputTraslateMatrix[4]*x0+invertOutputTraslateMatrix[5]*y0+invertOutputTraslateMatrix[6]*z0+invertOutputTraslateMatrix[7];
			z = invertOutputTraslateMatrix[8]*x0+invertOutputTraslateMatrix[9]*y0+invertOutputTraslateMatrix[10]*z0+invertOutputTraslateMatrix[11];
			x-=0.5;
			y-=0.5;
			z-=0.5;
			MyPoint* anew2dpoint = [[MyPoint alloc] initWithPoint:NSMakePoint(x, y)];
			[points2D addObject:anew2dpoint];
			[anew2dpoint release];
			meanzcoor+=z;
			[pointsZCoors addObject:[NSNumber numberWithFloat:z]];
		}
		meanzcoor/=[points3D count];
		BOOL ifAllPointsInSamePlane=YES;
		for(NSNumber* zcoor in pointsZCoors)
		{
			if(([zcoor floatValue]-meanzcoor)>=1||([zcoor floatValue]-meanzcoor)<=-1)
				ifAllPointsInSamePlane=NO;
		}
		if(ifAllPointsInSamePlane)
		{
			DCMPix* curImage= [[outputImageView dcmPixList] objectAtIndex:(int)(meanzcoor+0.5)]; 
			ROI* anewroi=[[ROI alloc] initWithType: roitype :[curImage pixelSpacingX] :[curImage pixelSpacingY] : NSMakePoint( [curImage originX], [curImage originY])];
			[anewroi setPoints:points2D];
			[[outputViewROIList objectAtIndex:(int)(meanzcoor+0.5)] addObject:anewroi];
			[anewroi release];
		}
		
		
	}
}
- (void)initializeOutputViewBrushROI
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
	if(!ifOutputMask1Available)
		return;
	float dimension[4];

	unsigned int i;
	NSArray* dimensionarray=[outputMaskResult objectForKey:@"Dimension"];
	for(i=0;i<4;i++)
		dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
//	float spacing[4],outtransformmatrix[16],vector[9];	
//	NSArray* spacingarray=[outputMaskResult objectForKey:@"Spacing"];
//	for(i=0;i<4;i++)
//		spacing[i]=[[spacingarray objectAtIndex:i] floatValue];
//	
//	NSArray* matrix=[outputMaskResult objectForKey:@"MatrixToPatientCo"];
//	for(i=0;i<16;i++)
//		outtransformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
//
//	for(i=0;i<3;i++)
//	{
//		vector[i]=outtransformmatrix[i*4]/spacing[0];
//		vector[i+3]=outtransformmatrix[i*4+1]/spacing[1];
//		vector[i+6]=outtransformmatrix[i*4+2]/spacing[2];
//	}
	NSData	*newData = [outputMaskResult objectForKey:@"Data"];
	if(newData&&[newData length])
	{
		unsigned char* maskDataPtr=(unsigned char*)[newData bytes];
		//if(currentSychronizeMode==SYNCMODE_ALLSAME)
		{
			int imageSize=dimension[0]*dimension[1];
			if(index*imageSize+imageSize-1>[newData length])
				return;
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
	changingParameters=YES;
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	[parameters setObject:[NSNumber numberWithDouble:value] forKey:paraName];
	NSLog(@"OsriX: Change Parameters");
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
	changingParameters=NO;
	if(![self prepareOutputImages])
	{
		NSRunAlertPanel(@"Failed", @"Cannot create output Images. Check MeVisLab log for more information" ,nil, nil, nil);
	}
	NSLog(@"OsriX: Upated after Changed Parameters");
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
		else if([toolName isEqualToString:@"Rectangle"])
		{
			[inputImageView setCurrentTool:tROI];
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
	NSMutableDictionary* inputImage5=[[NSMutableDictionary alloc] initWithCapacity:0];
	
	[inputImage5 setObject:[NSString stringWithString:@"InputImage4"] forKey:@"Description"];
	[inputImage5 setObject:[NSString stringWithString:@"overlay"] forKey:@"ImageType"];
	
	NSArray* roiList=[inputImageView dcmRoiList];
	
	NSMutableArray* overlyObjects=[NSMutableArray arrayWithCapacity:0];
	unsigned i,j,k;
	for(i=0;i<[roiList count];i++)
		for(j=0;j<[[roiList objectAtIndex:i] count];j++)
		{
			ROI* aROI=[[roiList objectAtIndex:i] objectAtIndex:j];
			if([aROI type]==t2DPoint)
			{
				NSMutableDictionary* anOverlayObject=[NSMutableDictionary dictionaryWithCapacity:0];
				[anOverlayObject setObject:[NSString stringWithString:@"points"] forKey:@"OverlayType"];
				NSMutableArray* markerPoints=[NSMutableArray arrayWithCapacity:0];
				NSString* roiName=[aROI name];
				[anOverlayObject setObject:roiName	forKey:@"Name"];
				for(k=0;k<[seedingToolsConfigList count];k++)
					if([roiName isEqualToString:[[seedingToolsConfigList objectAtIndex:k] valueForKey:@"ObjectName"]])
					{
						NSRect roiRect=[aROI rect];
						float x,y,z,x0,y0,z0;
						x0=roiRect.origin.x;
						y0=roiRect.origin.y;
						z0=i;
						x0+=0.5;
						y0+=0.5;
						z0+=0.5;
						x = transformmatrix[0]*x0+transformmatrix[1]*y0+transformmatrix[2]*z0+transformmatrix[3];
						y = transformmatrix[4]*x0+transformmatrix[5]*y0+transformmatrix[6]*z0+transformmatrix[7];
						z = transformmatrix[8]*x0+transformmatrix[9]*y0+transformmatrix[10]*z0+transformmatrix[11];
						
						NSDictionary* a3DPoint=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:x],@"x",[NSNumber numberWithFloat:y],@"y",[NSNumber numberWithFloat:z],@"z",[NSNumber numberWithInt:k+1],@"value",nil];
						[markerPoints addObject:a3DPoint];
						break;
					}
				[anOverlayObject setObject:markerPoints forKey:@"Points"];
				[overlyObjects addObject:anOverlayObject];
			}
			else if([aROI type]==tROI)
			{
				NSMutableDictionary* anOverlayObject=[NSMutableDictionary dictionaryWithCapacity:0];
				[anOverlayObject setObject:[NSString stringWithString:@"polygon"] forKey:@"OverlayType"];
				NSMutableArray* markerPoints=[NSMutableArray arrayWithCapacity:0];
				NSString* roiName=[aROI name];
				[anOverlayObject setObject:roiName	forKey:@"Name"];
				for(k=0;k<[seedingToolsConfigList count];k++)
					if([roiName isEqualToString:[[seedingToolsConfigList objectAtIndex:k] valueForKey:@"ObjectName"]])
					{
						NSMutableArray  *ptsarray = [aROI points];
						for(MyPoint* apoint in ptsarray)
						{
							float x,y,z,x0,y0,z0;
							x0=apoint.x;
							y0=apoint.y;
							z0=i;
							x0+=0.5;
							y0+=0.5;
							z0+=0.5;
							x = transformmatrix[0]*x0+transformmatrix[1]*y0+transformmatrix[2]*z0+transformmatrix[3];
							y = transformmatrix[4]*x0+transformmatrix[5]*y0+transformmatrix[6]*z0+transformmatrix[7];
							z = transformmatrix[8]*x0+transformmatrix[9]*y0+transformmatrix[10]*z0+transformmatrix[11];
							
							NSDictionary* a3DPoint=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:x],@"x",[NSNumber numberWithFloat:y],@"y",[NSNumber numberWithFloat:z],@"z",[NSNumber numberWithInt:k+1],@"value",nil];
							[markerPoints addObject:a3DPoint];
						}
						break;
					}
				[anOverlayObject setObject:markerPoints forKey:@"Points"];
				[anOverlayObject setObject:[NSNumber numberWithBool:YES] forKey:@"IsColsed"];
				[overlyObjects addObject:anOverlayObject];
			}
			
			
		}
	
	[inputImage5 setObject:overlyObjects forKey:@"OverlayObjects"];
	[bridgeExportToMeVisLab initializeAnImageForSharing:inputImage5];
	[inputImage5 release];
	
}
- (void)notifyMeVisLabSeedsChanged
{
	[self prepareInputMarkerImage];
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	[parameters setObject:[NSNumber numberWithBool:YES] forKey:@"PointMarkerUpdated"];
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
}
- (void)notifyMeVisLabCSOChanged
{
	[self prepareInputMarkerImage];
	NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
	[parameters setObject:[NSNumber numberWithBool:YES] forKey:@"CSOMarkerUpdated"];
	[bridgeExportToMeVisLab passingOnNotificationsToUpperBridge:parameters];
	
}
- (IBAction)updateOutputImage:(id)sender;
{
	[self notifyMeVisLabSeedsChanged];
	[self notifyMeVisLabCSOChanged];
	[self prepareOutputImages];
	
}
- (IBAction)closeCurrentWindow:(id)sender
{
	[[self window] setDelegate:self];
	[[self window] performClose:sender];
}
- (void)handleMeVisLabNotification:(NSDictionary*) parameters
{
	if(isInitializing||updatingROI||changingParameters)
		return;
	NSLog(@"OsriX: Output updated after Notification");
	if([[parameters objectForKey:@"NeedUpdateImage"] boolValue])
		[self prepareOutputImages];
	if([[parameters objectForKey:@"NeewShowSelfWindow"] boolValue])
		[[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
}
-(void)invert4X4Matrix:(float*)inElements: (float*) outElements
{
	// inverse( original_matrix, inverse_matrix )
	// calculate the inverse of a 4x4 matrix
	//
	//     -1     
	//     A  = ___1__ adjoint A
	//         det A
	//
	
	int i, j;
	double det;
	
	// calculate the 4x4 determinent
	// if the determinent is zero, 
	// then the inverse matrix is not unique.
	
	[self matrixDeterminant:inElements];
	if ( det == 0.0 ) 
    {
		return;
    }
	
	// calculate the adjoint matrix
	[self matrixAdjoint:inElements : outElements];
	
	// scale the adjoint matrix to get the inverse
	for (i=0; i<4; i++)
    {
		for(j=0; j<4; j++)
		{
			*(outElements+i*4+j) = *(outElements+i*4+j) / det;
		}
    }
}
-(double) matrixDeterminant:(float*) elem
{
	
	float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
	
	
	// assign to individual variable names to aid selecting
	//  correct elements
	
	a1 = elem[0]; b1 = elem[1]; 
	c1 = elem[2]; d1 = elem[3];
	
	a2 = elem[4]; b2 = elem[5]; 
	c2 = elem[6]; d2 = elem[7];
	
	a3 = elem[8]; b3 = elem[9]; 
	c3 = elem[10]; d3 = elem[11];
	
	a4 = elem[12]; b4 = elem[13]; 
	c4 = elem[14]; d4 = elem[15];
	
	return a1 * [self matrixDeterminant3x3: b2: b3: b4: c2: c3: c4: d2: d3: d4]
	- b1 * [self matrixDeterminant3x3: a2: a3: a4: c2: c3: c4: d2: d3: d4]
	+ c1 * [self matrixDeterminant3x3: a2: a3: a4: b2: b3: b4: d2: d3: d4]
	- d1 * [self matrixDeterminant3x3: a2: a3: a4: b2: b3: b4: c2: c3: c4];
}

//----------------------------------------------------------------------------
-(void) matrixAdjoint: (float*) inElem: (float*) outElem
{	
	// 
	//   adjoint( original_matrix, inverse_matrix )
	// 
	//     calculate the adjoint of a 4x4 matrix
	//
	//      Let  a   denote the minor determinant of matrix A obtained by
	//           ij
	//
	//      deleting the ith row and jth column from A.
	//
	//                    i+j
	//     Let  b   = (-1)    a
	//          ij            ji
	//
	//    The matrix B = (b  ) is the adjoint of A
	//                     ij
	//
	float a1, a2, a3, a4, b1, b2, b3, b4;
	float c1, c2, c3, c4, d1, d2, d3, d4;
	
	// assign to individual variable names to aid
	// selecting correct values
	
	a1 = inElem[0]; b1 = inElem[1]; 
	c1 = inElem[2]; d1 = inElem[3];
	
	a2 = inElem[4]; b2 = inElem[5]; 
	c2 = inElem[6]; d2 = inElem[7];
	
	a3 = inElem[8]; b3 = inElem[9];
	c3 = inElem[10]; d3 = inElem[11];
	
	a4 = inElem[12]; b4 = inElem[13]; 
	c4 = inElem[14]; d4 = inElem[15];
	
	
	// row column labeling reversed since we transpose rows & columns
	
	outElem[0]  =   
    [self matrixDeterminant3x3: b2: b3: b4: c2: c3: c4: d2: d3: d4];
	outElem[4]  = 
    - [self matrixDeterminant3x3: a2: a3: a4: c2: c3: c4: d2: d3: d4];
	outElem[8]  =   
    [self matrixDeterminant3x3: a2: a3: a4: b2: b3: b4: d2: d3: d4];
	outElem[12]  = 
    - [self matrixDeterminant3x3: a2: a3: a4: b2: b3: b4: c2: c3: c4];
	
	outElem[1]  = 
    - [self matrixDeterminant3x3: b1: b3: b4: c1: c3: c4: d1: d3: d4];
	outElem[5]  =   
    [self matrixDeterminant3x3: a1: a3: a4: c1: c3: c4: d1: d3: d4];
	outElem[9]  = 
    - [self matrixDeterminant3x3: a1: a3: a4: b1: b3: b4: d1: d3: d4];
	outElem[13]  =   
    [self matrixDeterminant3x3: a1: a3: a4: b1: b3: b4: c1: c3: c4];
	
	outElem[2]  =   
    [self matrixDeterminant3x3: b1: b2: b4: c1: c2: c4: d1: d2: d4];
	outElem[6]  = 
    - [self matrixDeterminant3x3: a1: a2: a4: c1: c2: c4: d1: d2: d4];
	outElem[10]  =   
    [self matrixDeterminant3x3: a1: a2: a4: b1: b2: b4: d1: d2: d4];
	outElem[14]  = 
    - [self matrixDeterminant3x3: a1: a2: a4: b1: b2: b4: c1: c2: c4];
	
	outElem[3]  = 
    - [self matrixDeterminant3x3: b1: b2: b3: c1: c2: c3: d1: d2: d3];
	outElem[7]  =   
    [self matrixDeterminant3x3: a1: a2: a3: c1: c2: c3: d1: d2: d3];
	outElem[11]  = 
    - [self matrixDeterminant3x3: a1: a2: a3: b1: b2: b3: d1: d2: d3];
	outElem[15]  =   
    [self matrixDeterminant3x3: a1: a2: a3: b1: b2: b3: c1: c2: c3];
}
-(float) matrixDeterminant3x3:(float)c10:(float)c11:(float)c12:(float)c20:(float)c21:(float)c22:(float)c30:(float)c31:(float)c32
                                     
{
	return c10*c21*c32 + c20*c31*c12 + c30*c11*c22 -
	c10*c31*c22 - c20*c11*c32 - c30*c21*c12;
}


@end
