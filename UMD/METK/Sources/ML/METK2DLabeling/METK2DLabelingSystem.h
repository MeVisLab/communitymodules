//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METK2DLabelingSystem.h
// \author  Konrad Mühler
// \date    2008-04-11
*/
//----------------------------------------------------------------------------------


#ifndef __METK2DLabelingSystem_H
#define __METK2DLabelingSystem_H

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef METK2DLABELING_EXPORTS
    // Export library symbols.
    #define METK2DLABELING_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define METK2DLABELING_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef METK2DLABELING_EXPORTS
  // Use the METK2DLABELING_EXPORT macro to export classes and functions
  #define METK2DLABELING_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define METK2DLABELING_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define METK2DLABELING_EXPORT
  #endif
#endif

#endif // __METK2DLabelingSystem_H


