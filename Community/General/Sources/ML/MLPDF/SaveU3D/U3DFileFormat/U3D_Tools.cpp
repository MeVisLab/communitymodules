//----------------------------------------------------------------------------------
//! Tools for U3D file creation.
/*!
// \file    U3D_Tools.cpp
// \author  Axel Newe
// \date    2014-01-20
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Tools.h"
#include "U3D_DataTypes.h"
#include "MLPDF_DataTypes.h"
#include "MLPDF_Tools.h"


ML_START_NAMESPACE

  
//***********************************************************************************

GroupNodeVector assembleU3DGroupNodeInfo(U3DObjectInfoVector &_u3dObjectInfoVector)   // TODO Make it more robust agains illegal group node definition strings
{
  GroupNodeVector groupNodes;
  
  GroupNodeStruct rootNode;  
  rootNode.name = "";

  groupNodes.push_back(rootNode);

  for (int i = 0; i < _u3dObjectInfoVector.size(); i++)
  {
    U3DObjectInfoStruct thisU3DObjectInfo = _u3dObjectInfoVector[i];
    std::string thisGroupPath = thisU3DObjectInfo.GroupPath;

    updateGroupNodesVector(groupNodes, thisGroupPath);
  }

  return groupNodes;
}

//***********************************************************************************

void quantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ) 
{
	u8Signs = (pos.x < 0.0) | ((pos.y < 0.0) << 1) | ((pos.z < 0.0) << 2);
	udX = (MLuint32) (0.5f + U3DQuant_Position * fabs(pos.x));
	udY = (MLuint32) (0.5f + U3DQuant_Position * fabs(pos.y));
	udZ = (MLuint32) (0.5f + U3DQuant_Position * fabs(pos.z));
}

void quantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ) 
{
	u8Signs = (pos.x < 0.0) | ((pos.y < 0.0) << 1) | ((pos.z < 0.0) << 2);
	udX = (MLuint32) (0.5f + U3DQuant_Normal * fabs(pos.x));
	udY = (MLuint32) (0.5f + U3DQuant_Normal * fabs(pos.y));
	udZ = (MLuint32) (0.5f + U3DQuant_Normal * fabs(pos.z));
}

//***********************************************************************************



ML_END_NAMESPACE