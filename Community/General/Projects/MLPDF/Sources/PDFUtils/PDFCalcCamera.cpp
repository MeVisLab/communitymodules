//----------------------------------------------------------------------------------
//! The ML module class PDFCalcCamera.
/*!
// \file    PDFCalcCamera.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-01-11
//
// Calculates PDF camera properties.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFCalcCamera.h"
#include "../shared/MLPDF_PDFDocumentTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDFCalcCamera, Module);

//----------------------------------------------------------------------------------

PDFCalcCamera::PDFCalcCamera() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  _calculateCameraFromInventorSceneFld      = addNotify("calculateCameraFromInventorScene");
  (_autoCalculateCameraFromInventorSceneFld = addBool("autoCalculateCameraFromInventorScene"))->setBoolValue(false);
  (_inventorCameraPositionFld               = addVector3("inventorCameraPosition"))->setVector3Value(Vector3(0, 0, 0));
  (_inventorCameraOrientationFld            = addVector4("inventorCameraOrientation"))->setVector4Value(Vector4(0, 0, 1, 0));
  (_inventorCameraFocalDistanceFld          = addFloat("inventorCameraFocalDistance"))->setFloatValue(0);
  (_inventorCameraHeightFld                 = addFloat("inventorCameraHeight"))->setFloatValue(0);

  //! PDF view camera fields
  (_cameraCenterOfOrbitFld  = addVector3("cameraCenterOfOrbit"))->setVector3Value(Vector3(0));
  (_cameraCenterToCameraFld = addVector3("cameraCenterToCamera"))->setVector3Value(Vector3(0));
  (_cameraRadiusOfOrbitFld  = addFloat("cameraRadiusOfOrbit"))->setFloatValue(0);
  (_cameraFOVAngleFld       = addFloat("cameraFOVAngle"))->setFloatValue(90.0f);
  (_cameraRollAngleFld      = addFloat("cameraRollAngle"))->setFloatValue(0);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDFCalcCamera::~PDFCalcCamera()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void PDFCalcCamera::handleNotification (Field* field)
{
  // _calculateCameraFromInventorSceneFld
  if (field == _calculateCameraFromInventorSceneFld)
  {
    _calculateCameraPropertiesFromInventorCamera();
  }

  // Inventor camera fields
  if (
      (
       (field == _autoCalculateCameraFromInventorSceneFld) ||
       (field == _inventorCameraPositionFld) ||
       (field == _inventorCameraOrientationFld) ||
       (field == _inventorCameraFocalDistanceFld) ||
       (field == _inventorCameraHeightFld)
      )
      &&
      (_autoCalculateCameraFromInventorSceneFld->getBoolValue())
     )
  {
    _calculateCameraPropertiesFromInventorCamera();
  }

}

//----------------------------------------------------------------------------------

void PDFCalcCamera::_calculateCameraPropertiesFromInventorCamera()
{
  // Input values
  Vector3 inventorCameraPosition      = _inventorCameraPositionFld->getVectorValue();
  Vector4 inventorCameraOrientation   = _inventorCameraOrientationFld->getVectorValue();
  float   inventorCameraFocalDistance = _inventorCameraFocalDistanceFld->getFloatValue();
  float   inventorCameraHeight        = _inventorCameraHeightFld->getFloatValue();

  // Result values
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  // Calculate result
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


ML_END_NAMESPACE