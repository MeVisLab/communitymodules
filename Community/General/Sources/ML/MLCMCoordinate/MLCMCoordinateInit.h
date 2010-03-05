//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMCoordinateInit.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCoordinateInit_H
#define __MLCMCoordinateInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int MLCMCoordinateInit (void);

ML_END_NAMESPACE

#endif // __MLCMCoordinateInit_H


