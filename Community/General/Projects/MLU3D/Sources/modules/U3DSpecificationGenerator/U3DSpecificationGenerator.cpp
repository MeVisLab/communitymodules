//----------------------------------------------------------------------------------
// U3DSpecificationGenerator module.
//
// \file    U3DSpecificationGenerator.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DSpecificationGenerator.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_SpecificationGenerator.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DSpecificationGenerator, Module);


//***********************************************************************************


U3DSpecificationGenerator::U3DSpecificationGenerator() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  //! Add fields for Specification Generator
  (_newSpecificationFld                        = addString("newSpecification"))->setStringValue("");
  (_newSpecificationOutputValidFld             = addBool("newSpecificationOutputValid"))->setBoolValue(false);
  (_newSpecificationTypeFld                    = addEnum("newSpecificationType", mlU3D::SpecificationObjectTypeStrings, mlU3D::NUM_SPECIFICATION_OBJECTTYPES))->setEnumValue(mlU3D::SPECIFICATION_OBJECTTYPE_MESH);
  (_newSpecificationSelectedTabFld             = addInt("selectedTab"))->setIntValue((int)mlU3D::SPECIFICATION_OBJECTTYPE_MESH);
  (_newSpecificationObjectNameFld              = addString("newSpecificationObjectName"))->setStringValue("");
  (_newSpecificationGroupPathFld               = addString("newSpecificationGroupPath"))->setStringValue("");
  (_newSpecificationUseDefaultColorFld         = addBool("newSpecificationUseDefaultColor"))->setBoolValue(true);
  (_newSpecificationUseVertexColorsFld         = addBool("newSpecificationUseVertexColors"))->setBoolValue(false);
  (_newSpecificationUseDefaultSpecularColorFld = addBool("newSpecificationUseDefaultSpecularColor"))->setBoolValue(true);
  (_newSpecificationColorFld                   = addColor("newSpecificationColor"))->setVector3Value(Vector3(0.651f, 0.651f, 0.651f));
  (_newSpecificationColorAlphaFld              = addFloat("newSpecificationColorAlpha"))->setFloatValue(1.0f);
  (_newSpecificationSpecularColorFld           = addColor("newSpecificationSpecularColor"))->setVector3Value(Vector3(0.75f, 0.75f, 0.75f));
  (_newSpecificationModelVisibilityFld         = addEnum("newSpecificationModelVisibility", mlU3D::SpecificationModelVisibilityStrings, mlU3D::NUM_SPECIFICATION_MODELVISIBILITY))->setEnumValue(mlU3D::SPECIFICATION_MODELVISIBILITY_FRONTANDBACKVISIBLE);
  (_newSpecificationMetaDataKeyFld             = addString("newSpecificationMetaDataKey"))->setStringValue("");
  (_newSpecificationMetaDataValueFld           = addString("newSpecificationMetaDataValue"))->setStringValue("");
  (_newSpecificationWEMLabelFld                = addString("newSpecificationWEMLabel"))->setStringValue("");
  (_newSpecificationFibersFld                  = addString("newSpecificationFibers"))->setStringValue("");
  (_newSpecificationPositionTypesFld           = addString("newSpecificationPositionTypes"))->setStringValue("");
  (_newSpecificationConnectionTypesFld         = addString("newSpecificationConnectionTypes"))->setStringValue("");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DSpecificationGenerator::~U3DSpecificationGenerator()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DSpecificationGenerator::handleNotification (Field* field)
{
  if (field == _newSpecificationTypeFld)
  {
    _updateObjectTypeTabView();
  }

  if ((field == _newSpecificationTypeFld) ||
    (field == _newSpecificationObjectNameFld) ||
    (field == _newSpecificationGroupPathFld) ||
    (field == _newSpecificationUseDefaultColorFld) ||
    (field == _newSpecificationUseVertexColorsFld) ||
    (field == _newSpecificationUseDefaultSpecularColorFld) ||
    (field == _newSpecificationColorFld) ||
    (field == _newSpecificationColorAlphaFld) ||
    (field == _newSpecificationSpecularColorFld) ||
    (field == _newSpecificationModelVisibilityFld) ||
    (field == _newSpecificationMetaDataKeyFld) ||
    (field == _newSpecificationMetaDataValueFld) ||
    (field == _newSpecificationWEMLabelFld) ||
    (field == _newSpecificationFibersFld) ||
    (field == _newSpecificationPositionTypesFld) ||
    (field == _newSpecificationConnectionTypesFld)
    )
  {
    _updateNewSpecification();
  }


  // Call parent class and handle apply/autoApply and in/outputs
  Module::handleNotification(field);
}


//***********************************************************************************


void U3DSpecificationGenerator::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  Module::activateAttachments();
}


//***********************************************************************************


void U3DSpecificationGenerator::_updateObjectTypeTabView()
{
  _newSpecificationSelectedTabFld->setIntValue((int)_newSpecificationTypeFld->getEnumValue());
}


//***********************************************************************************


void U3DSpecificationGenerator::_updateNewSpecification()
{
  std::string newSpecificationString = "";
  int newSpecificationObjectType = _newSpecificationTypeFld->getEnumValue();

  //  if (callingField == ctx.field("newSpecificationType")):
  //    UpdateTabView()

  if (newSpecificationObjectType == mlU3D::SPECIFICATION_OBJECTTYPE_POINTCLOUD)
  {
    newSpecificationString = "<PointSet>\n";
    newSpecificationString += mlU3D::SpecificationGenerator::getPositionTypes(_newSpecificationPositionTypesFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "</PointSet>\n";
    newSpecificationString += "\n";
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlU3D::SPECIFICATION_OBJECTTYPE_LINESET_FROM_MARKERS)
  {
    newSpecificationString = "<LineSet>\n";
    newSpecificationString += mlU3D::SpecificationGenerator::getPositionTypes(_newSpecificationPositionTypesFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getConnectionTypes(_newSpecificationConnectionTypesFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getColor(_newSpecificationColorFld->getColorValue(), _newSpecificationColorAlphaFld->getFloatValue(), _newSpecificationUseDefaultColorFld->getBoolValue(), false);
    newSpecificationString += mlU3D::SpecificationGenerator::getModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "<LineSet>\n";
    newSpecificationString += "\n";
    //    ctx.field("selectedTab").value = 1;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlU3D::SPECIFICATION_OBJECTTYPE_LINESET_FROM_FIBERS)
  {
    newSpecificationString = "<LineSet>\n";
    newSpecificationString += mlU3D::SpecificationGenerator::getFiberSetIndices(_newSpecificationFibersFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getColor(_newSpecificationColorFld->getColorValue(), _newSpecificationColorAlphaFld->getFloatValue(), _newSpecificationUseDefaultColorFld->getBoolValue(), false);
    newSpecificationString += mlU3D::SpecificationGenerator::getModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "<LineSet>\n";
    newSpecificationString += "\n";
    //    ctx.field("selectedTab").value = 1;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlU3D::SPECIFICATION_OBJECTTYPE_MESH)
  {
    newSpecificationString = "<Mesh>\n";
    newSpecificationString += mlU3D::SpecificationGenerator::getWEMLabel(_newSpecificationWEMLabelFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getColor(_newSpecificationColorFld->getColorValue(), _newSpecificationColorAlphaFld->getFloatValue(), _newSpecificationUseDefaultColorFld->getBoolValue(), _newSpecificationUseVertexColorsFld->getBoolValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getSpecularColor(_newSpecificationSpecularColorFld->getColorValue(), _newSpecificationUseDefaultSpecularColorFld->getBoolValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getOpacity(_newSpecificationColorAlphaFld->getFloatValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "</Mesh>\n";
    newSpecificationString += "\n";
    //    ctx.field("selectedTab").value = 2;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlU3D::SPECIFICATION_OBJECTTYPE_METADATA)
  {
    newSpecificationString = "<MetaData>\n";
    newSpecificationString += mlU3D::SpecificationGenerator::getMetaDataKey(_newSpecificationMetaDataKeyFld->getStringValue());
    newSpecificationString += mlU3D::SpecificationGenerator::getMetaDataValue(_newSpecificationMetaDataValueFld->getStringValue());
    newSpecificationString += "</MetaData>\n";
    newSpecificationString += "\n";
    //    ctx.field("selectedTab").value = 3;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }

  _newSpecificationFld->setStringValue(newSpecificationString);

  return;

}


//***********************************************************************************


ML_END_NAMESPACE