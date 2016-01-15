//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    CVLSegmentationEvaluationMetricInit.h
// \author  Stefan Saur
// \date    2009-07-21
*/
//----------------------------------------------------------------------------------


#ifndef __CVLSegmentationEvaluationMetricInit_H
#define __CVLSegmentationEvaluationMetricInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int CVLSegmentationEvaluationMetricInit ();

ML_END_NAMESPACE

#endif // __CVLSegmentationEvaluationMetricInit_H


