//
//  ToolPopUpButton.h
//  OsiriXMeVisLabBridge
//
//  Created by Felix Ritter on 15.05.11.
//  Copyright 2011 Felix Ritter. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ToolPopUpButton : NSPopUpButton {
@private
  NSControlSize controlSize;
  NSImage *iconImage;
  NSImage *arrowImage;
}
@property(nonatomic,assign) NSControlSize controlSize;
@property(nonatomic,retain) NSImage *iconImage;
@property(nonatomic,retain) NSImage *arrowImage;
@end
