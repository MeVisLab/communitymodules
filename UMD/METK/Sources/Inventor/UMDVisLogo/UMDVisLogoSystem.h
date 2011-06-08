//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisLogoSystem.h
// \author  Konrad Mühler
// \date    2007-06-12
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISLOGOSYSTEM_H__
#define __UMDVISLOGOSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDVISLOGO_EXPORTS
    // Export library symbols.
    #define UMDVISLOGO_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDVISLOGO_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDVISLOGO_EXPORTS
  // Use the UMDVISLOGO_EXPORT macro to export classes and functions
  #define UMDVISLOGO_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDVISLOGO_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDVISLOGO_EXPORT
  #endif
#endif

#endif
// __UMDVISLOGOSYSTEM_H__

