//----------------------------------------------------------------------------------
//! Methods for processing XMarkers for to a PRC file.
/*!
// \file    SavePRC_ProcessLineSets.cpp
// \author  Axel Newe
// \date    2015-08-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "PRCFileFormat/PRC_DataTypes.h"
#include "PRCFileFormat/PRC_Tools.h"
#include "MLPDF_DataTypes.h"
#include "MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************

void SavePRC::PreProcessLineSetData(ModelBoundingBoxStruct& boundingBox)
{
  StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PRCPointSet><PositionTypes>all</PositionTypes>");
  }
  else
  {
    pointSetSpecificationsVector = getObjectSpecificationsStringFromUI(_lineSetSpecificationFld, "<LineSet>");
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
      thisSpecificationParameters.ObjectName = "Line Set " + intToString(i+1);
    }

    PRCObjectInfoStruct thisPRCObjectInfo = CreateNewPRCObjectInfo(i, PRCOBJECTTYPE_LINESET, thisSpecificationParameters.ObjectName, defaultValues);
    thisPRCObjectInfo.GroupPath        = thisSpecificationParameters.GroupPath;
    thisPRCObjectInfo.ParentTreeNodeID = -1;
    thisPRCObjectInfo.RGBAColor        = getColorVec4(thisSpecificationParameters.Color, Vector4(0));  // If alpha = 0 -> Adobe doesn't render
    _prcObjectInfoVector.push_back(thisPRCObjectInfo);
		
    // Collect geometry info
    LineSetSpecificationStruct thisLineSetGeometry;
    thisLineSetGeometry.internalName = thisPRCObjectInfo.InternalName;
    thisLineSetGeometry.positions    = getAllPositionsFromColoredMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);

    if ( (_inLineConnections.size() > 0) && (thisSpecificationParameters.ConnectionTypes != "simple") )
    {
      thisLineSetGeometry.lines = getAllLinesFromIndexPairList(_inLineConnections, thisSpecificationParameters.ConnectionTypes, thisSpecificationParameters.LineWidth);
    }
    else
    {
      thisLineSetGeometry.lines = getStandardLinesFromColoredMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.LineWidth);
    }

    _lineSetsGeometryVector.push_back(thisLineSetGeometry);

    ModelBoundingBoxStruct newboundingBox = GetBoundingBoxFomPositions(thisLineSetGeometry.positions);
    UpdateBoundingBox(boundingBox, newboundingBox);
  }
	
}

//***********************************************************************************

void SavePRC::WriteLineSetDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode)
{
  const bool perSegmentColor = false;
  const MLuint32 numberOfSegmentColors = 0;
  const MLuint32 numberOfSegmentColorIndices = 0;

  for (PRCObjectInfoVector::iterator it = _prcObjectInfoVector.begin(); it != _prcObjectInfoVector.end(); ++it)
  {
    PRCObjectInfoStruct thisObjectInfo = *it;

    if ( (thisObjectInfo.Type == PRCOBJECTTYPE_LINESET) && (thisObjectInfo.ParentTreeNodeID == modelTreeNode) )
    {
      outPRCFile.begingroup(thisObjectInfo.DisplayName.c_str());

      size_t numLineSets = _lineSetsGeometryVector.size();

      for (size_t s = 0; s < numLineSets; s++)
      {
        LineSetSpecificationStruct thisLineSetGeometry = _lineSetsGeometryVector[s];

        if (thisLineSetGeometry.internalName == thisObjectInfo.InternalName)
        {
          Vector3 startPositionColor = thisLineSetGeometry.positions[0].color;
          double  startPositionAlpha = thisLineSetGeometry.positions[0].alpha;

          size_t numberOfPositions = thisLineSetGeometry.positions.size();
          double (*positionsArray)[3] = new double[numberOfPositions][3];

          for (MLuint32 p = 0; p < numberOfPositions; p++) {
            positionsArray[p][0] = thisLineSetGeometry.positions[p].position.x;
            positionsArray[p][1] = thisLineSetGeometry.positions[p].position.y;
            positionsArray[p][2] = thisLineSetGeometry.positions[p].position.z;
          }

          const size_t numberOfIndices = thisLineSetGeometry.lines.size();            
          const size_t lineIndexArraySize = numberOfIndices*3;
          MLuint32 *lineIndexArray = new MLuint32[lineIndexArraySize];

          for (MLuint32 i = 0; i < numberOfIndices; i++) {
            lineIndexArray[i*3] = 2;     // Number of points in this segment
            lineIndexArray[i*3+1] = thisLineSetGeometry.lines[i].startIndex;
            lineIndexArray[i*3+2] = thisLineSetGeometry.lines[i].endIndex;
          }

          PRC_RGBAColor c(startPositionColor[0],startPositionColor[1],startPositionColor[2],startPositionAlpha);

          if (thisObjectInfo.RGBAColor[3] > 0)  // If alpha value is not = "invisible" -> Use color from UI
          {
            c.Set(thisObjectInfo.RGBAColor[0],thisObjectInfo.RGBAColor[1],thisObjectInfo.RGBAColor[2],thisObjectInfo.RGBAColor[3]);
          }

          outPRCFile.addLines((MLuint32)numberOfPositions, positionsArray, (MLuint32)lineIndexArraySize, lineIndexArray, c, thisLineSetGeometry.lines[0].width, perSegmentColor, numberOfSegmentColors, NULL, numberOfSegmentColorIndices, NULL);

          delete[] positionsArray;
          delete[] lineIndexArray;

        }
      }

      outPRCFile.endgroup();
    }

  }

}

//***********************************************************************************


ML_END_NAMESPACE