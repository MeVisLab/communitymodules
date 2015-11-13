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


//----------------------------------------------------------------------------------

void PDFUtils::_createNewView()
{
  std::string newViewDisplayName = _newViewDisplayNameFld->getStringValue();

  if (newViewDisplayName != "")
  {
    std::string newViewString = "<View>\n";

    newViewString += "<DisplayName>" + newViewDisplayName + "</DisplayName>\n";
    newViewString += "<BackgroundColor>" + mlPDF::SpecificationGenerator::FormatColorString(_newViewBackgroundColorFld->getVectorValue()) + "</BackgroundColor>\n";
    newViewString += "<LightingScheme>" + mlPDF::intToString(_newViewLightingSchemeFld->getEnumValue()) + "</LightingScheme>\n";
    newViewString += "<CamCenterOfOrbit>" + mlPDF::SpecificationGenerator::FormatVec3String(_cameraCenterOfOrbitFld->getVectorValue()) + "</CamCenterOfOrbit>\n";
    newViewString += "<CamCenterToCamera>" + mlPDF::SpecificationGenerator::FormatVec3String(_cameraCenterToCameraFld->getVectorValue()) + "</CamCenterToCamera>\n";
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

void PDFUtils::_clearViews()
{
  _viewSpecificationsFld->setStringValue("");
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE