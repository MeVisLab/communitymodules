//----------------------------------------------------------------------------------
//! Tools for MLBaseListExtensions modules.
/*!
// \file    MLBaseListExtensions_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#include "MLBaseListExtensions_DataTypes.h"
#include "MLBaseListExtensions_Tools.h"

#include "mlModuleIncludes.h"

ML_START_NAMESPACE

//////////////////////////////////////////////

StringVector stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty)
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

void stringToLower(std::string &sourceString)
{
  for(unsigned short loop=0;loop < sourceString.size();loop++)
  {
    sourceString[loop] = static_cast<char>(std::tolower(sourceString[loop]));
  }
}

//***********************************************************************************


//////////////////////////////////////////////

ML_END_NAMESPACE