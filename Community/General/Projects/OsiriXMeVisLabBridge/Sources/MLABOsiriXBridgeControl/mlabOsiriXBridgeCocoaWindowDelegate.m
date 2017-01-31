/*
 *  mlabOsiriXBridgeCocoaWindowDelegate.m
 *  MLABOsiriXBridgeControl
 *
 *  Created by Felix Ritter on 01.02.10.
 *  This code is in the public domain.
 */

#import "mlabOsiriXBridgeCocoaWindowDelegate.h"

// OsiriX pasteboard type. Must match the type in OsiriX.
static NSString *kPasteBoardOsiriXType = @"OsiriX pasteboard";


@implementation MLABOsiriXBridgeCocoaWindowDelegate

- (id)initWithMLABWindowView:(NSView *)view
{
  if ((self = [super init])) {
    [self setMLABWindowView:view];
  }
  
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

- (void)setMLABWindowView:(NSView *)view
{
  mlabWindowView = view;
  
  // Register as an OsiriX drag destination
  NSMutableArray *registeredDraggedTypes = [NSMutableArray arrayWithArray:[mlabWindowView registeredDraggedTypes]];
  [registeredDraggedTypes addObject:kPasteBoardOsiriXType];
  [mlabWindowView registerForDraggedTypes:registeredDraggedTypes];
}

@end
