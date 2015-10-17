//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDSoSceneWriterMDInit.h
// \author  Konrad Mühler
// \date    2006-05-15
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSoSceneWriterMDInit_H
#define __UMDSoSceneWriterMDInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDSoSceneWriterMDInit (void);

ML_END_NAMESPACE

#endif // __UMDSoSceneWriterMDInit_H


