//----------------------------------------------------------------------------------
//! The ML module class PDFCreateViewSpecifications.
/*!
// \file    PDFCreateViewSpecifications.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-01-11
//
// Create PDF view specification string.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFCreateViewSpecifications.h"
#include "../shared/MLPDF_Defines.h"
#include "../shared/MLPDF_SpecificationGenerator.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDFCreateViewSpecifications, Module);

//----------------------------------------------------------------------------------

PDFCreateViewSpecifications::PDFCreateViewSpecifications() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  //! PDF view camera fields
  (_cameraCenterOfOrbitFld = addVector3("cameraCenterOfOrbit"))->setVector3Value(Vector3(0));
  (_cameraCenterToCameraFld = addVector3("cameraCenterToCamera"))->setVector3Value(Vector3(0));
  (_cameraRadiusOfOrbitFld = addFloat("cameraRadiusOfOrbit"))->setFloatValue(0);
  (_cameraFOVAngleFld = addFloat("cameraFOVAngle"))->setFloatValue(90.0f);
  (_cameraRollAngleFld = addFloat("cameraRollAngle"))->setFloatValue(0);

  //! Fields for creating views
  (_viewSpecificationsFld     = addString("viewSpecifications"))->setStringValue("");
  (_newViewDisplayNameFld     = addString("newViewDisplayName"))->setStringValue("");
  (_newViewBackgroundColorFld = addColor("newViewBackgroundColor"))->setStringValue("1 1 1");
  (_newViewLightingSchemeFld  = addEnum("newViewLightingScheme", mlPDF::LightingSchemeStrings, mlPDF::NUM_LIGHTING_SCHEMES))->setEnumValue(mlPDF::LIGHTING_SCHEME_CAD);
  (_newViewRenderModeFld      = addEnum("newViewRenderMode", mlPDF::ModelRenderModeStrings, mlPDF::NUM_MODEL_RENDERMODES))->setEnumValue(mlPDF::MODEL_RENDERMODE_SOLID);
  _addNewViewFld              = addNotify("addNewView");
  _clearViewsFld              = addNotify("clearViews");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDFCreateViewSpecifications::~PDFCreateViewSpecifications()
{
  // Destroy own dynamic data structures here

}

//----------------------------------------------------------------------------------

void PDFCreateViewSpecifications::handleNotification (Field* field)
{
  // View fields
  if (field == _addNewViewFld)
  {
    _createNewView();
  }

  if (field == _clearViewsFld)
  {
    _clearViews();
  }
}

//----------------------------------------------------------------------------------

void PDFCreateViewSpecifications::_createNewView()
{
  std::string newViewDisplayName = _newViewDisplayNameFld->getStringValue();

  if (newViewDisplayName != "")
  {
    std::string newViewString = "<View>\n";

    newViewString += "<DisplayName>" + newViewDisplayName + "</DisplayName>\n";
    newViewString += "<BackgroundColor>" + mlPDF::PDFTools::FormatColorString(_newViewBackgroundColorFld->getVectorValue()) + "</BackgroundColor>\n";
    newViewString += "<LightingScheme>" + mlPDF::intToString(_newViewLightingSchemeFld->getEnumValue()) + "</LightingScheme>\n";
    newViewString += "<RenderMode>" + mlPDF::intToString(_newViewRenderModeFld->getEnumValue()) + "</RenderMode>\n";
    newViewString += "<CamCenterOfOrbit>" + mlPDF::PDFTools::FormatVec3String(_cameraCenterOfOrbitFld->getVectorValue()) + "</CamCenterOfOrbit>\n";
    newViewString += "<CamCenterToCamera>" + mlPDF::PDFTools::FormatVec3String(_cameraCenterToCameraFld->getVectorValue()) + "</CamCenterToCamera>\n";
    newViewString += "<CamRadiusOfOrbit>" + mlPDF::floatToString(_cameraRadiusOfOrbitFld->getFloatValue()) + "</CamRadiusOfOrbit>\n";
    newViewString += "<CamRollAngle>" + mlPDF::floatToString(_cameraRollAngleFld->getFloatValue()) + "</CamRollAngle>\n";
    newViewString += "<CamFOVAngle>" + mlPDF::floatToString(_cameraFOVAngleFld->getFloatValue()) + "</CamFOVAngle>\n";

    newViewString += "</View>\n\n";

    std::string specificationsString = _viewSpecificationsFld->getStringValue();

    specificationsString += newViewString;

    _viewSpecificationsFld->setStringValue(specificationsString);
  }
}

//----------------------------------------------------------------------------------

void PDFCreateViewSpecifications::_clearViews()
{
  _viewSpecificationsFld->setStringValue("");
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE