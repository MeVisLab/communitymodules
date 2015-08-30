//----------------------------------------------------------------------------------
//! Tools for MLPDF specification generation.
/*!
// \file    MLPDF_SpecificationGenerator.cpp
// \author  Axel Newe
// \date    2015-08-26
//
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLPDF_SpecificationGenerator.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetMetaDataKey(std::string fieldValue)
{
  if (fieldValue == "")
  {
    fieldValue = "[No key specified]";
  }
  
  std::string result = "<MetaDataKey>";
  result += fieldValue;
  result += "</MetaDataKey>\n";
  
  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetMetaDataValue(std::string fieldValue)
{
  if (fieldValue == "")
  {
    fieldValue = "[No value specified]";
  }
  
  std::string result = "<MetaDataValue>";
  result += fieldValue;
  result += "</MetaDataValue>\n";
  
  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetWEMLabel(std::string fieldValue)
{
  std::string result = "";

  if (fieldValue != "")
  {
    result = "<WEMLabel>";
    result += fieldValue;
    result += "</WEMLabel>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetPositionTypes(std::string fieldValue)
{
  std::string result = "";

  if (fieldValue != "")
  {
    result = "<PositionTypes>";
    result += fieldValue;
    result += "</PositionTypes>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetConnectionTypes(std::string fieldValue)
{
  std::string result = "";

  if (fieldValue != "")
  {
    result = "<ConnectionTypes>";
    result += fieldValue;
    result += "</ConnectionTypes>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetObjectName(std::string fieldValue)
{
  std::string result = "";

  if (fieldValue != "")
  {
    result = "<ObjectName>";
    result += fieldValue;
    result += "</ObjectName>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetGroupPath(std::string fieldValue)
{
  std::string result = "" ;

  if (fieldValue != "")
  {
    std::replace(fieldValue.begin(), fieldValue.end(), '\\', '/');

    if (fieldValue.substr(0,1) != "/")
    {
      fieldValue = "/" + fieldValue;
    }

    if (fieldValue.substr(fieldValue.length()-1,1) != "/")
    {
      fieldValue += "/";
    }

    result = "<GroupPath>";
    result += fieldValue ; 
    result += "</GroupPath>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetColor(Vector3 colorValue, float alphaValue, bool useDefaultColor)
{
  std::string result = "";

  if (!useDefaultColor)
  {
    result = "<Color>"; 
    result += FormatColorString(colorValue, alphaValue);
    result += "</Color>\n";
  }

  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetSpecularColor(Vector3 colorValue, bool useDefaultColor)
{
  std::string result = "";

  if (!useDefaultColor)
  {
    result = "<SpecularColor>";
    result += FormatColorString(colorValue);
    result += "</SpecularColor>\n";
  }

  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetModelVisibility(int fieldValue)
{
  std::string result = "";
  std::string modelVisibility = "";

  if (fieldValue == SpecificationGenerator::MODELVISIBILITY_NOTVISIBLE)
  {
    modelVisibility = "0";
  }
  else if (fieldValue == SpecificationGenerator::MODELVISIBILITY_FRONTVISIBLE)
  {
      modelVisibility = "1";
  }
  else if (fieldValue == SpecificationGenerator::MODELVISIBILITY_BACKVISIBLE)
  {
      modelVisibility = "2";
  }
  else if (fieldValue == SpecificationGenerator::MODELVISIBILITY_FRONTANDBACKVISIBLE)
  {
      modelVisibility = "3";
  }

  if (modelVisibility != "")
  {
    result = "<ModelVisibility>";
    result += modelVisibility;
    result += "</ModelVisibility>\n";
  }
  
  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::FormatColorString(Vector3 colorValue)
{

  const int bufferLength = 18;
  char buffer[bufferLength];

  snprintf (buffer, bufferLength, "%.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z);

  std::string result(buffer);

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::FormatColorString(Vector3 colorValue, float alphaValue)
{

  const int bufferLength = 24;
  char buffer[bufferLength];

  snprintf (buffer, bufferLength, "%.3f %.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z, alphaValue);

  std::string result(buffer);

  return result;
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE