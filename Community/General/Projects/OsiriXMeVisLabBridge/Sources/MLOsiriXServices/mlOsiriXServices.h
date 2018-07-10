/*
 *  mlOsiriXServices.h
 *  MLOsiriXServices
 *
 *  Created by Felix Ritter on 18.01.08.
 *  This code is in the public domain.
 */

#import <Foundation/Foundation.h>
#import "mlOsiriXServicesProtocol.h"

ML_LIBRARY_EXPORT_ATTRIBUTE
@interface MLOsiriXServices : NSObject <MLOsiriXServices> {
@private
  id proxy;
}

/*
 *   All messages of the <MLOsiriXServices> protocol are forwarded via proxy
 */

@end
