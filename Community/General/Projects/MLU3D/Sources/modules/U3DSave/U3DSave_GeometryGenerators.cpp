//----------------------------------------------------------------------------------
// U3DSave module.
//
// \file    U3DSave_GeometryGenerators.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Saves a U3D to disk.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DSave.h"
#include "shared/MLU3D_Tools.h"
#include "shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//***********************************************************************************


U3DDataBlockWriter U3DSave::_createPointSetContinuationBlock(mlU3D::PointSetGenerator &pointSetGenerator) const
{
  MLuint8 signsXYZ;
  MLuint32 diffX;
  MLuint32 diffY;
  MLuint32 diffZ;

  _statusFld->setStringValue("Assembling data for PointSet: " + pointSetGenerator.displayName + ".");

  U3DDataBlockWriter thisPointSetContinuationBlock;
  thisPointSetContinuationBlock.blockType = mlU3D::BLOCKTYPE_POINTSETCONTINUATION;

  thisPointSetContinuationBlock.writeString(pointSetGenerator.resourceName);    // Write Point Set Name (9.6.2.2.1)
  thisPointSetContinuationBlock.writeU32(mlU3D::ReservedZero);                // Write Chain Index (9.6.2.2.2) (shall be zero) 
  thisPointSetContinuationBlock.writeU32(0x00000000);                      // Write Point Resolution Range - Start Resolution (9.6.2.2.3.1) 
  thisPointSetContinuationBlock.writeU32(pointSetGenerator.pointCount);         // Write Point Resolution Range - End Resolution (9.6.2.2.3.2) (# of points)

  for (MLuint32 currentPosition = 0; currentPosition < pointSetGenerator.pointCount; currentPosition++)
  {
    MLint32 splitPositionIndex = currentPosition - 1;

    if (0 == currentPosition)
      // Special case for first position!
      // No split position and no diff - instead the position itself is written
    {
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + 0x00000001, 0);  // Write Split Position Index (9.6.2.2.4.1)
    }
    else
    {
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + currentPosition, splitPositionIndex);  // Write Split Position Index (9.6.2.2.4.1)
    }

    // Write New Position Info (9.6.2.2.4.2)
    mlU3D::PositionStruct currentPositionData = pointSetGenerator.positions[currentPosition];
    mlU3D::PositionStruct predictedPositionData;
    if (splitPositionIndex >= 0) { predictedPositionData = pointSetGenerator.positions[splitPositionIndex]; }

    Vector3 positionDifferenceVec3 = currentPositionData.position - predictedPositionData.position;

    mlU3D::U3DTools::quantizePosition(positionDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

    thisPointSetContinuationBlock.writeCompressedU8(mlU3D::Context_cPosDiffSign, signsXYZ);          // Write New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
    thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffX, diffX);               // Write New Position Info - Position Difference X (9.6.1.3.4.10.2)
    thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffY, diffY);               // Write New Position Info - Position Difference Y (9.6.1.3.4.10.3)
    thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffZ, diffZ);               // Write New Position Info - Position Difference Z (9.6.1.3.4.10.4)

    MLuint32 newPointCount = 1;  // Always add 1 new point in this version
    MLuint32 newNormalCount = ((pointSetGenerator.normalCount > 0) ? newPointCount : 0);             // Should be zero -> no normals in this version
    thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cNormlCnt, newNormalCount);      // Write Point Description - New Normal Count (9.6.2.2.4.3) - always 1 normal per point 

    // Write New Normal Info (9.6.2.2.4.4) [UNUSED BY ACROBAT]
    for (MLuint32 newNormalIndex = 0; newNormalIndex < newNormalCount; newNormalIndex++)
    {
      //Vector3 currentNormalVec3; currentNormalVec3.x = 0; currentNormalVec3.y = 0; currentNormalVec3.z = 0;
      //Vector3 predictedNormalVec3 = currentNormalVec3;  // Normals are always 0,0,0 in this version
      Vector3 normalDifferenceVec3;// = currentNormalVec3 - predictedNormalVec3;

      mlU3D::U3DTools::quantizeNormal(normalDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

      // Write normal at line start
      thisPointSetContinuationBlock.writeCompressedU8(mlU3D::Context_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 
    }

    thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPointCnt_cLineCnt_cFaceCnt, newPointCount);  // Write Point Description - New Point Count (9.6.2.2.4.5) 

    // Write New Point Info (9.6.2.2.4.6)
    for (MLuint32 newPointInfoNumber = 0; newPointInfoNumber < newPointCount; newPointInfoNumber++)
    {
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cShading, 0x00000000);                      // Write Shading ID (9.6.2.2.4.6.1)
      thisPointSetContinuationBlock.writeCompressedU32(mlU3D::Context_cNormlIdx, newPointInfoNumber);             // Write Normal Local Index (9.6.2.2.4.6.2) - Must be written although no normals are used at all in this version!

      // Write no New Point Diffuse Color Coords (9.6.2.2.4.6.3) since it is not supported by this version
      // Write no New Point Specular Color Coords (9.6.2.2.4.6.4) since it is not supported by this version [UNUSED BY ACROBAT]
      // Write no New Point Texture Coords (9.6.2.2.4.6.5) since it is not supported by this version        [UNUSED BY ACROBAT]
    }

  }  // for (MLuint32 currentPosition = 0...

  return thisPointSetContinuationBlock;
}


//***********************************************************************************


void U3DSave::_addPointSetGeometry()
{
  for (size_t thisListIndex = 0; thisListIndex < _inU3DObject->pointSets.size(); thisListIndex++)
  {
    mlU3D::PointSetGenerator thisPointSetGenerator = _inU3DObject->pointSets[thisListIndex];
    _statusFld->setStringValue("Adding PointSet: " + thisPointSetGenerator.displayName + ".");

    U3DDataBlockWriter continuationBlock = _createPointSetContinuationBlock(thisPointSetGenerator);
    _outU3DFile->addModifierChain_PointSet(thisPointSetGenerator, continuationBlock);
  }
}


//***********************************************************************************


U3DDataBlockWriter U3DSave::_createLineSetContinuationBlock(mlU3D::LineSetGenerator &lineSetGenerator) const
{
  MLuint8 signsXYZ;
  MLuint32 diffX;
  MLuint32 diffY;
  MLuint32 diffZ;

  _statusFld->setStringValue("Assembling data for LineSet: " + lineSetGenerator.displayName + ".");

  U3DDataBlockWriter thisLineSetContinuationBlock;
  thisLineSetContinuationBlock.blockType = mlU3D::BLOCKTYPE_LINESETCONTINUATION;

  thisLineSetContinuationBlock.writeString(lineSetGenerator.resourceName);   // Write Line Set Name (9.6.3.2.1)
  thisLineSetContinuationBlock.writeU32(mlU3D::ReservedZero);              // Write Chain Index (9.6.3.2.2) (shall be zero) 
  thisLineSetContinuationBlock.writeU32(0x00000000);                    // Write Point Resolution Range - Start Resolution (9.6.3.2.3.1) 
  thisLineSetContinuationBlock.writeU32(lineSetGenerator.pointCount);        // Write Point Resolution Range - End Resolution (9.6.3.2.3.2) (# of points)

  for (MLuint32 currentPosition = 0; currentPosition < lineSetGenerator.pointCount; currentPosition++)
  {
    if (0 == currentPosition)
      // Special case for first position!
      // No split position and no diff - instead the position itself is written
    {
      mlU3D::PositionStruct startPosition = lineSetGenerator.positions[currentPosition];
      mlU3D::U3DTools::quantizePosition(startPosition.position, signsXYZ, diffX, diffY, diffZ);

      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + 1, 0);     // Write Line Description - Split Position Index (9.6.3.2.4.1) - for first entry with special value 0 and special context "r1" instead of "r0"
      thisLineSetContinuationBlock.writeCompressedU8(mlU3D::Context_cPosDiffSign, signsXYZ);  // Write Line Description - New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffX, diffX);       // Write Line Description - New Position Info - Position Difference X (9.6.1.3.4.10.2)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffY, diffY);       // Write Line Description - New Position Info - Position Difference Y (9.6.1.3.4.10.3)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffZ, diffZ);       // Write Line Description - New Position Info - Position Difference Z (9.6.1.3.4.10.4)

      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cNormlCnt, 0);                    // Write Line Description - New Normal Count (9.6.3.2.4.3) 
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPointCnt_cLineCnt_cFaceCnt, 0);  // Write Line Description - New Line Count (9.6.3.2.4.5)
    }
    else
    {
      MLuint32 splitPosition = currentPosition - 1;
      mlU3D::PositionStruct currentPositionData = lineSetGenerator.positions[currentPosition];
      mlU3D::PositionStruct predictedPositionData = lineSetGenerator.positions[splitPosition];
      Vector3 positionDifferenceVec3 = currentPositionData.position - predictedPositionData.position;
      mlU3D::U3DTools::quantizePosition(positionDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + currentPosition, splitPosition);  // Write Line Description - Split Position Index (9.6.3.2.4.1) 
      thisLineSetContinuationBlock.writeCompressedU8(mlU3D::Context_cPosDiffSign, signsXYZ);                       // Write Line Description - New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffX, diffX);                            // Write Line Description - New Position Info - Position Difference X (9.6.1.3.4.10.2)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffY, diffY);                            // Write Line Description - New Position Info - Position Difference Y (9.6.1.3.4.10.3)
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPosDiffZ, diffZ);                            // Write Line Description - New Position Info - Position Difference Z (9.6.1.3.4.10.4)

      mlU3D::LinesVector newLines = mlU3D::U3DMarkerListTools::getNewLinesFromAllLines(lineSetGenerator.lines, currentPosition);
      MLuint32 newLineCount = (MLuint32)newLines.size();

      MLuint32 newNormalCount = (MLuint32)((lineSetGenerator.normalCount > 0) ? newLineCount : 0);                // Should be zero -> no normals in this version
      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cNormlCnt, newNormalCount * 2);                // Write Line Description - New Normal Count (9.6.2.2.4.3) - always 2 normals per line

      // Write New Normal Info (9.6.3.2.4.4) [UNUSED BY ACROBAT]
      for (MLuint32 newNormalIndex = 0; newNormalIndex < newNormalCount; newNormalIndex++)
      {
        //Vector3 currentNormalVec3; currentNormalVec3.x = 0; currentNormalVec3.y = 0; currentNormalVec3.z = 0;
        //Vector3 predictedNormalVec3 = currentNormalVec3;  // Normals are always 0,0,0 in this version
        Vector3 normalDifferenceVec3;// = currentNormalVec3 - predictedNormalVec3;

        mlU3D::U3DTools::quantizeNormal(normalDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

        // Write normal at line start
        thisLineSetContinuationBlock.writeCompressedU8(mlU3D::Context_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 

        // Write normal at line end
        thisLineSetContinuationBlock.writeCompressedU8(mlU3D::Context_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 
      }

      thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cPointCnt_cLineCnt_cFaceCnt, newLineCount);  // Write Point Description - New Line Count (9.6.3.2.4.5) 

      // Write New Line Info (9.6.3.2.4.6)
      for (MLuint32 newLineInfoNumber = 0; newLineInfoNumber < newLineCount; newLineInfoNumber++)
      {
        MLuint32 startPosition = newLines[newLineInfoNumber].startIndex;
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cShading, 0x00000000);                       // Write Shading ID (9.6.3.2.4.6.1)
        thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + currentPosition, startPosition);  // Write First Position Index (9.6.3.2.4.6.2) 

        for (MLuint32 j = 0; j<2; j++)
        {
          MLuint32 normalLocalIndex = ((newNormalCount > 0) ? 2 * newLineInfoNumber + j : 0);        // newLineNumber shall always be be 0, so normalLocalIndex is 0 or 1
          thisLineSetContinuationBlock.writeCompressedU32(mlU3D::Context_cNormlIdx, normalLocalIndex);  // Write Normal Local Index (9.6.3.2.4.6.3) [UNUSED BY ACROBAT]

          // Write no New Line Diffuse Color Coords (9.6.3.2.4.6.4) since it is not supported by this version
          // Write no New Line Specular Color Coords (9.6.3.2.4.6.5) since it is not supported by this version [UNUSED BY ACROBAT]
          // Write no New Line Texture Coords (9.6.3.2.4.6.6) since it is not supported by this version        [UNUSED BY ACROBAT]
        }
      }

    }  // if (0 == currentPosition)...

  }  // for (MLuint32 currentPosition...

  return thisLineSetContinuationBlock;
}


//***********************************************************************************


void U3DSave::_addLineSetGeometry()
{
  for (size_t thisFiberSetIndex = 0; thisFiberSetIndex < _inU3DObject->lineSets.size(); thisFiberSetIndex++)
  {
    mlU3D::LineSetGenerator thisLineSetGenerator = _inU3DObject->lineSets[thisFiberSetIndex];
    _statusFld->setStringValue("Adding LineSet: " + thisLineSetGenerator.displayName + ".");

    U3DDataBlockWriter continuationBlock = _createLineSetContinuationBlock(thisLineSetGenerator);
    _outU3DFile->addModifierChain_LineSet(thisLineSetGenerator, continuationBlock);
  }
}


//***********************************************************************************


mlU3D::ColorMap U3DSave::_writeVertexColors(WEMPatch* patch, U3DDataBlockWriter& continuationBlock) const
{
  MLuint thisColorIndex = 0;
  mlU3D::ColorMap colorsMap;   // <Color,ColorIndex>

  const int numNodesInPatch = patch->getNumNodes();

  for (int n = 0; n < numNodesInPatch; n++)
  {
    WEMNode* thisNode = patch->getNodeAt(n);

    const Vector4 thisNodeColor = thisNode->getColor();

    mlU3D::ColorMap::iterator findIt = colorsMap.find(thisNodeColor);

    if (findIt == colorsMap.end())
    {
      // Color has not yet been added to map, so add it now
      colorsMap[thisNodeColor] = thisColorIndex;
      continuationBlock.writeF32Color(thisNodeColor);
      thisColorIndex++;
    }
    else
    {
      // Do nothing
    }

  }

  return colorsMap;
}


//***********************************************************************************


U3DDataBlockWriter U3DSave::_createCLODBaseMeshContinuationBlock(WEMPtr saveWEM, mlU3D::CLODMeshGenerator& meshGenerator) const
{
  mlU3D::ColorMap baseDiffuseColorsMap;

  WEMPatch* wemPatch = NULL;
  WEMFace*  wemFace = NULL;

  float progressStart = 0.2f;
  float progressEnd = 0.9f;
  float progressRangeforAllMeshes = progressEnd - progressStart; // 0.7

  float progressIntervalForOneWEMPatch = progressRangeforAllMeshes / _inU3DObject->meshes.size();
  float progressStartForThisMesh = progressStart + progressIntervalForOneWEMPatch * meshGenerator.meshNumber;
  _progressFld->setFloatValue(progressStartForThisMesh);
  _statusFld->setStringValue("Assembling data for Mesh: " + meshGenerator.displayName + ".");

  wemPatch = saveWEM->getWEMPatchById(meshGenerator.patchID);

  U3DDataBlockWriter thisCLODBaseMeshContinuationBlock;
  thisCLODBaseMeshContinuationBlock.blockType = mlU3D::BLOCKTYPE_CLODBASEMESHCONTINUATION;

  thisCLODBaseMeshContinuationBlock.writeString(meshGenerator.resourceName);       // Write Mesh Name (9.6.1.2.1)
  thisCLODBaseMeshContinuationBlock.writeU32(mlU3D::ReservedZero);               // Write Chain Index (9.6.1.2.2) (shall be zero) 
  thisCLODBaseMeshContinuationBlock.writeU32(meshGenerator.faceCount);            // Write Base Mesh Description - Face Count (9.6.1.2.3.1) (# of faces)
  thisCLODBaseMeshContinuationBlock.writeU32(meshGenerator.vertexCount);          // Write Base Mesh Description - Position Count (9.6.1.2.3.2) (# of vertices)
  thisCLODBaseMeshContinuationBlock.writeU32(meshGenerator.normalCount);          // Write Base Mesh Description - Normal Count (9.6.1.2.3.3) (# of normals)
  thisCLODBaseMeshContinuationBlock.writeU32(meshGenerator.diffuseColorCount);    // Write Base Mesh Description - Diffuse Color Count (9.6.1.2.3.4)
  thisCLODBaseMeshContinuationBlock.writeU32(meshGenerator.specularColorCount);   // Write Base Mesh Description - Specular Color Count (9.6.1.2.3.5)
  thisCLODBaseMeshContinuationBlock.writeU32(0x00000001);                    // Write Base Mesh Description - Texture Coord Count (9.6.1.2.3.6)

  // Write all vertex positions (in U3D, vertices are called "positions")        
  for (MLuint32 thisVertex = 0; thisVertex < meshGenerator.vertexCount; thisVertex++)
  {
    Vector3 wemPosition = wemPatch->getNodeAt(thisVertex)->getPosition();

    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[0]);             // Write Base Position - X (9.6.1.2.4.1.1)
    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[1]);             // Write Base Position - Y (9.6.1.2.4.1.2)
    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[2]);             // Write Base Position - Z (9.6.1.2.4.1.3)
  }

  for (MLuint32 thisNormal = 0; thisNormal < meshGenerator.normalCount; thisNormal++)
  {
    Vector3 nodeNormal = wemPatch->getNodeAt(thisNormal)->getNormal();

    thisCLODBaseMeshContinuationBlock.writeF32(nodeNormal[0]);              // Write Base Normal - X (9.6.1.2.4.2.1)
    thisCLODBaseMeshContinuationBlock.writeF32(nodeNormal[1]);              // Write Base Normal - Y (9.6.1.2.4.2.2)
    thisCLODBaseMeshContinuationBlock.writeF32(nodeNormal[2]);              // Write Base Normal - Z (9.6.1.2.4.2.3)
  }

  if (meshGenerator.diffuseColorCount > 0)
  {
    baseDiffuseColorsMap = _writeVertexColors(wemPatch, thisCLODBaseMeshContinuationBlock); // Write all Base Diffuse Colors (9.6.1.2.4.3)
  }

  for (MLuint32 thisSpecularColor = 0; thisSpecularColor < meshGenerator.specularColorCount; thisSpecularColor++)
  {
    thisCLODBaseMeshContinuationBlock.writeF32Color(meshGenerator.defaultSpecularColor, 1.0f);   // Write Base Specular Color (9.6.1.2.4.4)
  }

  // Write Write Base Texture Coord (9.6.1.2.4.5)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord U (9.6.1.2.4.5.1)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord V (9.6.1.2.4.5.2)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord S (9.6.1.2.4.5.3)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord T (9.6.1.2.4.5.4)

  for (MLuint32 thisFace = 0; thisFace < meshGenerator.faceCount; thisFace++)
  {
    thisCLODBaseMeshContinuationBlock.writeCompressedU32(mlU3D::Context_cShading, 0);       // Write Shading ID (9.6.1.2.4.6.1)

    wemFace = wemPatch->getFaceAt(thisFace);

    for (MLuint thisNode = 0; thisNode < 3; thisNode++)
    {
      WEMNode* thisWEMNode = wemFace->getNodeAt(static_cast<unsigned int>(thisNode));
      const MLuint32 VertexIndex = thisWEMNode->getEntryNumber();
      const Vector4 thisWEMNodeColor = thisWEMNode->getColor();
      MLuint32 NormalIndex = VertexIndex;

      // Write Base Corner Info - Base Position Index (9.6.1.2.4.6.2.1)
      thisCLODBaseMeshContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + meshGenerator.vertexCount, VertexIndex);

      // Write Base Corner Info - Base Normal Index (9.6.1.2.4.6.2.2)
      if (meshGenerator.normalCount > 0)
      {
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + meshGenerator.normalCount, NormalIndex);
      }

      // Write Base Corner Info - Base Diffuse Color Index (9.6.1.2.4.6.2.3)
      if (meshGenerator.diffuseColorCount > 0)
      {
        MLuint32 diffuseColorIndex = baseDiffuseColorsMap[thisWEMNodeColor];
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + meshGenerator.diffuseColorCount, diffuseColorIndex);
      }

      // Write Base Corner Info - Base Specular Color Index (9.6.1.2.4.6.2.4)
      if (meshGenerator.specularColorCount > 0)
      {
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(mlU3D::Context_StaticFull + meshGenerator.specularColorCount, 0);
      }

      // Write Base Corner Info - Base Texture Coord Index (9.6.1.2.4.6.2.5)
      //thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3D_StaticFull+1, 0);  // No texture layers
    }

    if (0 == (thisFace % 100))   // Set progress field every 100 faces to save GUI update cost
    {
      float progressFldThisFaceValue = (progressIntervalForOneWEMPatch / meshGenerator.faceCount) * (thisFace + 1);
      float progressFldValue = progressStartForThisMesh + progressFldThisFaceValue;
      _progressFld->setFloatValue(progressFldValue);
    }

  }

  return thisCLODBaseMeshContinuationBlock;

}


//***********************************************************************************


void U3DSave::_addCLODMeshGeometry()
{
  // Add a CLOD mesh declaration modifier chain for each WEM patch
  for (size_t thisMeshIndex = 0; thisMeshIndex < _inU3DObject->meshes.size(); thisMeshIndex++)
  {
    mlU3D::CLODMeshGenerator thisMeshGenerator = _inU3DObject->meshes[thisMeshIndex];
    _statusFld->setStringValue("Adding Mesh: " + thisMeshGenerator.displayName + ".");

    _outU3DFile->addModifierChain_CLODMeshDeclaration(thisMeshGenerator);

    // Continuation Block shall not be part of modifier chain!
    U3DDataBlockWriter continuationBlock = _createCLODBaseMeshContinuationBlock(thisMeshGenerator.wem, thisMeshGenerator);
    _outU3DFile->addDataBlock(continuationBlock);
  }
}



//***********************************************************************************


ML_END_NAMESPACE