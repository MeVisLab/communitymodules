/*
 *  mlMeVisLabServicesProtocol.h
 *  MLOsiriXBridge / OsiriXMeVisLabBridge
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import <Foundation/NSArray.h>

#ifndef OB_TARGET_IDENTIFIER_TYPE
#define OB_TARGET_IDENTIFIER_TYPE 1
typedef unsigned long long OBTargetIdentifier;
#endif

#define MLMeVisLabServices_ProtocolVersion 8

@protocol MLMeVisLabServices
@required
- (int) protocolVersion;

- (NSString *) servicesProviderName;
- (NSUInteger) servicesProviderVersion;

- (void) clientIsInitialized;

@optional
// CFBundleName
- (NSString *) servicesProviderAppName;
// CFBundleShortVersionString
- (NSString *) servicesProviderAppVersion;
// CFBundleIdentifier
- (NSString *) servicesProviderAppId;
@end


#define MLMeVisLabLoadDICOMServices_ProtocolVersion 8

@protocol MLMeVisLabLoadDICOMServices <MLMeVisLabServices>
@required
- (void) loadImagesWithFilenames:(in bycopy NSArray *)images forTargetWithIdentifier:(in OBTargetIdentifier)identifier;
@end
