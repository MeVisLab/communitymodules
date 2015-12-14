//----------------------------------------------------------------------------------
//!Methods for processing line sets for a U3D file.
/*!
// \file    SaveU3D_ProcessLineSets.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SaveU3D.h"
#include "../U3DFileFormat/U3D_DataTypes.h"
#include "../U3DFileFormat/U3D_Tools.h"
#include "../../shared/MLPDF_Defines.h"
#include "../../shared/MLPDF_Tools.h"
#include "../../shared/MLPDF_MarkerListTools.h"


ML_START_NAMESPACE


//***********************************************************************************


void SaveU3D::PreProcessLineSetData(U3DLineSetInfoVector &U3DLineSetInfoVector,
    	                              ModelBoundingBoxStruct& boundingBox)
{  

  StringVector lineSetSpecificationsVector;

  if (_simpleModeLineSetFld->getBoolValue())
  {
    lineSetSpecificationsVector.push_back("<U3DLineSet><PositionTypes>all</PositionTypes><ConnectionTypes>all</ConnectionTypes>");
  }
  else
  {
    lineSetSpecificationsVector = mlPDF::PDFTools::getObjectSpecificationsStringFromUI(_lineSetSpecificationFld, "<LineSet>");
  }
       
  const MLuint32 numberOfLineSetSpecifications = (MLuint32)lineSetSpecificationsVector.size();  

  if (0 == numberOfLineSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing line set specification.");

  for (MLuint32 i = 0; i < numberOfLineSetSpecifications; i++)
  {
    SpecificationParametersStruct thisSpecificationParameters = mlPDF::PDFTools::getAllSpecificationParametersFromString(lineSetSpecificationsVector[i]);

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = "Line Set " + mlPDF::intToString(i+1);
    }

    U3DLineSetInfoStruct  thisLineSetInfo;
    U3DObjectInfoStruct thisU3DObjectInfo = CreateNewU3DObjectInfo(i, U3DOBJECTTYPE_LINESET, thisSpecificationParameters.ObjectName, defaultValues);
    thisU3DObjectInfo.GroupPath     = thisSpecificationParameters.GroupPath;

    if (thisSpecificationParameters.Color == mlPDF::USEVERTEXCOLORS)
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for line sets
      thisU3DObjectInfo.DiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for line sets
      thisU3DObjectInfo.DiffuseColor = mlPDF::PDFTools::getColorVec4FromString(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisU3DObjectInfo.SpecularColor = mlPDF::PDFTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);

    thisU3DObjectInfo.DiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    thisU3DObjectInfo.Visibility    = (MLuint32)mlPDF::stringToInt(thisSpecificationParameters.ModelVisibility);
    _u3dObjectInfoVector.push_back(thisU3DObjectInfo);

    // Collect geometry info
    LineSetSpecificationStruct thisLineSetGeometry;
    thisLineSetGeometry.internalName = thisLineSetInfo.InternalName;
    thisLineSetGeometry.positions    = mlPDF::PDFMarkerListTools::getAllPositionsFromXMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);

    if ( (_inLineConnections.size() > 0) && (thisSpecificationParameters.ConnectionTypes != "simple") )
    {
      thisLineSetGeometry.lines = mlPDF::PDFMarkerListTools::getAllLinesFromIndexPairList(_inLineConnections, thisSpecificationParameters.ConnectionTypes, thisSpecificationParameters.LineWidth);
    }
    else
    {
      thisLineSetGeometry.lines = mlPDF::PDFMarkerListTools::getStandardLinesFromXMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.LineWidth);
    }

    _lineSetsGeometryVector.push_back(thisLineSetGeometry);

    // Collect line set info
    thisLineSetInfo.DiffuseColorCount  = 0;    // This is not really needed in this version
    thisLineSetInfo.SpecularColorCount = 0;    // This is not really needed in this version
    thisLineSetInfo.TextureCoordCount  = 0;    // This is not really needed in this version
    thisLineSetInfo.DefaultAmbientColor = defaultValues.defaultMaterialAmbientColor;
    thisLineSetInfo.DefaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
    thisLineSetInfo.DefaultDiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    thisLineSetInfo.DefaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;            
	  thisLineSetInfo.LineCount = (MLuint32)thisLineSetGeometry.lines.size();
	  thisLineSetInfo.PointCount = (MLuint32)thisLineSetGeometry.positions.size();
    thisLineSetInfo.NormalCount = 0; //thisLineSetInfo.LineCount; // No normals in this version since normals are not used by Acrobat
    //thisLineSetInfo.FiberSetID = i; 
    thisLineSetInfo.ShadingAttributes = U3D_SHADINGATTRIBUTES_NONE;
    thisLineSetInfo.ShadingAttributes |= ( (thisLineSetInfo.DiffuseColorCount > 0) ? U3D_SHADINGATTRIBUTES_DIFFUSECOLORS : 0 );   // Should not happen in this version
    thisLineSetInfo.ShadingAttributes |= ( (thisLineSetInfo.SpecularColorCount > 0) ? U3D_SHADINGATTRIBUTES_SPECULARCOLORS : 0 ); // Should not happen in this version
	  thisLineSetInfo.DisplayName  = thisU3DObjectInfo.DisplayName;
    thisLineSetInfo.ResourceName = thisU3DObjectInfo.ResourceName;
    U3DLineSetInfoVector.push_back(thisLineSetInfo);           

    ModelBoundingBoxStruct newboundingBox = mlPDF::PDFTools::GetBoundingBoxFomPositions(thisLineSetGeometry.positions);
    mlPDF::PDFTools::UpdateBoundingBox(boundingBox, newboundingBox);
  }
	
}


//***********************************************************************************

// Add a line set modifier chain for each FiberSet
void SaveU3D::AddAllLineSetModifierChains(U3DLineSetInfoVector U3DLineSetInfoVector)
{
  for (size_t thisFiberSetIndex = 0; thisFiberSetIndex < U3DLineSetInfoVector.size(); thisFiberSetIndex++)
  {
    U3DLineSetInfoStruct thisLineSetInfo = U3DLineSetInfoVector[thisFiberSetIndex];
    _statusFld->setStringValue("Adding LineSet: " + thisLineSetInfo.DisplayName + ".");

	  U3DDataBlockWriter continuationBlock = CreateLineSetContinuationBlock(_lineSetsGeometryVector[thisFiberSetIndex], thisLineSetInfo);
    outU3DFile->addModifierChain_LineSet(thisLineSetInfo, continuationBlock);
  }

  _progressFld->setFloatValue(0.3f);

}

//***********************************************************************************

// Add a line set continuation block for each line set
U3DDataBlockWriter SaveU3D::CreateLineSetContinuationBlock(LineSetSpecificationStruct lineSetGeometry, U3DLineSetInfoStruct lineSetInfo)
{
  MLuint8 signsXYZ;
	MLuint32 diffX;
  MLuint32 diffY;
  MLuint32 diffZ;

  _statusFld->setStringValue("Assembling data for LineSet: " + lineSetInfo.DisplayName + ".");

  U3DDataBlockWriter thisLineSetContinuationBlock;
  thisLineSetContinuationBlock.blockType = U3D_BLOCKTYPE_LINESETCONTINUATION;

	thisLineSetContinuationBlock.writeString(lineSetInfo.ResourceName);   // Write Line Set Name (9.6.3.2.1)
  thisLineSetContinuationBlock.writeU32(U3D_ReservedZero);              // Write Chain Index (9.6.3.2.2) (shall be zero) 
  thisLineSetContinuationBlock.writeU32(0x00000000);                    // Write Point Resolution Range - Start Resolution (9.6.3.2.3.1) 
  thisLineSetContinuationBlock.writeU32(lineSetInfo.PointCount);        // Write Point Resolution Range - End Resolution (9.6.3.2.3.2) (# of points)

	for (MLuint32 currentPosition = 0; currentPosition < lineSetInfo.PointCount; currentPosition++)
	{    
	  if (0 == currentPosition) 
    // Special case for first position!
    // No split position and no diff - instead the position itself is written
	  {
      PositionStruct startPosition = lineSetGeometry.positions[currentPosition];
      quantizePosition(startPosition.position, signsXYZ, diffX, diffY, diffZ);

	    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_StaticFull + 1, 0);     // Write Line Description - Split Position Index (9.6.3.2.4.1) - for first entry with special value 0 and special context "r1" instead of "r0"
  		thisLineSetContinuationBlock.writeCompressedU8(U3DContext_cPosDiffSign,signsXYZ);  // Write Line Description - New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
	  	thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffX,diffX);       // Write Line Description - New Position Info - Position Difference X (9.6.1.3.4.10.2)
		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffY,diffY);       // Write Line Description - New Position Info - Position Difference Y (9.6.1.3.4.10.3)
  		thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffZ,diffZ);       // Write Line Description - New Position Info - Position Difference Z (9.6.1.3.4.10.4)

		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cNormlCnt, 0);                    // Write Line Description - New Normal Count (9.6.3.2.4.3) 
		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPointCnt_cLineCnt_cFaceCnt, 0);  // Write Line Description - New Line Count (9.6.3.2.4.5)
    }
	  else 
	  {
	    MLuint32 splitPosition = currentPosition - 1;
	    PositionStruct currentPositionData = lineSetGeometry.positions[currentPosition];
	    PositionStruct predictedPositionData = lineSetGeometry.positions[splitPosition];
      Vector3 positionDifferenceVec3 = currentPositionData.position - predictedPositionData.position;
		  quantizePosition(positionDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

	    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_StaticFull+currentPosition, splitPosition);  // Write Line Description - Split Position Index (9.6.3.2.4.1) 
  		thisLineSetContinuationBlock.writeCompressedU8(U3DContext_cPosDiffSign,signsXYZ);                       // Write Line Description - New Position Info - Position Difference Signs (9.6.1.3.4.10.1)
	  	thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffX,diffX);                            // Write Line Description - New Position Info - Position Difference X (9.6.1.3.4.10.2)
		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffY,diffY);                            // Write Line Description - New Position Info - Position Difference Y (9.6.1.3.4.10.3)
  		thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPosDiffZ,diffZ);                            // Write Line Description - New Position Info - Position Difference Z (9.6.1.3.4.10.4)

      LinesVector newLines = mlPDF::PDFMarkerListTools::getNewLinesFromAllLines(lineSetGeometry.lines, currentPosition);
		  MLuint32 newLineCount = (MLuint32)newLines.size(); 
      
      MLuint32 newNormalCount = (MLuint32)( (lineSetInfo.NormalCount > 0) ? newLineCount : 0);                // Should be zero -> no normals in this version
		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cNormlCnt, newNormalCount*2);                // Write Line Description - New Normal Count (9.6.2.2.4.3) - always 2 normals per line
      
		  // Write New Normal Info (9.6.3.2.4.4) [UNUSED BY ACROBAT]
		  for (MLuint32 newNormalIndex = 0; newNormalIndex < newNormalCount; newNormalIndex++) 
      {
		    //Vector3 currentNormalVec3; currentNormalVec3.x = 0; currentNormalVec3.y = 0; currentNormalVec3.z = 0;
		    //Vector3 predictedNormalVec3 = currentNormalVec3;  // Normals are always 0,0,0 in this version
		    Vector3 normalDifferenceVec3;// = currentNormalVec3 - predictedNormalVec3;

        quantizeNormal(normalDifferenceVec3, signsXYZ, diffX, diffY, diffZ);

		    // Write normal at line start
        thisLineSetContinuationBlock.writeCompressedU8(U3DContext_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 

		  	// Write normal at line end
        thisLineSetContinuationBlock.writeCompressedU8(U3DContext_cDiffNormalSign, signsXYZ);   // Write Normal Difference Signs (9.6.2.2.4.4.1)
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalX, diffX);        // Write Normal Difference X (9.6.2.2.4.4.2) 
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalY, diffY);        // Write Normal Difference Y (9.6.2.2.4.4.3)
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cDiffNormalZ, diffZ);        // Write Normal Difference Z (9.6.2.2.4.4.4) 
      }
      
		  thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cPointCnt_cLineCnt_cFaceCnt, newLineCount);  // Write Point Description - New Line Count (9.6.3.2.4.5) 

		  // Write New Line Info (9.6.3.2.4.6)
		  for (MLuint32 newLineInfoNumber = 0; newLineInfoNumber < newLineCount; newLineInfoNumber++) 
      {
        MLuint32 startPosition = newLines[newLineInfoNumber].startIndex;
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cShading, 0x00000000);                       // Write Shading ID (9.6.3.2.4.6.1)
		    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_StaticFull+currentPosition, startPosition);  // Write First Position Index (9.6.3.2.4.6.2) 

        for (MLuint32 j=0; j<2; j++)
        {
			    MLuint32 normalLocalIndex = ( (newNormalCount > 0) ? 2*newLineInfoNumber + j : 0);        // newLineNumber shall always be be 0, so normalLocalIndex is 0 or 1
			    thisLineSetContinuationBlock.writeCompressedU32(U3DContext_cNormlIdx, normalLocalIndex);  // Write Normal Local Index (9.6.3.2.4.6.3) [UNUSED BY ACROBAT]

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

ML_END_NAMESPACE