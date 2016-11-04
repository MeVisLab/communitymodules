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
#include "shared/MLU3D_Tools.h"
#include "shared/MLU3D_MarkerListTools.h"
#include "shared/MLU3D_SpecificationGenerator.h"


ML_START_NAMESPACE


//***********************************************************************************


void U3DAssemble::_preProcessPointSetData()
{
  mlU3D::StringVector pointSetSpecificationsVector;

  if (_simpleModePointSetFld->getBoolValue())
  {
    pointSetSpecificationsVector.push_back("<PointSet><PositionTypes>all</PositionTypes></PointSet>");
  }
  else
  {
    pointSetSpecificationsVector = mlU3D::U3DTools::getObjectSpecificationsStringFromUI(_pointCloudSpecificationFld, "<PointSet>");
  }

  const MLuint32 numberOfPointSetSpecifications = (MLuint32)pointSetSpecificationsVector.size();

  if (0 == numberOfPointSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing point cloud specification.");

  for (MLuint32 i = 0; i < numberOfPointSetSpecifications; i++)
  {
    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(pointSetSpecificationsVector[i]);

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = "Point Set " + mlU3D::intToString(i + 1);
    }

    ObjectInfoStruct thisU3DObjectInfo = _createNewU3DObjectInfo(i, MODELTYPE_POINTSET, thisSpecificationParameters.ObjectName, defaultValues);
    thisU3DObjectInfo.GroupPath = thisSpecificationParameters.GroupPath;

    if (thisSpecificationParameters.Color == mlU3D::USEVERTEXCOLORS)
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for point sets
      thisU3DObjectInfo.DiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for point sets
      thisU3DObjectInfo.DiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisU3DObjectInfo.SpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);

    thisU3DObjectInfo.DiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    thisU3DObjectInfo.Visibility = (MLuint32)mlU3D::stringToInt(thisSpecificationParameters.ModelVisibility);
    _allU3DObjects.push_back(thisU3DObjectInfo);

    // Collect all position coordinates
    mlU3D::PositionsVector thisPointSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(_inPointPositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);
    mlU3D::ModelBoundingBoxStruct newboundingBox = mlU3D::U3DTools::GetBoundingBoxFomPositions(thisPointSetPositions);
    mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newboundingBox);

    // Add shader for point set
    mlU3D::LitTextureShader thisPointSetShader;
    thisPointSetShader.resourceName = thisU3DObjectInfo.ShaderName;
    thisPointSetShader.materialResourceName = thisU3DObjectInfo.MaterialName;
    _outU3DObject->litTextureShaders.push_back(thisPointSetShader);

    // Add material for point set
    mlU3D::MaterialResource thisPointSetMaterialResource;
    thisPointSetMaterialResource.resourceName = thisU3DObjectInfo.MaterialName;
    thisPointSetMaterialResource.ambientColor = thisU3DObjectInfo.AmbientColor;
    thisPointSetMaterialResource.diffuseColor = thisU3DObjectInfo.DiffuseColor;
    thisPointSetMaterialResource.specularColor = thisU3DObjectInfo.SpecularColor;
    thisPointSetMaterialResource.emissiveColor = thisU3DObjectInfo.EmissiveColor;
    _outU3DObject->materialResources.push_back(thisPointSetMaterialResource);

    // Add generator for point set
    mlU3D::PointSetGenerator thisPointSetGenerator;
    thisPointSetGenerator.diffuseColorCount    = 0;    // This is not really needed in this version
    thisPointSetGenerator.specularColorCount   = 0;    // This is not really needed in this version
    thisPointSetGenerator.textureCoordCount    = 0;    // This is not really needed in this version
    thisPointSetGenerator.defaultAmbientColor  = defaultValues.defaultMaterialAmbientColor;
    thisPointSetGenerator.defaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
    thisPointSetGenerator.defaultDiffuseColor  = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    thisPointSetGenerator.defaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;
    thisPointSetGenerator.pointCount           = (MLuint32)thisPointSetPositions.size();
    thisPointSetGenerator.normalCount          = 0;    // No normals in this version since normals are not used by Acrobat
    thisPointSetGenerator.shadingAttributes    = mlU3D::SHADINGATTRIBUTES_NONE;
    thisPointSetGenerator.shadingAttributes   |= ((thisPointSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisPointSetGenerator.shadingAttributes   |= ((thisPointSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisPointSetGenerator.internalName         = thisU3DObjectInfo.InternalName;
    thisPointSetGenerator.displayName          = thisU3DObjectInfo.DisplayName;
    thisPointSetGenerator.resourceName         = thisU3DObjectInfo.ResourceName;
    thisPointSetGenerator.materialName         = thisU3DObjectInfo.MaterialName;
    thisPointSetGenerator.shaderName           = thisU3DObjectInfo.ShaderName;
    thisPointSetGenerator.positions            = thisPointSetPositions;
    _outU3DObject->pointSets.push_back(thisPointSetGenerator);

  }

}


//***********************************************************************************


void U3DAssemble::_preProcessLineSetData()
{  
  mlU3D::StringVector lineSetSpecificationsVector;

  if (_simpleModeLineSetFld->getBoolValue())
  {
    lineSetSpecificationsVector.push_back("<LineSet><PositionTypes>all</PositionTypes><ConnectionTypes>all</ConnectionTypes></LineSet>");
  }
  else
  {
    lineSetSpecificationsVector = mlU3D::U3DTools::getObjectSpecificationsStringFromUI(_lineSetSpecificationFld, "<LineSet>");
  }
       
  const MLuint32 numberOfLineSetSpecifications = (MLuint32)lineSetSpecificationsVector.size();  

  if (0 == numberOfLineSetSpecifications)
  {
    return;
  }

  _statusFld->setStringValue("Analyzing line set specification.");

  for (MLuint32 i = 0; i < numberOfLineSetSpecifications; i++)
  {
    mlU3D::SpecificationParametersStruct thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(lineSetSpecificationsVector[i]);

    // Override default object name
    if (thisSpecificationParameters.ObjectName == "Object")
    {
      thisSpecificationParameters.ObjectName = "Line Set " + mlU3D::intToString(i+1);
    }

    ObjectInfoStruct thisU3DObjectInfo = _createNewU3DObjectInfo(i, MODELTYPE_LINESET, thisSpecificationParameters.ObjectName, defaultValues);
    thisU3DObjectInfo.GroupPath = thisSpecificationParameters.GroupPath;

    if (thisSpecificationParameters.Color == mlU3D::USEVERTEXCOLORS)
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for line sets
      thisU3DObjectInfo.DiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    }
    else
    {
      thisU3DObjectInfo.UseVertexColors = false; // Never use vertex colors for line sets
      thisU3DObjectInfo.DiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
    }

    thisU3DObjectInfo.SpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);

    thisU3DObjectInfo.DiffuseColor[3] = 1; // Make sure that opacity is set to opaque

    thisU3DObjectInfo.Visibility    = (MLuint32)mlU3D::stringToInt(thisSpecificationParameters.ModelVisibility);
    _allU3DObjects.push_back(thisU3DObjectInfo);

    // Collect all position coordinates
    mlU3D::PositionsVector thisLineSetPositions = mlU3D::U3DMarkerListTools::getAllPositionsFromXMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.PointSize);
    mlU3D::ModelBoundingBoxStruct newboundingBox = mlU3D::U3DTools::GetBoundingBoxFomPositions(thisLineSetPositions);
    mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newboundingBox);

    // Collect all line connections
    mlU3D::LinesVector thisLineSetLines;
    if ((_inLineConnections.size() > 0) && (thisSpecificationParameters.ConnectionTypes != "simple"))
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getAllLinesFromIndexPairList(_inLineConnections, thisSpecificationParameters.ConnectionTypes, thisSpecificationParameters.LineWidth);
    }
    else
    {
      thisLineSetLines = mlU3D::U3DMarkerListTools::getStandardLinesFromXMarkerList(_inLinePositions, thisSpecificationParameters.PositionTypes, thisSpecificationParameters.LineWidth);
    }

    // Add shader for point set
    mlU3D::LitTextureShader thisLineSetShader;
    thisLineSetShader.resourceName = thisU3DObjectInfo.ShaderName;
    thisLineSetShader.materialResourceName = thisU3DObjectInfo.MaterialName;
    _outU3DObject->litTextureShaders.push_back(thisLineSetShader);

    // Add material for point set
    mlU3D::MaterialResource thisLineSetMaterialResource;
    thisLineSetMaterialResource.resourceName = thisU3DObjectInfo.MaterialName;
    thisLineSetMaterialResource.ambientColor = thisU3DObjectInfo.AmbientColor;
    thisLineSetMaterialResource.diffuseColor = thisU3DObjectInfo.DiffuseColor;
    thisLineSetMaterialResource.specularColor = thisU3DObjectInfo.SpecularColor;
    thisLineSetMaterialResource.emissiveColor = thisU3DObjectInfo.EmissiveColor;
    _outU3DObject->materialResources.push_back(thisLineSetMaterialResource);

    // Add generator for line set
    mlU3D::LineSetGenerator thisLineSetGenerator;
    thisLineSetGenerator.diffuseColorCount    = 0;     // This is not really needed in this version
    thisLineSetGenerator.specularColorCount   = 0;     // This is not really needed in this version
    thisLineSetGenerator.textureCoordCount    = 0;     // This is not really needed in this version
    thisLineSetGenerator.defaultAmbientColor  = defaultValues.defaultMaterialAmbientColor;
    thisLineSetGenerator.defaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
    thisLineSetGenerator.defaultDiffuseColor  = defaultValues.defaultMaterialDiffuseColorWithTransparency;
    thisLineSetGenerator.defaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;
    thisLineSetGenerator.pointCount           = (MLuint32)thisLineSetPositions.size();
    thisLineSetGenerator.lineCount            = (MLuint32)thisLineSetLines.size();
    thisLineSetGenerator.normalCount          = 0;     // No normals in this version since normals are not used by Acrobat
    thisLineSetGenerator.shadingAttributes    = mlU3D::SHADINGATTRIBUTES_NONE;
    thisLineSetGenerator.shadingAttributes   |= ((thisLineSetGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);   // Should not happen in this version
    thisLineSetGenerator.shadingAttributes   |= ((thisLineSetGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0); // Should not happen in this version
    thisLineSetGenerator.internalName         = thisU3DObjectInfo.InternalName;
    thisLineSetGenerator.displayName          = thisU3DObjectInfo.DisplayName;
    thisLineSetGenerator.resourceName         = thisU3DObjectInfo.ResourceName;
    thisLineSetGenerator.materialName         = thisU3DObjectInfo.MaterialName;
    thisLineSetGenerator.shaderName           = thisU3DObjectInfo.ShaderName;
    thisLineSetGenerator.positions            = thisLineSetPositions;
    thisLineSetGenerator.lines                = thisLineSetLines;
    _outU3DObject->lineSets.push_back(thisLineSetGenerator);
  }
  
}


//***********************************************************************************


void U3DAssemble::_preProcessMeshData(WEMPtr saveWEM)
{
  if (!_inWEM)
  {
    return;
  }

  MLuint32 meshNumber = 0;

  bool simpleMode = _simpleModeMeshFld->getBoolValue();

  mlU3D::StringVector meshSpecificationsVector;

  if (simpleMode)
  {
    //meshSpecificationsVector.push_back("<U3DMesh>");
  }
  else
  {
    meshSpecificationsVector = mlU3D::U3DTools::getObjectSpecificationsStringFromUI(_meshSpecificationFld, "<Mesh>");
  }

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  WEMPatch*           wemPatch = NULL;
  WEMTrianglePatch*   addedTrianglePatch = NULL;

  _statusFld->setStringValue("Analyzing mesh specification and input WEM.");

  const MLuint32 numberOfWEMPatches = _inWEM->getNumWEMPatches();

  for (MLuint32 i = 0; i < numberOfWEMPatches; i++)
  {
    wemPatch = _inWEM->getWEMPatchAt(i);
    addedTrianglePatch = NULL;
    const unsigned int newId = saveWEM->getCurrentWEMPatchId();

    if (wemPatch->getNumFaces() > 0)
    {

      if (wemPatch->getPatchType() != WEM_PATCH_TRIANGLES)
      {
        WEMTrianglePatch* triangulatedPatch = NULL;

        ML_CHECK_NEW(triangulatedPatch, WEMTrianglePatch());
        wemPatch->triangulate(triangulatedPatch, WEM_TRIANGULATION_STRIP);
        addedTrianglePatch = saveWEM->addWEMPatchCopy(triangulatedPatch);
        addedTrianglePatch->computeBoundingBox();
        ML_DELETE(triangulatedPatch);
      }
      else
      {
        addedTrianglePatch = saveWEM->addWEMPatchCopy(reinterpret_cast<WEMTrianglePatch*>(wemPatch));
      }

      if (addedTrianglePatch != NULL)
      {
        addedTrianglePatch->setId(newId);

        // Adjust properties of main WEM bounding box
        WEMBoundingBox* thisWEMPatchBoundingBox = addedTrianglePatch->getBoundingBox();
        mlU3D::ModelBoundingBoxStruct newboundingBox;
        newboundingBox.start = thisWEMPatchBoundingBox->getMin();
        newboundingBox.end = thisWEMPatchBoundingBox->getMax();
        mlU3D::U3DTools::UpdateBoundingBox(_outU3DObject->modelBoundingBox, newboundingBox);

        std::string wemDescription = addedTrianglePatch->getDescription();
        std::string wemLabel = addedTrianglePatch->getLabel();

        mlU3D::SpecificationParametersStruct thisSpecificationParameters;

        // Create an artificial meshSpecificationVector if only WEM label & description shall be used 
        if (simpleMode)
        {
          meshSpecificationsVector.clear();

          // Parse WEM label & description...
          std::string u3dModelName = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "ModelName");
          std::string u3dGroupName = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "GroupName");
          std::string u3dGroupPath = mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "GroupPath");

          if ("" != u3dGroupName)
          {
            if ("" == u3dGroupPath)
            {
              u3dGroupPath += "/";
            }
            u3dGroupPath += u3dGroupName + "/";
          }

          std::string displayName = wemLabel;
          if (displayName == "") {
            displayName = "Mesh " + mlU3D::intToString(i + 1);
          }

          // ...and write data into meshSpecification string
          std::string meshSpecificationsString = "<Mesh>";
          meshSpecificationsString += "<WEMLabel>" + wemLabel + "</WEMLabel>";
          meshSpecificationsString += "<ObjectName>" + displayName + "</ObjectName>";
          meshSpecificationsString += "<GroupPath>" + u3dGroupPath + "</GroupPath>";
          meshSpecificationsString += "<Color>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "Color") + "</Color>";
          meshSpecificationsString += "<SpecularColor>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "SpecularColor") + "</SpecularColor>";
          meshSpecificationsString += "<Opacity>" + mlU3D::U3DTools::getSpecificParameterFromWEMDescription(wemDescription, "Opacity") + "</Opacity>";
          meshSpecificationsString += "<ModelVisibility>3</ModelVisibility>";
          meshSpecificationsString += "</Mesh>";

          // Add meshSpecification string to meshSpecificationVector
          meshSpecificationsVector.push_back(meshSpecificationsString);
        }

        for (size_t thisSpecificationIndex = 0; thisSpecificationIndex < meshSpecificationsVector.size(); thisSpecificationIndex++)
        {
          thisSpecificationParameters = mlU3D::U3DTools::getAllSpecificationParametersFromString(meshSpecificationsVector[thisSpecificationIndex]);
          if (thisSpecificationParameters.WEMLabel == wemLabel)
          {
            ObjectInfoStruct thisU3DObjectInfo = _createNewU3DObjectInfo(i, MODELTYPE_MESH, thisSpecificationParameters.ObjectName, defaultValues);

            thisU3DObjectInfo.GroupPath = thisSpecificationParameters.GroupPath;

            if (thisSpecificationParameters.Color == mlU3D::USEVERTEXCOLORS)
            {
              thisU3DObjectInfo.UseVertexColors = true;
              thisU3DObjectInfo.DiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
            }
            else
            {
              thisU3DObjectInfo.UseVertexColors = false;
              thisU3DObjectInfo.DiffuseColor = mlU3D::U3DTools::getColorVec4FromString(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
            }

            thisU3DObjectInfo.SpecularColor = mlU3D::U3DTools::getColorVec3FromString(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);

            if (thisSpecificationParameters.Opacity != "")
            {
              thisU3DObjectInfo.DiffuseColor[3] = mlU3D::stringToMLDouble(thisSpecificationParameters.Opacity);
            }

            thisU3DObjectInfo.Visibility = (MLuint32)mlU3D::stringToInt(thisSpecificationParameters.ModelVisibility);
            _allU3DObjects.push_back(thisU3DObjectInfo);

            // Add shader for mesh
            mlU3D::LitTextureShader thisMeshShader;
            thisMeshShader.resourceName = thisU3DObjectInfo.ShaderName;
            thisMeshShader.materialResourceName = thisU3DObjectInfo.MaterialName;
            _outU3DObject->litTextureShaders.push_back(thisMeshShader);

            // Add material for mesh
            mlU3D::MaterialResource thisMeshMaterialResource;
            thisMeshMaterialResource.resourceName = thisU3DObjectInfo.MaterialName;
            thisMeshMaterialResource.ambientColor = thisU3DObjectInfo.AmbientColor;
            thisMeshMaterialResource.diffuseColor = thisU3DObjectInfo.DiffuseColor;
            thisMeshMaterialResource.specularColor = thisU3DObjectInfo.SpecularColor;
            thisMeshMaterialResource.emissiveColor = thisU3DObjectInfo.EmissiveColor;
            _outU3DObject->materialResources.push_back(thisMeshMaterialResource);

            // Add generator for mesh
            mlU3D::CLODMeshGenerator thisMeshGenerator;
            thisMeshGenerator.diffuseColorCount = (thisU3DObjectInfo.UseVertexColors ? _getNumberOfDifferentColorsFromWEMPatch(addedTrianglePatch) : 0);
            thisMeshGenerator.specularColorCount = (thisU3DObjectInfo.UseVertexColors ? 1 : 0);
            thisMeshGenerator.textureCoordCount = 0;    // This is not really needed in this version
            thisMeshGenerator.defaultAmbientColor = defaultValues.defaultMaterialAmbientColor;
            thisMeshGenerator.defaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
            thisMeshGenerator.defaultDiffuseColor = defaultValues.defaultMaterialDiffuseColorWithTransparency;
            thisMeshGenerator.defaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;
            thisMeshGenerator.faceCount = addedTrianglePatch->getNumFaces();
            thisMeshGenerator.normalCount = addedTrianglePatch->getNumNodes();
            thisMeshGenerator.vertexCount = addedTrianglePatch->getNumNodes();
            thisMeshGenerator.patchID = addedTrianglePatch->getId();
            thisMeshGenerator.meshAttributes = mlU3D::MESH_ATTRIBUTES_DEFAULT;
            thisMeshGenerator.meshAttributes |= ((thisMeshGenerator.normalCount == 0) ? mlU3D::MESH_ATTRIBUTES_EXCLUDENORMALS : 0);
            thisMeshGenerator.shadingAttributes = mlU3D::SHADINGATTRIBUTES_NONE;
            thisMeshGenerator.shadingAttributes |= ((thisMeshGenerator.diffuseColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_DIFFUSECOLORS : 0);
            thisMeshGenerator.shadingAttributes |= ((thisMeshGenerator.specularColorCount > 0) ? mlU3D::SHADINGATTRIBUTES_SPECULARCOLORS : 0);
            thisMeshGenerator.internalName = thisU3DObjectInfo.InternalName;
            thisMeshGenerator.displayName = thisU3DObjectInfo.DisplayName;
            thisMeshGenerator.resourceName = thisU3DObjectInfo.ResourceName;
            thisMeshGenerator.materialName = thisU3DObjectInfo.MaterialName;
            thisMeshGenerator.shaderName = thisU3DObjectInfo.ShaderName;
            thisMeshGenerator.meshNumber = meshNumber++;
            thisMeshGenerator.wem = saveWEM;
            _outU3DObject->meshes.push_back(thisMeshGenerator);
          }
        }

      }  // if (addedTrianglePatch != NULL)

    }  // if (wemPatch->getNumFaces() > 0)

    wemPatch = NULL;
  }

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