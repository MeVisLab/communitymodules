//
//  mlEatDicom.m
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 10.07.11.
//  This code is in the public domain.
//

#import "mlEatDicom.h"

// Filename of 'eatDicom' executable
#define EATDICOM  @"eatDicom"


@interface EatDicom (Private)
- (void) transferCleanup;
- (void) getTaskOutput: (NSNotification *)notification;
- (BOOL) fireUpEatDicomOnImages: (NSArray *)images;
@end


@implementation EatDicom

- (id)init
{
  self = [super init];
  if (self) {
    // Initialization code here.
  }
  
  return self;
}

- (void)dealloc
{
  [self transferCleanup];
  
  [super dealloc];
}

- (void)transferCleanup
{
  [[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadCompletionNotification object:nil];
  
  // Not sure if termination is really required before releasing this object
  @try {
    [task terminate];
  }
  @catch (NSException *e) {
    // If the receiver has not been launched yet, this method raises an NSInvalidArgumentException.
    if (![[e name] isEqualToString:NSInvalidArgumentException]) {
      [e raise];
    }
  }
  [task release];
  task = nil;
  
  [taskOutput release];
  taskOutput = nil;
  
  [DICOMFilename release];
  DICOMFilename = nil;
}

- (void)getTaskOutput:(NSNotification *)notification
{
  NSData *data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
  // If the length of the data is zero, then the task is over
  if ([data length]) {
    [taskOutput appendString:[[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]];
  }
  
  // Schedule the file handle go read more data in the background again
  [[notification object] readInBackgroundAndNotify]; 
}

- (BOOL)importImages:(NSArray *)images
{
  BOOL success = NO;
  
  [self transferCleanup];
  
  task = [[NSTask alloc] init];
  taskOutput = [[NSMutableString alloc] init];
  
  [task setStandardOutput: [NSPipe pipe]];
  [task setStandardError: [task standardOutput]];
  
  [[NSNotificationCenter defaultCenter] addObserver:self 
                                           selector:@selector(getTaskOutput:) 
                                               name:NSFileHandleReadCompletionNotification 
                                             object:[[task standardOutput] fileHandleForReading]];
  
  [[[task standardOutput] fileHandleForReading] readInBackgroundAndNotify];
  
  // Locate eatDicom tool
  const char *envPath = getenv("PATH");
  if (!envPath) {
    NSLog (@"Environment variable PATH does not exist, unable to locate %@!", EATDICOM);
  }
  else {
    NSString *eatDicomPath = nil;
    NSArray *paths = [[NSString stringWithUTF8String:envPath] componentsSeparatedByString:@":"];
    for (NSString *path in paths) {
      NSString *execPath = [NSString pathWithComponents:[NSArray arrayWithObjects:[path stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]],EATDICOM,nil]];
      if ([[NSFileManager defaultManager] fileExistsAtPath:execPath]) {
        eatDicomPath = execPath; break;
      }
    }
    
    if (!eatDicomPath) {
      NSLog (@"%@ could not be located!\nPATH = %s", EATDICOM, envPath);
    }
    else {
      [task setLaunchPath: eatDicomPath];
      
      NSMutableArray *arguments = [[NSMutableArray alloc] init];
      
      for (id image in images) {
        NSLog (@"%@", image);
        [arguments addObject:@"-i"];
        [arguments addObject:image];
      }
      
      [arguments addObject:@"-od"];
      if (DICOMStoreFolder == nil || [DICOMStoreFolder length] == 0) {
        [self setDICOMStoreFolder:NSTemporaryDirectory()];
        if (DICOMStoreFolder == nil) {
          [self setDICOMStoreFolder:@"/tmp"];
        }
      }
      [arguments addObject:DICOMStoreFolder];
      [arguments addObject:@"-v1"];  // verbose mode 1 (output DICOM info file name)
      if (EatDicomOptions != nil && [EatDicomOptions length] > 0) {
        NSArray *opts = [[EatDicomOptions stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
        for (id opt in opts) {
          if ([opt length] > 0) {
            [arguments addObject:opt];
          }
        }
      }
      [task setArguments: arguments];
      [arguments release];
      
      // Launch eatDicom
      [task launch];
      [task waitUntilExit];
      
      int status = [task terminationStatus];
      if (status == 0) {
        NSLog(@"Import succeeded");
        
        // Read task output to get generated DICOM file name
        NSRange startLoc = [taskOutput rangeOfString:@"save DICOM info to <"];
        if (startLoc.location != NSNotFound) {
          NSRange endLoc = [taskOutput rangeOfString:@">" options:0 range:NSMakeRange(startLoc.location+startLoc.length,[taskOutput length]-(startLoc.location+startLoc.length))];
          if (endLoc.location != NSNotFound) {
            DICOMFilename = [[taskOutput substringWithRange:NSMakeRange(startLoc.location+startLoc.length,endLoc.location-(startLoc.location+startLoc.length))] retain];
          }
        }
        
        success = YES;
      }
      else {
        NSLog(@"Import failed\n%@", taskOutput);
        
        success = NO;
      }
    }
  }
  
  return success;
}

@synthesize DICOMFilename;
@synthesize DICOMStoreFolder;
@synthesize EatDicomOptions;
@synthesize taskOutput;

@end
