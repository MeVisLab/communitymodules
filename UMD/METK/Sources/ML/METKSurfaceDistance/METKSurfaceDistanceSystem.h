//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKSurfaceDistanceSystem.h
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKSURFACEDISTANCE_H
#define __METKSURFACEDISTANCE_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef METKSURFACEDISTANCE_EXPORTS
    // Export library symbols.
    #define METKSURFACEDISTANCE_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define METKSURFACEDISTANCE_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef METKSURFACEDISTANCE_EXPORTS
  // Use the METKMESSAGING_EXPORT macro to export classes and functions
  #define METKSURFACEDISTANCE_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define METKSURFACEDISTANCE_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define METKSURFACEDISTANCE_EXPORT
  #endif
#endif

#endif // __METKSURFACEDISTANCE_H


