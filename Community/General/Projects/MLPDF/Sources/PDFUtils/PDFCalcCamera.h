//----------------------------------------------------------------------------------
// The ML module class PDFCalcCamera.
//
// \file    PDFCalcCamera.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-01-11
//
// Calculates PDF camera properties.
//----------------------------------------------------------------------------------


#pragma once


#ifndef _PDFCalcCamera_H
#define _PDFCalcCamera_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/MLPDF_Tools.h"


ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

class MLPDF_EXPORT PDFCalcCamera : public Module
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFCalcCamera)

public:

  //! Constructor.
  PDFCalcCamera();

protected:

  //! Destructor.
  virtual ~PDFCalcCamera();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  NotifyField*   _calculateCameraFromInventorSceneFld;
  BoolField*     _autoCalculateCameraFromInventorSceneFld;
  Vector3Field*  _inventorCameraPositionFld;
  Vector4Field*  _inventorCameraOrientationFld;
  FloatField*    _inventorCameraFocalDistanceFld;
  FloatField*    _inventorCameraHeightFld;

  //! PDF view camera fields
  Vector3Field*  _cameraCenterOfOrbitFld;
  Vector3Field*  _cameraCenterToCameraFld;
  FloatField*    _cameraRadiusOfOrbitFld;
  FloatField*    _cameraRollAngleFld;
  FloatField*    _cameraFOVAngleFld;

  /* METHODS */

  // Camera methods ==========================================================

  // Calculate PDF camera properties
  void _calculateCameraPropertiesFromInventorCamera();

//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _PDFCalcCamera_H