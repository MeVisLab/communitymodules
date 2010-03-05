//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLXMarkerListCommunityModulesInit.h
// \author  Coert Metz
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLXMarkerListCommunityModulesInit_H
#define __MLXMarkerListCommunityModulesInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLXMarkerListCommunityModulesInit ();

ML_END_NAMESPACE

#endif // __MLXMarkerListCommunityModulesInit_H


