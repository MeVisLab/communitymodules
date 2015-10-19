//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDAnimation2Init.h
// \author  Konrad Mühler
// \date    2005-05-04
//
*/
//----------------------------------------------------------------------------------


#ifndef __AnimationInit_H
#define __AnimationInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
void AnimationInit (void);

ML_END_NAMESPACE

#endif // __AnimationInit_H


