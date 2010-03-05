//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMCurveListInit.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCurveListInit_H
#define __MLCMCurveListInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int MLCMCurveListInit (void);

ML_END_NAMESPACE

#endif // __MLCMCurveListInit_H


