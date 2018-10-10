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
#include "MLU3D_MarkerListTools.h"


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

void U3DTools::dequantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ)
{
	MLuint8 signX = u8Signs & 1;
	MLuint8 signY = u8Signs & (1<<1);
	MLuint8 signZ = u8Signs & (1<<2);
	pos.x = (((MLdouble)udX) - 0.5f) / mlU3D::Quant_Position;
	if (signX) pos.x *= -1;
	pos.y = (((MLdouble)udY) - 0.5f) / mlU3D::Quant_Position;
	if (signY) pos.y *= -1;
	pos.z = (((MLdouble)udZ) - 0.5f) / mlU3D::Quant_Position;
	if (signZ) pos.z *= -1;

}

void U3DTools::dequantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ)
{
	MLuint8 signX = u8Signs & 1;
	MLuint8 signY = u8Signs & (1 << 1);
	MLuint8 signZ = u8Signs & (1 << 2);
	pos.x = (((MLdouble)udX) - 0.5f) / mlU3D::Quant_Normal;
	if (signX) pos.x *= -1;
	pos.y = (((MLdouble)udY) - 0.5f) / mlU3D::Quant_Normal;
	if (signY) pos.y *= -1;
	pos.z = (((MLdouble)udZ) - 0.5f) / mlU3D::Quant_Normal;
	if (signZ) pos.z *= -1;
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

mlU3D::ModelNode U3DTools::createNewModelNode(const ObjectSpecificationMap& specification, mlU3D::U3DObjectPtr u3dObject)
  {
    //mlInfo(__FUNCTION__) << "Log message.";

    mlU3D::ModelNode newModelNode;

    //newModelNode.Type = modelType;
    newModelNode.displayName  = specification.at("ObjectName");  // Can't use [] operator here since "specification" parameter is const!
    newModelNode.internalName = specification.at("ObjectName");
    makeInternalNameUnique(newModelNode.internalName, u3dObject);
    newModelNode.groupPath    = specification.at("GroupPath");
    newModelNode.visibility   = (MLuint32)mlU3D::stringToInt(specification.at("ModelVisibility"));

    if (specification.at("ObjectType") != mlU3D::MODELTYPE_POINTSET)
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

// Get model specs from fiberset container
StringVector U3DTools::getLineSetSpecificationsStringFromFiberSetContainer(FiberSetContainer& fiberSetContainer)
{
  StringVector result;

  // Loop through all patches, search and parse label & description
  const MLuint32 numberOfFiberSets = (MLuint32)fiberSetContainer.getNumFiberSets();

  for (MLuint32 i = 0; i < numberOfFiberSets; i++)
  {
    FiberSetContainer::FiberSet thisFiberSet = fiberSetContainer.getFiberSet(i);

    Vector3 thisFiberSetColor     = thisFiberSet.getColor();
    std::string thisFiberSetLabel = thisFiberSet.getLabel();

    std::string objectName = thisFiberSetLabel;
    if (objectName == "") {
      objectName = "Object";  // This will be replaced later by the default name
    }

    // ...and write data into meshSpecification string
    std::string lineSetSpecificationsString = "<LineSet>";
    lineSetSpecificationsString += "<FiberSets>" + intToString(i) + "</FiberSets>";
    lineSetSpecificationsString += "<ObjectName>" + objectName + "</ObjectName>";
    lineSetSpecificationsString += "<Color>" + formatColorString(thisFiberSetColor) + "</Color>";
    lineSetSpecificationsString += "<ModelVisibility>3</ModelVisibility>";
    lineSetSpecificationsString += "</LineSet>";

    // Add meshSpecification string to meshSpecificationVector
    result.push_back(lineSetSpecificationsString);
    
  } // for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)

  return result;
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

      std::string objectName = thisWEMPatchLabel;
      if (objectName == "") {
        objectName = "Object"; // This will be replaced later by the default name
      }

      // ...and write data into meshSpecification string
      std::string meshSpecificationsString = "<Mesh>";
      meshSpecificationsString += "<WEMLabel>" + thisWEMPatchLabel + "</WEMLabel>";
      meshSpecificationsString += "<ObjectName>" + objectName + "</ObjectName>";
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
ObjectSpecificationMap U3DTools::getAllSpecificationParametersFromString(const std::string specificationString)
{
  ObjectSpecificationMap result;

  result["ObjectName"]      = getSpecificParameterFromString(specificationString, "<ObjectName>", "Object");
  result["GroupPath"]       = normalizeGroupPath(getSpecificParameterFromString(specificationString, "<GroupPath>"));
  result["Color"]           = getSpecificParameterFromString(specificationString, "<Color>");
  result["SpecularColor"]   = getSpecificParameterFromString(specificationString, "<SpecularColor>");
  result["Opacity"]         = getSpecificParameterFromString(specificationString, "<Opacity>");
  result["GlyphText"]       = getSpecificParameterFromString(specificationString, "<GlyphText>");     // U3D Glyphs are not supported by Acrobat... :-(
  result["MetaDataKey"]     = getSpecificParameterFromString(specificationString, "<MetaDataKey>");
  result["MetaDataValue"]   = getSpecificParameterFromString(specificationString, "<MetaDataValue>");
  result["ModelVisibility"] = getSpecificParameterFromString(specificationString, "<ModelVisibility>", "3");
  result["WEMLabel"]        = getSpecificParameterFromString(specificationString, "<WEMLabel>");
  result["FiberSets"]       = getSpecificParameterFromString(specificationString, "<FiberSets>");
  result["PositionTypes"]   = getSpecificParameterFromString(specificationString, "<PositionTypes>", "all");
  result["ConnectionTypes"] = getSpecificParameterFromString(specificationString, "<ConnectionTypes>", "simple");

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

std::string U3DTools::formatColorString(Vector3 colorValue)
{
  const int bufferLength = 18;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z);

  std::string result(buffer);

  return result;
}

std::string U3DTools::formatColorString(Vector3 colorValue, float alphaValue)
{
  const int bufferLength = 24;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f %.3f", colorValue.x, colorValue.y, colorValue.z, alphaValue);

  std::string result(buffer);

  return result;
}

std::string U3DTools::formatVec3String(Vector3 vectorValue)
{
  const int bufferLength = 27;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f %.3f %.3f", vectorValue.x, vectorValue.y, vectorValue.z);

  std::string result(buffer);

  return result;
}

std::string U3DTools::formatDouble(double value)
{
  const int bufferLength = 12;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%.3f", value);

  std::string result(buffer);

  return result;
}

std::string U3DTools::formatFloat(float value)
{
  return formatDouble((double)value);
}

std::string U3DTools::formatDate(std::tm value)
{
  const int bufferLength = 19;
  char buffer[bufferLength];

  snprintf(buffer, bufferLength, "%4u-%02u-%02u %02u:%02u:%02u", value.tm_year + 1900, value.tm_mon + 1, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec);

  std::string result(buffer);

  return result;
}

std::string U3DTools::formatUInt(unsigned int value, unsigned int length)
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
    newGeometryGeneratorInfo.GeometryName = namePrefix + " " + mlU3D::U3DTools::formatUInt(nextGeneratorNumber, 3);
    newGeometryGeneratorInfo.UseVertexColors = false;
    geometryGeneratorMap[specificationStringWithoutWhitespace] = newGeometryGeneratorInfo;
  }

  return specificationStringWithoutWhitespace;
}

//***********************************************************************************

void U3DTools::addPointSetModelAndGeometry(const mlU3D::StringVector& specificationsVector,U3DObjectPtr u3dObject,  const ml::XMarkerList& pointPositions)
{
  mlU3D::GeometryGeneratorMap pointSetGeometryGeneratorMap;
  const MLuint32 numberOfPointSetSpecifications = (MLuint32)specificationsVector.size();

  // *****************************
  // Generate model specifications
  // *****************************

  for (MLuint32 i = 0; i < numberOfPointSetSpecifications; i++)
  {
    ObjectSpecificationMap thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(specificationsVector[i]);
    thisSpecificationParameters["ObjectType"] = mlU3D::MODELTYPE_POINTSET;

    // Override default object name
    if (thisSpecificationParameters["ObjectName"] == "Object")
    {
      thisSpecificationParameters["ObjectName"] = u3dObject->defaultValues.defaultPointSetPefix + " " + mlU3D::intToString((int)(1 + u3dObject->getNumModels(u3dObject->defaultValues.defaultPointSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, u3dObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(pointSetGeometryGeneratorMap, thisSpecificationParameters["PositionTypes"], mlU3D::GEOMETRYPREFIX_POINTSET, u3dObject->getNumGeometryGenerators(mlU3D::GEOMETRYPREFIX_POINTSET));
    newModelNode.geometryGeneratorName = pointSetGeometryGeneratorMap[generatorKey].GeometryName;
    u3dObject->modelNodes.push_back(newModelNode);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = pointSetGeometryGeneratorMap.begin(); mapIterator != pointSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string specificationString = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    // Collect all position coordinates
    mlU3D::PositionsVector thisPointSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(pointPositions, specificationString);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::getBoundingBoxFomPositions(thisPointSetPositions);
    mlU3D::U3DTools::updateBoundingBox(u3dObject->modelBoundingBox, newBoundingBox);

    // Add generator for point set
    mlU3D::PointSetGenerator thisPointSetGenerator;
    thisPointSetGenerator.resourceName       = geometryName;
    thisPointSetGenerator.diffuseColorCount  = 0;    // This is not really needed in this version
    thisPointSetGenerator.specularColorCount = 0;    // This is not really needed in this version
    thisPointSetGenerator.textureCoordCount  = 0;    // Not supported by Acrobat for point sets
    thisPointSetGenerator.textureLayerCount  = 0;    // Not supported by Acrobat for point sets
    thisPointSetGenerator.pointCount = (MLuint32)thisPointSetPositions.size();
    thisPointSetGenerator.normalCount        = 0;    // No normals in this version since normals are not used by Acrobat
    thisPointSetGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
    thisPointSetGenerator.shadingAttributes |= ((thisPointSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisPointSetGenerator.shadingAttributes |= ((thisPointSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisPointSetGenerator.positions          = thisPointSetPositions;
    u3dObject->pointSets.push_back(thisPointSetGenerator);
  }
}

//***********************************************************************************

void U3DTools::addLineSetModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, const ml::XMarkerList& linePositions, ml::IndexPairList& lineConnections)
{
  const MLuint32 numberOfLineSetSpecifications = (MLuint32)specificationsVector.size();

  mlU3D::GeometryGeneratorMap lineSetGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  for (MLuint32 i = 0; i < numberOfLineSetSpecifications; i++)
  {
    Vector3 thisModelSpecularColor;
    Vector4 thisModelDiffuseColor;

    ObjectSpecificationMap thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(specificationsVector[i]);
    thisSpecificationParameters["ObjectType"] = mlU3D::MODELTYPE_LINESET;

    // Override default object name
    if (thisSpecificationParameters["ObjectName"] == "Object")
    {
      thisSpecificationParameters["ObjectName"] = u3dObject->defaultValues.defaultLineSetPefix + " " + mlU3D::intToString((int)(1 + u3dObject->getNumModels(u3dObject->defaultValues.defaultLineSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, u3dObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(lineSetGeometryGeneratorMap, thisSpecificationParameters["PositionTypes"] + " | " + thisSpecificationParameters["ConnectionTypes"], mlU3D::GEOMETRYPREFIX_LINESET);
    newModelNode.geometryGeneratorName = lineSetGeometryGeneratorMap[generatorKey].GeometryName;
    u3dObject->modelNodes.push_back(newModelNode);

    // Set colors
    if (thisSpecificationParameters["Color"] == mlU3D::USEVERTEXCOLORS)
    {
      // Never use vertex colors for line sets
      thisModelDiffuseColor = u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisModelDiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters["Color"], u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisModelSpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters["SpecularColor"], u3dObject->defaultValues.defaultMaterialSpecularColor);

    thisModelDiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    // Add shader for line set
    mlU3D::LitTextureShader thisLineSetShader;
    thisLineSetShader.resourceName = newModelNode.shaderName;
    thisLineSetShader.materialResourceName = "Material for " + newModelNode.internalName;
    u3dObject->litTextureShaders.push_back(thisLineSetShader);

    // Add material for line set
    mlU3D::MaterialResource thisLineSetMaterialResource;
    thisLineSetMaterialResource.resourceName = "Material for " + newModelNode.internalName;
    thisLineSetMaterialResource.ambientColor = u3dObject->defaultValues.defaultMaterialAmbientColor;
    thisLineSetMaterialResource.diffuseColor = thisModelDiffuseColor;
    thisLineSetMaterialResource.specularColor = thisModelSpecularColor;
    thisLineSetMaterialResource.emissiveColor = u3dObject->defaultValues.defaultMaterialEmissiveColor;
    u3dObject->materialResources.push_back(thisLineSetMaterialResource);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = lineSetGeometryGeneratorMap.begin(); mapIterator != lineSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string specificationString = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    mlU3D::StringVector specificationComponentsVector = mlU3D::U3DTools::stringSplit(specificationString, "|", false);
    std::string positionTypes = specificationComponentsVector[0];
    std::string connectionTypes = specificationComponentsVector[1];

    // Collect all position coordinates
    mlU3D::PositionsVector thisLineSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(linePositions, positionTypes);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::getBoundingBoxFomPositions(thisLineSetPositions);
    mlU3D::U3DTools::updateBoundingBox(u3dObject->modelBoundingBox, newBoundingBox);

    // Collect all line connections
    mlU3D::LinesVector thisLineSetLines;
    if ((lineConnections.size() > 0) && (connectionTypes != "simple"))
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getAllLinesFromIndexPairList(lineConnections, connectionTypes);
    }
    else
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getStandardLinesFromXMarkerList(linePositions, positionTypes);
    }

    // Add generator for line set
    mlU3D::LineSetGenerator thisLineSetGenerator;
    thisLineSetGenerator.resourceName       = geometryName;
    thisLineSetGenerator.diffuseColorCount  = 0;     // This is not really needed in this version
    thisLineSetGenerator.specularColorCount = 0;     // This is not really needed in this version
    thisLineSetGenerator.textureCoordCount  = 0;     // Not supported by Acrobat for line sets
    thisLineSetGenerator.textureLayerCount  = 0;     // Not supported by Acrobat for line sets
    thisLineSetGenerator.pointCount = (MLuint32)thisLineSetPositions.size();
    thisLineSetGenerator.lineCount          = (MLuint32)thisLineSetLines.size();
    thisLineSetGenerator.normalCount        = 0;           // No normals in this version since normals are not used by Acrobat
    thisLineSetGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisLineSetGenerator.positions          = thisLineSetPositions;
    thisLineSetGenerator.lines              = thisLineSetLines;
    u3dObject->lineSets.push_back(thisLineSetGenerator);
  }
}

//***********************************************************************************

void U3DTools::addLineSetModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, const FiberSetContainer& fiberSetContainer)
{

  const MLuint32 numberOfLineSetSpecifications = (MLuint32)specificationsVector.size();
  mlU3D::GeometryGeneratorMap lineSetGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  for (MLuint32 i = 0; i < numberOfLineSetSpecifications; i++)
  {
    Vector3 thisModelSpecularColor;
    Vector4 thisModelDiffuseColor;

    ObjectSpecificationMap thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(specificationsVector[i]);
    thisSpecificationParameters["ObjectType"] = mlU3D::MODELTYPE_LINESET;

    // Override default object name
    if (thisSpecificationParameters["ObjectName"] == "Object")
    {
      thisSpecificationParameters["ObjectName"] = u3dObject->defaultValues.defaultLineSetPefix + " " + mlU3D::intToString((int)(1 + u3dObject->getNumModels(u3dObject->defaultValues.defaultLineSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, u3dObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(lineSetGeometryGeneratorMap, thisSpecificationParameters["FiberSets"], mlU3D::GEOMETRYPREFIX_LINESET);
    newModelNode.geometryGeneratorName = lineSetGeometryGeneratorMap[generatorKey].GeometryName;
    u3dObject->modelNodes.push_back(newModelNode);

    // Set colors
    if (thisSpecificationParameters["Color"] == mlU3D::USEVERTEXCOLORS)
    {
      // Never use vertex colors for line sets
      thisModelDiffuseColor = u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisModelDiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters["Color"], u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisModelSpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters["SpecularColor"], u3dObject->defaultValues.defaultMaterialSpecularColor);

    thisModelDiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    // Add shader for line set
    mlU3D::LitTextureShader thisLineSetShader;
    thisLineSetShader.resourceName = newModelNode.shaderName;
    thisLineSetShader.materialResourceName = "Material for " + newModelNode.internalName;
    u3dObject->litTextureShaders.push_back(thisLineSetShader);

    // Add material for line set
    mlU3D::MaterialResource thisLineSetMaterialResource;
    thisLineSetMaterialResource.resourceName = "Material for " + newModelNode.internalName;
    thisLineSetMaterialResource.ambientColor = u3dObject->defaultValues.defaultMaterialAmbientColor;
    thisLineSetMaterialResource.diffuseColor = thisModelDiffuseColor;
    thisLineSetMaterialResource.specularColor = thisModelSpecularColor;
    thisLineSetMaterialResource.emissiveColor = u3dObject->defaultValues.defaultMaterialEmissiveColor;
    u3dObject->materialResources.push_back(thisLineSetMaterialResource);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = lineSetGeometryGeneratorMap.begin(); mapIterator != lineSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string fiberSets = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    // Collect all position coordinates
    mlU3D::PositionsVector thisLineSetPositions;
    mlU3D::LinesVector     thisLineSetLines;

    mlU3D::U3DMarkerListTools::getAllPositionsAndLinesFromFiberSetContainer(fiberSetContainer, fiberSets, thisLineSetPositions, thisLineSetLines);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::getBoundingBoxFomPositions(thisLineSetPositions);
    mlU3D::U3DTools::updateBoundingBox(u3dObject->modelBoundingBox, newBoundingBox);

    // Add generator for line set
    mlU3D::LineSetGenerator thisLineSetGenerator;
    thisLineSetGenerator.resourceName       = geometryName;
    thisLineSetGenerator.diffuseColorCount  = 0;     // This is not really needed in this version
    thisLineSetGenerator.specularColorCount = 0;     // This is not really needed in this version
    thisLineSetGenerator.textureCoordCount  = 0;     // Not supported by Acrobat for line sets
    thisLineSetGenerator.textureLayerCount  = 0;     // Not supported by Acrobat for line sets
    thisLineSetGenerator.pointCount         = (MLuint32)thisLineSetPositions.size();
    thisLineSetGenerator.lineCount          = (MLuint32)thisLineSetLines.size();
    thisLineSetGenerator.normalCount        = 0;     // No normals in this version since normals are not used by Acrobat
    thisLineSetGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisLineSetGenerator.positions          = thisLineSetPositions;
    thisLineSetGenerator.lines              = thisLineSetLines;
    u3dObject->lineSets.push_back(thisLineSetGenerator);

  }

}


//***********************************************************************************

WEM* U3DTools::constructQuadWem(Vector3 point_a, Vector3 point_b, Vector3 point_c, Vector3 point_d){
	WEM* return_value = new WEM();
	WEMTrianglePatch* patch = new WEMTrianglePatch();

	WEMNode* node_a = patch->addNode();
	node_a->setPosition(point_a);
	WEMNode* node_b = patch->addNode();
	node_b->setPosition(point_b);
	WEMNode* node_c = patch->addNode();
	node_c->setPosition(point_c);
	WEMNode* node_d = patch->addNode();
	node_d->setPosition(point_d);

	WEMFace* face_a = patch->addFace();
	face_a->setNode(0, node_a);
	face_a->setNode(1, node_b);
	face_a->setNode(2, node_c);
	WEMFace* face_b = patch->addFace();
	face_b->setNode(0, node_c);
	face_b->setNode(1, node_d);
	face_b->setNode(2, node_a);

	return_value->addWEMPatch(patch);
	return return_value;
}

//***********************************************************************************

void U3DTools::addMeshModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, WEM* inWEM, std::string geometryGeneratorPrefix)
{
  mlU3D::GeometryGeneratorMap meshGeometryGeneratorMap;
  const MLuint32 numberOfMeshSpecifications = (MLuint32)specificationsVector.size();

  WEMPtr tempWEM = NULL;
  ML_CHECK_NEW(tempWEM, WEM());
  MLuint32 meshNumber = 0;

  // *****************************
  // Generate model specifications
  // *****************************

  for (MLuint32 i = 0; i < numberOfMeshSpecifications; i++)
  {
    Vector3 thisModelSpecularColor;
    Vector4 thisModelDiffuseColor;
    bool    thisModelUseVertexColors = false;

    ObjectSpecificationMap thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(specificationsVector[i]);
    thisSpecificationParameters["ObjectType"] = mlU3D::MODELTYPE_MESH;

    // Override default object name
    if (thisSpecificationParameters["ObjectName"] == "Object")
    {
      thisSpecificationParameters["ObjectName"] = u3dObject->defaultValues.defaultMeshPefix + " " + mlU3D::intToString((int)(1 + u3dObject->getNumModels(u3dObject->defaultValues.defaultMeshPefix)));

    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, u3dObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(meshGeometryGeneratorMap, thisSpecificationParameters["WEMLabel"], mlU3D::GEOMETRYPREFIX_MESH);
    newModelNode.geometryGeneratorName = geometryGeneratorPrefix + meshGeometryGeneratorMap[generatorKey].GeometryName;
    u3dObject->modelNodes.push_back(newModelNode);

    // Set colors
    if (thisSpecificationParameters["Color"] == mlU3D::USEVERTEXCOLORS)
    {
      thisModelUseVertexColors = true;
      thisModelDiffuseColor = u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisModelUseVertexColors = false;
      thisModelDiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters["Color"], u3dObject->defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisModelSpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters["SpecularColor"], u3dObject->defaultValues.defaultMaterialSpecularColor);

    if (thisSpecificationParameters["Opacity"] != "")
    {
      thisModelDiffuseColor[3] = mlU3D::stringToMLDouble(thisSpecificationParameters["Opacity"]);
    }

    // Add shader for mesh
    mlU3D::LitTextureShader thisMeshShader;
    thisMeshShader.resourceName = newModelNode.shaderName;
    thisMeshShader.materialResourceName = "Material for " + newModelNode.internalName;
    u3dObject->litTextureShaders.push_back(thisMeshShader);

    // Add material for mesh
    mlU3D::MaterialResource thisMeshMaterialResource;
    thisMeshMaterialResource.resourceName = "Material for " + newModelNode.internalName;
    thisMeshMaterialResource.ambientColor = u3dObject->defaultValues.defaultMaterialAmbientColor;
    thisMeshMaterialResource.diffuseColor = thisModelDiffuseColor;
    thisMeshMaterialResource.specularColor = thisModelSpecularColor;
    thisMeshMaterialResource.emissiveColor = u3dObject->defaultValues.defaultMaterialEmissiveColor;
    u3dObject->materialResources.push_back(thisMeshMaterialResource);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = meshGeometryGeneratorMap.begin(); mapIterator != meshGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string wemLabel = (*mapIterator).first;
    std::string geometryName = geometryGeneratorPrefix + ((*mapIterator).second).GeometryName;
    bool useVertexColors = ((*mapIterator).second).UseVertexColors;


    // Search for correct WEM patch, triangulate it if necessary, creeate a copy and add it to mesh generator resource
    const MLuint32 numberOfInWEMPatches = inWEM->getNumWEMPatches();

    for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)
    {
      WEMPatch* thisInWEMPatch = inWEM->getWEMPatchAt(i);
      std::string thisInWEMPatchLabel = thisInWEMPatch->getLabel();

      if ((thisInWEMPatchLabel == wemLabel) && (thisInWEMPatch->getNumFaces() > 0))
      {

        WEMTrianglePatch* trianglePatch;

        if (thisInWEMPatch->getPatchType() != WEM_PATCH_TRIANGLES)
        {
          WEMTrianglePatch* triangulatedPatch = NULL;

          ML_CHECK_NEW(triangulatedPatch, WEMTrianglePatch());
          thisInWEMPatch->triangulate(triangulatedPatch, WEM_TRIANGULATION_STRIP);
          trianglePatch = tempWEM->addWEMPatchCopy(triangulatedPatch);
          ML_DELETE(triangulatedPatch);
        }
        else
        {
          trianglePatch = tempWEM->addWEMPatchCopy(reinterpret_cast<WEMTrianglePatch*>(thisInWEMPatch));
        }


        if (trianglePatch)
        {
          trianglePatch->buildEdgeConnectivity();
          trianglePatch->computeBoundingBox();
          trianglePatch->setId(i);

          // Adjust properties of main WEM bounding box
          WEMBoundingBox* thisWEMPatchBoundingBox = trianglePatch->getBoundingBox();
          mlU3D::ModelBoundingBoxStruct newBoundingBox;
          newBoundingBox.start = thisWEMPatchBoundingBox->getMin();
          newBoundingBox.end = thisWEMPatchBoundingBox->getMax();
          mlU3D::U3DTools::updateBoundingBox(u3dObject->modelBoundingBox, newBoundingBox);

          // Add generator for mesh
          mlU3D::CLODMeshGenerator thisMeshGenerator;
          thisMeshGenerator.resourceName       = geometryName;
          thisMeshGenerator.diffuseColorCount  = (useVertexColors ? getNumberOfDifferentColorsFromWEMPatch(trianglePatch) : 0);
          thisMeshGenerator.specularColorCount = (useVertexColors ? 1 : 0);
          thisMeshGenerator.textureCoordCount  = 0;
          thisMeshGenerator.textureLayerCount  = 0;
          thisMeshGenerator.faceCount          = trianglePatch->getNumFaces();
          thisMeshGenerator.normalCount        = trianglePatch->getNumNodes();
          thisMeshGenerator.vertexCount        = trianglePatch->getNumNodes();
          thisMeshGenerator.patchID            = trianglePatch->getId();
          thisMeshGenerator.meshAttributes     = mlU3D::MESH_ATTRIBUTES_DEFAULT;
          thisMeshGenerator.meshAttributes    |= ((thisMeshGenerator.normalCount == 0) ? mlU3D::MESH_ATTRIBUTES_EXCLUDENORMALS : 0);
          thisMeshGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
          thisMeshGenerator.shadingAttributes |= ((thisMeshGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);
          thisMeshGenerator.shadingAttributes |= ((thisMeshGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0);
          thisMeshGenerator.meshNumber         = meshNumber++;
          thisMeshGenerator.setMeshData(trianglePatch);
          u3dObject->meshes.push_back(thisMeshGenerator);

          break;
        }

      }

    } // for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)

  }

  tempWEM = NULL;
}

//***********************************************************************************

// Scan all node colors of a WEM patch and return number of different colors.
MLuint32 U3DTools::getNumberOfDifferentColorsFromWEMPatch(WEMTrianglePatch* patch)
{
  MLuint result = 0;

  std::set<Vector4> colorsSet;

  const int numNodesInPatch = patch->getNumNodes();

  for (int n = 0; n < numNodesInPatch; n++)
  {
    WEMNode* thisNode = patch->getNodeAt(n);

    const Vector4 thisNodeColor = thisNode->getColor();

    colorsSet.insert(thisNodeColor);
  }

  result = colorsSet.size();

  return result;
}

//***********************************************************************************

// Updates the model bounding box
void U3DTools::updateBoundingBox(ModelBoundingBoxStruct& existingBoundingBox, ModelBoundingBoxStruct newCorners)
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
ModelBoundingBoxStruct U3DTools::getBoundingBoxFomPositions(PositionsVector positions)
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

static std::vector<std::pair<MLfloat, MLfloat>> generateDefaultTextureMapping(MLint numberOfCoordinates){
	std::vector<std::pair<MLfloat, MLfloat>> returnValue;
	std::pair<MLfloat, MLfloat> corners[6];
	corners[0] = std::pair<MLfloat, MLfloat>(0, 0);
	corners[1] = std::pair<MLfloat, MLfloat>(1, 0);
	corners[2] = std::pair<MLfloat, MLfloat>(0, 1);
	corners[3] = std::pair<MLfloat, MLfloat>(0, 1);
	corners[4] = std::pair<MLfloat, MLfloat>(1, 0);
	corners[5] = std::pair<MLfloat, MLfloat>(1, 1);
	for (MLint iter = 0; iter < numberOfCoordinates; iter++){
		returnValue.push_back(corners[iter%6]);
	}
	return returnValue;
}

void U3DTools::setNodeShaderToTexture(U3DObjectPtr u3DObject, std::string nodeName, std::string textureName, std::vector<std::pair<MLfloat,MLfloat>> &textureMap){
	
	/* get iterator to model */
	auto node_iterator = u3DObject->modelNodes.begin();
	bool node_found = false;
	for (MLint i = 0; i < (MLint)u3DObject->modelNodes.size() && !node_found; i++){
		if (u3DObject->modelNodes[i].internalName == nodeName){
			node_found = true;
			node_iterator += i;
		}
	}
	if (!node_found){
		return;
	}
	
	/* get iterator to shader */
	std::string shader_name = node_iterator->shaderName;
	auto shader_iterator = u3DObject->litTextureShaders.begin();
	bool shader_found = false;
	for (MLint i = 0; i < (MLint)u3DObject->litTextureShaders.size() && !shader_found; i++){
		if (u3DObject->litTextureShaders[i].resourceName == shader_name){
			shader_found = true;
			shader_iterator += i;
		}
	}
	if (!shader_found){
		return;
	}

	/* get iterator to material */
	std::string material_name = shader_iterator->materialResourceName;
	auto material_iterator = u3DObject->materialResources.begin();
	bool material_found = false;
	for (MLint i = 0; i < (MLint)u3DObject->materialResources.size() && !material_found; i++){
		if (u3DObject->materialResources[i].resourceName == material_name){
			material_found = true;
			material_iterator += i;
		}
	}
	if (!material_found){
		return;
	}

	/* get iterator to geometry generator */
	std::string geometry_name = node_iterator->geometryGeneratorName;
	auto geometry_iterator = u3DObject->meshes.begin();
	bool geometry_found = false;
	for (MLint i = 0; i < (MLint)u3DObject->meshes.size() && !geometry_found; i++){
		if (u3DObject->meshes[i].resourceName == geometry_name){
			geometry_found = true;
			geometry_iterator += i;
		}
	}
	if (!geometry_found){
		return;
	}

	/* wrong amount of UV coordinates leads to an error in Adobe Reader -> abort */
	if (textureMap.size() > 0 && geometry_iterator->faceCount * 3 != textureMap.size()){
		return;
	}

	/* apply changes */
	geometry_iterator->textureLayerCount = 1;
	geometry_iterator->textureCoordinates = textureMap.size() > 0 ? textureMap : generateDefaultTextureMapping(geometry_iterator->faceCount*3);
	geometry_iterator->textureCoordCount = (MLuint32)geometry_iterator->textureCoordinates.size();
	shader_iterator->textureResourceName = textureName;
	material_iterator->ambientColor = Vector3(1, 1, 1);
	material_iterator->diffuseColor = Vector4(0, 0, 0, 1);
	material_iterator->specularColor = Vector3(0, 0, 0);
	material_iterator->reflectivity = 0;

}

//***********************************************************************************

} // end namespace mlU3D

ML_END_NAMESPACE