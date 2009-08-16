//
//  PlugProxyForMeVisLab.m
//  MeVisLabHub
//
//  Created by chuwa on 8/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "PlugProxyForMeVisLab.h"


@implementation PlugProxyForMeVisLab
#pragma mark-
#pragma mark  initialize and dealloc
-(id)initWithViewer:(ViewerController*)viewer
{
	self = [super init];
	
	if(self)
	{
		osrixViewController=viewer;
		// Vending services
		if ((incomingConnection = [[NSConnection alloc] init]))
		{
			[incomingConnection setRootObject:self];
			if ([incomingConnection registerName:@"OsiriXMeVisLabHub"] == YES) 
			{
				std::cout << "Successfully registered MeVisLabOsiriXSharedMemoryLoader service" << std::endl;
			}
			else 
			{
				std::cout << "Cound not register MeVisLabOsiriXSharedMemoryLoader service" << std::endl;
			}
		}
		// Notification observer for a lost connection
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(connectionDidDie:)
													 name:NSConnectionDidDieNotification
												   object:incomingConnection];
		
		
		sharedMemoryArray=[[NSMutableArray alloc] initWithCapacity:0];
		
		
	}
	
	return self;
}
-(void) dealloc
{
	[incomingConnection invalidate];
	[incomingConnection release];
	incomingConnection = nil;
	if([sharedMemoryArray count])
	{
		[self releaseAllImages];
		[sharedMemoryArray removeAllObjects];
	}
	
	[super dealloc];
}
#pragma mark-
#pragma mark  Distributed Object Connection
-(BOOL) conntactMeVisLabWithOperationName:(NSString*)operationName
{
	// Check for existing connection
	if (MeVisLabExporterProxy != nil) {
		[self disconnectProxy];
	}
	
	// Look for service on local host only
	MeVisLabExporterProxy = [[NSConnection rootProxyForConnectionWithRegisteredName:@"OsiriXMeVisLabHub" host:nil] retain];
	
	if (osirixMevisPluginProxy == nil) {
		std::cout << "Couldn't connect to OsiriX" << std::endl;
		
		//check if "the" network of mevis is running, if not start it otherwise give a warning
		
		return NO;
	}
	
	std::cout << "Connection to OsiriX established" << std::endl;
	// Set connection timeouts
	outgoingConnection = [osirixMevisPluginProxy connectionForProxy];
	[outgoingConnection setRequestTimeout:5];
	[outgoingConnection setReplyTimeout:30];
	
	// Notification observer for a lost connection
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(connectionDidDie:)
												 name:NSConnectionDidDieNotification
											   object:outgoingConnection];
	
	// Set protocol for the remote object
	[osirixMevisPluginProxy setProtocolForProxy:@protocol(MeVisOsiriXProxyProtocol)];
	
	[self getImagesFromOsiriX];
	
	return YES;
}
-(void) disconnectProxy
{
	// Send NSConnectionDidDieNotification to connected observers
	[outgoingConnection invalidate];
	[outgoingConnection release];
	outgoingConnection = nil;
	if(osirixMevisPluginProxy)
	{
		[osirixMevisPluginProxy release];
		osirixMevisPluginProxy=nil;
	}
	std::cout << "Unregistered OsiriXImporter service" << std::endl;
	
	
}

- (void) connectionDidDie: (NSNotification *)notification
{
	id aconnection=[notification object];
	if(aconnection==outgoingConnection)
	{
		if([sharedMemoryArray count])
		{
			unsigned i;
			for(i=0;i<[sharedMemoryArray count];i++)
				importerML->setInputImage(i,0L,0,0L, 0L,0L,0,0);
			[self releaseAllImages];
		}
		outgoingConnection=nil;
		if(osirixMevisPluginProxy)
		{
			[osirixMevisPluginProxy release];
			osirixMevisPluginProxy=nil;
		}
		
	}
	
}
#pragma mark-
#pragma mark  Handle input&outputImages
- (BOOL)setOperation:(inout bycopy NSDictionary *)request
{
	
}

#pragma mark-
#pragma mark  Memory Management function
-(void)releaseAllImages
{
	unsigned i=0;
	for(i=0;i<[sharedMemoryArray count];i++)
	{
		NSMutableDictionary* apiece=[sharedMemoryArray objectAtIndex:i];
		NSString* strid=[apiece objectForKey:@"SharedMemoryID"];
		if(strid)
		{
			[self releaseSharedVolume:apiece];
		}
	}
	
}
-(void) releaseSharedVolume:(NSMutableDictionary*)apiece
{
	
	NSData* aVolume = [apiece objectForKey:@"Data"];
	void* sharedVolumePtr=(void*) [aVolume bytes];
	long sharedVolumeSize=[aVolume length];
	if(sharedVolumePtr&&sharedVolumeSize>0)
	{
		@try
		{
			munmap(sharedVolumePtr,sharedVolumeSize);
		}
		@catch( NSException *ne)
		{
			NSLog(@"failed to unmap shared memory. Memory Leaking");
		}
	}
	[apiece removeObjectForKey:@"SharedMemoryID"];
	[apiece removeObjectForKey:@"Data"];
	
	
}

- (void*) createSharedVolume:(long)size:(NSString*)memidstr
{
	int err=1;
	int fd = -1;
	size_t sz = size;
	const char* memid;
	void* shmAddress;
	
	memid = [memidstr UTF8String]; 
	fd = shm_open( memid, O_RDWR | O_CREAT, FILE_MODE );
	//	NSLog(@"shm_open fd = %d\n", fd );
	ftruncate(fd, sz);
	if ( fd >= 0 )
	{
		@try
		{
			shmAddress = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0 );
		}
		@catch( NSException *ne)
		{
			std::cout << "failed to map shared memory." << std::endl;
			
		}
		if ( (long)shmAddress != -1 )
		{
			NSLog(@"mmap worked, shmAddress = 0x%x\n", shmAddress );
			err=0;
		}
		else
			std::cout << "mmap failed, errno = " << errno << std::endl;
		close(fd);
	}
	else
		std::cout << "mmap failed, errno = " << errno << std::endl;
	
	
	unlink(memid);
	if(err)
		return nil;
	else
		return shmAddress;
	
	
}


@end
