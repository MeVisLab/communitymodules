//
//  ToolPopUpButton.m
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  This code is in the public domain.
//

#import "ToolPopUpButton.h"

#if !defined(MAC_OS_X_VERSION_10_12) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12

static const NSCompositingOperation NSCompositingOperationSourceOver = NSCompositeSourceOver;
static const NSCompositingOperation NSCompositingOperationPlusDarker = NSCompositePlusDarker;

static const NSControlSize NSControlSizeRegular = NSRegularControlSize;
static const NSControlSize NSControlSizeSmall = NSSmallControlSize;

#endif

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
    controlSize = NSControlSizeRegular;
  }
  return self;
}

- (id)initWithCoder:(NSCoder*)coder
{	
  self = [super initWithCoder:coder];
  if (self) {
    [self setPullsDown:YES];
    [[self cell] setBordered:NO];
    controlSize = NSControlSizeRegular;
  }
  return self;
}

- (void) dealloc
{
  [iconImage release];
  [arrowImage release];

  [super dealloc];
}

- (void)setControlSize:(NSControlSize)newSize
{
  controlSize = newSize;
  
  [self setFrameSize:(controlSize == NSControlSizeSmall) ? NSMakeSize(24,24) : NSMakeSize(32,32)];
  
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
  NSRect canvasRect = [self bounds];
  // canvasRect.size = (controlSize == NSControlSizeSmall) ? NSMakeSize(24,24) : NSMakeSize(32,32);
  
  NSRect arrowRect = NSZeroRect;
  arrowRect.size = [arrowImage size];
  arrowRect.origin = NSMakePoint(canvasRect.size.width - arrowRect.size.width, canvasRect.size.height - arrowRect.size.height);
  
  NSRect iconRect = NSZeroRect;
  NSSize iconImgSize = [iconImage size];
  iconRect.size = NSMakeSize(MIN(canvasRect.size.width, iconImgSize.width), MIN(canvasRect.size.height, iconImgSize.height));
  iconRect.origin = NSMakePoint(MAX(0, canvasRect.size.width - iconRect.size.width - arrowRect.size.width)/2, (canvasRect.size.height - iconRect.size.height)/2);
  
  NSCompositingOperation compositing = ([self isHighlighted]) ? NSCompositingOperationPlusDarker : NSCompositingOperationSourceOver;
  [arrowImage drawInRect:arrowRect fromRect:NSZeroRect operation:NSCompositingOperationSourceOver fraction:1.0 respectFlipped:YES hints:NULL];
  [iconImage  drawInRect:iconRect  fromRect:NSZeroRect operation:compositing                      fraction:1.0 respectFlipped:YES hints:NULL];
}

@end
