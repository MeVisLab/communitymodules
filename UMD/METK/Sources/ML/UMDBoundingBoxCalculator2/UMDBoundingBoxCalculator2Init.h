//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDBoundingBoxCalculator2Init.h
// \author  Konrad
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDBoundingBoxCalculator2Init_H
#define __UMDBoundingBoxCalculator2Init_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDBoundingBoxCalculator2Init (void);

ML_END_NAMESPACE

#endif // __UMDBoundingBoxCalculator2Init_H


