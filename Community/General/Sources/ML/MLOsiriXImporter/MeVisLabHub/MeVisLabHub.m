//
//  DuplicateFilter.m
//  Duplicate
//
//  Created by rossetantoine on Wed Jun 09 2004.
//  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
//

#import "MeVisLabHub.h"

@implementation MeVisLabHub

- (void) initPlugin
{
}

- (long) filterImage:(NSString*) menuName
{
	if(!winController)
		winController=[[MeVisHubWindowController alloc] initFromNIB];
	if(winController)
	{
		[winController setWindowController:viewerController];
		if([menuName isEqualToString:NSLocalizedString(@"Configuration", nil)])
			[winController showConfigWindow];
		else if([menuName isEqualToString:NSLocalizedString(@"Help", nil)])
			[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.cmiv.liu.se/"]];
		else
			[winController showMainWindow:menuName];
	}
	else
	{
		return -1;//cannot load MeVisHubGUI.xib

	}
	return 0;//no error
}

@end
