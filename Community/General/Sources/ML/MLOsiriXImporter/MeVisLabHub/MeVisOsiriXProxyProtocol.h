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
//- (NSDictionary*)getImage:(NSString*)description;
// The reason to change getImage to setImage is passing NSData in a argument is more stable than in the return value on Mac at leas 10.5
// Although this two function are equivalent. This has caused me 10 hours of debugging. Just don't believe everything the book tells you.
- (void)setImage:(NSDictionary*)anImage ForDescription:(NSString*) description;
@end
