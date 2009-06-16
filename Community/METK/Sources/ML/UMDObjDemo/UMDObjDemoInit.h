//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDObjDemoInit.h
// \author  Konrad Mühler
// \date    2005-04-22
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDObjDemoInit_H
#define __UMDObjDemoInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDObjDemoInit (void);

ML_END_NAMESPACE

#endif // __UMDObjDemoInit_H


