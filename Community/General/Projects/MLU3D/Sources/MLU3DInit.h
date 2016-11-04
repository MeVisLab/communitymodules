//----------------------------------------------------------------------------------
// Dynamic library and runtime type system initialization.
//
// \file    MLU3DInit.h 
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------

// Add to library path:
// C:\Program Files\Microsoft SDKs\Windows\v7.1\Lib\x64

#ifndef _MLU3DInit_H
#define _MLU3DInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLU3DInit();

ML_END_NAMESPACE

#endif // _MLU3DInit_H
