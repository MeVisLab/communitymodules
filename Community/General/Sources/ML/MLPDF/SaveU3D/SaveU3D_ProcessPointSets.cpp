//----------------------------------------------------------------------------------
//! Methods for processing point sets for a U3D file.
/*!
// \file    SaveU3D_ProcessPointSets.cpp
// \author  Axel Newe
// \date    2014-01-20
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SaveU3D.h"
#include "U3DFileFormat/U3D_DataTypes.h"
#include "U3DFileFormat/U3D_Tools.h"
#include "../shared/MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************


void SaveU3D::PreProcessPointSetData(U3DPointSetInfoVector &U3DPointSetInfoVector,
                                     ModelBoundingBoxStruct& boundingBox)
{

  StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<U3DPointSet><PositionTypes>all</PositionTypes>");
  }
  else
  {
    pointSetSpecificationsVector = getObjectSpecificationsStringFromUI(_pointCloudSpecificationFld, "<PointSet>");
  }
  
  const MLuint32 numberOfPointSetSpecifications = (MLuint32)pointSetSpecificationsVector.size();  

  if (0 == numberOfPointSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing point cloud specification.");

  for (MLuint32 i = 0; i < numberOfPointSetSpecifications; i++)
  {
    SpecificationParametersStruct thisSpecificationParameters = getAllSpecificationParametersFromString(pointSetSpecificationsVector[i]);

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = "Point Set " + intToString(i+1);
    }

    U3DObjectInfoStruct thisU3DObjectInfo = CreateNewU3DObjectInfo(i, U3DOBJECTTYPE_POINTSET, thisSpecificationParameters.ObjectName, defaultValues);
    thisU3DObjectInfo.GroupPath = thisSpecificationParameters.GroupPath;
    thisU3DObjectInfo.DiffuseColor = getColorVec4(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
    thisU3DObjectInfo.SpecularColor = getColorVec3(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);
    thisU3DObjectInfo.Visibility = (MLuint32)stringToInt(thisSpecificationParameters.ModelVisibility);
    _u3dObjectInfoVector.push_back(thisU3DObjectInfo);

    PointSetSpecificationStruct thisPointSetGeometry;
    thisPointSetGeometry.internalName = thisU3DObjectInfo.InternalName;
    thisPointSetGeometry.positions = getAllPositionsFromXMarkerList(_inPointPositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);
    _pointSetsGeometryVector.push_back(thisPointSetGeometry);

    // Collect point set info
    U3DPointSetInfoStruct thisPointSetInfo;
    thisPointSetInfo.DiffuseColorCount = 0;     // This is not really needed in this version
    thisPointSetInfo.SpecularColorCount = 0;    // This is not really needed in this version
    thisPointSetInfo.TextureCoordCount = 0;     // This is not really needed in this version
    thisPointSetInfo.DefaultAmbientColor = defaultValues.defaultMaterialAmbientColor;
    thisPointSetInfo.DefaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
    thisPointSetInfo.DefaultDiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    thisPointSetInfo.DefaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;            
	  thisPointSetInfo.PointCount = (MLuint32)thisPointSetGeometry.positions.size();
    thisPointSetInfo.NormalCount = 0; // thisPointSetInfo.PointCount; // No normals in this version since normals are not used by Acrobat
    thisPointSetInfo.ShadingAttributes = U3D_SHADINGATTRIBUTES_NONE;
    thisPointSetInfo.ShadingAttributes |= ( (thisPointSetInfo.DiffuseColorCount > 0) ? U3D_SHADINGATTRIBUTES_DIFFUSECOLORS : 0 );   // Should not happen in this version
    thisPointSetInfo.ShadingAttributes |= ( (thisPointSetInfo.SpecularColorCount > 0) ? U3D_SHADINGATTRIBUTES_SPECULARCOLORS : 0 ); // Should not happen in this version
	  thisPointSetInfo.DisplayName  = thisU3DObjectInfo.DisplayName;
    thisPointSetInfo.ResourceName = thisU3DObjectInfo.ResourceName;
    U3DPointSetInfoVector.push_back(thisPointSetInfo);                

    ModelBoundingBoxStruct newboundingBox = GetBoundingBoxFomPositions(thisPointSetGeometry.positions);
    UpdateBoundingBox(boundingBox, newboundingBox);
  }
	
}


//***********************************************************************************


// Add a point set modifier chain for each point set
void SaveU3D::AddAllPointSetModifierChains(U3DPointSetInfoVector U3DPointSetInfoVector)
{
  for (size_t thisListIndex = 0; thisListIndex < U3DPointSetInfoVector.size(); thisListIndex++)
  {
    U3DPointSetInfoStruct thisPointSetInfo = U3DPointSetInfoVector[thisListIndex];
    _statusFld->setStringValue("Adding PointSet: " + thisPointSetInfo.DisplayName + ".");

	  U3DDataBlockWriter continuationBlock = CreatePointSetContinuationBlock(_pointSetsGeometryVector[thisListIndex], thisPointSetInfo);
    outU3DFile->addModifierChain_PointSet(thisPointSetInfo, continuationBlock);
  }

  _progressFld->setFloatValue(0.2f);

}


//***********************************************************************************


// Create Point Set Continuation block for point set
U3DDataBlockWriter SaveU3D::CreatePointSetContinuationBlock(PointSetSpecificationStruct pointSetGeometry, U3DPointSetInfoStruct pointSetInfo)
{
  MLuint8 signsXYZ;
	MLuint32 diffX;
  MLuint32 diffY;
  MLuint32 diffZ;

  _statusFld->setStringValue("Assembling data for PointSet: " + pointSetInfo.DisplayName + ".");

  U3DDataBlockWriter thisPointSetContinuationBlock;
  thisPointSetContinuationBlock.blockType = U3D_BLOCKTYPE_POINTSETCONTINUATION;

	thisPointSetContinuationBlock.writeString(pointSetInfo.ResourceName);    // Write Point Set Name (9.6.2.2.1)
  thisPointSetContinuationBlock.writeU32(U3D_ReservedZero);                // Write Chain Index (9.6.2.2.2) (shall be zero) 
  thisPointSetContinuationBlock.writeU32(0x00000000);                      // Write Point Resolution Range - Start Resolution (9.6.2.2.3.1) 
  thisPointSetContinuationBlock.writeU32(pointSetInfo.PointCount);         // Write Point Resolution Range - End Resolution (9.6.2.2.3.2) (# of points)

	for (MLuint32 currentPosition = 0; currentPosition < pointSetInfo.PointCount; currentPosition++)
	{
	  MLint32 splitPositionIndex = currentPosition - 1;

	  if (0 == currentPosition) 
    // Special case for first position!
    // No split position and no diff - instead the position itself is written
	  {
	    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_StaticFull + 0x00000001, 0);  // Write Split Position Index (9.6.2.2.4.1)
	  } 
	  else 
	  {
	    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_StaticFull + currentPosition, splitPositionIndex);  // Write Split Position Index (9.6.2.2.4.1)
	  }

	  // Write New Position Info (9.6.2.2.4.2)
	  PositionStruct currentPositionData = pointSetGeometry.positions[currentPosition];
	  PositionStruct predictedPositionData;
	  if (splitPositionIndex >= 0) { predictedPositionData = pointSetGeometry.positions[splitPositionIndex]; }

    Vector3 positionDifferenceVec3 = currentPositionData.position - predictedPositionData.position;

    quantizePosition(positionDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

    thisPointSetContinuationBlock.writeCompressedU8(U3DContext_cPosDiffSign, signsXYZ);          // Write New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffX, diffX);               // Write New Position Info - Position Difference X (9.6.1.3.4.10.2)
    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffY, diffY);               // Write New Position Info - Position Difference Y (9.6.1.3.4.10.3)
    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffZ, diffZ);               // Write New Position Info - Position Difference Z (9.6.1.3.4.10.4)

		MLuint32 newPointCount = 1;  // Always add 1 new point in this version
    MLuint32 newNormalCount = ( (pointSetInfo.NormalCount > 0) ? newPointCount : 0);             // Should be zero -> no normals in this version
		thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cNormlCnt, newNormalCount);      // Write Point Description - New Normal Count (9.6.2.2.4.3) - always 1 normal per point 
      
		// Write New Normal Info (9.6.2.2.4.4) [UNUSED BY ACROBAT]
		for (MLuint32 newNormalIndex = 0; newNormalIndex < newNormalCount; newNormalIndex++) 
    {
		  //Vector3 currentNormalVec3; currentNormalVec3.x = 0; currentNormalVec3.y = 0; currentNormalVec3.z = 0;
		  //Vector3 predictedNormalVec3 = currentNormalVec3;  // Normals are always 0,0,0 in this version
		  Vector3 normalDifferenceVec3;// = currentNormalVec3 - predictedNormalVec3;

      quantizeNormal(normalDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

		  // Write normal at line start
      thisPointSetContinuationBlock.writeCompressedU8(U3DContext_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
		  thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
		  thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
		  thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 
    }

    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cPointCnt_cLineCnt_cFaceCnt, newPointCount);  // Write Point Description - New Point Count (9.6.2.2.4.5) 

	  // Write New Point Info (9.6.2.2.4.6)
	  for (MLuint32 newPointInfoNumber = 0; newPointInfoNumber < newPointCount; newPointInfoNumber++) 
    {
	    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cShading, 0x00000000);                      // Write Shading ID (9.6.2.2.4.6.1)
	    thisPointSetContinuationBlock.writeCompressedU32(U3DContext_cNormlIdx, newPointInfoNumber);             // Write Normal Local Index (9.6.2.2.4.6.2) - Must be written although no normals are used at all in this version!

      // Write no New Point Diffuse Color Coords (9.6.2.2.4.6.3) since it is not supported by this version
      // Write no New Point Specular Color Coords (9.6.2.2.4.6.4) since it is not supported by this version [UNUSED BY ACROBAT]
      // Write no New Point Texture Coords (9.6.2.2.4.6.5) since it is not supported by this version        [UNUSED BY ACROBAT]
	 }

	}  // for (MLuint32 currentPosition = 0...

  return thisPointSetContinuationBlock;
}



ML_END_NAMESPACE