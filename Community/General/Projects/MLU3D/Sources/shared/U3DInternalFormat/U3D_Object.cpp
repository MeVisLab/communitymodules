//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Object.h"
#include "../../shared/MLU3D_Tools.h"


ML_START_NAMESPACE

namespace mlU3D {

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::U3DObject, RefCountedBase);

  //----------------------------------------------------------------------------------

  U3DObject::U3DObject() : RefCountedBase()
  {
    clear();
    resetBoundingBox();
  }

  //----------------------------------------------------------------------------------

  U3DObject::~U3DObject()
  {
  }

  //----------------------------------------------------------------------------------

  void U3DObject::clear()
  {
    metaData.clear();

    viewNodes.clear();
    groupNodes.clear();
    modelNodes.clear();
    lightNodes.clear();

    pointSets.clear();
    lineSets.clear();
    meshes.clear();

    lightResources.clear();
    viewResources.clear();
    litTextureShaders.clear();
    materialResources.clear();
    textureResources.clear();
    motionResources.clear();

    defaultValues.defaultPointSetPefix = "Point Set";
    defaultValues.defaultLineSetPefix  = "Line Set";
    defaultValues.defaultMeshPefix     = "Mesh";

    defaultValues.defaultMaterialName          = "DefaultMaterial";
    defaultValues.defaultMaterialAmbientColor  = Vector3(0.0f, 0.0f, 0.0f);
    defaultValues.defaultMaterialDiffuseColor  = Vector3(0.65f, 0.65f, 0.65f);
    defaultValues.defaultMaterialSpecularColor = Vector3(0.75f, 0.75f, 0.75f);
    defaultValues.defaultMaterialEmissiveColor = Vector3(0.0f, 0.0f, 0.0f);
    defaultValues.defaultMaterialAlpha         = 1.0;
    defaultValues.defaultMaterialReflectivity  = 0.5;
    defaultValues.defaultMaterialDiffuseColorWithTransparency = Vector4(0.65f, 0.65f, 0.65f, 1.0f);

    defaultValues.addDefaultBoundingBoxMetaData = true;
  }

  //----------------------------------------------------------------------------------

  void U3DObject::cloneFrom(const U3DObject& sourceU3D)
  {
    this->clear();

    this->metaData = sourceU3D.metaData;

    this->viewNodes = sourceU3D.viewNodes;
    this->groupNodes = sourceU3D.groupNodes;
    this->modelNodes = sourceU3D.modelNodes;
    this->lightNodes = sourceU3D.lightNodes;

    this->pointSets = sourceU3D.pointSets;
    this->lineSets = sourceU3D.lineSets;
    this->meshes = sourceU3D.meshes;

    this->lightResources = sourceU3D.lightResources;
    this->viewResources = sourceU3D.viewResources;
    this->litTextureShaders = sourceU3D.litTextureShaders;
    this->materialResources = sourceU3D.materialResources;
    this->textureResources = sourceU3D.textureResources;
    this->motionResources = sourceU3D.motionResources;

    this->defaultValues = sourceU3D.defaultValues;
  }

  //----------------------------------------------------------------------------------

  int U3DObject::getNumGeometryGenerators(std::string prefix)
  {
    int result = 0;

    int numPointSetGenerators = (int)pointSets.size();
    int numLineSetGenerators  = (int)lineSets.size();
    int numMeshGenerators     = (int)meshes.size();


    if (prefix != "")
    {
      result = 0;

      for (int m = 0; m < numPointSetGenerators; m++)
      {
        PointSetGenerator thisPointSetGenerator = pointSets[m];

        if (
          (thisPointSetGenerator.resourceName.size() >= prefix.size()) &&
          (thisPointSetGenerator.resourceName.substr(0, prefix.size()) == prefix)
          )
        {
          result++;
        }

      }

      for (int m = 0; m < numLineSetGenerators; m++)
      {
        LineSetGenerator thisLineSetGenerator = lineSets[m];

        if (
          (thisLineSetGenerator.resourceName.size() >= prefix.size()) &&
          (thisLineSetGenerator.resourceName.substr(0, prefix.size()) == prefix)
          )
        {
          result++;
        }

      }

      for (int m = 0; m < numMeshGenerators; m++)
      {
        CLODMeshGenerator thisMeshGenerator = meshes[m];

        if (
          (thisMeshGenerator.resourceName.size() >= prefix.size()) &&
          (thisMeshGenerator.resourceName.substr(0, prefix.size()) == prefix)
          )
        {
          result++;
        }

      }

    }
    else
    {
      result = numPointSetGenerators + numLineSetGenerators + numMeshGenerators;
    }

    return result;
  }

  //----------------------------------------------------------------------------------

  int U3DObject::getNumModels(std::string prefix) 
  {
    int numModels = (int)modelNodes.size();
    int result    = numModels;

    if (prefix != "")
    {
      result = 0;

      for (int m = 0; m < numModels; m++)
      {
        ModelNode thisModelNode = modelNodes[m];

        if (
            (thisModelNode.displayName.size() >= prefix.size()) && 
            (thisModelNode.displayName.substr(0, prefix.size()) == prefix)
           )
        {
          result++;
        }

      }

    }

    return result;
  }

  //----------------------------------------------------------------------------------

  std::string U3DObject::getDefaultShaderName()
  {
    return "Shader for " + defaultValues.defaultMaterialName;
  }

  //----------------------------------------------------------------------------------

  std::string U3DObject::getNetworkPanelInfo()
  {
    return mlU3D::intToString((int)this->modelNodes.size()) + " models / " + mlU3D::intToString(this->getNumGeometryGenerators()) + " geom.";
  }

  //----------------------------------------------------------------------------------

  void U3DObject::resetBoundingBox()
  {
    modelBoundingBox.start.x = ML_DOUBLE_MAX;
    modelBoundingBox.start.y = ML_DOUBLE_MAX;
    modelBoundingBox.start.z = ML_DOUBLE_MAX;
    modelBoundingBox.end.x   = ML_DOUBLE_MAX * -1;
    modelBoundingBox.end.y   = ML_DOUBLE_MAX * -1;
    modelBoundingBox.end.z   = ML_DOUBLE_MAX * -1;
  }

  //----------------------------------------------------------------------------------

  void U3DObject::updateGroupNodes()
  {
    this->groupNodes.clear();

    mlU3D::GroupNodeStructVector groupNodes = _assembleU3DGroupNodeInfo();
    _makeGroupPathNamesUnique(groupNodes);

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

        this->groupNodes.push_back(newGroupNode);
      }

    }
  }

  //----------------------------------------------------------------------------------

  GroupNodeStructVector U3DObject::_assembleU3DGroupNodeInfo()   // TODO Make it more robust agains illegal group node definition strings
  {
    mlU3D::GroupNodeStructVector groupNodes;

    mlU3D::GroupNodeStruct rootNode;
    rootNode.id = 0;
    rootNode.name = "";

    groupNodes.push_back(rootNode);

    for (mlU3D::ModelNodeVector::size_type i = 0; i < this->modelNodes.size(); i++)
    {
      mlU3D::ModelNode thisU3DModelNode = this->modelNodes[i];
      std::string thisGroupPath = thisU3DModelNode.groupPath;

      _updateGroupNodesVector(groupNodes, thisGroupPath);
    }

    return groupNodes;
  }

  //----------------------------------------------------------------------------------

  void U3DObject::_updateGroupNodesVector(GroupNodeStructVector &groupNodes, std::string thisGroupPath)
  {
    StringVector groupPathComponents = mlU3D::U3DTools::stringSplit(thisGroupPath, "/", false);
    size_t numGroupPathComponents = groupPathComponents.size();

    for (size_t i = 0; i < numGroupPathComponents; i++)
    {
      std::string thisNodeName = groupPathComponents[i];
      StringVector thisNodeParents;

      for (size_t p = 0; p < i; p++)
      {
        thisNodeParents.push_back(groupPathComponents[p]);
      }

      int existingGroupNodeIndex = -1;

      // Check if group node already exists
      for (size_t g = 0; g < groupNodes.size(); g++)
      {
        GroupNodeStruct thisGroupNode = groupNodes[g];

        if ((thisGroupNode.name == thisNodeName) && (thisGroupNode.parents == thisNodeParents))
        {
          existingGroupNodeIndex = static_cast<int>(g);
          break;
        }
      }

      if (-1 == existingGroupNodeIndex)    // Group node does not exist
      {
        // Create new group node
        GroupNodeStruct newGroupNode;
        newGroupNode.id = groupNodes.size();
        newGroupNode.name = thisNodeName;
        newGroupNode.parents = thisNodeParents;

        groupNodes.push_back(newGroupNode);
      }
      else  // Group node already exists
      {
        // Do nothing
      }

    }
  }

  //----------------------------------------------------------------------------------

  std::string U3DObject::_getFullGroupPathFromU3DGroupNodeStruct(mlU3D::GroupNodeStruct thisNode)
  {
    std::string fullPath = "/";
    for (size_t n = 0; n < thisNode.parents.size(); n++)
    {
      fullPath += thisNode.parents[n] + "/";
    }
    fullPath += thisNode.name + "/";

    return fullPath;
  }

  //----------------------------------------------------------------------------------

  void U3DObject::_makeGroupPathNamesUnique(mlU3D::GroupNodeStructVector& groupNodes)
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
            if ((parentNodesAreIdentical) && (thisSearchNode.parents[numChangedParentNodes] == originalNodeName))
            {
              thisSearchNode.parents[numChangedParentNodes] = thisNode.name;
            }

          }  // if (thisSearchNode.parents.size() > numChangedParentNodes)

        }  // for (int thisNodeIndex2 = 0; thisNodeIndex2 < numGroupNodes; thisNodeIndex2++)

        // Now we also need to change the names in the original U3DObjectInfoVector to set the correct parent name for the U3D object.
        std::string newFullPath = _getFullGroupPathFromU3DGroupNodeStruct(thisNode);

        for (size_t thisObjectInfoIndex = 0; thisObjectInfoIndex < this->modelNodes.size(); thisObjectInfoIndex++)
        {
          mlU3D::ModelNode &thisModelNode = this->modelNodes[thisObjectInfoIndex];
          std::string &thisModelInfoGroupPath = thisModelNode.groupPath;

          size_t oldPos = thisModelInfoGroupPath.find(originalFullPath);
          if (oldPos != std::string::npos)
          {
            thisModelInfoGroupPath.erase(oldPos, originalFullPath.size());
            thisModelInfoGroupPath.insert(oldPos, newFullPath);
          }

        }

      }  // if (numChangedParentNodes > 0)

    }  // for (int thisNodeIndex = 0; thisNodeIndex < numGroupNodes; thisNodeIndex++)

  }

  //----------------------------------------------------------------------------------

} // end namespace mlU3D

ML_END_NAMESPACE
