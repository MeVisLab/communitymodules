//----------------------------------------------------------------------------------
// U3DInit module.
//
// \file    U3DInit.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DInit.h"
#include "../../shared/MLU3D_Constants.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DInit, U3DGenerator);


//***********************************************************************************


U3DInit::U3DInit() : U3DGenerator()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();
  // Add and initialize parameter fields
  _initializeFld = addNotify("initialize");

  (_modelPrefixPointCloudsFld        = addString("modelPrefixPointClouds"))->setStringValue("PointSet");
  (_modelPrefixLineSetsFld           = addString("modelPrefixLineSets"))->setStringValue("LineSet");
  (_modelPrefixMeshesFld             = addString("modelPrefixMeshes"))->setStringValue("Mesh");

  (_defaultViewNameFld               = addString("defaultViewName"))->setStringValue("DefaultView");
  (_defaultLightNameFld              = addString("defaultLightName"))->setStringValue("DefaultAmbientLight");

  (_defaultMaterialNameFld           = addString("defaultMaterialName"))->setStringValue("DefaultMaterial");
  (_defaultMaterialDiffuseColorFld   = addColor("defaultMaterialDiffuseColor"))->setVector3Value(Vector3(0.65f, 0.65f, 0.65f));
  (_defaultMaterialSpecularColorFld  = addColor("defaultMaterialSpecularColor"))->setVector3Value(Vector3(0.75f, 0.75f, 0.75f));
  (_defaultMaterialAmbientColorFld   = addColor("defaultMaterialAmbientColor"))->setVector3Value(Vector3(0.0f, 0.0f, 0.0f));
  (_defaultMaterialEmissiveColorFld  = addColor("defaultMaterialEmissiveColor"))->setVector3Value(Vector3(0.0f, 0.0f, 0.0f));
  (_defaultMaterialAlphaFld          = addDouble("defaultMaterialAlpha"))->setDoubleValue(1.0f);
  (_defaultMaterialReflectivityFld   = addDouble("defaultMaterialReflectivity"))->setDoubleValue(0.5f);

  (_defaultLightColorFld             = addColor("defaultLightColor"))->setVector3Value(Vector3(1.0f, 1.0f, 1.0f));
  (_defaultLightIntensityFld         = addDouble("defaultLightIntensity"))->setDoubleValue(1.0f);

  (_addDefaultBoundingBoxMetaDataFld = addBool("addDefaultBoundingBoxMetaData"))->setBoolValue(true);
  (_addDefaultViewNodeFld            = addBool("addDefaultViewNode"))->setBoolValue(true);
  (_addDefaultLightNodeFld           = addBool("addDefaultLightNode"))->setBoolValue(true);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DInit::~U3DInit()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DInit::handleNotification (Field* field)
{
  // Regardless which field changed: re-initialize U3D object
  _initializeU3D();

  // Call parent class and handle apply/autoApply and in/outputs
  U3DGenerator::handleNotification(field);
}


//***********************************************************************************


void U3DInit::activateAttachments()
{
  // ... own initialization routines
  _initializeU3D();

  // call parent class
  U3DGenerator::activateAttachments();
}


//***********************************************************************************

void U3DInit::_initializeU3D()
{
  if (_outU3DObject)
  {
    _outU3DObject->clear();

    _outU3DObject->defaultValues.addDefaultBoundingBoxMetaData = _addDefaultBoundingBoxMetaDataFld->getBoolValue();

    std::string fieldStringValue = "";

    // Set default prefix for point clouds
    fieldStringValue = _modelPrefixPointCloudsFld->getStringValue();

    if ("" != fieldStringValue)
    {
      _outU3DObject->defaultValues.defaultPointSetPefix = fieldStringValue;
    }
    else
    {
      _modelPrefixPointCloudsFld->setStringValue(_outU3DObject->defaultValues.defaultPointSetPefix); // Reset old value
    }

    // Set default prefix for line sets
    fieldStringValue = _modelPrefixLineSetsFld->getStringValue();

    if ("" != fieldStringValue)
    {
      _outU3DObject->defaultValues.defaultLineSetPefix = fieldStringValue;
    }
    else
    {
      _modelPrefixLineSetsFld->setStringValue(_outU3DObject->defaultValues.defaultLineSetPefix); // Reset old value
    }

    // Set default prefix for meshes
    fieldStringValue = _modelPrefixMeshesFld->getStringValue();

    if ("" != fieldStringValue)
    {
      _outU3DObject->defaultValues.defaultMeshPefix = fieldStringValue;
    }
    else
    {
      _modelPrefixMeshesFld->setStringValue(_outU3DObject->defaultValues.defaultMeshPefix); // Reset old value
    }

    // Set default material name
    fieldStringValue = _defaultMaterialNameFld->getStringValue();

    if ("" != fieldStringValue)
    {
      _outU3DObject->defaultValues.defaultMaterialName = fieldStringValue;
    }
    else
    {
      _defaultMaterialNameFld->setStringValue(_outU3DObject->defaultValues.defaultMaterialName); // Reset old value
    }

    _outU3DObject->defaultValues.defaultMaterialAmbientColor  = _defaultMaterialAmbientColorFld->getVector3Value();
    _outU3DObject->defaultValues.defaultMaterialDiffuseColor  = _defaultMaterialDiffuseColorFld->getVector3Value();
    _outU3DObject->defaultValues.defaultMaterialAlpha         = _defaultMaterialAlphaFld->getDoubleValue();
    _outU3DObject->defaultValues.defaultMaterialReflectivity  = _defaultMaterialReflectivityFld->getDoubleValue();
    _outU3DObject->defaultValues.defaultMaterialDiffuseColorWithTransparency = Vector4(_outU3DObject->defaultValues.defaultMaterialDiffuseColor, _outU3DObject->defaultValues.defaultMaterialAlpha);
    _outU3DObject->defaultValues.defaultMaterialSpecularColor = _defaultMaterialSpecularColorFld->getVector3Value();
    _outU3DObject->defaultValues.defaultMaterialEmissiveColor = _defaultMaterialEmissiveColorFld->getVector3Value();

    if (_addDefaultViewNodeFld->getBoolValue())
    {
      _addDefaultViewNode();
      _addDefaultViewResource();
    }

    if (_addDefaultLightNodeFld->getBoolValue())
    {
      _addDefaultLightNode();
      _addDefaultLightResource();
    }

  }

  _networkPanelStatusFld->setStringValue("U3D initialized.");
  _outU3DObjectFld->touch();
}

//***********************************************************************************

void U3DInit::_addDefaultViewNode()
{
  mlU3D::ViewNode newViewNode;
  newViewNode.displayName = _defaultViewNameFld->getStringValue();
  newViewNode.resourceName = _defaultViewNameFld->getStringValue();
  _outU3DObject->viewNodes.push_back(newViewNode);
}

//***********************************************************************************

void U3DInit::_addDefaultViewResource()
{
  mlU3D::ViewResource defaultViewResouce;
  defaultViewResouce.resourceName = _defaultViewNameFld->getStringValue();
  _outU3DObject->viewResources.push_back(defaultViewResouce);
}

//***********************************************************************************

void U3DInit::_addDefaultLightNode()
{
  mlU3D::LightNode newLightNode;
  newLightNode.displayName = _defaultLightNameFld->getStringValue();
  newLightNode.resourceName = _defaultLightNameFld->getStringValue();
  _outU3DObject->lightNodes.push_back(newLightNode);
}

//***********************************************************************************

void U3DInit::_addDefaultLightResource()
{
  mlU3D::LightResource defaultLightResouce;
  defaultLightResouce.resourceName = _defaultLightNameFld->getStringValue();
  defaultLightResouce.lightType = mlU3D::LIGHTTYPE_AMBIENT;
  defaultLightResouce.lightColor = _defaultLightColorFld->getVector3Value();
  defaultLightResouce.lightAttenuation = Vector3(1.0f, 0.0f, 0.0f);
  defaultLightResouce.lightSpotAngle = 180.0f;
  defaultLightResouce.lightIntensity = _defaultLightIntensityFld->getDoubleValue();
  _outU3DObject->lightResources.push_back(defaultLightResouce);
}

//***********************************************************************************


ML_END_NAMESPACE