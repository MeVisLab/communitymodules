//----------------------------------------------------------------------------------
//! Methods for processing XMarkers for to a PRC file.
/*!
// \file    SavePRC_ProcessPointSets.cpp
// \author  Axel Newe
// \date    2015-08-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "PRCFileFormat/PRC_DataTypes.h"
#include "PRCFileFormat/PRC_Tools.h"
#include "shared/MLPDF_DataTypes.h"
#include "shared/MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************

void SavePRC::PreProcessPointSetData(ModelBoundingBoxStruct& boundingBox)
{
  StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PRCPointSet><PositionTypes>all</PositionTypes>");
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

    PRCObjectInfoStruct thisPRCObjectInfo = CreateNewPRCObjectInfo(i, PRCOBJECTTYPE_POINTSET, thisSpecificationParameters.ObjectName, defaultValues);
    thisPRCObjectInfo.GroupPath        = thisSpecificationParameters.GroupPath;
    thisPRCObjectInfo.ParentTreeNodeID = -1;
    thisPRCObjectInfo.RGBAColor        = getColorVec4(thisSpecificationParameters.Color, Vector4(0));  // If alpha = 0 -> Adobe doesn't render
    _prcObjectInfoVector.push_back(thisPRCObjectInfo);
		
    PointSetSpecificationStruct thisPointSetGeometry;
    thisPointSetGeometry.internalName = thisPRCObjectInfo.InternalName;
    thisPointSetGeometry.positions    = getAllPositionsFromColoredMarkerList(_inPointPositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);
    _pointSetsGeometryVector.push_back(thisPointSetGeometry);

    ModelBoundingBoxStruct newboundingBox = GetBoundingBoxFomPositions(thisPointSetGeometry.positions);
    UpdateBoundingBox(boundingBox, newboundingBox);
  }
	
}

//***********************************************************************************

void SavePRC::WritePointSetDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode)
{
  for (PRCObjectInfoVector::iterator it = _prcObjectInfoVector.begin(); it != _prcObjectInfoVector.end(); ++it)
  {
    PRCObjectInfoStruct thisObjectInfo = *it;

    if ( (thisObjectInfo.Type == PRCOBJECTTYPE_POINTSET) && (thisObjectInfo.ParentTreeNodeID == modelTreeNode) )
    {
      outPRCFile.begingroup(thisObjectInfo.DisplayName.c_str());

      size_t numPointSets = _pointSetsGeometryVector.size();

      for (size_t i = 0; i < numPointSets; i++)
      {
        PointSetSpecificationStruct thisPointSetGeometry = _pointSetsGeometryVector[i];

        if (thisPointSetGeometry.internalName == thisObjectInfo.InternalName)
        {
          size_t numPoints = thisPointSetGeometry.positions.size();

          for (size_t p = 0; p < numPoints; p++)
          {
            PositionStruct thisPosition = thisPointSetGeometry.positions[p];

            double positionsArray[3];
            positionsArray[0] = thisPosition.position.x;
            positionsArray[1] = thisPosition.position.y;
            positionsArray[2] = thisPosition.position.z;
            PRC_RGBAColor positionColor(thisPosition.color.x,thisPosition.color.y,thisPosition.color.z,thisPosition.alpha);

            if (thisObjectInfo.RGBAColor[3] > 0)  // If alpha value is not = "invisible" -> Use color from UI
            {
              positionColor.Set(thisObjectInfo.RGBAColor[0],thisObjectInfo.RGBAColor[1],thisObjectInfo.RGBAColor[2],thisObjectInfo.RGBAColor[3]);
            }

            outPRCFile.addPoint(positionsArray, positionColor, thisPosition.size);
          }

        }
      }

      outPRCFile.endgroup();
    }

  }

}

//***********************************************************************************


//***********************************************************************************






ML_END_NAMESPACE