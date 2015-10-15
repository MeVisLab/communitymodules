//----------------------------------------------------------------------------------
//! Tools for PRC file creation.
/*!
// \file    PRC_Tools.cpp
// \author  Axel Newe
// \date    2015-08-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PRC_Tools.h"
#include "PRC_DataTypes.h"
#include "../../shared/MLPDF_DataTypes.h"
#include "../../shared/MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************

GroupNodeVector assemblePRCGroupNodeInfo(PRCObjectInfoVector &_prcObjectInfoVector)   // TODO Make it more robust agains illegal group node definition strings
{
  GroupNodeVector groupNodes;
  
  GroupNodeStruct rootNode;  
  rootNode.name = "";

  groupNodes.push_back(rootNode);

  for (int i = 0; i < _prcObjectInfoVector.size(); i++)
  {
    PRCObjectInfoStruct thisPRCObjectInfo = _prcObjectInfoVector[i];
    std::string thisGroupPath = thisPRCObjectInfo.GroupPath;

    updateGroupNodesVector(groupNodes, thisGroupPath);
  }

  return groupNodes;
}


//***********************************************************************************

void mapParentTreeNodeIDs(PRCObjectInfoVector &_prcObjectInfoVector, GroupNodeVector groupNodes)
{
  for (PRCObjectInfoVector::iterator it = _prcObjectInfoVector.begin(); it != _prcObjectInfoVector.end(); it++) 
  {
    PRCObjectInfoStruct &thisObjectInfo = *it;

    std::string thisGroupPath = thisObjectInfo.GroupPath;
    thisObjectInfo.ParentTreeNodeID = 0;  // Set root as default

    if ( ("" != thisGroupPath) && ("/" != thisGroupPath) )
    {
      StringVector groupPathComponents = stringSplit(thisGroupPath,"/", false);
      size_t numGroupPathComponents = groupPathComponents.size();

      if (numGroupPathComponents > 0)
      {
        std::string parent = groupPathComponents[numGroupPathComponents-1];   // The last component is the direct parent
        StringVector grandparents;

        for (int gp = 0; gp < (numGroupPathComponents-1); gp++)  // Fails safely if no grandparents exist, because (numGroupPathComponents-1) = 0
        {
          grandparents.push_back(groupPathComponents[gp]);
        }

        // Now we have the parents and grandparents specified. 
        // All we have to do is to check, where exactly this combination of parents and grandparents exists in the groupNodes vector...

        for (int gn = 0; gn < groupNodes.size(); gn++)
        {
          GroupNodeStruct thisGroupNode = groupNodes[gn];

          if ( (thisGroupNode.name == parent) && (thisGroupNode.parents == grandparents) )
          {
            thisObjectInfo.ParentTreeNodeID = thisGroupNode.id;
          }

        }  // for (int gn = 0; gn < groupNodes.size(); gn++)

      }  // if (numGroupPathComponents > 0)

    }  // if ( ("" != thisGroupPath) && ("/" != thisGroupPath) )

  }  // for (PRCObjectInfoVector::iterator it = _prcObjectInfoVector.begin(); it != _prcObjectInfoVector.end(); it++) 

}

//***********************************************************************************

void findChildrenIDs(GroupNodeVector inGroupNodes, PRCModelTreeNode &node, StringVector compareParents)
{
  GroupNodeVector outGroupNodes;

  for (int i = 1; i < inGroupNodes.size(); i++) // Start with element index #1 since #0 is always the root node.
  {
    GroupNodeStruct thisInGroupNode = inGroupNodes[i];

    if (thisInGroupNode.parents == compareParents)
    {
      node.ChildrenIDs.push_back(thisInGroupNode.id);      
    }
    else
    {
      outGroupNodes.push_back(thisInGroupNode);
    }
  
  }  

}

//***********************************************************************************

void addChildrenIDsToModelTree(PRCModelTreeNodeVector &modelTree, GroupNodeVector groupNodes, PRCModelTreeNode rootNode)
{
  for (int c = 0; c < rootNode.ChildrenIDs.size(); c++)
  {
    MLint thisNodeChildID = rootNode.ChildrenIDs[c];
    std::string thisNodeChildName = "";
    StringVector compareParents;

    for (int gn = 1; gn < groupNodes.size(); gn++)
    {
      if (groupNodes[gn].id == thisNodeChildID)
      {
        thisNodeChildName = groupNodes[gn].name;
        compareParents = groupNodes[gn].parents;
        break;
      }
    }

    PRCModelTreeNode newNode;
    newNode.ID = thisNodeChildID;
    newNode.Name = thisNodeChildName;
    compareParents.push_back(thisNodeChildName);

    findChildrenIDs(groupNodes, newNode, compareParents);

    modelTree.push_back(newNode);

    addChildrenIDsToModelTree(modelTree, groupNodes, newNode);

  }

}

//***********************************************************************************

PRCModelTreeNodeVector assemblePRCModelTreeInfo(GroupNodeVector groupNodes)   
{
  PRCModelTreeNodeVector modelTree;

  PRCModelTreeNode rootNode;
  rootNode.ID = 0;
  rootNode.Name = "_root_";
  StringVector compareParents;

  findChildrenIDs(groupNodes, rootNode, compareParents);

  modelTree.push_back(rootNode);

  addChildrenIDsToModelTree(modelTree, groupNodes, rootNode);

  return modelTree;
}

//***********************************************************************************

PRCModelTreeNode getNodeFromPRCModelTree(PRCModelTreeNodeVector modelTree, MLint nodeID)
{
  PRCModelTreeNode result;
  result.ID = -1;

  for (int i = 0; i < modelTree.size(); i++)
  {
    PRCModelTreeNode thisNode = modelTree[i];

    if (thisNode.ID == nodeID)
    {
      result = thisNode;
      break;
    }

  }

  return result;
}

//***********************************************************************************




ML_END_NAMESPACE