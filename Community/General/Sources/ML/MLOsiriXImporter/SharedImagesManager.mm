//
//  SharedImagesManager.m
//  MLOsiriXImporter_debug
//
//  Created by chuwa on 8/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "SharedImagesManager.h"
#include <sys/mman.h>
#include <sys/fcntl.h>
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

@implementation SharedImagesManager
- (id)initWithIDStringPrefix:(NSString*)astring
{
	if ((self = [super init])) 
	{
		allocatedImages=[[NSMutableArray alloc] initWithCapacity:0];
		uniqueMemoryIDStringPrefix=astring;
		uniqueIndex=0;
	}
	return self;
}
- (void)dealloc
{
	[self releaseAllImages];
	[super dealloc];
}
-(void)releaseAllImages
{
	while([allocatedImages count])
	{
		NSMutableDictionary* anImage=[allocatedImages objectAtIndex:0];
		[self removeImage:anImage];
	}	
}
-(void) removeImage:(NSMutableDictionary*)anImage
{
	
	NSData* aVolumeData = [anImage objectForKey:@"Data"];
	if(aVolumeData)
	{
		void* sharedVolumePtr=(void*) [aVolumeData bytes];
		long sharedVolumeSize=[aVolumeData length];
		if(sharedVolumePtr&&sharedVolumeSize>0)
		{
			@try
			{
				if(munmap(sharedVolumePtr,sharedVolumeSize)==-1)
					NSLog(@"failed to unmap shared memory. Memory Leaking");
				if([[anImage objectForKey:@"NeedUnlink"] boolValue]==YES)
				{
					NSString* memidstr=[anImage objectForKey:@"SharedMemoryID"];
					shm_unlink([memidstr UTF8String]);
				}
				
			}
			@catch( NSException *ne)
			{
				NSLog(@"failed to unmap shared memory. Memory Leaking");
			}
		}
	}
	[allocatedImages removeObject:anImage];
	
	
}
- (BOOL)creatASharedImage:(NSMutableDictionary*)newImage ForDescription:(NSString*)description
{
	
	[newImage setObject:description	forKey:@"Description"];
	
	for(NSMutableDictionary* anImage in allocatedImages)
	{
		if([description isEqualToString:[anImage objectForKey:@"Description"]])
		{
			[self removeImage:anImage];
			break;
		}
	}
	
	
	NSString* imageType=[newImage objectForKey:@"ImageType"];
	
	if(![imageType isEqualToString:@"overlay"])
	{
		if(![self allocMemoryBlock:newImage])
			return NO;
		
	}
	[allocatedImages addObject:newImage];
	return YES;
	
}
- (void*)getMemoryPtrForImageDescription:(NSString*)description
{

	NSMutableDictionary* imageMatchsWithDescrpition=nil;
	for(NSMutableDictionary* anImage in allocatedImages)
	{
		if([description isEqualToString:[anImage objectForKey:@"Description"]])
		{
			imageMatchsWithDescrpition=anImage;
			break;
		}
	}
	
	if(imageMatchsWithDescrpition)
	{
		NSData* aVolumeData = [imageMatchsWithDescrpition objectForKey:@"Data"];
		void* sharedMemoryPtr=(void*) [aVolumeData bytes];

		return sharedMemoryPtr;
	}



	return nil;
}
- (NSMutableDictionary*)getImageForDescription:(NSString*)description
{
	
	NSMutableDictionary* imageMatchsWithDescrpition=nil;
	for(NSMutableDictionary* anImage in allocatedImages)
	{
		if([description isEqualToString:[anImage objectForKey:@"Description"]])
		{
			imageMatchsWithDescrpition=anImage;
			break;
		}
	}

	return imageMatchsWithDescrpition;
}

- (BOOL)allocMemoryBlock:(NSMutableDictionary*)anImage;
{
	NSString* memidstr=[anImage objectForKey:@"SharedMemoryID"];
	if(!memidstr)
	{
		memidstr=[NSString stringWithFormat:@"%@:%d",uniqueMemoryIDStringPrefix,uniqueIndex++];
	}
	long size=[[anImage objectForKey:@"MemSize"] longValue];
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
			NSLog(@"failed to map shared memory.");
			
		}
		if ( (long)shmAddress != -1 )
		{
			NSLog(@"mmap worked, shmAddress = 0x%x\n", shmAddress );
			err=0;
		}
		else
			NSLog(@"mmap failed, errno = %d\n", errno );
		close(fd);
	}
	else
		NSLog(@"open failed, errno = %d\n", errno );
	

	if(err)
	{
		return NO;
	}
		
	else
	{
		NSData* tempData=[[NSData alloc] initWithBytesNoCopy:shmAddress length:size freeWhenDone:NO];
		NSData* oldData=[anImage objectForKey:@"Data"];
		if(oldData&&(signed)[oldData length]==size)
		{
			void* oldptr=(void*)[oldData bytes];
			memcpy(shmAddress, oldptr, size);
		}		
		[anImage setObject:tempData forKey:@"Data"];
		[tempData release];
		if([anImage objectForKey:@"SharedMemoryID"]==nil)
		{
			[anImage setObject:memidstr forKey:@"SharedMemoryID"];
			[anImage setObject:[NSNumber numberWithBool:YES] forKey:@"NeedUnlink"];
		}
		else
		{
			[anImage setObject:[NSNumber numberWithBool:NO] forKey:@"NeedUnlink"];
		}
		return YES;
	}
	
	return NO;
}


@end
