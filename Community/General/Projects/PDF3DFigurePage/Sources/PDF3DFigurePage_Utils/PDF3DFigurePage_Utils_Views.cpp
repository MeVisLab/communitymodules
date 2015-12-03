//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_Utils.
/*!
// \file    PDF3DFigurePage_Utils.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDF3DFigurePage_Utils.h"
#include "shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createNewView()
{
  std::string newViewDisplayName = _newViewDisplayNameFld->getStringValue();

  if (newViewDisplayName != "")
  {
    std::string newViewString = "<View>\n";

    newViewString += "<DisplayName>" + newViewDisplayName + "</DisplayName>\n";
    newViewString += "<BackgroundColor>" + PDFTools::FormatColorString(_newViewBackgroundColorFld->getVectorValue()) + "</BackgroundColor>\n";
    newViewString += "<LightingScheme>" + intToString(_newViewLightingSchemeFld->getEnumValue()) + "</LightingScheme>\n";
    newViewString += "<RenderMode>" + intToString(_newViewRenderModeFld->getEnumValue()) + "</RenderMode>";
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

void PDF3DFigurePage_Utils::_clearViews()
{
  _viewSpecificationsFld->setStringValue("");
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createMetaDataSpecification()
{
  std::string metaDataSpecification = "";
  std::string viewSpecifications = _viewSpecificationsFld->getStringValue();

  if (PDFTools::stringTrimWhitespace(viewSpecifications) != "")
  {
    metaDataSpecification += "<MetaData>\n";
    metaDataSpecification += "<MetaDataKey>ViewSpecifications</MetaDataKey>\n";
    metaDataSpecification += "<MetaDataValue>\n\n";
    metaDataSpecification += viewSpecifications;
    metaDataSpecification += "</MetaDataValue>\n";
    metaDataSpecification += "</MetaData>\n";
  }

  _metaDataSpecificationFld->setStringValue(metaDataSpecification);
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE