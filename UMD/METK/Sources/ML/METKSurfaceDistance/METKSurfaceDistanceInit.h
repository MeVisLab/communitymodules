//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKSurfaceDistanceInit.h
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKSURFACEDISTANCESYSTEM_H
#define __METKSURFACEDISTANCESYSTEM_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int METKSurfaceDistanceInit (void);

ML_END_NAMESPACE

#endif // __METKMessagingInit_H


