//----------------------------------------------------------------------------------
// U3DAssemble module.
//
// \file    U3DAssemble.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Assembles a U3D object.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAssemble.h"
#include "shared/MLU3D_Tools.h"


// ML includes
#include <mlVersion.h>


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAssemble, WEMInspector);


//***********************************************************************************


U3DAssemble::U3DAssemble (std::string type) : WEMInspector(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  ML_CHECK_NEW(_outU3DObject, mlU3D::U3DObject());

  // Add output fields and set allowed types.
  (_outU3DObjectFld = addBase("outU3DObject"))->setBaseValueAndAddAllowedType(_outU3DObject);

  // Add input fields to the module and set allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);
  _inPointPositionsFld->addAllowedType<ml::ColoredMarkerList>();
  _inLinePositionsFld->addAllowedType<ml::ColoredMarkerList>();

  // Add fields for selecting simple, straightforward mode
  (_simpleModePointSetFld = addBool("simpleModePointSet"))->setBoolValue(false);
  (_simpleModeLineSetFld = addBool("simpleModeLineSet"))->setBoolValue(false);
  (_simpleModeMeshFld = addBool("simpleModeMesh"))->setBoolValue(false);

  // Add fields to the module and set their values.
  (_modelPrefixPointCloudsFld = addString("modelPrefixPointClouds"))->setStringValue("PointSet");
  (_modelPrefixLineSetsFld    = addString("modelPrefixLineSets"))->setStringValue("LineSet");
  (_modelPrefixMeshesFld      = addString("modelPrefixMeshes"))->setStringValue("Mesh");
  //(_modelPrefixGlyphsFld      = addString("modelPrefixGlyphs"))->setStringValue("Glyph");       // Not supported by Acrobat

  (_pointCloudSpecificationFld = addString("pointCloudSpecification"))->setStringValue("");
  (_lineSetSpecificationFld    = addString("lineSetSpecification"))->setStringValue("");
  (_meshSpecificationFld       = addString("meshSpecification"))->setStringValue("");
  //(_glyphSpecificationFld      = addString("glyphSpecification"))->setStringValue("");          // Not supported by Acrobat
  (_viewsSpecificationFld      = addString("viewsSpecification"))->setStringValue("");
  (_lightsSpecificationFld     = addString("lightsSpecification"))->setStringValue("");
  (_metaDataSpecificationFld   = addString("metaDataSpecification"))->setStringValue("");

  (_defaultViewNameFld     = addString("defaultViewName"))->setStringValue("DefaultView");
  (_defaultLightNameFld    = addString("defaultLightName"))->setStringValue("DefaultAmbientLight");

  (_defaultMaterialDiffuseColorFld  = addColor("defaultMaterialDiffuseColor"))->setVector3Value(Vector3(0.65f,0.65f,0.65f));
  (_defaultMaterialSpecularColorFld = addColor("defaultMaterialSpecularColor"))->setVector3Value(Vector3(0.75f,0.75f,0.75f));
  (_defaultMaterialAmbientColorFld  = addColor("defaultMaterialAmbientColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  (_defaultMaterialEmissiveColorFld = addColor("defaultMaterialEmissiveColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  (_defaultMaterialAlphaFld         = addFloat("defaultMaterialAlpha"))->setFloatValue(1.0f);

  (_defaultLightColorFld = addColor("defaultLightColor"))->setVector3Value(Vector3(1.0f,1.0f,1.0f));
  (_defaultLightIntensityFld = addDouble("defaultLightIntensity"))->setDoubleValue(1.0f);

  (_defaultBoundingBoxMetaDataFld = addBool("defaultBoundingBoxMetaData"))->setBoolValue(true);
  (_addDefaultViewNodeFld         = addBool("addDefaultViewNode"))->setBoolValue(true);
  (_addDefaultLightNodeFld        = addBool("addDefaultLightNode"))->setBoolValue(true);

  _finishedFld = addNotify("notifyFinished");

  (_statusFld   = addString("status"))    ->setStringValue("Idle.");
  (_progressFld = addProgress("progress"))->setFloatValue(0.0f);
  (_successFld  = addBool("success"))     ->setBoolValue(false);

  // Turn off the automatic saving on all notifications
  _listenToFinishingNotificationsFld       ->setBoolValue(false);
  _listenToRepaintNotificationsFld         ->setBoolValue(false);
  _listenToSelectionChangedNotificationsFld->setBoolValue(false);

  _autoUpdateFld->setBoolValue(false);
  _autoApplyFld->setBoolValue(false);

  // Init status fields
  _progressFld->setFloatValue(0);
  _statusFld->setStringValue("Idle.");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DAssemble::~U3DAssemble()
{
  // Destroy own dynamic data structures here
  _outU3DObject = NULL;
}


//***********************************************************************************


void U3DAssemble::handleNotification (Field* field)
{
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
  }

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
  }

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

  }

  // call parent class and handle apply/autoApply and in/outputs
  WEMInspector::handleNotification(field);
}


//***********************************************************************************


void U3DAssemble::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMInspector::activateAttachments();
}


//***********************************************************************************


void U3DAssemble::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  _assembleU3DObject();

  WEMInspector::_process();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();
}


//***********************************************************************************


void U3DAssemble::_assembleU3DObject()
{
  bool success = false;

  _progressFld->setFloatValue(0.0f);

  WEMPtr saveWEM = NULL;
  ML_CHECK_NEW(saveWEM,WEM());

  // Clear object info vector;
  _allU3DObjects.clear();

  // Reset out object
  _outU3DObject->clear();

  // Get default parameters from field values
  defaultValues = _getDefaultValuesFromFields(); 

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  _statusFld->setStringValue("Analyzing input data.");

  _preProcessPointSetData();
  _preProcessLineSetData();
  _preProcessMeshData(saveWEM);

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  _progressFld->setFloatValue(0.1f);

  if (_allU3DObjects.size() > 0)  // If at least one valid object (WEM patch, XMarker or FiberSet) was added.
  {
    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add nodes
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _statusFld->setStringValue("Adding main nodes.");

    _addDefaultViewNode();
    _addAllGroupNodes();
    _addAllModelNodes();
    _addDefaultLightNode();

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add resources
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _progressFld->setFloatValue(0.9f);
    _statusFld->setStringValue("Adding resources.");

    _addDefaultLightResource();
    _addDefaultViewResource();

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Finishing moves
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    mlU3D::MetaDataStruct metaDataPair;

    metaDataPair.key = "AssembledBy";
    metaDataPair.value = "U3DAssemble module from MeVisLab MLU3D library (v" + mlU3D::U3DTools::getModuleVersionNumberString() + ") by Axel Newe (axel.newe@fau.de)";
    _outU3DObject->metaData.push_back(metaDataPair);

    _addMetaData();

    _statusFld->setStringValue("Finished.");
    success = true;
  }
  else
  {
    _statusFld->setStringValue("No objects to export found.");
  }
  
  _progressFld->setFloatValue(1.0f);
  _successFld->setBoolValue(success);
  _outU3DObjectFld->touch();
  _finishedFld->touch();
}



//***********************************************************************************


ML_END_NAMESPACE