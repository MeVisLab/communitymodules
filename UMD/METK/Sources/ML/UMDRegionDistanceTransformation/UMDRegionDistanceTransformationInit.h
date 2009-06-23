//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDRegionDistanceTransformationInit.h
// \author  Konrad Muehler
// \date    2009-03-17
*/
//----------------------------------------------------------------------------------


#ifndef __UMDRegionDistanceTransformationInit_H
#define __UMDRegionDistanceTransformationInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDRegionDistanceTransformationInit ();

ML_END_NAMESPACE

#endif // __UMDRegionDistanceTransformationInit_H


