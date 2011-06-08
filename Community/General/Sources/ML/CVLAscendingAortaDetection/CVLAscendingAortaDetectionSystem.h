//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    CVLAscendingAortaDetectionSystem.h
// \author  Stefan Saur
// \date    2007-03-24
//
*/
//----------------------------------------------------------------------------------


#ifndef __CVLAscendingAortaDetectionSystem_H
#define __CVLAscendingAortaDetectionSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef CVLASCENDINGAORTADETECTION_EXPORTS
    // Export library symbols.
    #define CVLASCENDINGAORTADETECTION_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define CVLASCENDINGAORTADETECTION_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else

  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef CVLASCENDINGAORTADETECTION_EXPORTS
  // Use the CVLASCENDINGAORTADETECTION_EXPORT macro to export classes and functions
  #define CVLASCENDINGAORTADETECTION_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define CVLASCENDINGAORTADETECTION_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define CVLASCENDINGAORTADETECTION_EXPORT
  #endif
#endif

#endif // __CVLAscendingAortaDetectionSystem_H


