//
//  OsiriX2MeVisLabTBridge.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "OsiriX2MeVisLabTBridge.h"
#import "MeVisHubWindowController.h"
#import <arpa/inet.h>
@implementation OsiriX2MeVisLabTBridge
#pragma mark-
#pragma mark functions to start build the message bridge
- (id) initWithIncommingConnectionName:(NSString*)aname SupportSharedMem:(BOOL)isSupported
{
	self = [super init];
	if (self)
	{
		imagesManager=[[SharedImagesManager alloc] initWithIDStringPrefix:aname];
		outgoingConnectionRegisteredName=nil;
		ifSupportMemorySharing=isSupported;
		// Vending services
		
		NSSocketPort* port1=[[NSSocketPort alloc] init];
		if ((incomingConnection = [[NSConnection alloc] initWithReceivePort:port1 sendPort:nil])) {
			[incomingConnection setRootObject:self];
			[incomingConnection setRequestTimeout:5];
			[incomingConnection setReplyTimeout:30];
			if ([[NSSocketPortNameServer sharedInstance] registerPort:port1 name:aname]) {
				NSLog(@"Successfully registere service as %@", aname);
				incomingConnectionRegisteredName=[aname copy];
			}
			else {
				NSLog(@"Cound not registere service as %@", aname);
			}
		}
		[port1 release];

		
	}
	return self;
}
-(void)prepareToDealloc
{
	if(remoteObjectProxy)
	{
		NSMutableDictionary* anoperation=[NSMutableDictionary dictionaryWithCapacity:0];
		{
			NSString* operation=[NSString stringWithString:@"Breakup"];
			NSMutableDictionary* parameters=[NSMutableDictionary dictionaryWithCapacity:0];
			NSMutableArray* relatedImages=[NSMutableArray arrayWithCapacity:0];
			
			[anoperation setObject:operation forKey:@"Operation"];
			[anoperation setObject:parameters forKey:@"Parameters"];
			[anoperation setObject:relatedImages forKey:@"RelatedImages"];
		}
		
		[remoteObjectProxy setOperation:anoperation];
		
	}
	[remoteConnection invalidate];
	[[NSNotificationCenter defaultCenter] removeObserver: self];

	[[incomingConnection receivePort] invalidate];//this will work to unregister the NSConnection
	[[incomingConnection sendPort] invalidate];
	[incomingConnection setRootObject:nil];
	[incomingConnection registerName:nil];//this will not work as suggestion it alway return NO
	[incomingConnection invalidate];
	[incomingConnection release];
	
	if(incomingConnectionRegisteredName)
	{
		[[NSSocketPortNameServer sharedInstance] removePortForName:incomingConnectionRegisteredName];
	}
	incomingConnection = nil;
	
}
-(void)dealloc
{

	[imagesManager release];
	
	
	[super dealloc];
}
- (void) setWindowController:(MeVisHubWindowController*)window
{
	windowController=window;
}
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname
{
	if ([[NSSocketPortNameServer sharedInstance] registerPort:[incomingConnection receivePort] name:aname] == YES) {
		if(incomingConnectionRegisteredName)
		{
			[[NSSocketPortNameServer sharedInstance] removePortForName:incomingConnectionRegisteredName];
		}
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
	NSSocketPort  *port = (NSSocketPort *)[[NSSocketPortNameServer sharedInstance] 
										   portForName:registeredName 
										   host:@"*"];

	remoteConnection=[NSConnection connectionWithReceivePort:nil sendPort:port];
	
	remoteObjectProxy = [[remoteConnection rootProxy] retain];
	
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
				[parameters setObject:[NSNumber numberWithBool: ifSupportMemorySharing] forKey:@"SupportSharedMem"];
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
	remoteConnection=nil;
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
			//if(!remoteObjectProxy)
//				[self connectToRemoteObjectRegisteredAs:registeredName];
			outgoingConnectionRegisteredName=[registeredName copy];
		}
		else	if([operation isEqualToString:@"Breakup"])
		{
			
			[remoteConnection invalidate];
			
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
	
	NSLog([NSString stringWithFormat:@"OsiriX: MeVis is asking for:%@",  description] );
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
		if(ifSupportMemorySharing)
			[matchedImage removeObjectForKey:@"Data"];
		[matchedImage autorelease];
	}
	
	NSLog([NSString stringWithFormat:@"OsriX: OsiriX is returningg :%@",  description] );
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
	return [imagesManager creatASharedImage:anImage ForDescription:[anImage objectForKey:@"Description"] SupportSharedMem:ifSupportMemorySharing];
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
	NSLog([NSString stringWithFormat:@"OsiriX: OsiriX is asking for:%@",  description] );
	if(ifSupportMemorySharing==NO)
	{
		[imagesManager removeImage:[imagesManager getImageForDescription: description]];
	}
	NSDictionary* anImage=[remoteObjectProxy getImage:description];
	
	if(anImage)
	{
		NSMutableDictionary*anNewImage=[anImage mutableCopy];
		[imagesManager creatASharedImage:anNewImage ForDescription:description SupportSharedMem:ifSupportMemorySharing];


		[anNewImage release];
	}
	NSLog([NSString stringWithFormat:@"OsiriX: %@ received",  description] );
	return [imagesManager getImageForDescription: description];
	
}
-(void)passingOnNotificationsToWindowController:(NSDictionary*)parameters
{
	NSLog([NSString stringWithFormat:@"OsiriX: MeVis is notifying:%@",  @"paramerter changed"] );
	[windowController handleMeVisLabNotification: parameters];
}
@end
