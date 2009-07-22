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


#endif // __CVLSegmentationEvaluationMetricSystem_H


