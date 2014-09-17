//----------------------------------------------------------------------------------
//! Common data types for MLBaseListExtensions modules.
/*!
// \file    MLBaseListExtensions_Tools.h
// \author  Axel Newe
// \date    2014-09-01
//
//
*/
//----------------------------------------------------------------------------------


#ifndef _BaseListExtensions_DataTypes_H
#define _BaseListExtensions_DataTypes_H 

#include "mlModuleIncludes.h"

ML_START_NAMESPACE


//! auto update mode enum
typedef enum {
  AutoUpdateModeOff = 0,
  AutoUpdateModeAutoClear,
  AutoUpdateModeAutoUpdate,
  AutoUpdateModeCount
} AutoUpdateMode;

typedef std::vector<std::string> StringVector;

ML_END_NAMESPACE

#endif // _BaseListExtensions_DataTypes_H