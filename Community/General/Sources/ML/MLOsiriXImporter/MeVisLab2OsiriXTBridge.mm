//
//  MeVisLab2OsiriXTBridge.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MeVisLab2OsiriXTBridge.h"


@implementation MeVisLab2OsiriXTBridge

#pragma mark-
#pragma mark functions to start build the message bridge
- (id) initWithIncommingConnectionName:(NSString*)aname OutgoingConnection:(NSString*)bname
{
	self = [super init];
	if (self)
	{
		imagesManager=[[SharedImagesManager alloc] initWithIDStringPrefix:@"MeVis"];
		outgoingConnectionRegisteredName=[bname copy];
		// Vending services
		if ((incomingConnection = [[NSConnection alloc] init])) {
			[incomingConnection setRootObject:self];
			[incomingConnection setRequestTimeout:5];
			[incomingConnection setReplyTimeout:30];
			if ([incomingConnection registerName:aname] == YES) {
				std::cout << "Successfully registere service as " << [aname  UTF8String] << std::endl;
				incomingConnectionRegisteredName=[aname copy];
			}
			else {
				std::cout << "Cound not register service as "  << [aname  UTF8String] << std::endl;
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
- (void) setImporterML:(ml::OsiriXImporter*) aML
{
	importerML=aML;
}

- (void) setExporterML:(ml::OsiriXExporter*) aML
{
	exporterML=aML;
}
- (BOOL)registerIncomingConnectionWithName:(NSString*)aname
{
	if ([incomingConnection registerName:aname] == YES) {
		std::cout << "Successfully registere service as " << [aname  UTF8String] << std::endl;
		incomingConnectionRegisteredName=[aname copy];
		return YES;
	}
	else {
		std::cout << "Cound not register service as "  << [aname  UTF8String] << std::endl;
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
#pragma mark functions to interprete the operation request from OsiriX.
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
			[self passingOnNotificationsToImporter:parameters];
		}
	}
	return;
}
- (NSDictionary*)getImage:(NSString*)description
{
	return [self prepareImageForUpperBridgeFromExporter:description];
}
#pragma mark-
#pragma mark functions when works as a bridge between Importer and OsiriX.
-(NSDictionary*)getImageFromLowerBridge:(NSString*)description
{
	if(remoteObjectProxy==nil)
	{
		if(![self connectToRemoteObjectRegisteredAs:outgoingConnectionRegisteredName])
		{
			std::cout << "Cound not connect to OsiriX!"  << std::endl;
			return nil;
		}
	}
	
	NSDictionary* anImage=[remoteObjectProxy getImage:description];
	if(anImage)
		[imagesManager creatASharedImage:[anImage mutableCopy] ForDescription:description];
	
	return [imagesManager getImageForDescription: description];
}
-(void)passingOnNotificationsToImporter:(NSDictionary*)parameters
{
	importerML->updateParameters(parameters);
}
#pragma mark-
#pragma mark functions when works as a bridge between Exporter and OsiriX
-(NSDictionary*)prepareImageForUpperBridgeFromExporter:(NSString*)description
{
	NSMutableDictionary* tempImage=[NSMutableDictionary dictionaryWithCapacity:0];
	[tempImage setObject:description forKey:@"Description"];
	if([description isEqualToString:@"OutputImage0"])
	{
		exporterML->calcInputImageProps(tempImage, 0);
		[tempImage setObject:[NSString stringWithString:@"float"] forKey:@"ImageType"];
		int dimension[4]={0,0,0,0};
		int i;
		NSArray* dimensionarray=[tempImage objectForKey:@"Dimension"];
		for(i=0;i<4;i++)
			dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
		long size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(float);
		[tempImage setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
	}
	else if([description isEqualToString:@"OutputImage1"])
	{
		exporterML->calcInputImageProps(tempImage, 1);
		[tempImage setObject:[NSString stringWithString:@"char"] forKey:@"ImageType"];
		int dimension[4]={0,0,0,0};
		int i;
		NSArray* dimensionarray=[tempImage objectForKey:@"Dimension"];
		for(i=0;i<4;i++)
			dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
		long size=dimension[0]*dimension[1]*dimension[2]*dimension[3]*sizeof(char);
		[tempImage setObject:[NSNumber numberWithLong:size] forKey:@"MemSize"];
	}
	NSMutableDictionary* savedImage=[imagesManager getImageForDescription: description];
	
	if(savedImage&&[[savedImage objectForKey:@"MemSize"] longValue]==[[tempImage objectForKey:@"MemSize"] longValue])
	{
			[savedImage setObject:[tempImage objectForKey:@"Dimension"] forKey:@"Dimension"];
			[savedImage setObject:[tempImage objectForKey:@"Spacing"] forKey:@"Spacing"];
			[savedImage setObject:[tempImage objectForKey:@"MatrixToPatientCo"] forKey:@"MatrixToPatientCo"];
			[savedImage setObject:[tempImage objectForKey:@"Maximum"] forKey:@"Maximum"];
			[savedImage setObject:[tempImage objectForKey:@"Minimum"] forKey:@"Minimum"];

	}
	else
	{
		[imagesManager creatASharedImage:tempImage ForDescription:description];
	}
	NSMutableDictionary* resultImage=[imagesManager getImageForDescription: description];
	if(resultImage)
	{
		if([description isEqualToString:@"OutputImage0"])
		{
			exporterML->calcInSubImage(resultImage, 0);
		}
		else if([description isEqualToString:@"OutputImage1"])
		{
			exporterML->calcInSubImage(resultImage, 1);
		}

		resultImage=[resultImage mutableCopy];
		[resultImage removeObjectForKey:@"Data"];

	}
	
	
	return resultImage;
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

@end
