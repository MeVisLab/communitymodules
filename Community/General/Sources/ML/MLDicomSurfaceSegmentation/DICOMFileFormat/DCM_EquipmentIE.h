//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_EquipmentIE.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_EquipmentIE_H
#define __DCM_EquipmentIE_H

// Local includes
#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

class EquipmentIE
{
public:

  static void AddEquipmentIE(SmartPtr_DCMDataSet datasetPointer);

private:

  static void _addGeneralEquipmentModule(SmartPtr_DCMDataSet datasetPointer);
  static void _addEnhancedGeneralEquipmentModule(SmartPtr_DCMDataSet datasetPointer);


};


ML_END_NAMESPACE

#endif // __DCM_EquipmentIE_H