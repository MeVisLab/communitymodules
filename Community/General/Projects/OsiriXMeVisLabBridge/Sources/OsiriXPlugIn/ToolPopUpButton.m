//
//  ToolPopUpButton.m
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  Copyright 2011 Felix Ritter. All rights reserved.
//

#import "ToolPopUpButton.h"

@implementation ToolPopUpButton

@synthesize controlSize;
@synthesize iconImage;
@synthesize arrowImage;

- (id)initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    [self setPullsDown:YES];
    [[self cell] setBordered:NO];
    controlSize = NSRegularControlSize;
  }
  return self;
}

- (id)initWithCoder:(NSCoder*)coder
{	
  self = [super initWithCoder:coder];
  if (self) {
    [self setPullsDown:YES];
    [[self cell] setBordered:NO];
    controlSize = NSRegularControlSize;
  }
  return self;
}

- (void)setControlSize:(NSControlSize)newSize
{
  controlSize = newSize;
  
  [self setFrameSize:(controlSize == NSSmallControlSize) ? NSMakeSize(24,24) : NSMakeSize(32,32)];
  
  [self setNeedsDisplay];
}

- (void)setIconImage:(NSImage *)image
{
  [iconImage autorelease];
  iconImage = [image retain];
  
  [self setNeedsDisplay];
}

- (void)setArrowImage:(NSImage *)image
{
  [arrowImage autorelease];
  arrowImage = [image retain];
  
  [self setNeedsDisplay];
}

- (void)drawRect:(NSRect)rect
{
  NSRect canvasRect = rect;
  // canvasRect.size = (controlSize == NSSmallControlSize) ? NSMakeSize(24,24) : NSMakeSize(32,32);
  
  NSRect arrowRect = NSZeroRect;
  arrowRect.size = [arrowImage size];
  arrowRect.origin = NSMakePoint(canvasRect.size.width - arrowRect.size.width, canvasRect.size.height - arrowRect.size.height);
  
  NSRect iconRect = NSZeroRect;
  NSSize iconImgSize = [iconImage size];
  iconRect.size = NSMakeSize(MIN(canvasRect.size.width, iconImgSize.width), MIN(canvasRect.size.height, iconImgSize.height));
  iconRect.origin = NSMakePoint(MAX(0, canvasRect.size.width - iconRect.size.width - arrowRect.size.width)/2, (canvasRect.size.height - iconRect.size.height)/2);
  
  NSCompositingOperation compositing = ([[self cell] isHighlighted]) ? NSCompositePlusDarker : NSCompositeSourceOver;
  [arrowImage drawInRect:arrowRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:NULL];
  [iconImage  drawInRect:iconRect  fromRect:NSZeroRect operation:compositing           fraction:1.0 respectFlipped:YES hints:NULL];
}

@end
