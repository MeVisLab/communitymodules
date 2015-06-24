//
//  mlEatDicom.h
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 10.07.11.
//  This code is in the public domain.
//

#import <Foundation/Foundation.h>

@interface EatDicom : NSObject {
@private
  NSTask *task;
  NSString *DICOMStoreFolder;
  NSString *EatDicomOptions;
  NSString *DICOMFilename;
  NSMutableString *taskOutput;
}

@property (copy) NSString *DICOMStoreFolder;
@property (copy) NSString *EatDicomOptions;
@property (readonly) NSString *DICOMFilename;
@property (readonly, getter=eatDicomOutput) NSString *taskOutput;

- (BOOL)importImages:(NSArray *)images;

@end
