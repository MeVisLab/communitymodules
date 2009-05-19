//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLTestModulesInit.h
// \author  Wolf Spindler
// \date    2009-05-19
*/
//----------------------------------------------------------------------------------


#ifndef __MLTestModulesInit_H
#define __MLTestModulesInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLTestModulesInit ();

ML_END_NAMESPACE

#endif // __MLTestModulesInit_H


