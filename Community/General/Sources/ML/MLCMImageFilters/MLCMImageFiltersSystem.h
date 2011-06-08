//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLCMImageFiltersSystem.h
// \author  Theo van Walsum
// \date    2007-08-14
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMImageFiltersSystem_H
#define __MLCMImageFiltersSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLCMIMAGEFILTERS_EXPORTS
    // Export library symbols.
    #define MLCMIMAGEFILTERS_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLCMIMAGEFILTERS_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLCMIMAGEFILTERS_EXPORTS
  // Use the MLCMIMAGEFILTERS_EXPORT macro to export classes and functions
  #define MLCMIMAGEFILTERS_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLCMIMAGEFILTERS_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLCMIMAGEFILTERS_EXPORT
  #endif
#endif

#endif // __MLCMImageFiltersSystem_H


