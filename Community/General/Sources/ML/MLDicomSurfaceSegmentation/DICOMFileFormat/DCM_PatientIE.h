//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_PatientIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_PatientIE_H
#define __DCM_PatientIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class PatientIE
{
public:

  static void AddPatientIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addPatientModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_PatientIE_H