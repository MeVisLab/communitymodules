//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLPDFInit.h 
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-02-06
*/
//----------------------------------------------------------------------------------

// Add to library path:
// C:\Program Files\Microsoft SDKs\Windows\v7.1\Lib\x64

#ifndef _MLPDFInit_H
#define _MLPDFInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLPDFInit();

ML_END_NAMESPACE

#endif // _MLPDFInit_H
