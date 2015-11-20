//----------------------------------------------------------------------------------
//! Tools for MLPDF specification generation.
/*!
// \file    MLPDF_PDFDocumentTools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-15
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLPDF_PDFDocumentTools.h"

// Inventor includes
#include <Inventor/nodes/SoNodes.h>


ML_START_NAMESPACE

namespace mlPDF {

//----------------------------------------------------------------------------------

void PDFDocumentTools::CalculateCameraPropertiesFromInventorCamera(
    Vector3 inventorCameraPosition, Vector4 inventorCameraOrientation, float inventorCameraFocalDistance, float inventorCameraHeight, // inputs
    Vector3& camCenterOfOrbit, Vector3& camCenterToCamera, float& camRadiusOfOrbit, float& camRollAngle, float& camFOVAngle
    )
{
  SbVec3f cameraRotationAxis(inventorCameraOrientation[0], inventorCameraOrientation[1], inventorCameraOrientation[2]);
  SbRotation cameraOrientation(cameraRotationAxis, inventorCameraOrientation[3]);

  SbMatrix cameraRotationMatrix;
  cameraOrientation.getValue(cameraRotationMatrix);

  //Vector3 kosXAxis(1,0,0);
  //Vector3 kosYAxis(0,1,0);
  Vector3 kosZAxis(0,0,1);
  Vector3 camXAxis(cameraRotationMatrix[0][0], cameraRotationMatrix[0][1], cameraRotationMatrix[0][2]);
  Vector3 camYAxis(cameraRotationMatrix[1][0], cameraRotationMatrix[1][1], cameraRotationMatrix[1][2]);
  Vector3 camZAxis(cameraRotationMatrix[2][0], cameraRotationMatrix[2][1], cameraRotationMatrix[2][2]);
  Vector3 camYAxisInverted = camYAxis * -1;
  Vector3 centerToCamera = camZAxis * inventorCameraFocalDistance;
  Vector3 centerOfOrbit = inventorCameraPosition - centerToCamera;

  Vector4 mx(cameraRotationMatrix[0][0], cameraRotationMatrix[0][1], cameraRotationMatrix[0][2], cameraRotationMatrix[0][3]);
  Vector4 my(cameraRotationMatrix[1][0], cameraRotationMatrix[1][1], cameraRotationMatrix[1][2], cameraRotationMatrix[1][3]);
  Vector4 mz(cameraRotationMatrix[2][0], cameraRotationMatrix[2][1], cameraRotationMatrix[2][2], cameraRotationMatrix[2][3]);
  Vector4 mr(cameraRotationMatrix[3][0], cameraRotationMatrix[3][1], cameraRotationMatrix[3][2], cameraRotationMatrix[3][3]);
  Matrix4 camMatrix(mx, my, mz, mr);

  Vector3 projectionAxis1 = camZAxis.cross(kosZAxis); 
  Vector3 projectionAxis2 = projectionAxis1.cross(camZAxis);

  projectionAxis2.normalize();

  // Calc roll angle
  double rollAangleRadians = acos(camYAxisInverted.dot(projectionAxis2));
  double rollAngleDegrees = (rollAangleRadians / M_PI * 180.0) + 180.0;

  double angleRadBetweenProjectionAxisAndCamXAxis = acos(camXAxis.dot(projectionAxis2));
  if (angleRadBetweenProjectionAxisAndCamXAxis <  (M_PI/2.0))
  {
    rollAngleDegrees = 360.0 - rollAngleDegrees;
  }

  // Calc FOV angle
  double fieldOfViewAngle = 45.0;
  if (inventorCameraHeight != 0)
  {
    fieldOfViewAngle = 180.0/(M_PI/inventorCameraHeight);
  }
  if (fieldOfViewAngle > 180.0)  // Can happen if Orthographic camera mode is selected in inventor
  {
    fieldOfViewAngle = 45.0;
  }

  // Return result values
  camCenterOfOrbit  = centerOfOrbit;
  camCenterToCamera = centerToCamera;
  camRadiusOfOrbit  = inventorCameraFocalDistance;
  camRollAngle      = rollAngleDegrees;
  camFOVAngle       = fieldOfViewAngle;
}

//----------------------------------------------------------------------------------

void PDFDocumentTools::CalculateDefaultCameraProperties(
  Vector3 boundingBoxCenter, double boundingBoxRadius,                                                                     // inputs
  Vector3& camCenterOfOrbit, Vector3& camCenterToCamera, float& camRadiusOfOrbit, float& camRollAngle, float& camFOVAngle  // outputs
  )
{
  // Return result values
  camCenterOfOrbit  = boundingBoxCenter;
  camCenterToCamera = Vector3(0, -1, 0);
  camRadiusOfOrbit  = (float)boundingBoxRadius*2;
  camRollAngle      = 0.0;
  camFOVAngle       = 60.0;
}

//----------------------------------------------------------------------------------

} // end namespace mlPDF

ML_END_NAMESPACE