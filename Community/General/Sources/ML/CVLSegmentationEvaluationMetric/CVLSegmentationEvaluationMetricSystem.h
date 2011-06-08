//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    CVLSegmentationEvaluationMetricSystem.h
// \author  Stefan Saur
// \date    2009-07-21
*/
//----------------------------------------------------------------------------------


#ifndef __CVLSegmentationEvaluationMetricSystem_H
#define __CVLSegmentationEvaluationMetricSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef CVLSEGMENTATIONEVALUATIONMETRIC_EXPORTS
    // Export library symbols.
    #define CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef CVLSEGMENTATIONEVALUATIONMETRIC_EXPORTS
  // Use the CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT macro to export classes and functions
  #define CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define CVLSEGMENTATIONEVALUATIONMETRIC_EXPORT
  #endif
#endif

#endif // __CVLSegmentationEvaluationMetricSystem_H


