//----------------------------------------------------------------------------------
// Specification generator for U3D file handling.
//
// \file    MLU3D_SpecificationGenerator.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "MLU3D_SpecificationGenerator.h"
#include "MLU3D_Constants.h"
#include "MLU3D_Tools.h"


ML_START_NAMESPACE

namespace mlU3D {

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::getMetaDataKey(std::string fieldValue)
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

std::string SpecificationGenerator::getMetaDataValue(std::string fieldValue)
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

std::string SpecificationGenerator::getWEMLabel(std::string fieldValue)
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

std::string SpecificationGenerator::getFiberSetIndices(std::string fieldValue)
{
  std::string result = "";

  if (fieldValue != "")
  {
    result = "<FiberSets>";
    result += fieldValue;
    result += "</FiberSets>\n";
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::getPositionTypes(std::string fieldValue)
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

std::string SpecificationGenerator::getConnectionTypes(std::string fieldValue)
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

std::string SpecificationGenerator::getObjectName(std::string fieldValue)
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

std::string SpecificationGenerator::getGroupPath(std::string fieldValue)
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

std::string SpecificationGenerator::getColor(Vector3 colorValue, float alphaValue, bool useDefaultColor, bool useVertexColor)
{
  std::string result = "";

  if (useVertexColor)
  {
    result = "<Color>" + USEVERTEXCOLORS + "</Color>\n";
  }
  else if (!useDefaultColor)
  {
    result = "<Color>"; 
    result += U3DTools::formatColorString(colorValue, alphaValue);
    result += "</Color>\n";
  }

  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::getSpecularColor(Vector3 colorValue, bool useDefaultColor)
{
  std::string result = "";

  if (!useDefaultColor)
  {
    result = "<SpecularColor>";
    result += U3DTools::formatColorString(colorValue);
    result += "</SpecularColor>\n";
  }

  return result;

}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::getOpacity(float alphaValue)
{
  std::string result = "";

  result = "<Opacity>";
  result += U3DTools::formatFloat(alphaValue);
  result += "</Opacity>\n";

  return result;
}

//----------------------------------------------------------------------------------

std::string SpecificationGenerator::getModelVisibility(int fieldValue)
{
  std::string result = "";
  std::string modelVisibility = "";

  if (fieldValue == SPECIFICATION_MODELVISIBILITY_NOTVISIBLE)
  {
    modelVisibility = "0";
  }
  else if (fieldValue == SPECIFICATION_MODELVISIBILITY_FRONTVISIBLE)
  {
      modelVisibility = "1";
  }
  else if (fieldValue == SPECIFICATION_MODELVISIBILITY_BACKVISIBLE)
  {
      modelVisibility = "2";
  }
  else if (fieldValue == SPECIFICATION_MODELVISIBILITY_FRONTANDBACKVISIBLE)
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