//----------------------------------------------------------------------------------
// U3DSave module.
//
// \file    U3DSave_Nodes.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Saves a U3D to disk.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DSave.h"
#include "shared/U3DInternalFormat/U3D_Object_Nodes.h"


ML_START_NAMESPACE


//***********************************************************************************


void U3DSave::_addViewNodes()
{
  mlU3D::ViewNodeVector allViewNodes = _inU3DObject->viewNodes;

  for (size_t thisNodeListIndex = 0; thisNodeListIndex < allViewNodes.size(); thisNodeListIndex++)
  {
    mlU3D::ViewNode thisViewNode = allViewNodes[thisNodeListIndex];
    _statusFld->setStringValue("Adding View Node: " + thisViewNode.displayName + ".");

    // Add view node modifier chain
    _outU3DFile->addModifierChain_ViewNode(thisViewNode.nodeName, thisViewNode.resourceName);
  }
}


//***********************************************************************************


void U3DSave::_addGroupNodes()
{
  mlU3D::GroupNodeVector allGroupNodes = _inU3DObject->groupNodes;

  for (size_t thisNodeListIndex = 0; thisNodeListIndex < allGroupNodes.size(); thisNodeListIndex++)
  {
    mlU3D::GroupNode thisGroupNode = allGroupNodes[thisNodeListIndex];

    if (thisGroupNode.nodeName != "")  // Do not add root node
    {
      _statusFld->setStringValue("Adding Group Node: " + thisGroupNode.displayName + ".");

      // Add view node modifier chain
      _outU3DFile->addModifierChain_GroupNode(thisGroupNode.nodeName, thisGroupNode.parentName);    // Add modifier chain for each group
    }
  }
}


//***********************************************************************************


void U3DSave::_addModelNodes()
{
  mlU3D::ModelNodeVector allModelNodes = _inU3DObject->modelNodes;

  for (size_t thisNodeListIndex = 0; thisNodeListIndex < allModelNodes.size(); thisNodeListIndex++)
  {
    mlU3D::ModelNode thisModelNode = allModelNodes[thisNodeListIndex];
    _statusFld->setStringValue("Adding Model Node: " + thisModelNode.displayName + ".");

    // Add view node modifier chain
    _outU3DFile->addModifierChain_ModelNodeWithShadingModifier(thisModelNode);
  }
}


//***********************************************************************************


void U3DSave::_addLightNodes()
{
  mlU3D::LightNodeVector allLightNodes = _inU3DObject->lightNodes;

  for (size_t thisNodeListIndex = 0; thisNodeListIndex < allLightNodes.size(); thisNodeListIndex++)
  {
    mlU3D::LightNode thisLightNode = allLightNodes[thisNodeListIndex];
    _statusFld->setStringValue("Adding Light Node: " + thisLightNode.displayName + ".");

    // Add light node modifier chain
    _outU3DFile->addModifierChain_LightNode(thisLightNode.nodeName, thisLightNode.resourceName);
  }
}


//***********************************************************************************


ML_END_NAMESPACE