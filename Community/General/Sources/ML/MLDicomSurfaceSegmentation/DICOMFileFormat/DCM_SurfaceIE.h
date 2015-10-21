//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_SurfaceIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_SurfaceIE_H
#define __DCM_SurfaceIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class SurfaceIE
{
public:

  static void AddSurfaceIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addSurfaceSegmentationModule(SmartPtr_DCMDataSet datasetPointer);
  static void _addSurfaceMeshModule(SmartPtr_DCMDataSet datasetPointer);
  static void _addSOPCommonModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_SurfaceIE_H