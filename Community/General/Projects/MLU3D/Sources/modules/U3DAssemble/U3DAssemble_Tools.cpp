//----------------------------------------------------------------------------------
// U3DAssemble module.
//
// \file    U3DAssemble_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Assembles a U3D object.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAssemble.h"
#include "shared/MLU3D_Tools.h"
#include "shared/U3DInternalFormat/U3D_Object_DataTypes.h"

ML_START_NAMESPACE


//***********************************************************************************

ObjectInfoStruct U3DAssemble::_createNewU3DObjectInfo(int objectIndex, ModelType objectType, std::string objectName, DefaultValuesStruct defaultValues)
{
  ObjectInfoStruct newU3DObjectInfo;

  newU3DObjectInfo.Type = objectType;
  newU3DObjectInfo.DisplayName = objectName;
  newU3DObjectInfo.InternalName = newU3DObjectInfo.DisplayName;
  _makeInternalNameUnique(newU3DObjectInfo.InternalName, _allU3DObjects);

  // Set default object name if object name is empty
  if ("" == newU3DObjectInfo.DisplayName) 
  {
    std::stringstream stringconv;
    stringconv << objectIndex;
    newU3DObjectInfo.DisplayName = stringconv.str(); 

    std::string prefix = _modelPrefixMeshesFld->getStringValue();
    
    if ("" == prefix)  // Just in case the field was cleared...
    {
      prefix = "Mesh";
    }

    if (MODELTYPE_POINTSET == objectType)
    {
      prefix = _modelPrefixPointCloudsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
        prefix = "PointSet";
      }
    }

    if (MODELTYPE_LINESET == objectType)
    {
      prefix = _modelPrefixLineSetsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
        prefix = "LineSet";
      }
    }

    // Not supported by Acrobat
    //if (MODELTYPE_GLYPH == objectType)
    //{
    //  prefix = _modelPrefixGlyphsFld->getStringValue();
    //
    //  if ("" == prefix)  // Just in case the field was cleared...
    //  {
    //    prefix = "Glyph";
    //  }
    //}

    newU3DObjectInfo.DisplayName.insert(0,"__" + prefix);
  }

  newU3DObjectInfo.GroupPath = "";

  newU3DObjectInfo.AmbientColor = defaultValues.defaultMaterialAmbientColor;
  newU3DObjectInfo.SpecularColor = defaultValues.defaultMaterialSpecularColor;
  newU3DObjectInfo.DiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
  newU3DObjectInfo.EmissiveColor = defaultValues.defaultMaterialEmissiveColor;

  newU3DObjectInfo.ShaderName   = "Shader for " + newU3DObjectInfo.InternalName;
  newU3DObjectInfo.MaterialName = "Material for " + newU3DObjectInfo.InternalName;
  newU3DObjectInfo.ResourceName = "Geometry for " + newU3DObjectInfo.InternalName;

  newU3DObjectInfo.Visibility = mlU3D::MODELVISIBILITY_FRONTANDBACKVISIBLE;

  return newU3DObjectInfo;
}

//***********************************************************************************

void U3DAssemble::_makeInternalNameUnique(std::string& objectName, U3DObjectsVector &u3dObjects)
{
  for (size_t m = 0; m < u3dObjects.size(); m++)
  {
    std::string compareStr = u3dObjects[m].InternalName;

    // Look if name already exists
    if (objectName == compareStr) 
    {
      objectName = objectName + " ";  // Just add a space... this is a hack that allows to use names that are internally unique but not as regards the displaying in Acrobat...
      _makeInternalNameUnique(objectName, u3dObjects);
      break;
    }
  }

  return;
}

//***********************************************************************************

std::string U3DAssemble::_getFullGroupPathFromU3DGroupNodeStruct(mlU3D::GroupNodeStruct thisNode)
{
  std::string fullPath = "/";
  for (size_t n = 0; n < thisNode.parents.size(); n++)
  {
    fullPath += thisNode.parents[n] + "/";
  }
  fullPath += thisNode.name + "/";       
  
  return fullPath;
}

//***********************************************************************************

// Makes sure that the names of the groups in the group paths are unique
void U3DAssemble::_makeGroupPathNamesUnique(mlU3D::GroupNodeStructVector& groupNodes, U3DObjectsVector &u3dObjects)
{
  size_t numGroupNodes = groupNodes.size();

  for (size_t thisNodeIndex = 0; thisNodeIndex < numGroupNodes; thisNodeIndex++)
  {
    mlU3D::GroupNodeStruct &thisNode = groupNodes[thisNodeIndex];
    mlU3D::StringVector changedNodeParents;
    std::string originalNodeName = thisNode.name;
    std::string originalFullPath = _getFullGroupPathFromU3DGroupNodeStruct(thisNode);

    bool nodeNameIsUnique = false;
    changedNodeParents.clear();     // An empty parent node vector means, that the node name has not been changed!

    while (!nodeNameIsUnique)
    {
      nodeNameIsUnique = true;

      for (size_t prevNodeIndex = 0; prevNodeIndex < thisNodeIndex; prevNodeIndex++)
      {
        mlU3D::GroupNodeStruct prevNode = groupNodes[prevNodeIndex];

        if (thisNode.name == prevNode.name)
        {
          thisNode.name = thisNode.name + " "; // Just add a space... this is a hack that allows to use names that are internally unique but not as regards the displaying in Acrobat...
          changedNodeParents = thisNode.parents;
          nodeNameIsUnique = false;
          break; // for
        }
      }

    }
    
    size_t numChangedParentNodes = changedNodeParents.size();

    if (numChangedParentNodes > 0)  // Node name has been changed
    {
      // Now scan all parent entries, search for the parents of the node that has been changed and change the name there as well...
      for (size_t thisNodeIndex2 = 0; thisNodeIndex2 < numGroupNodes; thisNodeIndex2++)
      {
        mlU3D::GroupNodeStruct &thisSearchNode = groupNodes[thisNodeIndex2];

        if (thisSearchNode.parents.size() > numChangedParentNodes)
        {

          bool parentNodesAreIdentical = true;

          for (size_t s = 0; s < numChangedParentNodes; s++)
          {
            if (thisSearchNode.parents[s] != changedNodeParents[s])
            {
              parentNodesAreIdentical = false;
              break;
            }
          }

          // If matching parents entry was found: change noe name
          if ( (parentNodesAreIdentical) && (thisSearchNode.parents[numChangedParentNodes] == originalNodeName) )
          {
            thisSearchNode.parents[numChangedParentNodes] = thisNode.name;
          }

        }  // if (thisSearchNode.parents.size() > numChangedParentNodes)

      }  // for (int thisNodeIndex2 = 0; thisNodeIndex2 < numGroupNodes; thisNodeIndex2++)

      // Now we also need to change the names in the original U3DObjectInfoVector to set the correct parent name for the U3D object.
      std::string newFullPath = _getFullGroupPathFromU3DGroupNodeStruct(thisNode);

      for (size_t thisObjectInfoIndex = 0; thisObjectInfoIndex < u3dObjects.size(); thisObjectInfoIndex++)
      {
        ObjectInfoStruct &thisObjectInfo = u3dObjects[thisObjectInfoIndex];
        std::string &thisObjectInfoGroupPath = thisObjectInfo.GroupPath;

        size_t oldPos = thisObjectInfoGroupPath.find(originalFullPath);
        if (oldPos != std::string::npos)
        {
            thisObjectInfoGroupPath.erase(oldPos, originalFullPath.size());
            thisObjectInfoGroupPath.insert(oldPos, newFullPath);
        }

      }

    }  // if (numChangedParentNodes > 0)

  }  // for (int thisNodeIndex = 0; thisNodeIndex < numGroupNodes; thisNodeIndex++)

}

//***********************************************************************************

mlU3D::GroupNodeStructVector U3DAssemble::_assembleU3DGroupNodeInfo(U3DObjectsVector &u3dObjects)   // TODO Make it more robust agains illegal group node definition strings
{
  mlU3D::GroupNodeStructVector groupNodes;

  mlU3D::GroupNodeStruct rootNode;
  rootNode.name = "";

  groupNodes.push_back(rootNode);

  for (U3DObjectsVector::size_type i = 0; i < u3dObjects.size(); i++)
  {
    ObjectInfoStruct thisU3DObjectInfo = u3dObjects[i];
    std::string thisGroupPath = thisU3DObjectInfo.GroupPath;

    mlU3D::U3DTools::updateGroupNodesVector(groupNodes, thisGroupPath);
  }

  return groupNodes;
}

//***********************************************************************************

DefaultValuesStruct U3DAssemble::_getDefaultValuesFromFields()
{
  DefaultValuesStruct DefaultValues;

  DefaultValues.defaultModelName = "";  // No default value for this; can be set only by parameters extracted from the WEM description

  DefaultValues.defaultLightName = _defaultLightNameFld->getStringValue();
  if ("" == DefaultValues.defaultLightName) DefaultValues.defaultLightName = "DefaultAmbientLight";    

  DefaultValues.defaultViewName = _defaultViewNameFld->getStringValue();
  if ("" == DefaultValues.defaultViewName) DefaultValues.defaultViewName = "DefaultView";    

  DefaultValues.defaultMaterialAmbientColor = _defaultMaterialAmbientColorFld->getVector3Value();
  DefaultValues.defaultMaterialDiffuseColor = _defaultMaterialDiffuseColorFld->getVector3Value();
  DefaultValues.defaultMaterialSpecularColor = _defaultMaterialSpecularColorFld->getVector3Value();
  DefaultValues.defaultMaterialEmissiveColor = _defaultMaterialEmissiveColorFld->getVector3Value();
  DefaultValues.defaultMaterialAlpha         = _defaultMaterialAlphaFld->getFloatValue();

  DefaultValues.defaultMaterialDiffuseColorWithTransparency = Vector4(DefaultValues.defaultMaterialDiffuseColor, DefaultValues.defaultMaterialAlpha);

  return DefaultValues;
}

//***********************************************************************************

void U3DAssemble::_addDefaultViewNode()
{
  if (_addDefaultViewNodeFld->getBoolValue())
  {
    mlU3D::ViewNode newViewNode;

    newViewNode.displayName = defaultValues.defaultViewName;
    newViewNode.resourceName = defaultValues.defaultViewName;

    _outU3DObject->viewNodes.push_back(newViewNode);
  }
}

//***********************************************************************************

void U3DAssemble::_addAllGroupNodes()
{
  mlU3D::GroupNodeStructVector groupNodes = _assembleU3DGroupNodeInfo(_allU3DObjects);
  _makeGroupPathNamesUnique(groupNodes, _allU3DObjects);

  for (size_t g = 0; g < groupNodes.size(); g++)
  {
    mlU3D::GroupNodeStruct thisGroupNodeStruct = groupNodes[g];

    mlU3D::GroupNode newGroupNode;
    newGroupNode.nodeName = thisGroupNodeStruct.name;
    newGroupNode.displayName = "";
    newGroupNode.parentName = "";

    if (thisGroupNodeStruct.name != "")  // Do not add root node
    {
      std::string parent = "";

      size_t numParents = thisGroupNodeStruct.parents.size();

      if (numParents > 0)
      {
        newGroupNode.parentName = thisGroupNodeStruct.parents[numParents - 1];
      }

      for (size_t p = 0; p < numParents; p++)
      {
        newGroupNode.displayName += thisGroupNodeStruct.parents[p] + "/";
      }

      newGroupNode.displayName += newGroupNode.nodeName;

      _outU3DObject->groupNodes.push_back(newGroupNode);
    }

  }
}

//***********************************************************************************

void U3DAssemble::_addAllModelNodes()
{
  for (size_t j = 0; j < _allU3DObjects.size(); j++)
  {
    ObjectInfoStruct thisU3DObjectInfo = _allU3DObjects[j];

    mlU3D::ModelNode newModelNode;

    newModelNode.displayName = thisU3DObjectInfo.DisplayName;
    newModelNode.internalName = thisU3DObjectInfo.InternalName;
    newModelNode.groupPath = thisU3DObjectInfo.GroupPath;
    newModelNode.shaderName = thisU3DObjectInfo.ShaderName;
    newModelNode.resourceName = thisU3DObjectInfo.ResourceName;
    newModelNode.visibility = thisU3DObjectInfo.Visibility;

    _outU3DObject->modelNodes.push_back(newModelNode);
  }
}

//***********************************************************************************

void U3DAssemble::_addDefaultLightNode()
{
  if (_addDefaultLightNodeFld->getBoolValue())
  {
    mlU3D::LightNode newLightNode;

    newLightNode.displayName = defaultValues.defaultLightName;
    newLightNode.resourceName = defaultValues.defaultLightName;

    _outU3DObject->lightNodes.push_back(newLightNode);
  }
}

//***********************************************************************************

void U3DAssemble::_addDefaultLightResource()
{
  if (_addDefaultLightNodeFld->getBoolValue())
  {
    // Add default light resource

    mlU3D::LightResource defaultLightResouce;

    defaultLightResouce.resourceName = defaultValues.defaultLightName;
    defaultLightResouce.lightType = mlU3D::LIGHTTYPE_AMBIENT;
    defaultLightResouce.lightColor = _defaultLightColorFld->getVector3Value();
    defaultLightResouce.lightAttenuation = Vector3(1.0f, 0.0f, 0.0f);
    defaultLightResouce.lightSpotAngle = 180.0f;
    defaultLightResouce.lightIntensity = _defaultLightIntensityFld->getDoubleValue();

    _outU3DObject->lightResources.push_back(defaultLightResouce);
  }
}

//***********************************************************************************

void U3DAssemble::_addDefaultViewResource()
{
  if (_addDefaultViewNodeFld->getBoolValue())
  {
    mlU3D::ViewResource defaultViewResouce;

    defaultViewResouce.resourceName = defaultValues.defaultLightName;

    _outU3DObject->viewResources.push_back(defaultViewResouce);
  }
}

//***********************************************************************************

void U3DAssemble::_addMetaData()
{
    _statusFld->setStringValue("Adding meta data.");

    mlU3D::MetaDataStruct metaDataPair;

    if (_defaultBoundingBoxMetaDataFld->getBoolValue())
    {
      metaDataPair.key = "BoundingBoxCenter";
      metaDataPair.value = mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.center.x)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.center.y)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.center.z));

      _outU3DObject->metaData.push_back(metaDataPair);

      metaDataPair.key = "BoundingBoxStart";
      metaDataPair.value = mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.start.x)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.start.y)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.start.z));

      _outU3DObject->metaData.push_back(metaDataPair);

      metaDataPair.key = "BoundingBoxEnd";
      metaDataPair.value = mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.end.x)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.end.y)) + " " +
                           mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.end.z));

      _outU3DObject->metaData.push_back(metaDataPair);

      metaDataPair.key = "BoundingBoxRadius";
      metaDataPair.value = mlU3D::floatToString(static_cast<float>(_outU3DObject->modelBoundingBox.radius));

      _outU3DObject->metaData.push_back(metaDataPair);
    }

    mlU3D::StringVector metaDataSpecificationsVector = mlU3D::U3DTools::getObjectSpecificationsStringFromUI(_metaDataSpecificationFld, "<MetaData>");
    const MLuint32 numberOfMEtaDataPairs = (MLuint32)metaDataSpecificationsVector.size();  

    for (MLuint32 i = 0; i < numberOfMEtaDataPairs; i++)
    {
      mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(metaDataSpecificationsVector[i]);

      metaDataPair.key = thisSpecificationParameters.MetaDataKey;
      metaDataPair.value = thisSpecificationParameters.MetaDataValue;

      _outU3DObject->metaData.push_back(metaDataPair);
    }

}

//***********************************************************************************


ML_END_NAMESPACE