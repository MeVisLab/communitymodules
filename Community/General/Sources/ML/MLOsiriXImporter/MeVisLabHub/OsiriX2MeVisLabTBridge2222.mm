//
//  OsiriX2MeVisLabTBridge.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "OsiriX2MeVisLabTBridge.h"
#import "MeVisHubWindowController.h"

@implementation OsiriX2MeVisLabTBridge
#pragma mark-
#pragma mark functions to start build the message bridge
- (id) initWithWindow:(MeVisHubWindowController*)window;
{
	self = [super init];
	if (self)
	{
		windowController=window;
		imagesManager=[[SharedImagesManager alloc] initWithIDStringPrefix:@"OsiriX:"];
		// Vending services
		if ((incomingConnection = [[NSConnection alloc] init])) {
			[incomingConnection setRootObject:self];
			[incomingConnection setRequestTimeout:5];
			[incomingConnection setReplyTimeout:30];
			if ([incomingConnection registerName:@"OsiriXMeVisLabHub"] == YES) {
				NSLog(@"Successfully registere service as OsiriXMeVisLabHub");
				incomingConnectionRegisteredName=[NSString stringWithString:@"OsiriXMeVisLabHub"];
			}
			else {
				NSLog(@"Cannot registere service as OsiriXMeVisLabHub");
			}
		}
		
	}
	return self;
}

-(void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver: self];
	[incomingConnection invalidate];
	[incomingConnection release];
	incomingConnection = nil;
	[imagesManager release];
	if(meVisLabImporterProxy)
		[meVisLabImporterProxy release];
	if(meVisLabExporterProxy)
		[meVisLabExporterProxy release];
	
	[super dealloc];
}
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname
{
	if ([incomingConnection registerName:aname] == YES) {
		NSLog(@"Successfully registere service as %@", aname );
		incomingConnectionRegisteredName=[aname copy];
		return YES;
	}
	else {
		NSLog(@"Cound not register service as %@", aname );
		return NO;
	}
}
- (BOOL)connectToExporterMLNamedAs:(NSString*)registeredName
{
	meVisLabExporterProxy = [[NSConnection rootProxyForConnectionWithRegisteredName:registeredName host:nil] retain];
	
	if(meVisLabExporterProxy)
	{
	
		[meVisLabExporterProxy setProtocolForProxy:@protocol(MeVisOsiriXProxyProtocol)];
		
		NSConnection *outgoingConnection = [meVisLabExporterProxy connectionForProxy];

		// Notification observer for a lost connection
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(connectionDidDie:)
													 name:NSConnectionDidDieNotification
												   object:outgoingConnection];
		NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
		{
			NSString* operation=[NSString stringWithString:@"CallBack"];
			NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
			{
				[parameters setObject:incomingConnectionRegisteredName forKey:@"RegisteredName"];
			}
			NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
			[anoperation setObject:operation forKey:@"Operation"];
			[anoperation setObject:parameters forKey:@"Parameters"];
			[anoperation setObject:relatedImages forKey:@"RelatedImages"];
		}
		
		[meVisLabExporterProxy setOperation:anoperation];
		
	}
	else
		return NO;
	
	return YES;
	

}
- (BOOL)connectToImporterMLNamedAs:(NSString*)registeredName
{
	meVisLabImporterProxy = [[NSConnection rootProxyForConnectionWithRegisteredName:registeredName host:nil] retain];
	
	if(meVisLabImporterProxy)
	{
		
		[meVisLabImporterProxy setProtocolForProxy:@protocol(MeVisOsiriXProxyProtocol)];
		
		NSConnection *outgoingConnection = [meVisLabImporterProxy connectionForProxy];
		
		// Notification observer for a lost connection
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(connectionDidDie:)
													 name:NSConnectionDidDieNotification
												   object:outgoingConnection];
//		NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
//		{
//			NSString* operation=[NSString stringWithString:@"CallBack"];
//			NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
//			{
//				[parameters setObject:incomingConnectionRegisteredName forKey:@"RegisteredName"];
//			}
//			NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
//			
//			[anoperation setObject:operation forKey:@"Operation"];
//			[anoperation setObject:parameters forKey:@"Parameters"];
//			[anoperation setObject:relatedImages forKey:@"RelatedImages"];
//		}
//		
//		[meVisLabImporterProxy setOperation:anoperation];
		
	}
	else
		return NO;
	
	return YES;
	
	
}
- (void) connectionDidDie: (NSNotification *)notification
{
	if(meVisLabImporterProxy)
	{
		[meVisLabImporterProxy release];
		meVisLabImporterProxy=nil;
	}
	if(meVisLabExporterProxy)
	{
		[meVisLabExporterProxy release];
		meVisLabExporterProxy=nil;
	}
}
#pragma mark-
#pragma mark functions to interprete the operation request from MeVisLab.
- (BOOL)setOperation:(inout bycopy NSDictionary *)request
{
	NSString* operation=[request objectForKey:@"Operation"];
	if(operation)
	{
		if([operation isEqualToString:@"CallBack"])
		{
			NSMutableDictionary* parameters=[request objectForKey:@"Parameters"];
			NSString* registeredName=[parameters objectForKey:@"RegisteredName"];
			NSString* senderClass=[registeredName substringToIndex:14];
			if([senderClass isEqualToString:@"OsiriXExporter"]&&!meVisLabExporterProxy) 
				[self connectToExporterMLNamedAs:registeredName];
			if([senderClass isEqualToString:@"OsiriXImporter"]&&!meVisLabImporterProxy) 
				[self connectToImporterMLNamedAs:registeredName];
		}
		else	if([operation isEqualToString:@"SetOutputImages"])
		{
			NSMutableArray* relatedImages=[request objectForKey:@"RelatedImages"];
			[imagesManager addImagesInArray:relatedImages];
		}
	}
	else
		return NO;
	return YES;
}
#pragma mark-
#pragma mark functions to interprete the operation request from windowController.

- (BOOL) putImagesForSharing:(NSArray*)images
{
	return [imagesManager addImagesInArray:images];
	
}
- (NSDictionary*)getSharedImageForDescription:(NSString*)description
{		
	return [imagesManager getImageForDescription:description];
}
-(BOOL) sendRequestToExporter:(NSMutableDictionary*) arequest
{
	if(meVisLabExporterProxy)
	{
		if(![meVisLabExporterProxy setOperation:anoperation])
		{

			NSRunAlertPanel(@"Failed", @"MeVisLab returns error." ,nil, nil, nil);
			return NO;
		}
	}
	else
	{
		NSRunAlertPanel(@"Failed", @"The connection to MeVisLabExporter is broken." ,nil, nil, nil);
		return NO;
	}
	return YES;
	
}
-(BOOL) sendRequestToImporter:(NSMutableDictionary*) arequest
{
	if(meVisLabImporterProxy)
	{
		if(![meVisLabImporterProxy setOperation:anoperation])
		{
			
			NSRunAlertPanel(@"Failed", @"MeVisLab returns error." ,nil, nil, nil);
			return NO;
		}
	}
	else
	{
		NSRunAlertPanel(@"Failed", @"The connection to MeVisLabExporter is broken." ,nil, nil, nil);
		return NO;
	}
	return YES;
	
}

-(void) runTheOperation
{
	NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
	{
		NSString* operation=[NSString stringWithString:@"GetOutputImage"];
		NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
		NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
		[anoperation setObject:operation forKey:@"Operation"];
		[anoperation setObject:parameters forKey:@"Parameters"];
		[anoperation setObject:relatedImages forKey:@"RelatedImages"];
	}
	if([self sendRequestToExporter:anoperation])
	{
		NSMutableArray* relatedImages=[anoperation objectForKey:@"RelatedImages"];
		[imagesManager addImagesInArray:relatedImages];
	}
	
}
- (void) notifyInputImagesChanged
{
	NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
	{
		NSString* operation=[NSString stringWithString:@"InputImagesChanged"];
		NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
		NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
		[anoperation setObject:operation forKey:@"Operation"];
		[anoperation setObject:parameters forKey:@"Parameters"];
		[anoperation setObject:relatedImages forKey:@"RelatedImages"];
	}
	if([self sendRequestToImporter:anoperation])
	{
		NSMutableArray* relatedImages=[anoperation objectForKey:@"RelatedImages"];
		[imagesManager addImagesInArray:relatedImages];
	}
	
	
}
- (void) notifyInputMarkersChanged
{
	NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
	{
		NSString* operation=[NSString stringWithString:@"InputMarkerChanged"];
		NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
		NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
		[anoperation setObject:operation forKey:@"Operation"];
		[anoperation setObject:parameters forKey:@"Parameters"];
		[anoperation setObject:relatedImages forKey:@"RelatedImages"];
	}
	if([self sendRequestToImporter:anoperation])
	{
		NSMutableArray* relatedImages=[anoperation objectForKey:@"RelatedImages"];
		[imagesManager addImagesInArray:relatedImages];
	}
	
	
}
- (void) notifyParametersChanged:(NSDictionary*)parameters
{
	NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
	{
		NSString* operation=[NSString stringWithString:@"InputMarkerChanged"];
		NSDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
		NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
		[anoperation setObject:operation forKey:@"Operation"];
		[anoperation setObject:parameters forKey:@"Parameters"];
		[anoperation setObject:relatedImages forKey:@"RelatedImages"];
	}
	if([self sendRequestToImporter:anoperation])
	{
		NSMutableArray* relatedImages=[anoperation objectForKey:@"RelatedImages"];
		[imagesManager addImagesInArray:relatedImages];
	}
	
	
}
@end
