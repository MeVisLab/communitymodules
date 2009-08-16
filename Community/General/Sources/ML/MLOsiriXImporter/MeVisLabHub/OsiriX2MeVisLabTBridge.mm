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
- (id) initWithIncommingConnectionName:(NSString*)aname OutgoingConnection:(NSString*)bname
{
	self = [super init];
	if (self)
	{
		imagesManager=[[SharedImagesManager alloc] initWithIDStringPrefix:aname];
		outgoingConnectionRegisteredName=[bname copy];
		// Vending services
		if ((incomingConnection = [[NSConnection alloc] init])) {
			[incomingConnection setRootObject:self];
			[incomingConnection setRequestTimeout:5];
			[incomingConnection setReplyTimeout:30];
			if ([incomingConnection registerName:aname] == YES) {
				NSLog(@"Successfully registere service as %@", aname);
				incomingConnectionRegisteredName=[aname copy];
			}
			else {
				NSLog(@"Cound not registere service as %@", aname);
			}
		}
		
	}
	return self;
}
-(void)prepareToDealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver: self];\
	[[incomingConnection receivePort] invalidate];//this will work to unregister the NSConnection
	[[incomingConnection sendPort] invalidate];
	[incomingConnection setRootObject:nil];
	[incomingConnection registerName:nil];//this will not work as suggestion it alway return NO
	[incomingConnection invalidate];
	[incomingConnection release];
	incomingConnection = nil;
	
}
-(void)dealloc
{

	[imagesManager release];
	if(remoteObjectProxy)
		[remoteObjectProxy release];
	
	[super dealloc];
}
- (void) setWindowController:(MeVisHubWindowController*)window
{
	windowController=window;
}
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname
{
	if ([incomingConnection registerName:aname] == YES) {
		NSLog(@"Successfully registere service as %@", aname);
		incomingConnectionRegisteredName=[aname copy];
		return YES;
	}
	else {
		NSLog(@"Cound not registere service as %@", aname);
		return NO;
	}
}
- (BOOL)connectToRemoteObjectRegisteredAs:(NSString*)registeredName
{
	remoteObjectProxy = [[NSConnection rootProxyForConnectionWithRegisteredName:registeredName host:nil] retain];
	
	if(remoteObjectProxy)
	{
		outgoingConnectionRegisteredName=[registeredName copy];
		[remoteObjectProxy setProtocolForProxy:@protocol(MeVisOsiriXProxyProtocol)];
		
		NSConnection *outgoingConnection = [remoteObjectProxy connectionForProxy];

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
		
		[remoteObjectProxy setOperation:anoperation];
		
	}
	else
		return NO;
	
	return YES;
	

}

- (void) connectionDidDie: (NSNotification *)notification
{
	if(remoteObjectProxy)
	{
		[remoteObjectProxy release];
		remoteObjectProxy=nil;
	}
}
#pragma mark-
#pragma mark functions exported to the remote NSProxy.
- (void)setOperation:(NSDictionary *)request
{
	NSString* operation=[request objectForKey:@"Operation"];
	if(operation)
	{
		if([operation isEqualToString:@"CallBack"])
		{
			NSMutableDictionary* parameters=[request objectForKey:@"Parameters"];
			NSString* registeredName=[parameters objectForKey:@"RegisteredName"];
			if(!remoteObjectProxy)
				[self connectToRemoteObjectRegisteredAs:registeredName];
		}
		else	if([operation isEqualToString:@"ParameterUpdate"])
		{
			NSDictionary* parameters=[request objectForKey:@"Parameters"];
			[self passingOnNotificationsToWindowController:parameters];
		}
	}

	return ;
}
- (NSDictionary*)getImage:(NSString*)description
{
	return [self prepareImageForUpperBridgeFromOsiriX:description];
}
#pragma mark-
#pragma mark functions when works as a bridge between Importer and OsiriX.
-(NSDictionary*)prepareImageForUpperBridgeFromOsiriX:(NSString*)description
{

	NSDictionary* savedImage=[imagesManager getImageForDescription: description];
	NSMutableDictionary* matchedImage=nil;
	if(savedImage)
	{
		matchedImage=[savedImage mutableCopy];
		[matchedImage removeObjectForKey:@"Data"];
	}
	

	return matchedImage;
}
-(void)passingOnNotificationsToUpperBridge:(NSDictionary*)parameters
{
	if(remoteObjectProxy==nil)
	{
		if(![self connectToRemoteObjectRegisteredAs:outgoingConnectionRegisteredName])
		{
			NSLog(@"Cound not connect to MeVisLab Exporter!");
			return;
		}
	}
	NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
	{
		NSString* operation=[NSString stringWithString:@"ParameterUpdate"];

		NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
		
		[anoperation setObject:operation forKey:@"Operation"];
		[anoperation setObject:parameters forKey:@"Parameters"];
		[anoperation setObject:relatedImages forKey:@"RelatedImages"];
	}
	
	[remoteObjectProxy setOperation:anoperation];
}
- (BOOL)initializeAnImageForSharing:(NSMutableDictionary*)anImage
{
	return [imagesManager creatASharedImage:anImage ForDescription:[anImage objectForKey:@"Description"]];
}
#pragma mark-
#pragma mark functions when works as a bridge between Exporter and OsiriX
-(NSDictionary*)getImageFromLowerBridge:(NSString*)description
{
	if(remoteObjectProxy==nil)
	{
		if(![self connectToRemoteObjectRegisteredAs:outgoingConnectionRegisteredName])
		{
			NSLog(@"Cound not connect to MeVisLab Exporter!");
			return nil;
		}
	}
	
	NSDictionary* anImage=[remoteObjectProxy getImage:description];
	if(anImage)
		[imagesManager creatASharedImage:[anImage mutableCopy] ForDescription:description];
	
	return [imagesManager getImageForDescription: description];
	
}
-(void)passingOnNotificationsToWindowController:(NSDictionary*)parameters
{
	
}
@end
