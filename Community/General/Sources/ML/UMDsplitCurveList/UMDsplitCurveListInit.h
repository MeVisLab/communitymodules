//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDsplitCurveListInit.h
// \author  Tobias Mönch
// \date    2009-08-05
*/
//----------------------------------------------------------------------------------


#ifndef __UMDsplitCurveListInit_H
#define __UMDsplitCurveListInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int UMDsplitCurveListInit ();

ML_END_NAMESPACE

#endif // __UMDsplitCurveListInit_H


