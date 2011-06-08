//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDRegionDistanceTransformationSystem.h
// \author  Konrad Muehler
// \date    2009-03-17
*/
//----------------------------------------------------------------------------------


#ifndef __UMDRegionDistanceTransformationSystem_H
#define __UMDRegionDistanceTransformationSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDREGIONDISTANCETRANSFORMATION_EXPORTS
    // Export library symbols.
    #define UMDREGIONDISTANCETRANSFORMATION_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDREGIONDISTANCETRANSFORMATION_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef UMDREGIONDISTANCETRANSFORMATION_EXPORTS
  // Use the UMDREGIONDISTANCETRANSFORMATION_EXPORT macro to export classes and functions
  #define UMDREGIONDISTANCETRANSFORMATION_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define UMDREGIONDISTANCETRANSFORMATION_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define UMDREGIONDISTANCETRANSFORMATION_EXPORT
  #endif
#endif

#endif // __UMDRegionDistanceTransformationSystem_H


