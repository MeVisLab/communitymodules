//
//  ToolPopUpButton.h
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  This code is in the public domain.
//

#import <AppKit/AppKit.h>

@interface ToolPopUpButton : NSPopUpButton {
@private
  NSControlSize controlSize;
  NSImage *iconImage;
  NSImage *arrowImage;
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wproperty-attribute-mismatch"
@property(nonatomic,assign) NSControlSize controlSize;
#pragma GCC diagnostic pop
@property(nonatomic,retain) NSImage *iconImage;
@property(nonatomic,retain) NSImage *arrowImage;
@end
