//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLDicomSurfaceSegmentationInit.h 
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------


#ifndef __MLDicomSurfaceSegmentationInit_H
#define __MLDicomSurfaceSegmentationInit_H

ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLDicomSurfaceSegmentationInit();

ML_END_NAMESPACE

#endif // __MLDicomSurfaceSegmentationInit_H
