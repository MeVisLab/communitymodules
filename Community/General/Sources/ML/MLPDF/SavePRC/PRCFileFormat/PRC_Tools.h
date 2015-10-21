//----------------------------------------------------------------------------------
//! Tools for PRC file creation.
/*!
// \file    PRC_Tools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-08-01
//
// Creates PRC file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


#ifndef _PRC_Tools_H
#define _PRC_Tools_H 

// Local includes
#include "PRC_DataTypes.h"
#include "../../shared/MLPDF_DataTypes.h"


ML_START_NAMESPACE


// Tool methods ============================================================

// Scans through all PRC object infos and assembles group nodes
GroupNodeVector assemblePRCGroupNodeInfo(PRCObjectInfoVector &_prcObjectInfoVector);

// Assigns the parent tree node IDs based on the group nodes vector
void mapParentTreeNodeIDs(PRCObjectInfoVector &_prcObjectInfoVector, GroupNodeVector groupNodes);

// Scans through all group nodes and assembles the node tree
PRCModelTreeNodeVector assemblePRCModelTreeInfo(GroupNodeVector groupNodes);

// Retrieves a model tree node from model tree
PRCModelTreeNode getNodeFromPRCModelTree(PRCModelTreeNodeVector modelTree, MLint nodeID);







ML_END_NAMESPACE

#endif // _PRC_Tools_H