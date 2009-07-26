//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    CVLAscendingAortaDetectionInit.h
// \author  Stefan Saur
// \date    2007-03-24
//
*/
//----------------------------------------------------------------------------------


#ifndef __CVLAscendingAortaDetectionInit_H
#define __CVLAscendingAortaDetectionInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int CVLAscendingAortaDetectionInit (void);

ML_END_NAMESPACE

#endif // __CVLAscendingAortaDetectionInit_H


