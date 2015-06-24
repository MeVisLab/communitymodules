/*
 *  mlOsiriXServices.h
 *  MLOsiriXServices
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import <Foundation/Foundation.h>
#import "mlOsiriXServicesProtocol.h"

@interface MLOsiriXServices : NSObject <MLOsiriXServices> {
@private
  id proxy;
}

/*
 *   All messages of the <MLOsiriXServices> protocol are forwarded via proxy
 */

@end
