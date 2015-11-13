//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    PDF3DFigurePageInit.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
*/
//----------------------------------------------------------------------------------


#pragma once


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int PDF3DFigurePageInit();

ML_END_NAMESPACE
