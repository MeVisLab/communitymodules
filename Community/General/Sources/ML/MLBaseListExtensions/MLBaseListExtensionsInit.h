//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLBaseListExtensionsInit.h
// \author  Axel Newe
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#ifndef __MLBaseListExtensionsInit_H
#define __MLBaseListExtensionsInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLBaseListExtensionsInit();

ML_END_NAMESPACE

#endif // __MLBaseListExtensionsInit_H

