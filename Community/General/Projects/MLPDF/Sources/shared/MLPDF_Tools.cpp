//----------------------------------------------------------------------------------
// Tools for PDF file creation.
//
// \file    MLPDF_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//----------------------------------------------------------------------------------


// Global includes
#include <sys/stat.h>

// Local includes
#include "MLPDF_Tools.h"

// ML includes
#include <mlModuleIncludes.h>
#include <mlApplicationProperties.h>


ML_START_NAMESPACE

namespace mlPDF {

//***********************************************************************************

// Parses the specification string from the UI and extracts a specific property
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

std::string PDFTools::stringTrimWhitespace(std::string sourceString)
{
  std::string result = sourceString;
  const std::string whitespaceChars = " \n\r\t";

  // Remove leading whitespace
  size_t firstCharIndex = result.find_first_not_of(whitespaceChars);
  if (firstCharIndex != result.npos)
  {
    result.erase(0, result.find_first_not_of(whitespaceChars));
  }

  // Remove trailing whitespace
  result.erase(result.find_last_not_of(whitespaceChars) + 1);

  return result;
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
  return "1.4";
}

//***********************************************************************************

long PDFTools::getFileSize(std::string filename)
{
  struct stat fileCheckBuffer;
  int rc = stat(filename.c_str(), &fileCheckBuffer);

  return rc == 0 ? fileCheckBuffer.st_size : -1;
}

//***********************************************************************************

Vector4 PDFTools::getColorVec4FromString(std::string colorString, const Vector4 defaultColor)
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

Vector3 PDFTools::getColorVec3FromString(std::string colorString, const Vector3 defaultColor)
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

Vector3 PDFTools::getVec3FromString(std::string vectorString, const Vector3 defaultValue)
{
  return getColorVec3FromString(vectorString, defaultValue);
}

//***********************************************************************************

std::string PDFTools::FormatColorString(Vector3 colorValue)
{
  const int bufferLength = 18;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z);

  std::string result(buffer);

  return result;
}

std::string PDFTools::FormatColorString(Vector3 colorValue, float alphaValue)
{
  const int bufferLength = 24;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z, alphaValue);

  std::string result(buffer);

  return result;
}

std::string PDFTools::FormatVec3String(Vector3 vectorValue)
{
  const int bufferLength = 27;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", vectorValue.x, vectorValue.y, vectorValue.z);

  std::string result(buffer);

  return result;
}

std::string PDFTools::FormatDouble(double value)
{
  const int bufferLength = 12;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f", value);

  std::string result(buffer);

  return result;
}

std::string PDFTools::FormatFloat(float value)
{
  return FormatDouble((double)value);
}

std::string PDFTools::FormatDate(std::tm value)
{
  const int bufferLength = 19;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%4u-%02u-%02u %02u:%02u:%02u", value.tm_year + 1900, value.tm_mon + 1, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec);

  std::string result(buffer);

  return result;
}

//***********************************************************************************


} // end namespace mlPDF

ML_END_NAMESPACE