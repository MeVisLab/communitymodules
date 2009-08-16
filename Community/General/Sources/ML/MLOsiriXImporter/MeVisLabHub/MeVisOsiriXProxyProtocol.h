/*
 *  MeVisOsiriXProxyProtocol.h
 *  MeVisOsiriXPlugin
 *
 *  Created by chunliang wang on 6/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#import <Foundation/Foundation.h>

@protocol MeVisOsiriXProxyProtocol
- (void)setOperation:(NSDictionary *)request;
- (NSDictionary*)getImage:(NSString*)description;
@end
