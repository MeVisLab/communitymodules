//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_FrameOfReferenceIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_FrameOfReferenceIE_H
#define __DCM_FrameOfReferenceIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class FrameOfReferenceIE
{
public:

  static void AddFrameOfReferenceIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addFrameOfReferenceModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_FrameOfReferenceIE_H