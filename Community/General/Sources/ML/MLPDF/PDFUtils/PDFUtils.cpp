//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Utilities for PDF creation.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFUtils.h"
#include "shared/MLPDF_Defines.h"
#include "shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"


#include <ColoredMarkerList.h>

// ML includes
#include "mlVersion.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDFUtils, WEMProcessor);

//----------------------------------------------------------------------------------

PDFUtils::PDFUtils(std::string type) : WEMProcessor(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Input fields and allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);
  _inPointPositionsFld->addAllowedType<ml::ColoredMarkerList>();
  _inLinePositionsFld->addAllowedType<ml::ColoredMarkerList>();

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  _calculateCameraFromInventorSceneFld = addNotify("calculateCameraFromInventorScene");
  (_autoCalculateCameraFromInventorSceneFld = addBool("autoCalculateCameraFromInventorScene"))->setBoolValue(false);
  (_inventorCameraPositionFld               = addVector3("inventorCameraPosition"))->setVector3Value(Vector3(0, 0, 0));
  (_inventorCameraOrientationFld            = addVector4("inventorCameraOrientation"))->setVector4Value(Vector4(0, 0, 1, 0));
  (_inventorCameraFocalDistanceFld          = addFloat("inventorCameraFocalDistance"))->setFloatValue(0);
  (_inventorCameraHeightFld                 = addFloat("inventorCameraHeight"))->setFloatValue(0);

  //! PDF view camera fields
  (_cameraCenterOfOrbitFld = addVector3("cameraCenterOfOrbit"))->setVector3Value(Vector3(0));
  (_cameraCenterToCameraFld = addVector3("cameraCenterToCamera"))->setVector3Value(Vector3(0));
  (_cameraRadiusOfOrbitFld = addFloat("cameraRadiusOfOrbit"))->setFloatValue(0);
  (_cameraFOVAngleFld = addFloat("cameraFOVAngle"))->setFloatValue(90.0f);
  (_cameraRollAngleFld = addFloat("cameraRollAngle"))->setFloatValue(0);

  //! Fields for SoVertexProperty and SoIndexedLineSet
  (_vertexListFld = addString("vertexList"))->setStringValue("[  ]");
  (_coordIndexListFld = addString("coordIndexList"))->setStringValue("0");

  //! Fields for creating views
  (_viewSpecificationsFld     = addString("viewSpecifications"))->setStringValue("");
  (_newViewDisplayNameFld     = addString("newViewDisplayName"))->setStringValue("");
  (_newViewBackgroundColorFld = addColor("newViewBackgroundColor"))->setStringValue("1 1 1");
  (_newViewLightingSchemeFld  = addEnum("newViewLightingScheme", mlPDF::LightingSchemeStrings, mlPDF::NUM_LIGHTING_SCHEMES))->setEnumValue(mlPDF::LIGHTING_SCHEME_CAD);
  _addNewViewFld              = addNotify("addNewView");
  _clearViewsFld              = addNotify("clearViews");

  //! Fields for WEM/mesh handling
  (_selectedWEMPatchFld         = addString("selectedWEMPatch"))->setStringValue("");
  (_selectedWEMPatchIdFld       = addInt("selectedWEMPatchId"))->setIntValue(-1);
  (_availableWEMPatchesFld      = addString("availableWEMPatches"))->setStringValue("");
  (_selectedWEMPatchNewLabelFld = addString("selectedWEMPatchNewLabel"))->setStringValue("");

  //! Fields for specification generator
  (_newSpecificationObjectNameFld              = addString("newSpecificationObjectName"))->setStringValue("");
  (_newSpecificationGroupPathFld               = addString("newSpecificationGroupPath"))->setStringValue("");
  (_newSpecificationModelVisibilityFld         = addEnum("newSpecificationModelVisibility", mlPDF::NewSpecificationModelVisibilityStrings, mlPDF::NUM_MODELVISIBILITY))->setEnumValue(mlPDF::MODELVISIBILITY_FRONTANDBACKVISIBLE);
  (_newSpecificationUseDefaultColorFld         = addBool("newSpecificationUseDefaultColor"))->setBoolValue(true);
  (_newSpecificationColorFld                   = addColor("newSpecificationColor"))->setStringValue("0.651 0.651 0.651");
  (_newSpecificationColorAlphaFld              = addFloat("newSpecificationColorAlpha"))->setFloatValue(1);
  (_newSpecificationUseDefaultSpecularColorFld = addBool("newSpecificationUseDefaultSpecularColor"))->setBoolValue(true);
  (_newSpecificationSpecularColorFld           = addColor("newSpecificationSpecularColor"))->setStringValue("0.75 0.75 0.75");


  // Set WEM processor fields
  _copyInputWEMsFld->setBoolValue(true);
  _autoUpdateFld->setBoolValue(true);
  _autoApplyFld->setBoolValue(true);
  _overwriteLabelDescriptionFld->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDFUtils::~PDFUtils()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void PDFUtils::handleNotification (Field* field)
{
  // _inPointPositionsFld
  if (field == _inPointPositionsFld)
  {
    ml::Base *inValue = _inPointPositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inPointPositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
         _inPointPositions = *inList;
      }

    }
    else
    {
      _inPointPositions.clear();
    }

    _calculateInventorPropertyFields();
  }

  // _inLinePositionsFld
  if (field == _inLinePositionsFld)
  {
    ml::Base *inValue = _inLinePositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inLinePositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
         _inLinePositions = *inList;
      }
    }
    else
    {
      _inLinePositions.clear();
    }

    _calculateInventorPropertyFields();
  }

  // _inLineConnectionsFld
  if (field == _inLineConnectionsFld)
  {
    ml::IndexPairList* inList = ((ml::IndexPairList*)_inLineConnectionsFld->getBaseValue());

    if (inList != NULL)
    {
      _inLineConnections = (*inList);
    }
    else
    {
      _inLineConnections.clear();
    }

    _calculateInventorPropertyFields();
  }

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

  // View fields
  if (field == _addNewViewFld)
  {
    _createNewView();
  }

  if (field == _clearViewsFld)
  {
    _clearViews();
  }

  // WEM fields
  if (field == _selectedWEMPatchFld)
  {
    _selectedWEMPatchChanged(_outWEM);
  }

  if (field == _selectedWEMPatchNewLabelFld)
  {
    _selectedWEMPatchNewLabelChanged();
  }

  // call parent class and handle apply/autoApply and in/outputs
  WEMProcessor::handleNotification(field);
}

//----------------------------------------------------------------------------------

void PDFUtils::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMProcessor::activateAttachments();
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE