//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_Tools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_Tools_H
#define _PDF_Tools_H 

#include "MLPDFSystem.h"
#include "MLPDF_DataTypes.h"

#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>
#include <mlFiberSet.h>

#include "mlModuleIncludes.h"

// Inventor includes
#include <Inventor/nodes/SoNodes.h>  // for M_PI

#include <sys/stat.h>


ML_START_NAMESPACE

namespace mlPDF {

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

inline float degToRad(float degrees)
{
  return (degrees/180.0f) * M_PI;
}

inline float radToDeg(float radians)
{
  return (radians / M_PI * 180.0);
}


class MLPDF_EXPORT PDFTools
{

public:

  // Tool methods ============================================================

  // Split a string
  static StringVector stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty);

  // Convert String to lower case
  static void stringLower(std::string &sourceString);

  // Converts a string to bool
  static bool stringToBool(std::string sourceString);

  // Trims whitespace characters from string
  static std::string stringTrimWhitespace(std::string sourceString);

  // Convert MeVisLab version number into string
  static std::string getMeVisLabVersionNumberString();

  // Return string with module's version number
  static std::string getModuleVersionNumberString();

  //***********************************************************************************

  // Parses a string and try to get color values from it.
  static Vector4 getColorVec4(std::string colorString, const Vector4 defaultColor);
  static Vector3 getColorVec3(std::string colorString, const Vector3 defaultColor);

  // Converts a (color) Vector to a color string.
  static std::string FormatColorString(Vector3 colorValue);
  static std::string FormatColorString(Vector3 colorValue, float alphaValue);
  static std::string FormatVec3String(Vector3 vectorValue);

  //***********************************************************************************

  // Get data from object (point cloud, line set, mesh) specification fields
  static StringVector getObjectSpecificationsStringFromUI(ml::StringField *inputField, std::string delimiter);

  // Parses the specification string from the UI and extracts a specific U3D property
  static std::string getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult = "");

  // Parses input string from UI and extracts object specification parameters
  static SpecificationParametersStruct getAllSpecificationParametersFromString(const std::string specificationString);

  // Parses the description string of the WEM to set U3D properties.
  static std::string getSpecificParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult = "");

  // Get parent node name from the group name path
  static std::string getParentNameFromGroupPath(std::string groupPath);

  // Make sure each group path has a leading and a trailing "/"
  static std::string normalizeGroupPath(std::string groupPath);

  // Updates a GroupNodeVector with a new group path
  static void updateGroupNodesVector(GroupNodeVector &groupNodes, std::string thisGroupPath);

  //***********************************************************************************

  // Updates the model bounding box
  static void UpdateBoundingBox(ModelBoundingBoxStruct& boundingBox, ModelBoundingBoxStruct newCorners);

  // Get bounding box edges from positions
  static ModelBoundingBoxStruct GetBoundingBoxFomPositions(PositionsVector positions);

  //***********************************************************************************

  // Get all positions (vertices) from ColoredMarkerList
  static PositionsVector getAllPositionsFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

  // Get all positions (vertices) from XMarkerList
  static PositionsVector getAllPositionsFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

  //***********************************************************************************

  // Get all line connections from IndexPairList
  static LinesVector getAllLinesFromIndexPairList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes, double lineWidth);

  // Automatically calculate simple line connections from XMarkerList
  static LinesVector getStandardLinesFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

  // Automatically calculate simple line connections from  ColoredMarkerList
  static LinesVector getStandardLinesFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

  // Get all lines that end at a given position
  static LinesVector getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition);


  //***********************************************************************************

  // Fill a FiberSetContainer from line positions and line connections
  static void fillFiberSetContainerFromPositionsAndConnections(ml::FiberSetContainer& outFiberSetContainer, const ml::XMarkerList& inLinePositions, const ml::IndexPairList& inLineConnections);

  //////////////////////////////////////////////

};

} // end namespace mlPDF

ML_END_NAMESPACE

#endif // _PDF_Tools_H