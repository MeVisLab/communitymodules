//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    LabelingInit.h
// \author  Konrad Mühler
// \date    2007-07-13
*/
//----------------------------------------------------------------------------------


#ifndef __LabelingInit_H
#define __LabelingInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int LabelingInit (void);

ML_END_NAMESPACE

#endif // __LabelingInit_H


