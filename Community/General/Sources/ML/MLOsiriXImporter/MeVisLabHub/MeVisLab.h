/*
 * MeVisLab.h
 */

#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>


@class MeVisLabApplication, MeVisLabDocument, MeVisLabApplication;



/*
 * Standard Suite
 */

// The application's top-level scripting object.
@interface MeVisLabApplication : SBApplication

- (SBElementArray *) documents;

@property (copy, readonly) NSString *name;  // The name of the application.
@property (readonly) BOOL frontmost;  // Is this the frontmost (active) application?
@property (copy, readonly) NSString *version;  // The version number of the application.

- (void) open:(NSArray *)x;  // Open a document.
- (void) quit;  // Quit the application.
- (BOOL) exists:(id)x;  // Verify that an object exists.
- (id) runMacro:(NSString *)x withParameters:(id)withParameters withParameter:(id)withParameter;  // Run a macro module by calling the scripting function assigned to the consoleCommand tag in the Commands section of the module declaration.

@end

// A network document.
@interface MeVisLabDocument : SBObject

@property (copy, readonly) NSString *name;  // The document's name.
@property (copy, readonly) NSString *path;  // The document's path.

- (void) close;  // Close a document.

@end



/*
 * MeVisLab Suite
 */

// The application's top-level scripting object.
@interface MeVisLabApplication (MeVisLabSuite)

- (SBElementArray *) documents;

@property (copy, readonly) NSArray *licenseFeatures;  // The license features of the application.

@end

