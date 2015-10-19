//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKUndoInit.h
// \author  kmuehler
// \date    2008-06-19
*/
//----------------------------------------------------------------------------------


#ifndef __METKUndoInit_H
#define __METKUndoInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int METKUndoInit ();

ML_END_NAMESPACE

#endif // __METKUndoInit_H


