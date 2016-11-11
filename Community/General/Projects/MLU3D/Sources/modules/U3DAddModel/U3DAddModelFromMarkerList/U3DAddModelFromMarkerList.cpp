//----------------------------------------------------------------------------------
// U3DManager module.
//
// \file    U3DAddModelFromMarkerList.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddModelFromMarkerList.h"
#include "../../../shared/MLU3D_Tools.h"
#include "../../../shared/MLU3D_Constants.h"
#include "../../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddModelFromMarkerList, U3DProcessor);


//***********************************************************************************

U3DAddModelFromMarkerList::U3DAddModelFromMarkerList() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  // Add input fields to the module and set allowed types.
  (_inPositionsFld = addBase("inPositions"))->setBaseValueAndAddAllowedType(&_inPositions);
  _inPositionsFld->addAllowedType<ml::ColoredMarkerList>();

  // Add and initialize parameter fields
  (_simpleModePointSetFld      = addBool("simpleModePointSet"))->setBoolValue(false);
  (_pointCloudSpecificationFld = addString("pointCloudSpecification"))->setStringValue("");

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddModelFromMarkerList::~U3DAddModelFromMarkerList()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddModelFromMarkerList::handleNotification(Field* field)
{
  if (field == _inPositionsFld)
  {
    ml::Base *inValue = _inPositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inPositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
        _inPositions = *inList;
      }

    }
    else
    {
      _inPositions.clear();
    }

    if (_autoApplyFld->getBoolValue())
    {
      process();
    }

  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddModelFromMarkerList::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//***********************************************************************************

void U3DAddModelFromMarkerList::process()
{
  // Execute module functionaylity here.
  if (_inU3DValid)
  {
    _addPointSetModelData();
    _outU3DObject->updateGroupNodes();
    //_updateGroupNodes();
    _networkPanelStatusFld->setStringValue(mlU3D::intToString((int)_outU3DObject->modelNodes.size()) + " models / " + mlU3D::intToString(_outU3DObject->getNumGeometryGenerators()) + " geom.");
  }
  else
  {
    _networkPanelStatusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}

//***********************************************************************************

void U3DAddModelFromMarkerList::_addPointSetModelData()
{
 
  mlU3D::StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PointSet><PositionTypes>all</PositionTypes></PointSet>");
  }
  else
  {
    pointSetSpecificationsVector = mlU3D::U3DTools::getModelSpecificationsStringFromUI(_pointCloudSpecificationFld, "<PointSet>");
  }

  const MLuint32 numberOfPointSetSpecifications = (MLuint32)pointSetSpecificationsVector.size();

  if (0 == numberOfPointSetSpecifications)
  {
    return;
  }

  mlU3D::GeometryGeneratorMap pointSetGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  for (MLuint32 i = 0; i < numberOfPointSetSpecifications; i++)
  {
    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(pointSetSpecificationsVector[i]);
    thisSpecificationParameters.ObjectType = mlU3D::MODELTYPE_POINTSET;

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = _outU3DObject->defaultValues.defaultPointSetPefix + " " + mlU3D::intToString((int)(1 + _outU3DObject->getNumModels(_outU3DObject->defaultValues.defaultPointSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, _outU3DObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(pointSetGeometryGeneratorMap, thisSpecificationParameters.PositionTypes, mlU3D::GEOMETRYPREFIX_POINTSET, _outU3DObject->getNumGeometryGenerators(mlU3D::GEOMETRYPREFIX_POINTSET));
    newModelNode.geometryGeneratorName = pointSetGeometryGeneratorMap[generatorKey].GeometryName;
    _outU3DObject->modelNodes.push_back(newModelNode);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = pointSetGeometryGeneratorMap.begin(); mapIterator != pointSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string specificationString = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    // Collect all position coordinates
    mlU3D::PositionsVector thisPointSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(_inPositions, specificationString);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::GetBoundingBoxFomPositions(thisPointSetPositions);
    mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newBoundingBox);

    // Add generator for point set
    mlU3D::PointSetGenerator thisPointSetGenerator;
    thisPointSetGenerator.resourceName       = geometryName;
    thisPointSetGenerator.diffuseColorCount  = 0;    // This is not really needed in this version
    thisPointSetGenerator.specularColorCount = 0;    // This is not really needed in this version
    thisPointSetGenerator.textureCoordCount  = 0;    // This is not really needed in this version
    thisPointSetGenerator.pointCount         = (MLuint32)thisPointSetPositions.size();
    thisPointSetGenerator.normalCount        = 0;    // No normals in this version since normals are not used by Acrobat
    thisPointSetGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
    thisPointSetGenerator.shadingAttributes |= ((thisPointSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisPointSetGenerator.shadingAttributes |= ((thisPointSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisPointSetGenerator.positions          = thisPointSetPositions;
    _outU3DObject->pointSets.push_back(thisPointSetGenerator);
  }

}

//***********************************************************************************


ML_END_NAMESPACE