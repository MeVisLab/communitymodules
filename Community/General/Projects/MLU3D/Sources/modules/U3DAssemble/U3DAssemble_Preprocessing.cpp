//----------------------------------------------------------------------------------
// U3DAssemble module.
//
// \file    U3DAssemble_Preprocessing.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Assembles a U3D object.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAssemble.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//***********************************************************************************


void U3DAssemble::_addPointSetModelData()
{
  mlU3D::StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PointSet><PositionTypes>all</PositionTypes></PointSet>");
  }
  else
  {
    pointSetSpecificationsVector = mlU3D::U3DTools::getModelSpecificationsStringFromUI(_pointCloudSpecificationFld, "<PointSet>");
  }

  const MLuint32 numberOfPointSetSpecifications = (MLuint32)pointSetSpecificationsVector.size();

  if (0 == numberOfPointSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing point cloud specification.");

  mlU3D::GeometryGeneratorMap pointSetGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  std::string prefix = _modelPrefixPointCloudsFld->getStringValue();

  if ("" == prefix)  // Just in case the field was cleared...
  {
    prefix = "PointSet";
  }

  for (MLuint32 i = 0; i < numberOfPointSetSpecifications; i++)
  {
    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(pointSetSpecificationsVector[i]);
    thisSpecificationParameters.ObjectType = mlU3D::MODELTYPE_POINTSET;

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = _outU3DObject->defaultValues.defaultPointSetPefix + " " + mlU3D::intToString((int)(1 + _outU3DObject->getNumModels(_outU3DObject->defaultValues.defaultPointSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, _outU3DObject);
    
    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(pointSetGeometryGeneratorMap, thisSpecificationParameters.PositionTypes, mlU3D::GEOMETRYPREFIX_POINTSET);
    newModelNode.geometryGeneratorName = pointSetGeometryGeneratorMap[generatorKey].GeometryName;
    _outU3DObject->modelNodes.push_back(newModelNode);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = pointSetGeometryGeneratorMap.begin(); mapIterator != pointSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string specificationString = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    // Collect all position coordinates
    mlU3D::PositionsVector thisPointSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(_inPointPositions, specificationString);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::GetBoundingBoxFomPositions(thisPointSetPositions);
    mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newBoundingBox);

    // Add generator for point set
    mlU3D::PointSetGenerator thisPointSetGenerator;
    thisPointSetGenerator.resourceName         = geometryName;
    thisPointSetGenerator.diffuseColorCount    = 0;    // This is not really needed in this version
    thisPointSetGenerator.specularColorCount   = 0;    // This is not really needed in this version
    thisPointSetGenerator.textureCoordCount    = 0;    // This is not really needed in this version
    thisPointSetGenerator.pointCount           = (MLuint32)thisPointSetPositions.size();
    thisPointSetGenerator.normalCount          = 0;    // No normals in this version since normals are not used by Acrobat
    thisPointSetGenerator.shadingAttributes    = mlU3D::SHADINGATTRIBUTES_NONE;
    thisPointSetGenerator.shadingAttributes   |= ((thisPointSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisPointSetGenerator.shadingAttributes   |= ((thisPointSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisPointSetGenerator.positions            = thisPointSetPositions;
    _outU3DObject->pointSets.push_back(thisPointSetGenerator);
  }
}


//***********************************************************************************


void U3DAssemble::_addLineSetModelData()
{  
  mlU3D::StringVector lineSetSpecificationsVector;

  if (_simpleModeLineSetFld->getBoolValue())
  {
    lineSetSpecificationsVector.push_back("<LineSet><PositionTypes>all</PositionTypes><ConnectionTypes>all</ConnectionTypes></LineSet>");
  }
  else
  {
    lineSetSpecificationsVector = mlU3D::U3DTools::getModelSpecificationsStringFromUI(_lineSetSpecificationFld, "<LineSet>");
  }
       
  const MLuint32 numberOfLineSetSpecifications = (MLuint32)lineSetSpecificationsVector.size();  

  if (0 == numberOfLineSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing line set specification.");

  mlU3D::GeometryGeneratorMap lineSetGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  std::string prefix = _modelPrefixLineSetsFld->getStringValue();

  if ("" == prefix)  // Just in case the field was cleared...
  {
    prefix = "LineSet";
  }

  for (MLuint32 i = 0; i < numberOfLineSetSpecifications; i++)
  {
    Vector3 thisModelSpecularColor;
    Vector4 thisModelDiffuseColor;

    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(lineSetSpecificationsVector[i]);
    thisSpecificationParameters.ObjectType = mlU3D::MODELTYPE_LINESET;

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = _outU3DObject->defaultValues.defaultLineSetPefix + " " + mlU3D::intToString((int)(1 + _outU3DObject->getNumModels(_outU3DObject->defaultValues.defaultLineSetPefix)));
    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, _outU3DObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(lineSetGeometryGeneratorMap, thisSpecificationParameters.PositionTypes + "|" + thisSpecificationParameters.ConnectionTypes, mlU3D::GEOMETRYPREFIX_LINESET);
    newModelNode.geometryGeneratorName = lineSetGeometryGeneratorMap[generatorKey].GeometryName;
    _outU3DObject->modelNodes.push_back(newModelNode);

    // Set colors
    if (thisSpecificationParameters.Color == mlU3D::USEVERTEXCOLORS)
    {
      // Never use vertex colors for line sets
      thisModelDiffuseColor = _outU3DObject->defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisModelDiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters.Color, _outU3DObject->defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisModelSpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, _outU3DObject->defaultValues.defaultMaterialSpecularColor);

    thisModelDiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    // Add shader for line set
    mlU3D::LitTextureShader thisLineSetShader;
    thisLineSetShader.resourceName         = newModelNode.shaderName;
    thisLineSetShader.materialResourceName = "Material for " + newModelNode.internalName;
    _outU3DObject->litTextureShaders.push_back(thisLineSetShader);

    // Add material for line set
    mlU3D::MaterialResource thisLineSetMaterialResource;
    thisLineSetMaterialResource.resourceName  = "Material for " + newModelNode.internalName;
    thisLineSetMaterialResource.ambientColor  = _outU3DObject->defaultValues.defaultMaterialAmbientColor;
    thisLineSetMaterialResource.diffuseColor  = thisModelDiffuseColor;
    thisLineSetMaterialResource.specularColor = thisModelSpecularColor;
    thisLineSetMaterialResource.emissiveColor = _outU3DObject->defaultValues.defaultMaterialEmissiveColor;
    _outU3DObject->materialResources.push_back(thisLineSetMaterialResource);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = lineSetGeometryGeneratorMap.begin(); mapIterator != lineSetGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string specificationString = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;

    mlU3D::StringVector specificationComponentsVector = mlU3D::U3DTools::stringSplit(specificationString, "|", false);
    std::string positionTypes   = specificationComponentsVector[0];
    std::string connectionTypes = specificationComponentsVector[1];

    // Collect all position coordinates
    mlU3D::PositionsVector thisLineSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(_inLinePositions, positionTypes);
    mlU3D::ModelBoundingBoxStruct newBoundingBox = mlU3D::U3DTools::GetBoundingBoxFomPositions(thisLineSetPositions);
    mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newBoundingBox);

    // Collect all line connections
    mlU3D::LinesVector thisLineSetLines;
    if ((_inLineConnections.size() > 0) && (connectionTypes != "simple"))
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getAllLinesFromIndexPairList(_inLineConnections, connectionTypes);
    }
    else
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getStandardLinesFromXMarkerList(_inLinePositions, positionTypes);
    }

    // Add generator for line set
    mlU3D::LineSetGenerator thisLineSetGenerator;
    thisLineSetGenerator.resourceName       = geometryName;
    thisLineSetGenerator.diffuseColorCount  = 0;     // This is not really needed in this version
    thisLineSetGenerator.specularColorCount = 0;    // This is not really needed in this version
    thisLineSetGenerator.textureCoordCount  = 0;     // This is not really needed in this version
    thisLineSetGenerator.pointCount         = (MLuint32)thisLineSetPositions.size();
    thisLineSetGenerator.lineCount          = (MLuint32)thisLineSetLines.size();
    thisLineSetGenerator.normalCount        = 0;           // No normals in this version since normals are not used by Acrobat
    thisLineSetGenerator.shadingAttributes  = mlU3D::SHADINGATTRIBUTES_NONE;
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisLineSetGenerator.shadingAttributes |= ((thisLineSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisLineSetGenerator.positions          = thisLineSetPositions;
    thisLineSetGenerator.lines              = thisLineSetLines;
    _outU3DObject->lineSets.push_back(thisLineSetGenerator);
  }
  
}


//***********************************************************************************


void U3DAssemble::_addMeshModelData()
{
  if (!_inWEM)
  {
    return;
  }

  WEMPtr tempWEM = NULL;
  ML_CHECK_NEW(tempWEM, WEM());

  MLuint32 meshNumber = 0;
  bool simpleMode = _simpleModeMeshFld->getBoolValue();
  mlU3D::StringVector meshSpecificationsVector;

  if (simpleMode)
  {
    meshSpecificationsVector = mlU3D::U3DTools::getMeshSpecificationsStringFromWEM(_inWEM);
  }
  else
  {
    meshSpecificationsVector = mlU3D::U3DTools::getModelSpecificationsStringFromUI(_meshSpecificationFld, "<Mesh>");
  }

  const MLuint32 numberOfMeshSpecifications = (MLuint32)meshSpecificationsVector.size();

  if (0 == numberOfMeshSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing  mesh specification and input WEM.");

  mlU3D::GeometryGeneratorMap meshGeometryGeneratorMap;

  // *****************************
  // Generate model specifications
  // *****************************

  std::string prefix = _modelPrefixMeshesFld->getStringValue();

  if ("" == prefix)  // Just in case the field was cleared...
  {
    prefix = "Mesh";
  }

  for (MLuint32 i = 0; i < numberOfMeshSpecifications; i++)
  {
    Vector3 thisModelSpecularColor;
    Vector4 thisModelDiffuseColor;
    bool    thisModelUseVertexColors;

    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(meshSpecificationsVector[i]);
    thisSpecificationParameters.ObjectType = mlU3D::MODELTYPE_MESH;

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = _outU3DObject->defaultValues.defaultMeshPefix + " " + mlU3D::intToString((int)(1 + _outU3DObject->getNumModels(_outU3DObject->defaultValues.defaultMeshPefix)));

    }

    // Create model node
    mlU3D::ModelNode newModelNode = mlU3D::U3DTools::createNewModelNode(thisSpecificationParameters, _outU3DObject);

    std::string generatorKey = mlU3D::U3DTools::updateGeometryGeneratorMap(meshGeometryGeneratorMap, thisSpecificationParameters.WEMLabel, mlU3D::GEOMETRYPREFIX_MESH);
    newModelNode.geometryGeneratorName = meshGeometryGeneratorMap[generatorKey].GeometryName;
    _outU3DObject->modelNodes.push_back(newModelNode);

    // Set colors
    if (thisSpecificationParameters.Color == mlU3D::USEVERTEXCOLORS)
    {
      thisModelUseVertexColors = true;
      thisModelDiffuseColor = _outU3DObject->defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisModelUseVertexColors = false;
      thisModelDiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters.Color, _outU3DObject->defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisModelSpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, _outU3DObject->defaultValues.defaultMaterialSpecularColor);

    if (thisSpecificationParameters.Opacity != "")
    {
      thisModelDiffuseColor[3] = mlU3D::stringToMLDouble(thisSpecificationParameters.Opacity);
    }

    // Add shader for mesh
    mlU3D::LitTextureShader thisMeshShader;
    thisMeshShader.resourceName         = newModelNode.shaderName;
    thisMeshShader.materialResourceName = "Material for " + newModelNode.internalName;
    _outU3DObject->litTextureShaders.push_back(thisMeshShader);

    // Add material for mesh
    mlU3D::MaterialResource thisMeshMaterialResource;
    thisMeshMaterialResource.resourceName  = "Material for " + newModelNode.internalName;
    thisMeshMaterialResource.ambientColor  = _outU3DObject->defaultValues.defaultMaterialAmbientColor;
    thisMeshMaterialResource.diffuseColor  = thisModelDiffuseColor;
    thisMeshMaterialResource.specularColor = thisModelSpecularColor;
    thisMeshMaterialResource.emissiveColor = _outU3DObject->defaultValues.defaultMaterialEmissiveColor;
    _outU3DObject->materialResources.push_back(thisMeshMaterialResource);
  }

  // ****************************
  // Generate geometry generators
  // ****************************

  for (mlU3D::GeometryGeneratorMap::const_iterator mapIterator = meshGeometryGeneratorMap.begin(); mapIterator != meshGeometryGeneratorMap.end(); mapIterator++)
  {
    std::string wemLabel     = (*mapIterator).first;
    std::string geometryName = ((*mapIterator).second).GeometryName;
    bool useVertexColors     = ((*mapIterator).second).UseVertexColors;


    // Search for correct WEM patch, triangulate it if necessary, creeate a copy and add it to mesh generator resource
    const MLuint32 numberOfInWEMPatches = _inWEM->getNumWEMPatches();

    for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)
    {
      WEMPatch* thisInWEMPatch = _inWEM->getWEMPatchAt(i);
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
          trianglePatch->computeBoundingBox();
          ML_DELETE(triangulatedPatch);
        }
        else
        {
          trianglePatch = tempWEM->addWEMPatchCopy(reinterpret_cast<WEMTrianglePatch*>(thisInWEMPatch));
          trianglePatch->computeBoundingBox();
        }


        if (trianglePatch)
        {
          // Adjust properties of main WEM bounding box
          WEMBoundingBox* thisWEMPatchBoundingBox = trianglePatch->getBoundingBox();
          mlU3D::ModelBoundingBoxStruct newBoundingBox;
          newBoundingBox.start = thisWEMPatchBoundingBox->getMin();
          newBoundingBox.end = thisWEMPatchBoundingBox->getMax();
          mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newBoundingBox);

          // Add generator for mesh
          mlU3D::CLODMeshGenerator thisMeshGenerator;
          thisMeshGenerator.resourceName       = geometryName;
          thisMeshGenerator.diffuseColorCount  = (useVertexColors ? _getNumberOfDifferentColorsFromWEMPatch(trianglePatch) : 0);
          thisMeshGenerator.specularColorCount = (useVertexColors ? 1 : 0);
          thisMeshGenerator.textureCoordCount  = 0;    // This is not really needed in this version
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
          _outU3DObject->meshes.push_back(thisMeshGenerator);

          break;
        }

      }

    } // for (MLuint32 i = 0; i < numberOfInWEMPatches; i++)

  }

  tempWEM = NULL;
}


//***********************************************************************************


// Scan all node colors of a WEM patch and return number of different colors.
MLuint32 U3DAssemble::_getNumberOfDifferentColorsFromWEMPatch(WEMTrianglePatch* patch) const
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


// Writes all vertex colors of a WEM patch to the continuation block
mlU3D::ColorMap U3DAssemble::_writeVertexColors(WEMPatch* patch, U3DDataBlockWriter& continuationBlock) const
{
  MLuint thisColorIndex = 0;
  mlU3D::ColorMap colorsMap;   // <Color,ColorIndex>

  const int numNodesInPatch = patch->getNumNodes();

  for (int n = 0; n < numNodesInPatch; n++)
  {
    WEMNode* thisNode = patch->getNodeAt(n);

    const Vector4 thisNodeColor = thisNode->getColor();

    mlU3D::ColorMap::iterator findIt = colorsMap.find(thisNodeColor);

    if (findIt == colorsMap.end())
    {
      // Color has not yet been added to map, so add it now
      colorsMap[thisNodeColor] = thisColorIndex;
      continuationBlock.writeF32Color(thisNodeColor);
      thisColorIndex++;
    }
    else
    {
      // Do nothing
    }

  }

  return colorsMap;
}


//***********************************************************************************


ML_END_NAMESPACE