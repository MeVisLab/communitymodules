//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMCoordinateSystem.h
// \author  Reinhard Hameeteman
// \date    2010-03-05
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMCoordinateSystem_H
#define __MLCMCoordinateSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCMCOORDINATE_EXPORTS
    // Export library symbols.
    #define MLCMCOORDINATE_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCMCOORDINATE_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCMCOORDINATE_EXPORTS
  // Use the MLCMCoordinate_EXPORT macro to export classes and functions
  #define MLCMCOORDINATE_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLCMCOORDINATE_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLCMCOORDINATE_EXPORT
  #endif
#endif

#endif // __MLCMCoordinateSystem_H


