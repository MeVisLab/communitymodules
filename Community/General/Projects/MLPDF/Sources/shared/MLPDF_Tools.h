//----------------------------------------------------------------------------------
// Tools for PDF file creation.
//
// \file    MLPDF_Tools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//----------------------------------------------------------------------------------


#ifndef _PDF_Tools_H
#define _PDF_Tools_H 

// Local includes
#include "MLPDFSystem.h"

// ML includes
#include <mlModuleIncludes.h>

// Inventor includes
#include <Inventor/nodes/SoNodes.h>  // for M_PI

// Std includes
#include <sys/stat.h>


ML_START_NAMESPACE

namespace mlPDF {

typedef std::vector<std::string> StringVector;

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

inline float roundWithPrecision(float value, int decimals)
{
  return floor(value * pow(10, decimals)) / pow(10, decimals);
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

  // Get a file size
  static long getFileSize(std::string filename);

  //***********************************************************************************

  // Parses a string and try to get color values from it.
  static Vector4 getColorVec4FromString(std::string colorString, const Vector4 defaultColor = Vector4(0));
  static Vector3 getColorVec3FromString(std::string colorString, const Vector3 defaultColor = Vector3(0));
  static Vector3 getVec3FromString(std::string vectorString, const Vector3 defaultValue = Vector3(0));

  // Converts a (color) Vector to a color string.
  static std::string FormatColorString(Vector3 colorValue);
  static std::string FormatColorString(Vector3 colorValue, float alphaValue);
  static std::string FormatVec3String(Vector3 vectorValue);

  // Converts float/double to string.
  static std::string FormatDouble(double value);
  static std::string FormatFloat(float value);

  // Converts std::tm to string.
  static std::string FormatDate(std::tm value);

  //***********************************************************************************

  // Parses the specification string from the UI and extracts a specific property
  static std::string getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult = "");

  //////////////////////////////////////////////

};

} // end namespace mlPDF

ML_END_NAMESPACE

#endif // _PDF_Tools_H