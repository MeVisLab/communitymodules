//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils_Views.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Utilities for PDF creation.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFUtils.h"
#include "shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

void PDFUtils::_createNewView()
{
  std::string newViewDisplayName = _newViewDisplayNameFld->getStringValue();

  if (newViewDisplayName != "")
  {
    std::string newViewString = "<View>\n";

    newViewString += "<DisplayName>" + newViewDisplayName + "</DisplayName>\n";
    newViewString += "<BackgroundColor>" + PDFTools::FormatColorString(_newViewBackgroundColorFld->getVectorValue()) + "</BackgroundColor>\n";
    newViewString += "<LightingScheme>" + intToString(_newViewLightingSchemeFld->getEnumValue()) + "</LightingScheme>\n";
    newViewString += "<RenderMode>" + intToString(_newViewRenderModeFld->getEnumValue()) + "</RenderMode>\n";
    newViewString += "<CamCenterOfOrbit>" + PDFTools::FormatVec3String(_cameraCenterOfOrbitFld->getVectorValue()) + "</CamCenterOfOrbit>\n";
    newViewString += "<CamCenterToCamera>" + PDFTools::FormatVec3String(_cameraCenterToCameraFld->getVectorValue()) + "</CamCenterToCamera>\n";
    newViewString += "<CamRadiusOfOrbit>" + floatToString(_cameraRadiusOfOrbitFld->getFloatValue()) + "</CamRadiusOfOrbit>\n";
    newViewString += "<CamRollAngle>" + floatToString(_cameraRollAngleFld->getFloatValue()) + "</CamRollAngle>\n";
    newViewString += "<CamFOVAngle>" + floatToString(_cameraFOVAngleFld->getFloatValue()) + "</CamFOVAngle>\n";

    newViewString += "</View>\n\n";

    std::string specificationsString = _viewSpecificationsFld->getStringValue();

    specificationsString += newViewString;

    _viewSpecificationsFld->setStringValue(specificationsString);
  }
}

//----------------------------------------------------------------------------------

void PDFUtils::_clearViews()
{
  _viewSpecificationsFld->setStringValue("");
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE