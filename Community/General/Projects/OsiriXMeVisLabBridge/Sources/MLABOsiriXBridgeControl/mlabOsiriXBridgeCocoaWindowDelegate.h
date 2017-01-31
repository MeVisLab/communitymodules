/*
 *  mlabOsiriXBridgeCocoaWindowDelegate.h
 *  MLABOsiriXBridgeControl
 *
 *  Created by Felix Ritter on 01.02.10.
 *  This code is in the public domain.
 */

#ifndef MLABOSIRIXBRIDGECOCOAWINDOWDELEGATE_H_
#define MLABOSIRIXBRIDGECOCOAWINDOWDELEGATE_H_

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>


@interface MLABOsiriXBridgeCocoaWindowDelegate : NSObject {
@private
  NSView *mlabWindowView;
}

- (id)initWithMLABWindowView:(NSView *)view;
- (void)setMLABWindowView:(NSView *)view;

@end

#endif
