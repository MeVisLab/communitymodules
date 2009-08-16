//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLOsiriXImporterInit.h
// \author  Chunliang Wang
// \date    2009-07-07
*/
//----------------------------------------------------------------------------------


#ifndef __MLOsiriXImporterInit_H
#define __MLOsiriXImporterInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLOsiriXImporterInit ();

ML_END_NAMESPACE

#endif // __MLOsiriXImporterInit_H


