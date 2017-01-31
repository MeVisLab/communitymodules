//
//  IndeterminatedProgressController.m
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  This code is in the public domain.
//

#import "IndeterminatedProgressController.h"

@implementation IndeterminatedProgressPanel
- (BOOL) canBecomeKeyWindow
{
  return NO;
}
@end


@implementation IndeterminatedProgressController

- (id) init
{
  if ((self = [super initWithWindowNibName:@"IndeterminatedProgressWindow"])) {
    [[self window] setDelegate:self];
  }
  return self;
}

- (IBAction) showWindow:(id)sender
{
  if (! [[self window] isVisible]) {
    [[self window] center];
    [super showWindow:sender];
    
    [mProgress startAnimation:self];
  }
}

- (void) close
{
  [mProgress stopAnimation:self];
  
  [super close];
}

- (void) setMessage:(NSString *)msg
{
  [mMessage setStringValue:msg];
  [mMessage displayIfNeeded];
}

- (void) setInfo:(NSString *)info
{
  [mInfo setStringValue:info];
  [mInfo displayIfNeeded];
}

@end
