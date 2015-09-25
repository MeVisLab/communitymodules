//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_StudyIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_StudyIE_H
#define __DCM_StudyIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class StudyIE
{
public:

  static void AddStudyIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addGeneralStudyModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_StudyIE_H