// **InsertLicense** code

/*
 _______________________________________________________________________
 __________________  F R A U N H O F E R   M E V I S  __________________
 |
 |   $Rev: 100478 $
 |
 |   Author(s)      : Felix Ritter
 |   Creation date  : 02/2010
 |
 __________________  F R A U N H O F E R   M E V I S  __________________
 _______________________________________________________________________
 */

#import "mlabOsiriXBridgeCocoaWindowDelegate.h"

// OsiriX pasteboard type. Must match the type in OsiriX.
static NSString *kPasteBoardOsiriXType = @"OsiriX pasteboard";


@implementation MLABOsiriXBridgeCocoaWindowDelegate

- (id)initWithMLABWindowView:(NSView *)view
{
  if ((self = [super init])) {
    [self setMLABWindowView:view];
  }
  
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

- (void)setMLABWindowView:(NSView *)view
{
  mlabWindowView = view;
  
  // Register as an OsiriX drag destination
  NSMutableArray *registeredDraggedTypes = [NSMutableArray arrayWithArray:[mlabWindowView registeredDraggedTypes]];
  [registeredDraggedTypes addObject:kPasteBoardOsiriXType];
  [mlabWindowView registerForDraggedTypes:registeredDraggedTypes];
}

@end
