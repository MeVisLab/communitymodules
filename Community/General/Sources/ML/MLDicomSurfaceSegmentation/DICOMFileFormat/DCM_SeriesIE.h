//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_SeriesIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_SeriesIE_H
#define __DCM_SeriesIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class SeriesIE
{
public:

  static void AddSeriesIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addGeneralSeriesModule(SmartPtr_DCMDataSet datasetPointer);
  static void _addSegmentationSeriesModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_SeriesIE_H