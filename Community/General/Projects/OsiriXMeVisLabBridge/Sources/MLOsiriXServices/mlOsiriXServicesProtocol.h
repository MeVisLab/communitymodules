/*
 *  mlOsiriXServicesProtocol.h
 *  MLOsiriXServices
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#ifndef OB_TARGET_IDENTIFIER_TYPE
#define OB_TARGET_IDENTIFIER_TYPE 1
typedef unsigned long long OBTargetIdentifier;
#endif

typedef enum {
  MLOSSuccess           =  0,
  MLOSTargetExistsError = -1,
  MLOSUnkownTargetError = -2,
  MLOSUnknownError      = -255
} MLOSStatus;

#define MLOsiriXServices_ProtocolVersion 14

@protocol MLOsiriXServices
@required
- (int) protocolVersion;

- (NSString *) servicesProviderName;
- (NSUInteger) servicesProviderVersion;

- (OBTargetIdentifier) registerTargetWithName:(NSString *)name status:(MLOSStatus *)status;
- (MLOSStatus) unregisterTargetWithIdentifier:(OBTargetIdentifier)identifier;

- (NSString *) osirixIncomingFolderPath;
- (NSDictionary *) seriesInfoOfViewerDisplayingImageSeriesWithFilename:(NSString *)filename;

@optional
// CFBundleName
- (NSString *) servicesProviderAppName;
// CFBundleShortVersionString
- (NSString *) servicesProviderAppVersion;
// CFBundleIdentifier
- (NSString *) servicesProviderAppId;
@end
