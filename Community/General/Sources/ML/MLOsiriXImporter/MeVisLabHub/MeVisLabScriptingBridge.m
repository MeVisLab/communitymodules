//
//  MeVisLabScriptingBridge.m
//  MeVisLabHub
//
//  Created by Felix Ritter on 24.08.09.
//  Copyright 2009 Fraunhofer MEVIS - Institute for Medical Image Computing. All rights reserved.
//

#import "MeVisLabScriptingBridge.h"


@implementation MeVisLabScriptingBridge

- (id)init
{
	if ((self = [super init])) {
		MeVisLab = [SBApplication applicationWithBundleIdentifier:@"de.mevis.MeVisLab"];
	}
	return self;
}

- (void)dealloc
{
	[MeVisLab release];
	
	[super dealloc];
}

- (BOOL)openNetworkFile:(NSString *)filename
{
	if ([MeVisLab isRunning]) {
		[MeVisLab open:[NSArray arrayWithObject:filename]];
		
		return YES;
	}
	return NO;
}

- (BOOL)closeNetwork:(NSString *)filename
{
	if ([MeVisLab isRunning]) {
		NSArray *docs = [[MeVisLab documents] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"path == %@", filename]];
		if ([docs count] > 0) {
			MeVisLabDocument *doc = [docs objectAtIndex:0];
			[doc close];
			
			return YES;
		}
	}
	return NO;
}

@end
