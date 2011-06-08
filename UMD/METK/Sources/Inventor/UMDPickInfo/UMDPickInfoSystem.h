//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDPickInfoSystem.h
// \author  Stefan Hiller
// \date    2007-09-11
*/
//----------------------------------------------------------------------------------


#ifndef __UMDPICKINFOSYSTEM_H__
#define __UMDPICKINFOSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDPICKINFO_EXPORTS
    // Export library symbols.
    #define UMDPICKINFO_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDPICKINFO_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDPICKINFO_EXPORTS
  // Use the UMDPICKINFO_EXPORT macro to export classes and functions
  #define UMDPICKINFO_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDPICKINFO_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDPICKINFO_EXPORT
  #endif
#endif

#endif
// __UMDPICKINFOSYSTEM_H__

