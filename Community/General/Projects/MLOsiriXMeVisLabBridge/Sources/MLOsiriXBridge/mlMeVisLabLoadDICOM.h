/*
 *  mlMeVisLabLoadDICOM.h
 *  MLOsiriXBridge
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import "mlMeVisLabServicesProtocol.h"

#import <Foundation/NSObject.h>
#import <Foundation/NSTask.h>
#import <Foundation/NSString.h>
#import <Foundation/NSConnection.h>

@interface MeVisLabLoadDICOM : NSObject <MeVisLabLoadDICOM> {
@private
  NSUInteger managerRetainCount;
  NSConnection *incomingConnection;  
  NSArray *slicesFileList;
}

@property (readonly) NSArray *slicesFileList;

+ (MeVisLabLoadDICOM *) sharedManager;
- (id) retainManager;
- (void) releaseManager;

@end
