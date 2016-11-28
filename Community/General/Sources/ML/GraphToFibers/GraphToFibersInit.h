//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    GraphToFibersInit.h
// \author  Axel Newe
// \date    2016-03-31
*/
//----------------------------------------------------------------------------------


#pragma once


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int GraphToFibersInit();

ML_END_NAMESPACE
