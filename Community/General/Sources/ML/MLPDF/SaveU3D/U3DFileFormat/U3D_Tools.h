//----------------------------------------------------------------------------------
//! Tools for U3D file creation.
/*!
// \file    U3D_Tools.h
// \author  Axel Newe
// \date    2014-01-20
//
// Creates U3D file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


#ifndef _U3D_Tools_H
#define _U3D_Tools_H 

// Local includes
#include "U3D_Constants.h"
#include "U3D_DataTypes.h"
#include "U3D_FileWriter.h"
#include "shared/MLPDF_DataTypes.h"


ML_START_NAMESPACE
  

// Tool methods ============================================================

// Scans through all U3D object infos and assembles group nodes
GroupNodeVector assembleU3DGroupNodeInfo(U3DObjectInfoVector &_u3dObjectInfoVector);

// Quantize position (vertex)
void quantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);

// Quantize normal
void quantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);






ML_END_NAMESPACE

#endif // _U3D_Tools_H