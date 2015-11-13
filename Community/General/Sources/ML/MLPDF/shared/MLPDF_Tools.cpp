//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#include "MLPDF_DataTypes.h"
#include "MLPDF_Tools.h"

#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

#include "mlModuleIncludes.h"
#include "mlApplicationProperties.h"

ML_START_NAMESPACE

namespace mlPDF {

//***********************************************************************************

// Get data from object (point cloud, line set, mesh) specification fields
StringVector PDFTools::getObjectSpecificationsStringFromUI(ml::StringField *inputField, std::string delimiter)
{
  StringVector result;

  std::string specificationString = inputField->getStringValue();

  result = stringSplit(specificationString, delimiter, false);  // TODO Make stringSplit robust against lower/upper case

  return result;
}

//***********************************************************************************

// Parses the specification string from the UI and extracts a specific U3D property
std::string PDFTools::getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult)
{
  std::string result = failResult; 
  
  std::string startKeyword = parameterKeyword;
  std::string endKeyword = parameterKeyword;
  endKeyword.insert(1,"/");

  size_t startKeywordPosition = specificationString.find(startKeyword);

  if (startKeywordPosition != std::string::npos)
  {
    std::string keywordSectionPlusRest = specificationString.substr(startKeywordPosition);
    size_t endKeywordPosition = keywordSectionPlusRest.find(endKeyword);

    if (endKeywordPosition == std::string::npos)
    {
      endKeywordPosition = specificationString.length()-1;
    }

    std::string keywordSection = keywordSectionPlusRest.substr(0, endKeywordPosition);

    if ( (keywordSection != "") && (keywordSection.length() > startKeyword.length()) )
    {
      std::string resultCandidate = keywordSection.substr(startKeyword.length());
      result = stringTrimWhitespace(resultCandidate);
    }     
  }
  
  return result;
}

//***********************************************************************************

// Parses input string from UI and extracts object specification parameters
SpecificationParametersStruct PDFTools::getAllSpecificationParametersFromString(const std::string specificationString)
{
  SpecificationParametersStruct result;

  result.ObjectName       = getSpecificParameterFromString(specificationString, "<ObjectName>", "Object");
  result.GroupPath        = normalizeGroupPath(getSpecificParameterFromString(specificationString, "<GroupPath>"));
  result.Color            = getSpecificParameterFromString(specificationString, "<Color>");
  result.SpecularColor    = getSpecificParameterFromString(specificationString, "<SpecularColor>");
  result.GlyphText        = getSpecificParameterFromString(specificationString, "<GlyphText>");     // U3D Glyphs are not supported by Acrobat... :-(
  result.MetaDataKey      = getSpecificParameterFromString(specificationString, "<MetaDataKey>");
  result.MetaDataValue    = getSpecificParameterFromString(specificationString, "<MetaDataValue>");
  result.ModelVisibility  = getSpecificParameterFromString(specificationString, "<ModelVisibility>", "3");            
  result.WEMLabel         = getSpecificParameterFromString(specificationString, "<WEMLabel>");
  result.PositionTypes    = getSpecificParameterFromString(specificationString, "<PositionTypes>", "all");
  result.ConnectionTypes  = getSpecificParameterFromString(specificationString, "<ConnectionTypes>", "simple");
  result.PointSize        = stringToDouble(getSpecificParameterFromString(specificationString, "<PointSize>", "3"));
  result.LineWidth        = stringToDouble(getSpecificParameterFromString(specificationString, "<LineWidth>", "1"));

 return result;
}


//***********************************************************************************

std::string PDFTools::getSpecificParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult)
{
  std::string result = failResult; 
  std::string keyword (parameter+"=");
  size_t keywordPosition = wemDescription.find(keyword);

  if (keywordPosition != std::string::npos)
  {
    std::string keywordSectionPlusRest = wemDescription.substr(keywordPosition);
    size_t delimiterPosition = keywordSectionPlusRest.find(";");

    std::string keywordSection = keywordSectionPlusRest.substr(0, delimiterPosition);

    if (keywordSection != "") 
    {
      std::string resultCandidate = keywordSection.substr(keyword.length());
      result = resultCandidate;
    }     
  }

  return result;
}

//***********************************************************************************

//***********************************************************************************

std::string PDFTools::getParentNameFromGroupPath(std::string groupPath)  
{
  std::string result = "";  // Default = root node
    
  StringVector groupPathComponents = stringSplit(groupPath,"/", false);

  if (groupPathComponents.size() > 0)
  {
    result = groupPathComponents.back();
  }

  return result;
}

//***********************************************************************************

// Make sure each group path has a leading and a trailing "/"
std::string PDFTools::normalizeGroupPath(std::string groupPath)
{
  std::string result = groupPath;

  if (result == "")
  {
    result = "/";
  }
  else
  {
    if (groupPath[0] != '/')
    {
      result = "/" + result;
    }

    if (groupPath[groupPath.size()-1] != '/')
    {
      result += "/";
    }
  }

  return result;
}

//***********************************************************************************

void PDFTools::updateGroupNodesVector(GroupNodeVector &groupNodes, std::string thisGroupPath)
{
    StringVector groupPathComponents = stringSplit(thisGroupPath,"/", false);
    size_t numGroupPathComponents = groupPathComponents.size();

    for (int i = 0; i < numGroupPathComponents; i++)
    {
      std::string thisNodeName = groupPathComponents[i];
      StringVector thisNodeParents;

      for (int p = 0; p < i; p++)
      {
        thisNodeParents.push_back(groupPathComponents[p]);
      }

      int existingGroupNodeIndex = -1;

      // Check if group node already exists
      for (int g = 0; g < groupNodes.size(); g++)
      {
        GroupNodeStruct thisGroupNode = groupNodes[g];

        if ( (thisGroupNode.name == thisNodeName) && (thisGroupNode.parents == thisNodeParents) )
        {
          existingGroupNodeIndex = g;
          break;
        }       
      }

      if (-1 == existingGroupNodeIndex)    // Group node does not exist
      {
        // Create new group node
        GroupNodeStruct newGroupNode;  
        newGroupNode.id = groupNodes.size();
        newGroupNode.name = thisNodeName;
        newGroupNode.parents = thisNodeParents;

        groupNodes.push_back(newGroupNode);
      }
      else  // Group node already exists
      {
        // Do nothing
      }

    }
}

//***********************************************************************************

StringVector PDFTools::stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty)
{
  StringVector result;

  if (delimiter.empty()) {
      result.push_back(stringToSplit);
      return result;
  }

  std::string::const_iterator substart = stringToSplit.begin(), subend;

  bool endlessLoop = true;  // Do this to avoid warning
  while (endlessLoop) 
  {
    subend = search(substart, stringToSplit.end(), delimiter.begin(), delimiter.end());
    std::string temp(substart, subend);

    // Trim Whitespace
    temp = stringTrimWhitespace(temp); 

    if (keepEmpty || !temp.empty()) 
    {
      result.push_back(temp);
    }
    if (subend == stringToSplit.end()) 
    {
      break;
    }
    substart = subend + delimiter.size();
  }

  return result;
}

//***********************************************************************************

void PDFTools::stringLower(std::string &sourceString)
{
  for(unsigned short loop=0;loop < sourceString.size();loop++)
  {
    sourceString[loop] = (char)std::tolower(sourceString[loop]);
  }
}

//***********************************************************************************

bool PDFTools::stringToBool(std::string sourceString)
{
  stringLower(sourceString);

  return ((sourceString == "true") || (sourceString != "0"));
}

//***********************************************************************************

std::string PDFTools::getMeVisLabVersionNumberString()
{
  /*
  std::string MeVisLabVersionString = intToString(MEVISLAB_VERSION); // Preprocessor define must be parsed

  std::string MeVisLabMajorVersionString = MeVisLabVersionString.substr(0,MeVisLabVersionString.length()-2);
  std::string MeVisLabMinorVersionString = MeVisLabVersionString.substr(MeVisLabVersionString.length()-2,2);

  if (MeVisLabMinorVersionString[0] == '0')
  {
    MeVisLabMinorVersionString = MeVisLabMinorVersionString.substr(1,1);
  }

  MeVisLabVersionString = MeVisLabMajorVersionString + "." + MeVisLabMinorVersionString;

  return MeVisLabVersionString;
  */

  return ml::ApplicationProperties::getString("Version");
}

//***********************************************************************************

std::string PDFTools::getModuleVersionNumberString()
{
  return "1.2";
}

//***********************************************************************************

Vector4 PDFTools::getColorVec4(std::string colorString, const Vector4 defaultColor) 
{
  Vector4 result = defaultColor;

  size_t cutPos;
  StringVector parts;

  while( (cutPos = colorString.find_first_of(" ")) != colorString.npos )
  {
    if (cutPos > 0)
    {
      parts.push_back(colorString.substr(0,cutPos));
    }

    colorString = colorString.substr(cutPos+1);
  }

  if (colorString.length() > 0)
  {
    parts.push_back(colorString);
  }

  if ( (parts.size() == 3) || (parts.size() == 4) )
  {
    for (size_t i = 0; i < parts.size(); i++)
    {
      result[i] = stringToDouble(parts[i]);
    }

    if (parts.size() == 3)
    {
      result[3] = 1;  // Set alpha value to default = 1 if only color triplet was given
    }

  }

  return result;
}

Vector3 PDFTools::getColorVec3(std::string colorString, const Vector3 defaultColor) 
{
  Vector3 result = defaultColor;

  size_t cutPos;
  StringVector parts;

  while( (cutPos = colorString.find_first_of(" ")) != colorString.npos )
  {
    if (cutPos > 0)
    {
      parts.push_back(colorString.substr(0,cutPos));
    }

    colorString = colorString.substr(cutPos+1);
  }

  if (colorString.length() > 0)
  {
    parts.push_back(colorString);
  }

  if ( (parts.size() == 3) )
  {      
    for (size_t i = 0; i < parts.size(); i++)
    {
      result[i] = stringToDouble(parts[i]);
    }
  }

  return result;
}

//***********************************************************************************

// Updates the model bounding box
void PDFTools::UpdateBoundingBox(ModelBoundingBoxStruct& existingBoundingBox, ModelBoundingBoxStruct newCorners)
{
  MLdouble smallestX = ML_DOUBLE_MAX;
  MLdouble smallestY = ML_DOUBLE_MAX;
  MLdouble smallestZ = ML_DOUBLE_MAX;
   
  MLdouble biggestX = ML_DOUBLE_MIN;
  MLdouble biggestY = ML_DOUBLE_MIN;
  MLdouble biggestZ = ML_DOUBLE_MIN;

  if (newCorners.start.x < smallestX) { smallestX = newCorners.start.x; }
  if (newCorners.start.x > biggestX)  { biggestX = newCorners.start.x; }
  if (newCorners.end.x < smallestX)   { smallestX = newCorners.end.x; }
  if (newCorners.end.x > biggestX)    { biggestX = newCorners.end.x; }

  if (newCorners.start.y < smallestY) { smallestY = newCorners.start.y; }
  if (newCorners.start.y > biggestY)  { biggestY = newCorners.start.y; }
  if (newCorners.end.y < smallestY)   { smallestY = newCorners.end.y; }
  if (newCorners.end.y > biggestY)    { biggestY = newCorners.end.y; }

  if (newCorners.start.z < smallestZ) { smallestZ = newCorners.start.z; }
  if (newCorners.start.z > biggestZ)  { biggestZ = newCorners.start.z; }
  if (newCorners.end.z < smallestZ)   { smallestZ = newCorners.end.z; }
  if (newCorners.end.z > biggestZ)    { biggestZ = newCorners.end.z; }

  if (existingBoundingBox.start.x > smallestX) { existingBoundingBox.start.x = smallestX; }
  if (existingBoundingBox.start.y > smallestY) { existingBoundingBox.start.y = smallestY; }
  if (existingBoundingBox.start.z > smallestZ) { existingBoundingBox.start.z = smallestZ; }

  if (existingBoundingBox.end.x < biggestX) { existingBoundingBox.end.x = biggestX; }
  if (existingBoundingBox.end.y < biggestY) { existingBoundingBox.end.y = biggestY; }
  if (existingBoundingBox.end.z < biggestZ) { existingBoundingBox.end.z = biggestZ; }

  MLdouble centerX = (existingBoundingBox.start.x + existingBoundingBox.end.x) / 2.f;
  MLdouble centerY = (existingBoundingBox.start.y + existingBoundingBox.end.y) / 2.f;
  MLdouble centerZ = (existingBoundingBox.start.z + existingBoundingBox.end.z) / 2.f;

  existingBoundingBox.center.x = centerX;
  existingBoundingBox.center.y = centerY;
  existingBoundingBox.center.z = centerZ;

  MLdouble newRadius = sqrt(pow(existingBoundingBox.end.x-existingBoundingBox.center.x,2) + pow(existingBoundingBox.end.y-existingBoundingBox.center.y,2) + pow(existingBoundingBox.end.z-existingBoundingBox.center.z,2));

  if (newRadius > existingBoundingBox.radius)
  {
    existingBoundingBox.radius = newRadius;
  }
}

//***********************************************************************************

// Get bounding box edges from positions
ModelBoundingBoxStruct PDFTools::GetBoundingBoxFomPositions(PositionsVector positions)
{
  ModelBoundingBoxStruct result;

  MLdouble smallestX = 0;
  MLdouble smallestY = 0;
  MLdouble smallestZ = 0;
   
  MLdouble biggestX = 0;
  MLdouble biggestY = 0;
  MLdouble biggestZ = 0;

  for (int i = 0; i < positions.size(); i++)
  {
    PositionStruct thisPosition = positions[i];

    if (thisPosition.position.x < smallestX) { smallestX = thisPosition.position.x; }
    if (thisPosition.position.x > biggestX)  { biggestX = thisPosition.position.x; }

    if (thisPosition.position.y < smallestY) { smallestY = thisPosition.position.y; }
    if (thisPosition.position.y > biggestY)  { biggestY = thisPosition.position.y; }

    if (thisPosition.position.z < smallestZ) { smallestZ = thisPosition.position.z; }
    if (thisPosition.position.z > biggestZ)  { biggestZ = thisPosition.position.z; }
  }

  result.start.x = smallestX;
  result.start.y = smallestY;
  result.start.z = smallestZ;

  result.end.x = biggestX;
  result.end.y = biggestY;
  result.end.z = biggestZ;

  return result;
}

//***********************************************************************************

// Get all positions (vertices) from ColoredMarkerList
PositionsVector PDFTools::getAllPositionsFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double pointSize)
{
  PositionsVector result;
  
  PositionStruct newPosition;

  StringVector allowedPositionTypesVector = stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  for (int i = 0; i < positionsListLength; i++)
  {
    ml::ColoredMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), MLintToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      newPosition.position.x = thisPosition.x();
      newPosition.position.y = thisPosition.y();
      newPosition.position.z = thisPosition.z();
      newPosition.color.x    = thisPosition.r();
      newPosition.color.y    = thisPosition.g();
      newPosition.color.z    = thisPosition.b();
      newPosition.alpha      = thisPosition.alpha();
      newPosition.size       = pointSize;

      result.push_back(newPosition);
    }

  }
  
  return result;
}

//***********************************************************************************

// Get all positions (vertices) from XMarkerList
PositionsVector PDFTools::getAllPositionsFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double pointSize)
{
  PositionsVector result;
  
  PositionStruct newPosition;

  StringVector allowedPositionTypesVector = stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  for (int i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), intToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      newPosition.position.x = thisPosition.x();
      newPosition.position.y = thisPosition.y();
      newPosition.position.z = thisPosition.z();
      newPosition.color      = Vector3(0);       // Not defined by XMarkerList
      newPosition.alpha      = 1;                // Not defined by XMarkerList
      newPosition.size       = pointSize;

      result.push_back(newPosition);
    }

  }
  
  return result;
}

//***********************************************************************************

// Get all line connections from IndexPairList
LinesVector PDFTools::getAllLinesFromIndexPairList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes, double lineWidth)
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
    if (  ("all" == allowedConnectionTypes) || ( std::find(allowedConnectionTypesVector.begin(), allowedConnectionTypesVector.end(), MLintToString(thisConnection.type)) != allowedConnectionTypesVector.end() )  )
    {
      thisLine.startIndex = static_cast<MLuint32>(thisConnection.index1);
      thisLine.endIndex = static_cast<MLuint32>(thisConnection.index2);
      thisLine.width = lineWidth;

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

//***********************************************************************************

// Automatically calculate simple line connections from XMarkerList
LinesVector PDFTools::getStandardLinesFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth)
{
  LinesVector result;
  LineStruct thisLine;
  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning
  thisLine.width = lineWidth;

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

//***********************************************************************************

// Automatically calculate simple line connections from ColoredMarkerList
LinesVector PDFTools::getStandardLinesFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth)
{
  LinesVector result;
  LineStruct thisLine;
  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning
  thisLine.width = lineWidth;


  StringVector allowedPositionTypesVector = stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  int startPositionIndex = -2;
  int endPositionIndex = -1;

  for (int i = 0; i < positionsListLength; i++)
  {
    ml::ColoredMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), MLintToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
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

//***********************************************************************************

// Get all lines that end at a given position
LinesVector PDFTools::getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition)
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

//***********************************************************************************

void PDFTools::fillFiberSetContainerFromPositionsAndConnections(FiberSetContainer& outFiberSetContainer, const XMarkerList& inLinePositions, const IndexPairList& inLineConnections)
{
  outFiberSetContainer.deleteAllFiberSets();

  std::set<int> typeIDsSet;
  std::map<int, int> numPositionsPerType;
  std::map<int, std::string> typeLabels;

  // Step 1: Scan all positions and get all type ID from them
  for (XMarkerList::const_iterator it = inLinePositions.cbegin(); it != inLinePositions.cend(); ++it)
  {
    XMarker thisMarker = *it;
    typeIDsSet.insert(thisMarker.type);
    numPositionsPerType[thisMarker.type]++;
    typeLabels[thisMarker.type] = thisMarker.name();
  }

  // Step 2: Now create fibers
  for (std::set<int>::const_iterator typeIdIterator = typeIDsSet.cbegin(); typeIdIterator != typeIDsSet.cend(); ++typeIdIterator)
  {
    int thisTypeID = *typeIdIterator;

    FiberSetContainer::FiberSet* newFiberSet = outFiberSetContainer.createFiberSet();
    newFiberSet->setColor(Vector3(1, 0, 0));
    newFiberSet->setLabel(typeLabels[thisTypeID]);

    // Add all connections to the temporary connections list if type ID matches
    XMarkerList  workPositions;
    for (XMarkerList::const_iterator outPositionsIterator = inLinePositions.cbegin(); outPositionsIterator != inLinePositions.cend(); ++outPositionsIterator)
    {
      XMarker thisMarker = *outPositionsIterator;

      if (thisMarker.type == thisTypeID)
      {
        workPositions.push_back(thisMarker);
      }
    }
    MLint workPositionsSize = (MLint)workPositions.size();

    // Add all connections to the temporary connections list if type ID matches
    IndexPairList workConnections;
    for (IndexPairList::const_iterator outConnectionsIterator = inLineConnections.cbegin(); outConnectionsIterator != inLineConnections.cend(); ++outConnectionsIterator)
    {
      IndexPair thisPair = *outConnectionsIterator;

      if (thisPair.type == thisTypeID)
      {
        workConnections.push_back(thisPair);
      }
    }

    // If temporary connections list is still empty at this point: create default list
    if (workConnections.size() == 0)
    {
      int numPositionsOfThisType = numPositionsPerType[thisTypeID];

      for (int p = 0; p < numPositionsOfThisType - 1; p++)
      {
        IndexPair thisPair(p, p + 1);
        workConnections.push_back(thisPair);
      }
    }

    // Now finally create fibers from connections
    for (IndexPairList::const_iterator workListIterator = workConnections.cbegin(); workListIterator != workConnections.cend(); ++workListIterator)
    {
      IndexPair thisWorkPair = *workListIterator;

      MLint startIndex = thisWorkPair.index1;
      MLint endIndex = thisWorkPair.index2;

      if ((startIndex < workPositionsSize) && (endIndex < workPositionsSize))
      {
        XMarker startMarker = workPositions[startIndex];
        XMarker endMarker = workPositions[endIndex];

        FiberSetContainer::FiberSet::Fiber* newFiber = newFiberSet->createFiber();

        FiberSetContainer::FiberSet::Fiber::FiberPoint startPoint;
        startPoint.setCoordinates(startMarker.x(), startMarker.y(), startMarker.z());
        FiberSetContainer::FiberSet::Fiber::FiberPoint endPoint;
        endPoint.setCoordinates(endMarker.x(), endMarker.y(), endMarker.z());

        newFiber->appendPoint(startPoint);
        newFiber->appendPoint(endPoint);
        newFiber->setLabel(1.0);
      }
    }

    workPositions.clearList();
    workConnections.clearList();
  }

}


//***********************************************************************************


} // end namespace mlPDF

ML_END_NAMESPACE