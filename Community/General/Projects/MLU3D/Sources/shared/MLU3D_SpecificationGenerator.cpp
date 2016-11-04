//----------------------------------------------------------------------------------
// Specification generator for U3D file handling.
//
// \file    MLU3D_SpecificationGenerator.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "MLU3D_SpecificationGenerator.h"
//#include "MLU3D_Defines.h"
#include "MLU3D_Tools.h"


ML_START_NAMESPACE

namespace mlU3D {

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

std::string SpecificationGenerator::GetColor(Vector3 colorValue, float alphaValue, bool useDefaultColor, bool useVertexColor)
{
  std::string result = "";

  if (useVertexColor)
  {
    result = "<Color>" + USEVERTEXCOLORS + "</Color>\n";
  }
  else if (!useDefaultColor)
  {
    result = "<Color>"; 
    result += U3DTools::FormatColorString(colorValue, alphaValue);
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
    result += U3DTools::FormatColorString(colorValue);
    result += "</SpecularColor>\n";
  }

  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetOpacity(float alphaValue)
{
  std::string result = "";

  result = "<Opacity>";
  result += U3DTools::FormatFloat(alphaValue);
  result += "</Opacity>\n";

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::GetModelVisibility(int fieldValue)
{
  std::string result = "";
  std::string modelVisibility = "";

  if (fieldValue == NEW_SPECIFICATION_MODELVISIBILITY_NOTVISIBLE)
  {
    modelVisibility = "0";
  }
  else if (fieldValue == NEW_SPECIFICATION_MODELVISIBILITY_FRONTVISIBLE)
  {
      modelVisibility = "1";
  }
  else if (fieldValue == NEW_SPECIFICATION_MODELVISIBILITY_BACKVISIBLE)
  {
      modelVisibility = "2";
  }
  else if (fieldValue == NEW_SPECIFICATION_MODELVISIBILITY_FRONTANDBACKVISIBLE)
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

} // end namespace mlU3D

ML_END_NAMESPACE