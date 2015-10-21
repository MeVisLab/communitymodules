//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_Tools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_CommonTools_H
#define _PDF_CommonTools_H 

#include "MLPDF_DataTypes.h"

#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

#include "mlModuleIncludes.h"

#include <sys/stat.h>

ML_START_NAMESPACE

// Inlines ================================================================

inline std::string floatToString(float floatValue)
{
  std::ostringstream buffer;
  buffer << floatValue;
  return buffer.str();
}

inline std::string MLintToString(MLint intValue)
{
  std::ostringstream buffer;
  buffer << intValue;
  return buffer.str();
}

inline std::string intToString(int intValue)
{
  std::ostringstream buffer;
  buffer << intValue;
  return buffer.str();
}

inline std::string stringTrimWhitespace(std::string sourceString)
{
  std::string result = sourceString;
  const std::string whitespaceChars = " \n\r\t";

  // Remove leading whitespace
  size_t firstCharIndex = sourceString.find_first_not_of(whitespaceChars);
  if (firstCharIndex != sourceString.npos)
  {
    sourceString.erase(0,sourceString.find_first_not_of(whitespaceChars));
  }
  
  // Remove trailing whitespace
  sourceString.erase(sourceString.find_last_not_of(" \n\r\t")+1);

  return result;
}

inline MLdouble stringToDouble(std::string sourceString)
{
  return atof(sourceString.c_str());
}

inline MLdouble stringToMLDouble(std::string sourceString)
{
  return (MLdouble)stringToDouble(sourceString);
}

inline int stringToInt(std::string sourceString)
{
  int result = atoi(sourceString.c_str());

  return result;
}

inline bool fileExists(std::string& filename)
{
  struct stat fileCheckBuffer;

  return (stat (filename.c_str(), &fileCheckBuffer) == 0);
}

// Tool methods ============================================================

// Split a string
StringVector stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty);

// Convert String to lower case
void stringLower(std::string &sourceString);

// Convert MeVisLab version number into string
std::string getMeVisLabVersionNumberString();

// Return string with module's version number
std::string getModuleVersionNumberString();

//***********************************************************************************

// Parses a string and try to get color values from it.
Vector4 getColorVec4(std::string colorString, const Vector4 defaultColor);
Vector3 getColorVec3(std::string colorString, const Vector3 defaultColor);

//***********************************************************************************

// Get data from object (point cloud, line set, mesh) specification fields
StringVector getObjectSpecificationsStringFromUI(ml::StringField *inputField, std::string delimiter);

// Parses the specification string from the UI and extracts a specific U3D property
std::string getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult = "");

// Parses input string from UI and extracts object specification parameters
SpecificationParametersStruct getAllSpecificationParametersFromString(const std::string specificationString);

// Parses the description string of the WEM to set U3D properties.
std::string getSpecificParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult = "");

// Get parent node name from the group name path
std::string getParentNameFromGroupPath(std::string groupPath);

// Make sure each group path has a leading and a trailing "/"
std::string normalizeGroupPath(std::string groupPath);

// Updates a GroupNodeVector with a new group path
void updateGroupNodesVector(GroupNodeVector &groupNodes, std::string thisGroupPath);



//***********************************************************************************

// Updates the model bounding box
void UpdateBoundingBox(ModelBoundingBoxStruct& boundingBox, ModelBoundingBoxStruct newCorners);

// Get bounding box edges from positions
ModelBoundingBoxStruct GetBoundingBoxFomPositions(PositionsVector positions);

//***********************************************************************************

// Get all positions (vertices) from ColoredMarkerList
PositionsVector getAllPositionsFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

// Get all positions (vertices) from XMarkerList
PositionsVector getAllPositionsFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

//***********************************************************************************

// Get all line connections from IndexPairList
LinesVector getAllLinesFromIndexPairList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes, double lineWidth);

// Automatically calculate simple line connections from XMarkerList
LinesVector getStandardLinesFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

// Automatically calculate simple line connections from  ColoredMarkerList
LinesVector getStandardLinesFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

// Get all lines that end at a given position
LinesVector getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition);



//////////////////////////////////////////////




ML_END_NAMESPACE

#endif // _PDF_CommonTools_H