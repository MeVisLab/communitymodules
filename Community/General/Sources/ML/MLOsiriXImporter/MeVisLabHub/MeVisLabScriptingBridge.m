//
//  MeVisLabScriptingBridge.m
//  MeVisLabHub
//
//  Created by Felix Ritter on 24.08.09.
//  Copyright 2009 Fraunhofer MEVIS - Institute for Medical Image Computing. All rights reserved.
//

#import "MeVisLabScriptingBridge.h"

// Generated with:  sdef /Applications/MeVisLab.app | sdp -fh --basename MeVisLab
#import "MeVisLab.h"

#define MEVISLAB_BUNDLEID  @"de.mevis.MeVisLab"


@implementation MeVisLabScriptingBridge

- (BOOL)openNetworkWithFilename:(NSString *)filename
{
	MeVisLabApplication *MeVisLab = [SBApplication applicationWithBundleIdentifier:MEVISLAB_BUNDLEID];
	
	if ([MeVisLab isRunning]) {
		[MeVisLab open:[NSArray arrayWithObject:filename]];
		
		return YES;
	}
	return NO;
}

- (BOOL)closeNetworkWithFilename:(NSString *)filename
{
	MeVisLabApplication *MeVisLab = [SBApplication applicationWithBundleIdentifier:MEVISLAB_BUNDLEID];
	
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
