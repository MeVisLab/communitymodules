//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    ViewpointInit.h
// \author  Konrad Mühler
// \date    2007-07-26
*/
//----------------------------------------------------------------------------------


#ifndef __ViewpointInit_H
#define __ViewpointInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int ViewpointInit (void);

ML_END_NAMESPACE

#endif // __ViewpointInit_H


