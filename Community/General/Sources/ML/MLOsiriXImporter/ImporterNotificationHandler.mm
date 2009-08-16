//
//  ImporterNotificationHandler.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ImporterNotificationHandler.h"


@implementation ImporterNotificationHandler
- (id) initWithImporterML:(ml::OsiriXImporter*) aML
{
	if ((self = [super init])) {
		
		importerML=aML;

		[self registerSelfToNSNotificationCenter];

		
	}
	return self;
	
}
-(void)registerSelfToNSNotificationCenter
{
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(exporterIsAdded:) name:@"AnOsiriXExporterAdded" object:nil];
}
-(void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver: self];
	[super dealloc];
}
- (void) exporterIsAdded: (NSNotification *)notification
{
	importerML->setBridgeToOsiriX([notification object]);
	[[NSNotificationCenter defaultCenter] removeObserver: self];
}
@end
