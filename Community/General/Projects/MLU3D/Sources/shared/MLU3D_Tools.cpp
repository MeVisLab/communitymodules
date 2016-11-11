//----------------------------------------------------------------------------------
// Tools for U3D file handling.
//
// \file    MLU3D_Tools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "MLU3D_Constants.h"
#include "MLU3D_Tools.h"

// ML includes
#include <mlApplicationProperties.h>


ML_START_NAMESPACE

namespace mlU3D {

//***********************************************************************************

  void U3DTools::quantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ)
  {
    u8Signs = (pos.x < 0.0) | ((pos.y < 0.0) << 1) | ((pos.z < 0.0) << 2);
    udX = (MLuint32)(0.5f + mlU3D::Quant_Position * fabs(pos.x));
    udY = (MLuint32)(0.5f + mlU3D::Quant_Position * fabs(pos.y));
    udZ = (MLuint32)(0.5f + mlU3D::Quant_Position * fabs(pos.z));
  }

  void U3DTools::quantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ)
  {
    u8Signs = (pos.x < 0.0) | ((pos.y < 0.0) << 1) | ((pos.z < 0.0) << 2);
    udX = (MLuint32)(0.5f + mlU3D::Quant_Normal * fabs(pos.x));
    udY = (MLuint32)(0.5f + mlU3D::Quant_Normal * fabs(pos.y));
    udZ = (MLuint32)(0.5f + mlU3D::Quant_Normal * fabs(pos.z));
  }

  //***********************************************************************************

  void U3DTools::makeInternalNameUnique(std::string& objectName, mlU3D::U3DObjectPtr u3dObject)
  {
    for (size_t m = 0; m < u3dObject->modelNodes.size(); m++)
    {
      std::string compareStr = u3dObject->modelNodes[m].internalName;

      // Look if name already exists
      if (objectName == compareStr)
      {
        objectName = objectName + " ";  // Just add a space... this is a hack that allows to use names that are internally unique but not as regards the displaying in Acrobat...
        makeInternalNameUnique(objectName, u3dObject);
        break;
      }
    }

    return;
  }

  //***********************************************************************************

  mlU3D::ModelNode U3DTools::createNewModelNode(SpecificationParametersStruct specification, mlU3D::U3DObjectPtr u3dObject)
  {
    //mlInfo(__FUNCTION__) << "Log message.";

    mlU3D::ModelNode newModelNode;

    //newModelNode.Type = modelType;
    newModelNode.displayName  = specification.ObjectName;
    newModelNode.internalName = specification.ObjectName;
    makeInternalNameUnique(newModelNode.internalName, u3dObject);
    newModelNode.groupPath    = specification.GroupPath;
    newModelNode.visibility   = (MLuint32)mlU3D::stringToInt(specification.ModelVisibility);

    if (specification.ObjectType != mlU3D::MODELTYPE_POINTSET)
    {
      newModelNode.shaderName = "Shader for " + newModelNode.internalName;
    }
    else
    {
      newModelNode.shaderName   = "";  // Use default shader for Point Sets
    }

    newModelNode.geometryGeneratorName = "Geometry for " + newModelNode.internalName;

    return newModelNode;
  }

  //***********************************************************************************

// Get model specs from WEM atributes
StringVector U3DTools::getMeshSpecificationsStringFromWEM(ml::WEM* wem)
{
  StringVector result;

  // Loop through all patches, search and parse label & description
  const MLuint32 numberOfWEMPatches = wem->getNumWEMPatches();

  for (MLuint32 i = 0; i < numberOfWEMPatches; i++)
  {
    WEMPatch* thisInWEMPatch = wem->getWEMPatchAt(i);

    if (thisInWEMPatch)
    {
      std::string thisWEMPatchLabel = thisInWEMPatch->getLabel();
      std::string thisWEMPatchDescription = thisInWEMPatch->getDescription();

      // Parse WEM label & description...
      std::string u3dModelName = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "ModelName");
      std::string u3dGroupName = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "GroupName");
      std::string u3dGroupPath = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "GroupPath");

      if ("" != u3dGroupName)
      {
        if ("" == u3dGroupPath)
        {
          u3dGroupPath += "/";
        }
        u3dGroupPath += u3dGroupName + "/";
      }

      std::string displayName = thisWEMPatchLabel;
      if (displayName == "") {
        displayName = "Mesh " + mlU3D::intToString(i + 1);
      }

      // ...and write data into meshSpecification string
      std::string meshSpecificationsString = "<Mesh>";
      meshSpecificationsString += "<WEMLabel>" + thisWEMPatchLabel + "</WEMLabel>";
      meshSpecificationsString += "<ObjectName>" + displayName + "</ObjectName>";
      meshSpecificationsString += "<GroupPath>" + u3dGroupPath + "</GroupPath>";
      meshSpecificationsString += "<Color>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "Color") + "</Color>";
      meshSpecificationsString += "<SpecularColor>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "SpecularColor") + "</SpecularColor>";
      meshSpecificationsString += "<Opacity>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(thisWEMPatchDescription, "Opacity") + "</Opacity>";
      meshSpecificationsString += "<ModelVisibility>3</ModelVisibility>";
      meshSpecificationsString += "</Mesh>";

      // Add meshSpecification string to meshSpecificationVector
      result.push_back(meshSpecificationsString);
    }

  } // for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)

  return result;
}

//***********************************************************************************

// Get model specs from (point cloud, line set, mesh) specification fields
StringVector U3DTools::getModelSpecificationsStringFromUI(ml::StringField *inputField, std::string delimiter)
{
  StringVector result;

  std::string specificationString = inputField->getStringValue();

  result = stringSplit(specificationString, delimiter, false);  // TODO Make stringSplit robust against lower/upper case

  return result;
}

//***********************************************************************************

// Parses the specification string from the UI and extracts a specific U3D property
std::string U3DTools::getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult)
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

// Parses input string from UI and extracts object specification parameters
SpecificationParametersStruct U3DTools::getAllSpecificationParametersFromString(const std::string specificationString)
{
 SpecificationParametersStruct result;

  result.ObjectName       = getSpecificParameterFromString(specificationString, "<ObjectName>", "Object");
  result.GroupPath        = normalizeGroupPath(getSpecificParameterFromString(specificationString, "<GroupPath>"));
  result.Color            = getSpecificParameterFromString(specificationString, "<Color>");
  result.SpecularColor    = getSpecificParameterFromString(specificationString, "<SpecularColor>");
  result.Opacity          = getSpecificParameterFromString(specificationString, "<Opacity>");
  result.GlyphText        = getSpecificParameterFromString(specificationString, "<GlyphText>");     // U3D Glyphs are not supported by Acrobat... :-(
  result.MetaDataKey      = getSpecificParameterFromString(specificationString, "<MetaDataKey>");
  result.MetaDataValue    = getSpecificParameterFromString(specificationString, "<MetaDataValue>");
  result.ModelVisibility  = getSpecificParameterFromString(specificationString, "<ModelVisibility>", "3");            
  result.WEMLabel         = getSpecificParameterFromString(specificationString, "<WEMLabel>");
  result.PositionTypes    = getSpecificParameterFromString(specificationString, "<PositionTypes>", "all");
  result.ConnectionTypes  = getSpecificParameterFromString(specificationString, "<ConnectionTypes>", "simple");

 return result;
}


//***********************************************************************************

std::string U3DTools::getSpecificParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult)
{
  std::string result = failResult; 
  std::string keyword (parameter+"=");
  size_t keywordPosition = wemDescription.find(keyword);

  if (keywordPosition != std::string::npos)
  {
    std::string keywordSectionPlusRest = wemDescription.substr(keywordPosition);
    size_t delimiterPosition = keywordSectionPlusRest.find(";");

    std::string keywordSection = keywordSectionPlusRest.substr(0, delimiterPosition);

    if (keywordSection != "") 
    {
      std::string resultCandidate = keywordSection.substr(keyword.length());
      result = resultCandidate;
    }     
  }

  return result;
}

//***********************************************************************************

std::string U3DTools::getParentNameFromGroupPath(std::string groupPath)  
{
  std::string result = "";  // Default = root node
    
  StringVector groupPathComponents = stringSplit(groupPath, "/", false, false);

  if (groupPathComponents.size() > 0)
  {
    result = groupPathComponents.back();
  }

  return result;
}

//***********************************************************************************

// Make sure each group path has a leading and a trailing "/"
std::string U3DTools::normalizeGroupPath(std::string groupPath)
{
  std::string result = groupPath;

  if (result == "")
  {
    result = "/";
  }
  else
  {
    if (groupPath[0] != '/')
    {
      result = "/" + result;
    }

    if (groupPath[groupPath.size()-1] != '/')
    {
      result += "/";
    }
  }

  return result;
}

//***********************************************************************************

StringVector U3DTools::stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty, const bool trimWhitespace)
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
    if (trimWhitespace)
    {
      temp = stringTrimWhitespace(temp);
    }

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

void U3DTools::stringLower(std::string &sourceString)
{
  for(unsigned short loop=0;loop < sourceString.size();loop++)
  {
    sourceString[loop] = (char)std::tolower(sourceString[loop]);
  }
}

//***********************************************************************************

bool U3DTools::stringToBool(std::string sourceString)
{
  stringLower(sourceString);

  return ((sourceString == "true") || (sourceString != "0"));
}

//***********************************************************************************

std::string U3DTools::stringTrimWhitespace(std::string sourceString)
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

std::string U3DTools::getMeVisLabVersionNumberString()
{
  return ml::ApplicationProperties::getString("Version");
}

//***********************************************************************************

std::string U3DTools::getLibraryVersionNumberString()
{
  return "1.0";
}

//***********************************************************************************

long U3DTools::getFileSize(std::string filename)
{
  struct stat fileCheckBuffer;
  int rc = stat(filename.c_str(), &fileCheckBuffer);

  return rc == 0 ? fileCheckBuffer.st_size : -1;
}

//***********************************************************************************

Vector4 U3DTools::getColorVec4FromString(std::string colorString, const Vector4 defaultColor)
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

Vector3 U3DTools::getColorVec3FromString(std::string colorString, const Vector3 defaultColor)
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

Vector3 U3DTools::getVec3FromString(std::string vectorString, const Vector3 defaultValue)
{
  return getColorVec3FromString(vectorString, defaultValue);
}

//***********************************************************************************

std::string U3DTools::FormatColorString(Vector3 colorValue)
{
  const int bufferLength = 18;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z);

  std::string result(buffer);

  return result;
}

std::string U3DTools::FormatColorString(Vector3 colorValue, float alphaValue)
{
  const int bufferLength = 24;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z, alphaValue);

  std::string result(buffer);

  return result;
}

std::string U3DTools::FormatVec3String(Vector3 vectorValue)
{
  const int bufferLength = 27;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", vectorValue.x, vectorValue.y, vectorValue.z);

  std::string result(buffer);

  return result;
}

std::string U3DTools::FormatDouble(double value)
{
  const int bufferLength = 12;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f", value);

  std::string result(buffer);

  return result;
}

std::string U3DTools::FormatFloat(float value)
{
  return FormatDouble((double)value);
}

std::string U3DTools::FormatDate(std::tm value)
{
  const int bufferLength = 19;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%4u-%02u-%02u %02u:%02u:%02u", value.tm_year + 1900, value.tm_mon + 1, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec);

  std::string result(buffer);

  return result;
}

std::string U3DTools::FormatUInt(unsigned int value, unsigned int length)
{
  const int bufferLength = 12;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%0*u", length, value);

  std::string result(buffer);

  return result;
}


//***********************************************************************************

std::string U3DTools::updateGeometryGeneratorMap(mlU3D::GeometryGeneratorMap& geometryGeneratorMap, std::string specificationString, std::string namePrefix, int startIndex)
{
  // Remove all whitespaces from specification string
  mlU3D::StringVector specificationComponentsVector = mlU3D::U3DTools::stringSplit(specificationString, ",", false);

  std::string specificationStringWithoutWhitespace = "";
  int nextGeneratorNumber = (int)geometryGeneratorMap.size() + 1 + startIndex;

  for (size_t i = 0; i < specificationComponentsVector.size(); i++)
  {
    if (i != 0)
    {
      specificationStringWithoutWhitespace += ",";

    }
    specificationStringWithoutWhitespace += specificationComponentsVector[i];
  }

  if (geometryGeneratorMap.count(specificationStringWithoutWhitespace) == 0)
  {
    mlU3D::GeometryGeneratorInfoStruct newGeometryGeneratorInfo;
    newGeometryGeneratorInfo.GeometryName = namePrefix + " " + mlU3D::U3DTools::FormatUInt(nextGeneratorNumber, 3);
    newGeometryGeneratorInfo.UseVertexColors = false;
    geometryGeneratorMap[specificationStringWithoutWhitespace] = newGeometryGeneratorInfo;
  }

  return specificationStringWithoutWhitespace;
}

//***********************************************************************************

// Updates the model bounding box
void U3DTools::UpdateBoundingBox(ModelBoundingBoxStruct& existingBoundingBox, ModelBoundingBoxStruct newCorners)
{
  MLdouble smallestX = ML_DOUBLE_MAX;
  MLdouble smallestY = ML_DOUBLE_MAX;
  MLdouble smallestZ = ML_DOUBLE_MAX;
   
  MLdouble biggestX = ML_DOUBLE_MAX * -1;
  MLdouble biggestY = ML_DOUBLE_MAX * -1;
  MLdouble biggestZ = ML_DOUBLE_MAX * -1;

  if (newCorners.start.x < smallestX) { smallestX = newCorners.start.x; }
  if (newCorners.start.x > biggestX)  { biggestX = newCorners.start.x; }
  if (newCorners.end.x < smallestX)   { smallestX = newCorners.end.x; }
  if (newCorners.end.x > biggestX)    { biggestX = newCorners.end.x; }

  if (newCorners.start.y < smallestY) { smallestY = newCorners.start.y; }
  if (newCorners.start.y > biggestY)  { biggestY = newCorners.start.y; }
  if (newCorners.end.y < smallestY)   { smallestY = newCorners.end.y; }
  if (newCorners.end.y > biggestY)    { biggestY = newCorners.end.y; }

  if (newCorners.start.z < smallestZ) { smallestZ = newCorners.start.z; }
  if (newCorners.start.z > biggestZ)  { biggestZ = newCorners.start.z; }
  if (newCorners.end.z < smallestZ)   { smallestZ = newCorners.end.z; }
  if (newCorners.end.z > biggestZ)    { biggestZ = newCorners.end.z; }

  if (existingBoundingBox.start.x > smallestX) { existingBoundingBox.start.x = smallestX; }
  if (existingBoundingBox.start.y > smallestY) { existingBoundingBox.start.y = smallestY; }
  if (existingBoundingBox.start.z > smallestZ) { existingBoundingBox.start.z = smallestZ; }

  if (existingBoundingBox.end.x < biggestX) { existingBoundingBox.end.x = biggestX; }
  if (existingBoundingBox.end.y < biggestY) { existingBoundingBox.end.y = biggestY; }
  if (existingBoundingBox.end.z < biggestZ) { existingBoundingBox.end.z = biggestZ; }

  MLdouble centerX = (existingBoundingBox.start.x + existingBoundingBox.end.x) / 2.f;
  MLdouble centerY = (existingBoundingBox.start.y + existingBoundingBox.end.y) / 2.f;
  MLdouble centerZ = (existingBoundingBox.start.z + existingBoundingBox.end.z) / 2.f;

  existingBoundingBox.center.x = centerX;
  existingBoundingBox.center.y = centerY;
  existingBoundingBox.center.z = centerZ;

  MLdouble newRadius = sqrt(pow(existingBoundingBox.end.x-existingBoundingBox.center.x,2) + pow(existingBoundingBox.end.y-existingBoundingBox.center.y,2) + pow(existingBoundingBox.end.z-existingBoundingBox.center.z,2));

  if (newRadius > existingBoundingBox.radius)
  {
    existingBoundingBox.radius = newRadius;
  }
}

//***********************************************************************************

// Get bounding box edges from positions
ModelBoundingBoxStruct U3DTools::GetBoundingBoxFomPositions(PositionsVector positions)
{
  ModelBoundingBoxStruct result;

  MLdouble smallestX = ML_DOUBLE_MAX;
  MLdouble smallestY = ML_DOUBLE_MAX;
  MLdouble smallestZ = ML_DOUBLE_MAX;

  MLdouble biggestX = ML_DOUBLE_MAX * -1;
  MLdouble biggestY = ML_DOUBLE_MAX * -1;
  MLdouble biggestZ = ML_DOUBLE_MAX * -1;

  for (PositionsVector::size_type i = 0; i < positions.size(); i++)
  {
    PositionStruct thisPosition = positions[i];

    if (thisPosition.position.x < smallestX) { smallestX = thisPosition.position.x; }
    if (thisPosition.position.x > biggestX)  { biggestX = thisPosition.position.x; }

    if (thisPosition.position.y < smallestY) { smallestY = thisPosition.position.y; }
    if (thisPosition.position.y > biggestY)  { biggestY = thisPosition.position.y; }

    if (thisPosition.position.z < smallestZ) { smallestZ = thisPosition.position.z; }
    if (thisPosition.position.z > biggestZ)  { biggestZ = thisPosition.position.z; }
  }

  result.start.x = smallestX;
  result.start.y = smallestY;
  result.start.z = smallestZ;

  result.end.x = biggestX;
  result.end.y = biggestY;
  result.end.z = biggestZ;

  return result;
}

//***********************************************************************************


} // end namespace mlU3D

ML_END_NAMESPACE