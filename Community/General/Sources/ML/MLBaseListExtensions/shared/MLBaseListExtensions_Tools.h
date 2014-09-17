//----------------------------------------------------------------------------------
//! Tools for MLBaseListExtensions modules.
/*!
// \file    MLBaseListExtensions_Tools.h
// \author  Axel Newe
// \date    2014-09-01
//
//
*/
//----------------------------------------------------------------------------------


#ifndef _BaseListExtensions_Tools_H
#define _BaseListExtensions_Tools_H 

#include "MLBaseListExtensions_DataTypes.h"

#include "mlModuleIncludes.h"

ML_START_NAMESPACE

//////////////////////////////////////////////

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

inline double stringToDouble(std::string sourceString)
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


// Tool methods ============================================================

// Split a string
StringVector stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty);

// Convert String to lower case
void stringToLower(std::string &sourceString);


//////////////////////////////////////////////


ML_END_NAMESPACE

#endif // _BaseListExtensions_Tools_H