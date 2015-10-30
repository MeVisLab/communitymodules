//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils_Camera.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Utilities for PDF creation.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFUtils.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFUtils::_calculateCameraPropertiesFromInventorCamera()
{
  Vector3 inventorCameraPosition = _inventorCameraPositionFld->getVectorValue();
  Vector4 inventorCameraOrientation = _inventorCameraOrientationFld->getVectorValue();
  float   inventorCameraFocalDistance = _inventorCameraFocalDistanceFld->getFloatValue();
  float   inventorCameraHeight = _inventorCameraHeightFld->getFloatValue();
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateCameraPropertiesFromInventorCamera(
    inventorCameraPosition, inventorCameraOrientation, inventorCameraFocalDistance, inventorCameraHeight,
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  _cameraCenterOfOrbitFld->setVector3Value(camCenterOfOrbit);
  _cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  _cameraRadiusOfOrbitFld->setFloatValue(camRadiusOfOrbit);
  _cameraRollAngleFld->setFloatValue(camRollAngle);
  _cameraFOVAngleFld->setFloatValue(camFOVAngle);
}

//----------------------------------------------------------------------------------

void PDFUtils::_calculateDefaultCameraProperties()
{
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateDefaultCameraProperties(
    // Inputs
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  _cameraCenterOfOrbitFld->setVector3Value(camCenterOfOrbit);
  _cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  _cameraRadiusOfOrbitFld->setFloatValue(camRadiusOfOrbit);
  _cameraRollAngleFld->setFloatValue(camRollAngle);
  _cameraFOVAngleFld->setFloatValue(camFOVAngle);
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE