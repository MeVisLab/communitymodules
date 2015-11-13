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
#include "shared/MLPDF_Defines.h"
#include "shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"


#include <ColoredMarkerList.h>

// ML includes
#include "mlVersion.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDF3DFigurePage_Utils, WEMProcessor);

//----------------------------------------------------------------------------------

PDF3DFigurePage_Utils::PDF3DFigurePage_Utils(std::string type) : WEMProcessor(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Input fields and allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);

  (_outPointPositionsFld  = addBase("outPointPositions"))->setBaseValueAndAddAllowedType(&_outPointPositions);
  (_outLinePositionsFld   = addBase("outLinePositions"))->setBaseValueAndAddAllowedType(&_outLinePositions);
  (_outLineConnectionsFld = addBase("outLineConnections"))->setBaseValueAndAddAllowedType(&_outLineConnections);
  (_outFibersFld          = addBase("outFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

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

  //! Fields for WEM/mesh editing
  (_selectedWEMPatchFld                  = addString("selectedWEMPatch"))->setStringValue("");
  (_selectedWEMPatchIdFld                = addInt("selectedWEMPatchId"))->setIntValue(-1);
  (_availableWEMPatchesFld               = addString("availableWEMPatches"))->setStringValue("");
  (_selectedWEMPatchNewLabelFld          = addString("selectedWEMPatchNewLabel"))->setStringValue("");
  (_selectedWEMPatchGroupPathFld         = addString("selectedWEMPatchGroupPath"))->setStringValue("");
  (_selectedWEMPatchUseDefaultColorFld   = addBool("selectedWEMPatchUseDefaultColor"))->setBoolValue(true);
  (_selectedWEMPatchColorFld             = addColor("selectedWEMPatchColor"))->setStringValue("0.651 0.651 0.651");
  (_selectedWEMPatchColorAlphaFld        = addFloat("selectedWEMPatchColorAlpha"))->setFloatValue(0);

  //! Fields for line set editing
  (_selectedLineSetFld                   = addString("selectedLineSet"))->setStringValue("");
  (_selectedLineSetIdFld                 = addInt("selectedLineSetId"))->setIntValue(-1);
  (_availableLineSetsFld                 = addString("availableLineSets"))->setStringValue("");
  (_selectedLineSetNewLabelFld           = addString("selectedLineSetNewLabel"))->setStringValue("");
  (_selectedLineSetGroupPathFld          = addString("selectedLineSetGroupPath"))->setStringValue("");
  (_selectedLineSetUseDefaultColorFld    = addBool("selectedLineSetUseDefaultColor"))->setBoolValue(true);
  (_selectedLineSetColorFld              = addColor("selectedLineSetColor"))->setStringValue("0.651 0.651 0.651");

  //! Fields for point set editing
  (_selectedPointSetFld                  = addString("selectedPointSet"))->setStringValue("");
  (_selectedPointSetIdFld                = addInt("selectedPointSetId"))->setIntValue(-1);
  (_availablePointSetsFld                = addString("availablePointSets"))->setStringValue("");
  (_selectedPointSetNewLabelFld          = addString("selectedPointSetNewLabel"))->setStringValue("");
  (_selectedPointSetGroupPathFld         = addString("selectedPointSetGroupPath"))->setStringValue("");

  //! Fields for PointSet/LineSet properties
  (_pointPositionsMaxTypeIDFld = addInt("pointPositionsMaxTypeID"))->setIntValue(-1);
  (_linePositionsMaxTypeIDFld = addInt("linePositionsMaxTypeID"))->setIntValue(-1);
  (_lineConnectionsMaxTypeIDFld = addInt("lineConnectionsMaxTypeID"))->setIntValue(-1);
  (_lineDefinitionsMaxTypeIDFld = addInt("lineDefinitionsMaxTypeID"))->setIntValue(-1);
  (_pointPositionsNextTypeIDFld = addInt("pointPositionsNextTypeID"))->setIntValue(0);
  (_linePositionsNextTypeIDFld = addInt("linePositionsNextTypeID"))->setIntValue(0);
  (_lineConnectionsNextTypeIDFld = addInt("lineConnectionsNextTypeID"))->setIntValue(0);
  (_lineDefinitionsNextTypeIDFld = addInt("lineDefinitionsNextTypeID"))->setIntValue(0);


  // Set WEM processor fields
  _copyInputWEMsFld->setBoolValue(true);
  _autoUpdateFld->setBoolValue(true);
  _autoApplyFld->setBoolValue(true);
  _overwriteLabelDescriptionFld->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDF3DFigurePage_Utils::~PDF3DFigurePage_Utils()
{
  // Destroy own dynamic data structures here
  _inPointPositions.clearList();
  _inLinePositions.clearList();
  _inLineConnections.clearList();
  _outFiberSetContainer.deleteAllFiberSets();
  _outPointPositions.clearList();  
  _outLinePositions.clearList();
  _outLineConnections.clearList();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::handleNotification (Field* field)
{
  // _inPointPositionsFld
  if (field == _inPointPositionsFld)
  {
    ml::Base *inValue = _inPointPositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::XMarkerList))
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
         _inPointPositions = *inList;
      }
    }
    else
    {
      _inPointPositions.clear();
    }

    _calculateListPropertyFields();
    _calculateInventorPropertyFields();
    _updatePointSetOutputs();
  }

  // _inLinePositionsFld
  if (field == _inLinePositionsFld)
  {
    ml::Base *inValue = _inLinePositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::XMarkerList))
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
        _inLinePositions = *inList;
      }
    }
    else
    {
      _inLinePositions.clear();
    }

    _calculateListPropertyFields();
    _calculateInventorPropertyFields();
    _updateLineSetOutputs();
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

    _calculateListPropertyFields();
    _calculateInventorPropertyFields();
    _updateLineSetOutputs();
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
    _updateSelectedWEMPatchLabel();
    _updateSelectedWEMPatchDescription();
  }

  if (field == _selectedWEMPatchGroupPathFld)
  {
    _updateSelectedWEMPatchDescription();
  }

  if (
      (field == _selectedWEMPatchUseDefaultColorFld) ||
      (field == _selectedWEMPatchColorFld) ||
      (field == _selectedWEMPatchColorAlphaFld)
     )
  {
    _updateWEMPatchNodesColor();
    _updateSelectedWEMPatchDescription();
  }


  // call parent class and handle apply/autoApply and in/outputs
  WEMProcessor::handleNotification(field);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::activateAttachments()
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