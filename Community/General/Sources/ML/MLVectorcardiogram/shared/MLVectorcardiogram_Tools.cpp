//----------------------------------------------------------------------------------
//! Tools for VCG modules.
/*!
// \file    MLVectorcardiogram_Tools.cpp
// \author  Axel Newe
// \date    2015-09-25
//
*/
//----------------------------------------------------------------------------------


#include "MLVectorcardiogram_DataTypes.h"
#include "MLVectorcardiogram_Tools.h"

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
    sourceString[loop] = (char)std::tolower(sourceString[loop]);
  }
}

//***********************************************************************************


//////////////////////////////////////////////

ML_END_NAMESPACE