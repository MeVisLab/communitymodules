//----------------------------------------------------------------------------------
//! Tool methods for creating U3D file.
/*!
// \file    SaveU3D_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SaveU3D.h"
#include "../U3DFileFormat/U3D_Tools.h"
#include "../U3DFileFormat/U3D_DataTypes.h"
#include "../../shared/MLPDF_Tools.h"


ML_START_NAMESPACE


//***********************************************************************************

U3DObjectInfoStruct SaveU3D::CreateNewU3DObjectInfo(int objectIndex, U3DObjectType objectType, std::string objectName, U3DDefaultValuesStruct defaultValues)
{
  U3DObjectInfoStruct newU3DObjectInfo;

  newU3DObjectInfo.Type = objectType;
  newU3DObjectInfo.DisplayName = objectName;
  newU3DObjectInfo.InternalName = newU3DObjectInfo.DisplayName;
  makeInternalNameUnique(newU3DObjectInfo.InternalName);

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

    if (U3DOBJECTTYPE_POINTSET == objectType)
    {
      prefix = _modelPrefixPointCloudsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
        prefix = "PointSet";
      }
    }

    if (U3DOBJECTTYPE_LINESET == objectType)
    {
      prefix = _modelPrefixLineSetsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
        prefix = "LineSet";
      }
    }

    // Not supported by Acrobat
    //if (U3DOBJECTTYPE_GLYPH == objectType)
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

  newU3DObjectInfo.Visibility = U3D_MODELVISIBILITY_FRONTANDBACKVISIBLE;

  return newU3DObjectInfo;
}

//***********************************************************************************

void SaveU3D::makeInternalNameUnique(std::string& objectName)
{
  for (size_t m = 0; m < _u3dObjectInfoVector.size(); m++)
  {
    std::string compareStr = _u3dObjectInfoVector[m].InternalName;

    // Look if name already exists
    if (objectName == compareStr) 
    {
      objectName = objectName + " ";  // Just add a space... this is a hack that allows to use names that are internally unique but not as regards the displaying in Acrobat...
      makeInternalNameUnique(objectName);
      break;
    }
  }

  return;
}

//***********************************************************************************

std::string SaveU3D::getFullGroupPathFromU3DGroupNodeStruct(GroupNodeStruct thisNode)
{
  std::string fullPath = "/";
  for (int n = 0; n < thisNode.parents.size(); n++)
  {
    fullPath += thisNode.parents[n] + "/";
  }
  fullPath += thisNode.name + "/";       
  
  return fullPath;
}

//***********************************************************************************

// Makes sure that the names of the groups in the group paths are unique
void SaveU3D::makeGroupPathNamesUnique(GroupNodeVector& groupNodes, U3DObjectInfoVector &_u3dObjectInfoVector)
{
  size_t numGroupNodes = groupNodes.size();

  for (int thisNodeIndex = 0; thisNodeIndex < numGroupNodes; thisNodeIndex++)
  {
    GroupNodeStruct &thisNode = groupNodes[thisNodeIndex];
    StringVector changedNodeParents;
    std::string originalNodeName = thisNode.name;
    std::string originalFullPath = getFullGroupPathFromU3DGroupNodeStruct(thisNode);

    bool nodeNameIsUnique = false;
    changedNodeParents.clear();     // An empty parent node vector means, that the node name has not been changed!

    while (!nodeNameIsUnique)
    {
      nodeNameIsUnique = true;

      for (int prevNodeIndex = 0; prevNodeIndex < thisNodeIndex; prevNodeIndex++)
      {
        GroupNodeStruct prevNode = groupNodes[prevNodeIndex];

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
      for (int thisNodeIndex2 = 0; thisNodeIndex2 < numGroupNodes; thisNodeIndex2++)
      {
        GroupNodeStruct &thisSearchNode = groupNodes[thisNodeIndex2];

        if (thisSearchNode.parents.size() > numChangedParentNodes)
        {

          bool parentNodesAreIdentical = true;

          for (int s = 0; s < numChangedParentNodes; s++)
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
      std::string newFullPath = getFullGroupPathFromU3DGroupNodeStruct(thisNode);

      for (int thisObjectInfoIndex = 0; thisObjectInfoIndex < _u3dObjectInfoVector.size(); thisObjectInfoIndex++)
      {
        U3DObjectInfoStruct &thisObjectInfo = _u3dObjectInfoVector[thisObjectInfoIndex];
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

U3DDefaultValuesStruct SaveU3D::getDefaultValuesFromFields()
{
  U3DDefaultValuesStruct DefaultValues;

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

std::string SaveU3D::addAllModelGroupNodes(StringVector &modelNames) const
{
  // Add all model group nodes 
  std::sort(modelNames.begin(), modelNames.end());                                        // Sort group names to prepare for removing duplicates
  modelNames.erase(std::unique(modelNames.begin(), modelNames.end()), modelNames.end());  // Remove duplicate group names  
  size_t modelGroupCount = modelNames.size();
  std::string modelGroup = "";

  for (size_t j = 0; j < modelGroupCount; j++)
  {
    outU3DFile->addModifierChain_GroupNode(modelNames[j]);           // Add modifier chain for each group
    modelGroup = modelNames[j];
  }

  return modelGroup;
}

//***********************************************************************************

void SaveU3D::addAllGroupNodes(GroupNodeVector groupNodes) const
{
  for (size_t g = 0; g < groupNodes.size(); g++)
  {
    if (groupNodes[g].name != "")  // Do not add root node
    {
      std::string parent = "";

      size_t numParents = groupNodes[g].parents.size();

      if (numParents > 0)
      {
        parent = groupNodes[g].parents[numParents-1];
      }

      outU3DFile->addModifierChain_GroupNode(groupNodes[g].name, parent);    // Add modifier chain for each group
    }
  }
}

//***********************************************************************************

void SaveU3D::addAllGroupNodes(StringVector &groupNames, std::string modelGroupName, size_t modelGroupCount) const
{
  // Add all group nodes (except the model group(s) to avoid overwriting them) 
  std::sort(groupNames.begin(), groupNames.end());                                        // Sort group names to prepare for removing duplicates
  groupNames.erase(std::unique(groupNames.begin(), groupNames.end()), groupNames.end());  // Remove dupicate group names  

  for (size_t j = 0; j < groupNames.size(); j++)
  {
    if (groupNames[j] != modelGroupName)
    {
      outU3DFile->addModifierChain_GroupNode(groupNames[j], modelGroupName);           // Add modifier chain for each group
    }
  }
}

//***********************************************************************************

void SaveU3D::addU3DModelNodes() const
{
  // Add a model node and shading modifier for each U3D object 
  for (size_t j = 0; j < _u3dObjectInfoVector.size(); j++)
    {
      U3DObjectInfoStruct thisU3DObjectInfo = _u3dObjectInfoVector[j];
      outU3DFile->addModifierChain_ModelNodeWithShadingModifier(thisU3DObjectInfo);
    }
}

//***********************************************************************************

void SaveU3D::addDefaultViewNode() const
{
  if (_addDefaultViewNodeFld->getBoolValue())
  {
    // Add default view node modifier chain
    outU3DFile->addModifierChain_ViewNode(defaultValues.defaultViewName, defaultValues.defaultViewName);
  }
}

//***********************************************************************************

void SaveU3D::addDefaultLightNode() const
{
  if (_addDefaultLightNodeFld->getBoolValue())
  {
    // Add default light node modifier chain
    outU3DFile->addModifierChain_LightNode(defaultValues.defaultLightName, defaultValues.defaultLightName);
  }
}

//***********************************************************************************

void SaveU3D::addShaderAndMaterialResources() const
{
    // Add shader & material resource for each U3D object 
    for (MLuint32 thisU3DObjectIndex = 0; thisU3DObjectIndex < _u3dObjectInfoVector.size(); thisU3DObjectIndex++)  
    {
      U3DObjectInfoStruct thisU3DObjectInfo = _u3dObjectInfoVector[thisU3DObjectIndex];

      outU3DFile->addStandardBlock_LitTextureShader(thisU3DObjectInfo.ShaderName, thisU3DObjectInfo.MaterialName);
      outU3DFile->addStandardBlock_MaterialResource(thisU3DObjectInfo);
    }
}

//***********************************************************************************

void SaveU3D::addLightResources() const
{
  if (_addDefaultLightNodeFld->getBoolValue())
  {
    // Add default light resource
    U3DLightResourceDescriptionStruct defaultLightDescription;
    defaultLightDescription.LightResourceName = defaultValues.defaultLightName;
    defaultLightDescription.LightType = U3D_LIGHTTYPE_AMBIENT;
    defaultLightDescription.LightColor = _defaultLightColorFld->getVector3Value();
    defaultLightDescription.LightAttenuation = Vector3(1.0f, 0.0f, 0.0f);
    defaultLightDescription.LightSpotAngle = 180.0f;
    defaultLightDescription.LightIntensity = _defaultLightIntensityFld->getDoubleValue();
    outU3DFile->addStandardBlock_LightResource(defaultLightDescription);
  }
}

//***********************************************************************************

void SaveU3D::addViewResources() const
{
  if (_addDefaultViewNodeFld->getBoolValue())
  {
    // Add default view resource
    outU3DFile->addStandardBlock_ViewResource(defaultValues.defaultViewName);  
  }
}

//***********************************************************************************

void SaveU3D::addMetaData(MetaDataVector &metaDataVector, ModelBoundingBoxStruct boundingBox) 
{
    _statusFld->setStringValue("Adding meta data.");

    MetaDataStruct metaDataPair;

    if (_defaultBoundingBoxMetaDataFld->getBoolValue())
    {
      metaDataPair.key = "BoundingBoxCenter";
      metaDataPair.value = mlPDF::floatToString(static_cast<float>(boundingBox.center.x)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.center.y)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.center.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.key = "BoundingBoxStart";
      metaDataPair.value = mlPDF::floatToString(static_cast<float>(boundingBox.start.x)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.start.y)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.start.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.key = "BoundingBoxEnd";
      metaDataPair.value = mlPDF::floatToString(static_cast<float>(boundingBox.end.x)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.end.y)) + " " + 
                           mlPDF::floatToString(static_cast<float>(boundingBox.end.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.key = "BoundingBoxRadius";
      metaDataPair.value = mlPDF::floatToString(static_cast<float>(boundingBox.radius));

      metaDataVector.push_back(metaDataPair);  
    }

    StringVector metaDataSpecificationsVector = mlPDF::PDFTools::getObjectSpecificationsStringFromUI(_metaDataSpecificationFld, "<MetaData>");
    const MLuint32 numberOfMEtaDataPairs = (MLuint32)metaDataSpecificationsVector.size();  

    for (MLuint32 i = 0; i < numberOfMEtaDataPairs; i++)
    {
      SpecificationParametersStruct thisSpecificationParameters = mlPDF::PDFTools::getAllSpecificationParametersFromString(metaDataSpecificationsVector[i]);

      metaDataPair.key = thisSpecificationParameters.MetaDataKey;
      metaDataPair.value = thisSpecificationParameters.MetaDataValue;

      metaDataVector.push_back(metaDataPair);  
    }

}

//***********************************************************************************


ML_END_NAMESPACE