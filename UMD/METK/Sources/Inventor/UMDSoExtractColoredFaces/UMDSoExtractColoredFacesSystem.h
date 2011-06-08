//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDSoExtractColoredFacesSystem.h
// \author  Konrad Mühler
// \date    2008-02-01
*/
//----------------------------------------------------------------------------------


#ifndef __UMDSOEXTRACTCOLOREDFACESSYSTEM_H__
#define __UMDSOEXTRACTCOLOREDFACESSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDSOEXTRACTCOLOREDFACES_EXPORTS
    // Export library symbols.
    #define UMDSOEXTRACTCOLOREDFACES_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDSOEXTRACTCOLOREDFACES_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDSOEXTRACTCOLOREDFACES_EXPORTS
  // Use the UMDSOEXTRACTCOLOREDFACES_EXPORT macro to export classes and functions
  #define UMDSOEXTRACTCOLOREDFACES_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDSOEXTRACTCOLOREDFACES_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDSOEXTRACTCOLOREDFACES_EXPORT
  #endif
#endif

#endif
// __UMDSOEXTRACTCOLOREDFACESSYSTEM_H__

