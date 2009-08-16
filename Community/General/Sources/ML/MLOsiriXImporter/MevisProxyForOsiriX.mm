//
//  MevisProxyForOsiriX.m
//  _MLOsiriXImpoter_debug
//
//  Created by chuwa on 6/9/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MevisProxyForOsiriX.h"
#include <sys/mman.h>
#include <sys/fcntl.h>
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)



@implementation MevisProxyForOsiriX

#pragma mark-
#pragma mark  initialize and dealloc
- (id) initWithImporterML:(ml::OsiriXImporter*) aML
{
	if ((self = [super init])) {
	
		sharedMemoryArray=[[NSMutableArray alloc] initWithCapacity:0];
		importerML=aML;
		int i;
		for(i=0;i<OUTPUTPORTNUMBER;i++)
		{
			NSMutableDictionary* apiece=[NSMutableDictionary dictionaryWithCapacity:0];
			[apiece setObject:[NSNumber numberWithInt:i] forKey:@"Index"];
			[sharedMemoryArray insertObject:apiece atIndex:i];
		}
		
		
	}
	return self;
}

- (void) dealloc
{
	
	[[NSNotificationCenter defaultCenter] removeObserver: self];
	[self disconnectProxy];
	//release shared memory
	if([sharedMemoryArray count])
	{
		[self releaseAllImages];
		[sharedMemoryArray removeAllObjects];
	}
	

	[super dealloc];
}
#pragma mark-
#pragma mark  Distributed Object Connection
-(BOOL) tryBuildConnectionWithOsiriX
{
	// Check for existing connection
	if (osirixMevisPluginProxy != nil) {
		[self disconnectProxy];
	}
	
	// Look for service on local host only
	osirixMevisPluginProxy = [[NSConnection rootProxyForConnectionWithRegisteredName:@"OsiriXMeVisLabHub" host:nil] retain];
	
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
-(BOOL) isConnectedWithOsiriX
{
	if(outgoingConnection&&[outgoingConnection isValid])
		return YES;
	else
		return NO;
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
	//	[[NSNotificationCenter defaultCenter] removeObserver: self]; is it necessary to put here?
	

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
#pragma mark  Handle inputImages
-(BOOL) getImagesFromOsiriX
{
	if(osirixMevisPluginProxy)
	{
		NSMutableDictionary* arequest=[NSMutableDictionary dictionaryWithCapacity:0];
		[arequest setObject:[NSString stringWithString:@"GetInputImages"] forKey:@"Operation"];
		BOOL success=[osirixMevisPluginProxy setOperation:arequest];
		if(success)
		{
			NSArray* inputimages=[arequest objectForKey:@"RelatedImages"];
			unsigned i;
			for(i=0;i<[inputimages count];i++)
			{
				[self setImage:[inputimages objectAtIndex:i] ForIndex:i];
			}
		}
	}
	else
		return NO;
	return YES;
}
- (BOOL)setImage:(NSDictionary*)aImageDict ForIndex:(int)imgindex
{
	int dimension[4]={0,0,0,0};
	float spacing[4]={0.0,0.0,0.0,0.0};
	float transformmatrix[16];
	float maxvalue, minvalue;

	if((unsigned)imgindex>=[sharedMemoryArray count])
	{
		return NO;
	}

	NSString* imgtypestr=[aImageDict objectForKey:@"ImageType"];
	if([imgtypestr isEqualToString:@"float"]||[imgtypestr isEqualToString:@"char"])
	{
	
		NSString* memidstr=[aImageDict objectForKey:@"SharedMemoryID"];
		NSString* oldmemidstr=nil;
		if([memidstr isEqualToString:@"NULL"])
		{
			std::cout << "OsiriX Importer: Image for "<< imgindex << " port is empty!" << std::endl;
			return YES;
		}

		oldmemidstr=[[sharedMemoryArray objectAtIndex:imgindex] objectForKey:@"SharedMemoryID"];

		if(oldmemidstr&&[oldmemidstr isEqualToString:memidstr])
		{
			//if the memoryID is the same then the new image is using the same block of shared memory so noting need to be done here
			return YES;
		}
		else
		{	
			//if there is memory allocated for this port already, release the old shared memory
			if(oldmemidstr && (![oldmemidstr isEqualToString:@"null"]))
			{
				
				[self releaseSharedVolume:[sharedMemoryArray objectAtIndex:imgindex]];
				
			}
			
			
			long size=[[aImageDict objectForKey:@"ByteSize"] longValue];
			void* sharedVolumePtr =nil;
			if(size>0)
				sharedVolumePtr=[self createSharedVolume:size:memidstr];
			if(sharedVolumePtr)
			{
				NSData* tempData=[NSData dataWithBytesNoCopy:sharedVolumePtr length:size freeWhenDone:NO];
				NSMutableDictionary* apiece=[sharedMemoryArray objectAtIndex:imgindex];
				[apiece setObject:memidstr forKey:@"SharedMemoryID"];
				[apiece setObject:tempData forKey:@"Data"];
			
				int i;
				NSArray* dimensionarray=[aImageDict objectForKey:@"Dimension"];
				for(i=0;i<4;i++)
					dimension[i]=[[dimensionarray objectAtIndex:i] intValue];
				
				
				NSArray* spacingarray=[aImageDict objectForKey:@"Spacing"];
				for(i=0;i<4;i++)
					spacing[i]=[[spacingarray objectAtIndex:i] floatValue];

				NSArray* matrix=[aImageDict objectForKey:@"MatrixToPatientCo"];
				for(i=0;i<16;i++)
					transformmatrix[i]=[[matrix objectAtIndex:i] floatValue];
				maxvalue=[[aImageDict objectForKey:@"Maximum"] floatValue];
				minvalue=[[aImageDict objectForKey:@"Minimum"] floatValue];
				int imagetype=MLfloatType;
				if([imgtypestr isEqualToString:@"float"])
					imagetype=MLfloatType;
				else if([imgtypestr isEqualToString:@"char"])
					imagetype=MLuint8Type;
				
				importerML->setInputImage(imgindex,sharedVolumePtr,imagetype,dimension, spacing,transformmatrix,maxvalue,minvalue);
				
			}
			else
			{
				std::cout << "OsiriX Importer: Failed to allocate the shared memory" << std::endl;
				importerML->setInputImage(imgindex,0L,0,0L, 0L,0L,0,0);
				return NO;
			}
		}
		//else if the memoryid is the same, we will just use the same memory as last time but tell the importer it is modified.
	}
	else if([imgtypestr isEqualToString:@"ROI"])
	{
	}


	std::cout << "OsiriX Importer: Images updated" << std::endl;

	return YES;
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
