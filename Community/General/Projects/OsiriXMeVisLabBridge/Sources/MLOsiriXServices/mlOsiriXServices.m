/*
 *  mlOsiriXServices.m
 *  MLOsiriXServices
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import "mlOsiriXServices.h"

@interface MLOsiriXServices (Private)
- (void) cleanup;
@end

#pragma GCC diagnostic push
/* disable warning: method 'xyz' in protocol not implemented */
#pragma GCC diagnostic ignored "-Wprotocol"

@implementation MLOsiriXServices

#pragma GCC diagnostic pop

- (void) dealloc
{
  [self cleanup];
  [super dealloc];
}

- (void) cleanup
{
  if (proxy) {
    NSConnection *outgoingConnection = [proxy connectionForProxy];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [outgoingConnection invalidate];
    [proxy release];
    proxy = nil;
  }
}

- (BOOL) connect
{
  // Check for existing connection
  if (proxy != nil) {
    return YES;
  }
  
  // Look for service on local host only
  proxy = [[NSConnection rootProxyForConnectionWithRegisteredName:@"MLOsiriXServices" host:nil] retain];
  
  if (proxy == nil) {
    [self cleanup];
    
    [NSException raise:NSObjectInaccessibleException
                format:@"Couldn't connect to MLOsiriXServices. Did you start OsiriX?"];
    
    return NO;
  }
  
  NSLog(@"Established connection to MLOsiriXServices");
  
  // Set connection timeouts
  NSConnection *outgoingConnection = [proxy connectionForProxy];
  [outgoingConnection setRequestTimeout:5];
  [outgoingConnection setReplyTimeout:5];
  
  // Notification observer for a lost connection
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(connectionDidDie:)
                                               name:NSConnectionDidDieNotification
                                             object:outgoingConnection];
  
  // Set protocol for the remote object
  [proxy setProtocolForProxy:@protocol(MLOsiriXServices)];
  
  if ([proxy respondsToSelector:@selector(protocolVersion)]) {
    int protocolVersion = [proxy protocolVersion];
    if (protocolVersion != MLOsiriXServices_ProtocolVersion) {
      [self cleanup];
      
      [NSException raise:NSInternalInconsistencyException
                  format:@"Communication protocol version mismatch. Expected MLOsiriXServices protocol version %d, but got version %d", MLOsiriXServices_ProtocolVersion, protocolVersion];
      
      return NO;
    }
  }

  if ([proxy respondsToSelector:@selector(servicesProviderAppName)]) {
    NSString *servicesProviderAppName = [proxy servicesProviderAppName];
    if ([proxy respondsToSelector:@selector(servicesProviderAppVersion)]) {
      NSString *servicesProviderAppVersion = [proxy servicesProviderAppVersion];
      NSLog(@"Connected to %@ %@", servicesProviderAppName, servicesProviderAppVersion);
    } else {
      NSLog(@"Connected to %@", servicesProviderAppName);
    }
  }
  
  return YES;
}

- (void) connectionDidDie: (NSNotification *)notification
{
#pragma unused ( notification )
  NSLog(@"MLOsiriXServices connection died");
  
  [self cleanup];
}

- (NSMethodSignature *) methodSignatureForSelector: (SEL)selector
{
  if ([self connect] && [proxy respondsToSelector:selector]) {
    return [proxy methodSignatureForSelector:selector];
  } else {
    return [super methodSignatureForSelector:selector];
  }	
}

- (void) forwardInvocation: (NSInvocation *)anInvocation
{
  if ([self connect] && [proxy respondsToSelector:[anInvocation selector]]) {
    [anInvocation invokeWithTarget:proxy];
  } else {
    [super forwardInvocation:anInvocation];
  }
}

@end
