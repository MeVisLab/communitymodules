// **InsertLicense** code

/*
 _______________________________________________________________________
 __________________  F R A U N H O F E R   M E V I S  __________________
 |
 |   $Rev: 100478 $
 |
 |   Author(s)      : Felix Ritter
 |   Creation date  : 02/2010
 |
 __________________  F R A U N H O F E R   M E V I S  __________________
 _______________________________________________________________________
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
