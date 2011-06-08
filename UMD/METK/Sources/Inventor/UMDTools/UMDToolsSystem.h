//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDToolsSystem.h
// \author  Christian Tietjen
// \date    2004-12-23
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDTOOLSSYSTEM_H__
#define __UMDTOOLSSYSTEM_H__

#ifdef WIN32

#pragma warning (disable : 4505)

// '__thiscall ...' ist keine Inline-Funktion
#pragma warning (disable : 4710)

// d.h. kein UDT oder Verweis auf einen UDT. Loest Fehler aus, wenn mit Infix-Notation verwendet
#pragma warning (disable : 4284)

// '==' unsichere Kombination von Typ 'int' mit Typ 'const bool' in einer Operation
#pragma warning (disable : 4805)

#pragma warning( push, 3 )
#endif

// include MeVis OpenGL headers first (includes GL/glew.h)
#include <mlOpenGL.h>

#include <iostream>
#include <set>
#include <map>
#include <vector>

#ifdef WIN32
#pragma warning( pop )
#endif

// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDTOOLS_EXPORTS
    // Export library symbols.
    #define UMDTOOLS_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDTOOLS_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDTOOLS_EXPORTS
  // Use the UMDTOOLS_EXPORT macro to export classes and functions
  #define UMDTOOLS_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDTOOLS_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDTOOLS_EXPORT
  #endif
#endif

#endif
// __UMDTOOLSSYSTEM_H__
