//
//  MeVisLabScriptingBridge.h
//  MeVisLabHub
//
//  Created by Felix Ritter on 24.08.09.
//  Copyright 2009 Fraunhofer MEVIS - Institute for Medical Image Computing. All rights reserved.
//

#import <ScriptingBridge/ScriptingBridge.h>

// Generated with:  sdef /Applications/MeVisLab.app | sdp -fh --basename MeVisLab
#import "MeVisLab.h"


@interface MeVisLabScriptingBridge : NSObject {
	MeVisLabApplication *MeVisLab;
}
- (BOOL)openNetworkFile:(NSString *)filename;
- (BOOL)closeNetwork:(NSString *)filename;
@end
