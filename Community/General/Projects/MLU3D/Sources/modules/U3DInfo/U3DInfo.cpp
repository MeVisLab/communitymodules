//----------------------------------------------------------------------------------
// U3DInfo module.
//
// \file    U3DInfo.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DInfo.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DInfo, U3DInspector);


//***********************************************************************************


U3DInfo::U3DInfo() : U3DInspector()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add and initialize parameter fields
  (_numMetaDataEntriesFld         = addInt("numMetaDataEntries"))         ->setIntValue(0);
  (_defaultBoundingBoxMetaDataFld = addBool("defaultBoundingBoxMetaData"))->setBoolValue(false);

  (_numViewNodesFld  = addInt("numViewNodes")) ->setIntValue(0);
  (_numGroupNodesFld = addInt("numGroupNodes"))->setIntValue(0);
  (_numModelNodesFld = addInt("numModelNodes"))->setIntValue(0);
  (_numLightNodesFld = addInt("numLightNodes"))->setIntValue(0);

  (_numPointSetsFld = addInt("numPointSets"))->setIntValue(0);
  (_numLineSetsFld  = addInt("numLineSets")) ->setIntValue(0);
  (_numMeshesFld    = addInt("numMeshes"))   ->setIntValue(0);

  (_numLightResourcesFld    = addInt("numLightResources"))   ->setIntValue(0);
  (_numViewResourcesFld     = addInt("numViewResources"))    ->setIntValue(0);
  (_numShadersFld           = addInt("numShaders"))          ->setIntValue(0);
  (_numMaterialResourcesFld = addInt("numMaterialResources"))->setIntValue(0);
  (_numTextureResourcesFld  = addInt("numTextureResources")) ->setIntValue(0);
  (_numMotionResourcesFld   = addInt("numMotionResources"))  ->setIntValue(0);

  _autoApplyFld->setBoolValue(true);

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DInfo::~U3DInfo()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DInfo::handleNotification (Field* field)
{
  // Call parent class first to make sure that input object is set correctly.
  U3DInspector::handleNotification(field);
}


//***********************************************************************************


void U3DInfo::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DInspector::activateAttachments();
}


//***********************************************************************************


void U3DInfo::process()
{
  if (_inU3DValid)
  {
    _numMetaDataEntriesFld         ->setIntValue(_inU3DObject->metaData.size());
    _defaultBoundingBoxMetaDataFld ->setBoolValue(_inU3DObject->defaultValues.addDefaultBoundingBoxMetaData);

    _numViewNodesFld  ->setIntValue(_inU3DObject->viewNodes.size());
    _numGroupNodesFld ->setIntValue(_inU3DObject->groupNodes.size());
    _numModelNodesFld ->setIntValue(_inU3DObject->modelNodes.size());
    _numLightNodesFld ->setIntValue(_inU3DObject->lightNodes.size());

    _numPointSetsFld ->setIntValue(_inU3DObject->pointSets.size());
    _numLineSetsFld  ->setIntValue(_inU3DObject->lineSets.size());
    _numMeshesFld    ->setIntValue(_inU3DObject->meshes.size());

    _numLightResourcesFld    ->setIntValue(_inU3DObject->lightResources.size());
    _numViewResourcesFld     ->setIntValue(_inU3DObject->viewResources.size());
    _numShadersFld           ->setIntValue(_inU3DObject->litTextureShaders.size());
    _numMaterialResourcesFld ->setIntValue(_inU3DObject->materialResources.size());
    _numTextureResourcesFld  ->setIntValue(_inU3DObject->textureResources.size());
    _numMotionResourcesFld   ->setIntValue(_inU3DObject->motionResources.size());
  }
  else
  {
    _numMetaDataEntriesFld         ->setIntValue(0);
    _defaultBoundingBoxMetaDataFld ->setBoolValue(false);

    _numViewNodesFld  ->setIntValue(0);
    _numGroupNodesFld ->setIntValue(0);
    _numModelNodesFld ->setIntValue(0);
    _numLightNodesFld ->setIntValue(0);

    _numPointSetsFld ->setIntValue(0);
    _numLineSetsFld  ->setIntValue(0);
    _numMeshesFld    ->setIntValue(0);

    _numLightResourcesFld    ->setIntValue(0);
    _numViewResourcesFld     ->setIntValue(0);
    _numShadersFld           ->setIntValue(0);
    _numMaterialResourcesFld ->setIntValue(0);
    _numTextureResourcesFld  ->setIntValue(0);
    _numMotionResourcesFld   ->setIntValue(0);
  }
}


//***********************************************************************************


ML_END_NAMESPACE