//
//  MeVisLabScriptingBridge.h
//  MeVisLabHub
//
//  Created by Felix Ritter on 24.08.09.
//  Copyright 2009 Fraunhofer MEVIS - Institute for Medical Image Computing. All rights reserved.
//

#import <ScriptingBridge/ScriptingBridge.h>


@interface MeVisLabScriptingBridge : NSObject {
}
- (BOOL)openNetworkWithFilename:(NSString *)filename;
- (BOOL)closeNetworkWithFilename:(NSString *)filename;
@end
