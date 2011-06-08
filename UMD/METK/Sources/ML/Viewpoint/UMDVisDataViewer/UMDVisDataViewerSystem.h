//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDVisDataViewerSystem.h
// \author
// \date    2007-04-26
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVISDATAVIEWERSYSTEM_H__
#define __UMDVISDATAVIEWERSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDVISDATAVIEWER_EXPORTS
    // Export library symbols.
    #define UMDVISDATAVIEWER_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDVISDATAVIEWER_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDVISDATAVIEWER_EXPORTS
  // Use the UMDVISDATAVIEWER_EXPORT macro to export classes and functions
  #define UMDVISDATAVIEWER_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDVISDATAVIEWER_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDVISDATAVIEWER_EXPORT
  #endif
#endif

#endif
// __UMDVISDATAVIEWERSYSTEM_H__

