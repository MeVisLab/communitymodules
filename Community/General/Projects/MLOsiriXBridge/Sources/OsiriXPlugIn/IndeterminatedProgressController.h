//
//  IndeterminatedProgressController.h
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  Copyright 2011 Felix Ritter. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IndeterminatedProgressPanel : NSPanel
{
}
- (BOOL) canBecomeKeyWindow;
@end

@interface IndeterminatedProgressController : NSWindowController <NSWindowDelegate>
{
  IBOutlet NSProgressIndicator *mProgress;
  IBOutlet NSTextField *mMessage;
  IBOutlet NSTextField *mInfo;
}

- (IBAction) showWindow:(id)sender;
- (void) setMessage:(NSString *)msg;
- (void) setInfo:(NSString *)info;
- (void) close;

@end
