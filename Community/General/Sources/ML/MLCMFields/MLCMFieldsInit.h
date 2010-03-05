//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLCMFieldsInit.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMFieldsInit_H
#define __MLCMFieldsInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int MLCMFieldsInit (void);

ML_END_NAMESPACE

#endif // __MLCMFieldsInit_H


