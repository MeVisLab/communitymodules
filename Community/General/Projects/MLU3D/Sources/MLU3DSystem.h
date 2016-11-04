//----------------------------------------------------------------------------------
// Project global and OS specific declarations.
//
// \file    MLU3DSystem.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _MLU3DSystem_H
#define _MLU3DSystem_H


// DLL export macro definition.
#ifdef MLU3D_EXPORTS
  // Use the MLU3D_EXPORT macro to export classes and functions.
  #define MLU3D_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define MLU3D_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif


#endif // _MLU3DSystem_H
