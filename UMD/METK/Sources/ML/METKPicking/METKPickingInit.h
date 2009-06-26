//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKPickingInit.h
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKPICKINGINIT_H
#define __METKPICKINGINIT_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int METKPickingInit (void);

ML_END_NAMESPACE

#endif // __METKPICKINGINIT_H


