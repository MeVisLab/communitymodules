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

#define MeVisLabLoadDICOM_ProtocolVersion 8

@protocol MeVisLabLoadDICOM
- (int) protocolVersion;

- (NSString *) servicesProviderName;
- (NSUInteger) servicesProviderVersion;

- (void) clientIsInitialized;
- (void) loadImagesWithFilenames:(in bycopy NSArray *)images forTargetWithIdentifier:(in OBTargetIdentifier)identifier;
@end
