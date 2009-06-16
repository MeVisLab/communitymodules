//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDCodedSegmentationInit.h
// \author  Konrad Mühler
// \date    2007-01-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDCodedSegmentationInit_H
#define __UMDCodedSegmentationInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDCodedSegmentationInit (void);

ML_END_NAMESPACE

#endif // __UMDCodedSegmentationInit_H


