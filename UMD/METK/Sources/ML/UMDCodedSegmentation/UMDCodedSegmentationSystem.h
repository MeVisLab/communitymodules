//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDCodedSegmentationSystem.h
// \author  Konrad Mühler
// \date    2007-01-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDCodedSegmentationSystem_H
#define __UMDCodedSegmentationSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDCODEDSEGMENTATION_EXPORTS
    // Export library symbols.
    #define UMDCODEDSEGMENTATION_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDCODEDSEGMENTATION_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDCODEDSEGMENTATION_EXPORTS
  // Use the UMDCODEDSEGMENTATION_EXPORT macro to export classes and functions
  #define UMDCODEDSEGMENTATION_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDCODEDSEGMENTATION_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDCODEDSEGMENTATION_EXPORT
  #endif
#endif

#endif // __UMDCodedSegmentationSystem_H


