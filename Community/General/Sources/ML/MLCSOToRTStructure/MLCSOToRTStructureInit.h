//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCSOToRTStructureInit.h
// \author  linal
// \date    2010-01-20
*/
//----------------------------------------------------------------------------------


#ifndef __MLCSOToRTStructureInit_H
#define __MLCSOToRTStructureInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLCSOToRTStructureInit ();

ML_END_NAMESPACE

#endif // __STUCSOToRTStructureInit_H


