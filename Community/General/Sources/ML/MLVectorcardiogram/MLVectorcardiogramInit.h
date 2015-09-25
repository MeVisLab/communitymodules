//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLVectorcardiogramInit.h
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------


#ifndef __MLVectorcardiogramInit_H
#define __MLVectorcardiogramInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLVectorcardiogramInit();

ML_END_NAMESPACE

#endif // __MLVectorcardiogramInit_H
