//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDInventorPointerInit.h
// \author  Konrad
// \date    2006-02-02
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDInventorPointerInit_H
#define __UMDInventorPointerInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int UMDInventorPointerInit (void);

ML_END_NAMESPACE

#endif // __UMDInventorPointerInit_H


