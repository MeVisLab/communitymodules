//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METK2DLabelingInit.h
// \author  Konrad Mühler
// \date    2008-04-11
*/
//----------------------------------------------------------------------------------


#ifndef __METK2DLabelingInit_H
#define __METK2DLabelingInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int METK2DLabelingInit ();

ML_END_NAMESPACE

#endif // __METK2DLabelingInit_H


