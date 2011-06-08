//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDSoView2DExtensionExampleSystem.h
// \author  Konrad Mühler
// \date    2008-01-07
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__
#define __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORTS
    // Export library symbols.
    #define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORTS
  // Use the UMDVIEW2DEXTENSIONEXAMPLE_EXPORT macro to export classes and functions
  #define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORT
  #endif
#endif

#endif
// __UMDSOVIEW2DEXTENSIONEXAMPLESYSTEM_H__

