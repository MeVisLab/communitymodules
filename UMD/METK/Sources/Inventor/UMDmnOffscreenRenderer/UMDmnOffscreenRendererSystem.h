//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDmnOffscreenRendererSystem.h
// \author  Mathias Neugebauer
// \date    2007-03-30
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDMNOFFSCREENRENDERERSYSTEM_H__
#define __UMDMNOFFSCREENRENDERERSYSTEM_H__

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDMNOFFSCREENRENDERER_EXPORTS
    // Export library symbols.
    #define UMDMNOFFSCREENRENDERER_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDMNOFFSCREENRENDERER_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDMNOFFSCREENRENDERER_EXPORTS
  // Use the UMDMNOFFSCREENRENDERER_EXPORT macro to export classes and functions
  #define UMDMNOFFSCREENRENDERER_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDMNOFFSCREENRENDERER_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDMNOFFSCREENRENDERER_EXPORT
  #endif
#endif

// Enable usage of PBUFFERS only on windows systems
#ifdef WIN32
  #define USE_WINPBUFFER
  #define USE_AVISAVE
#endif


#endif
// __UMDMNOFFSCREENRENDERERSYSTEM_H__

