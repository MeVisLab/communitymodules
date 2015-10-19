//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDBoundingBox2Init.h
// \author  Konrad Mühler
// \date    2005-05-10
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDBoundingBox2Init_H
#define __UMDBoundingBox2Init_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDBoundingBox2Init (void);

ML_END_NAMESPACE

#endif // __UMDBoundingBox2Init_H


