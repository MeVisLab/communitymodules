//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  Axel Newe
// \date    2015-12-03
*/
//----------------------------------------------------------------------------------


#pragma once


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLWEMColorizeNodesInit();

ML_END_NAMESPACE
