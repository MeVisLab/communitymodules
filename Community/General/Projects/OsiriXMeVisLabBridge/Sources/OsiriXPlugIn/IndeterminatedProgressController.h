//
//  IndeterminatedProgressController.h
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  This code is in the public domain.
//

#import <AppKit/AppKit.h>

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
