//----------------------------------------------------------------------------------
//! Tool methods for creating PRC file.
/*!
// \file    SavePRC_Tools.cpp
// \author  Axel Newe
// \date    2015-08-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "PRCFileFormat/PRC_Tools.h"
#include "PRCFileFormat/PRC_DataTypes.h"
#include "MLPDF_DataTypes.h"
#include "MLPDF_Tools.h"

ML_START_NAMESPACE



//***********************************************************************************

PRCObjectInfoStruct SavePRC::CreateNewPRCObjectInfo(int objectIndex, PRCObjectType objectType, std::string objectName, PRCDefaultValuesStruct defaultValues)
{
  PRCObjectInfoStruct newPRCObjectInfo;

  newPRCObjectInfo.Type = objectType;
  newPRCObjectInfo.DisplayName = objectName;
  newPRCObjectInfo.InternalName = newPRCObjectInfo.DisplayName;
  makeInternalNameUnique(newPRCObjectInfo.InternalName);

  // Set default object name if object name is empty
  if ("" == newPRCObjectInfo.DisplayName) 
  {
    std::stringstream stringconv;
    stringconv << objectIndex;
    newPRCObjectInfo.DisplayName = stringconv.str(); 

    std::string prefix = _modelPrefixMeshesFld->getStringValue();
    
    if ("" == prefix)  // Just in case the field was cleared...
    {
	    prefix = "Mesh";
    }

	  if (PRCOBJECTTYPE_POINTSET == objectType)
	  {
      prefix = _modelPrefixPointCloudsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
	      prefix = "PointSet";
      }
	  }

	  if (PRCOBJECTTYPE_LINESET == objectType)
	  {
      prefix = _modelPrefixLineSetsFld->getStringValue();
    
      if ("" == prefix)  // Just in case the field was cleared...
      {
	      prefix = "LineSet";
      }
	  }

	  //if (PRCOBJECTTYPE_GLYPH == objectType)
	  //{
    //  prefix = _modelPrefixGlyphsFld->getStringValue();
    //
    //  if ("" == prefix)  // Just in case the field was cleared...
    //  {
	  //    prefix = "Glyph";
    //  }
	  //}

    newPRCObjectInfo.DisplayName.insert(0,"__" + prefix);
  }

  newPRCObjectInfo.GroupPath = "";


  return newPRCObjectInfo;
}

//***********************************************************************************

void SavePRC::makeInternalNameUnique(std::string& objectName)
{
  for (size_t m = 0; m < _prcObjectInfoVector.size(); m++)
  {
    std::string compareStr = _prcObjectInfoVector[m].InternalName;

    // Look if name already exists
    if (objectName == compareStr) 
    {
      objectName = objectName + " ";  // Just add a space... this is a hack that allows to use names that are internally unique but not as regards the displaying in Acrobat...
      makeInternalNameUnique(objectName);
      break;
    }
  }

  return;
}

//***********************************************************************************
/*
PRCDefaultValuesStruct SavePRC::getDefaultValuesFromFields()
{
  PRCDefaultValuesStruct DefaultValues;

  DefaultValues.defaultModelName = "";  // No default value for this; can be set only by parameters extracted from the WEM description

  DefaultValues.defaultLightName = _defaultLightNameFld->getStringValue();
  if ("" == DefaultValues.defaultLightName) DefaultValues.defaultLightName = "DefaultAmbientLight";    

  DefaultValues.defaultViewName = _defaultViewNameFld->getStringValue();
  if ("" == DefaultValues.defaultViewName) DefaultValues.defaultViewName = "DefaultView";    

  DefaultValues.defaultMaterialAmbientColor = _defaultMaterialAmbientColorFld->getVector3Value();
  DefaultValues.defaultMaterialDiffuseColor = _defaultMaterialDiffuseColorFld->getVector3Value();
  DefaultValues.defaultMaterialSpecularColor = _defaultMaterialSpecularColorFld->getVector3Value();
  DefaultValues.defaultMaterialEmissiveColor = _defaultMaterialEmissiveColorFld->getVector3Value();
  DefaultValues.defaultMaterialTransparency = _defaultMaterialTransparencyFld->getFloatValue();

  DefaultValues.defaultMaterialDiffuseColorWithTransparency = Vector4(DefaultValues.defaultMaterialDiffuseColor, DefaultValues.defaultMaterialTransparency);

  return DefaultValues;
}
*/

//***********************************************************************************
/*
void SavePRC::addMetaData(PRCMetaDataVector &metaDataVector, PRCModelBoundingBoxStruct boundingBox) 
{
    _statusFld->setStringValue("Adding meta data.");

    PRCMetaDataStruct metaDataPair;

    if (_defaultBoundingBoxMetaDataFld->getBoolValue())
    {
      metaDataPair.Key = "BoundingBoxCenter";
      metaDataPair.Value = floatToString(static_cast<float>(boundingBox.center.x)) + " " + 
                           floatToString(static_cast<float>(boundingBox.center.y)) + " " + 
                           floatToString(static_cast<float>(boundingBox.center.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.Key = "BoundingBoxStart";
      metaDataPair.Value = floatToString(static_cast<float>(boundingBox.start.x)) + " " + 
                           floatToString(static_cast<float>(boundingBox.start.y)) + " " + 
                           floatToString(static_cast<float>(boundingBox.start.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.Key = "BoundingBoxEnd";
      metaDataPair.Value = floatToString(static_cast<float>(boundingBox.end.x)) + " " + 
                           floatToString(static_cast<float>(boundingBox.end.y)) + " " + 
                           floatToString(static_cast<float>(boundingBox.end.z));

      metaDataVector.push_back(metaDataPair);  

      metaDataPair.key = "BoundingBoxRadius";
      metaDataPair.value = floatToString(static_cast<float>(boundingBox.radius));

      metaDataVector.push_back(metaDataPair);  
    }

    StringVector metaDataSpecificationsVector = GetPRCObjectSpecificationsFromUI(_metaDataSpecificationFld, "<PRCMetaData>");
    const MLuint32 numberOfMEtaDataPairs = (MLuint32)metaDataSpecificationsVector.size();  

    for (MLuint32 i = 0; i < numberOfMEtaDataPairs; i++)
    {
      SpecificationParametersStruct thisSpecificationParameters = getAllPRCParametersFromSpecification(metaDataSpecificationsVector[i]);

      metaDataPair.Key = thisSpecificationParameters.PRCMetaDataKey;
      metaDataPair.Value = thisSpecificationParameters.PRCMetaDataValue;

      metaDataVector.push_back(metaDataPair);  
    }

}
*/
//***********************************************************************************
/*
// Get all PRC line connections from input connections field
LinesVector SavePRC::getAllPRCLinesFromConnectionsList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes) const
{
  LinesVector result;
  LineStruct thisLine;

  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning

  StringVector allowedConnectionTypesVector = stringSplit(allowedConnectionTypes, ",", false);

  size_t connectionsListLength = connectionsList.size();

  for (int i = 0; i < connectionsListLength; i++)
  {
    ml::IndexPair thisConnection = connectionsList[i];

    // If thisConnection is of allowed type
    if (  ("all" == allowedConnectionTypes) || ( std::find(allowedConnectionTypesVector.begin(), allowedConnectionTypesVector.end(), intToString((int)thisConnection.type)) != allowedConnectionTypesVector.end() )  )
    {
      thisLine.startIndex = static_cast<MLuint32>(thisConnection.index1);
      thisLine.endIndex = static_cast<MLuint32>(thisConnection.index2);

      // Make sure start index is < endIndex
      if (thisLine.startIndex > thisLine.endIndex)
      {
        MLuint32 temp = thisLine.endIndex;
        thisLine.endIndex = thisLine.startIndex;
        thisLine.startIndex = temp;
      }

      result.push_back(thisLine);
    }

  }
  
  return result;
}
*/
//***********************************************************************************
/*
// Automatically calculate simple line connections from input positions field
LinesVector SavePRC::getStandardPRCLinesFromPositionsList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes) const
{
  LinesVector result;
  LineStruct thisLine;
  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning

  StringVector allowedPositionTypesVector = stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  int startPositionIndex = -2;
  int endPositionIndex = -1;

  for (int i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), intToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      startPositionIndex++;
      endPositionIndex++;

      if (startPositionIndex >= 0)
      {
        thisLine.startIndex = startPositionIndex;
        thisLine.endIndex = endPositionIndex;
        result.push_back(thisLine);
      }
      
    }

  }
  
  return result;
}
*/
//***********************************************************************************
/*
// Get all PRC lines from positions 
LinesVector SavePRC::getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition) const
{
  LinesVector result;
  LineStruct newLine;

  size_t numberOfLines = allLines.size();

  for (int i = 0; i < numberOfLines; i++)
  {
    if (allLines[i].endIndex == endPosition)
    {
      newLine.startIndex = allLines[i].startIndex;
      newLine.endIndex = allLines[i].endIndex;
      result.push_back(newLine);
    }
  }

  return result;
}
*/
//***********************************************************************************


ML_END_NAMESPACE