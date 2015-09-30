//----------------------------------------------------------------------------------
//! Methods for processing WEMs for a U3D file.
/*!
// \file    SaveU3D_ProcessMeshes.cpp
// \author  Axel Newe
// \date    2014-01-20
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SaveU3D.h"
#include "U3DFileFormat/U3D_DataTypes.h"
#include "U3DFileFormat/U3D_Tools.h"
#include "MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************


void SaveU3D::PreProcessMeshData(WEMPtr saveWEM, 
							                   U3DMeshInfoVector &meshInfoVector,
                                 ModelBoundingBoxStruct& boundingBox)
{
  if (!_inWEM) 
  { 
    return; 
  }

  MLuint32 meshNumber = 0;

  bool simpleMode = _simpleModeMeshFld->getBoolValue();

  StringVector meshSpecificationsVector;

  if (simpleMode)
  {
    //meshSpecificationsVector.push_back("<U3DMesh>");
  }
  else
  {
    meshSpecificationsVector = getObjectSpecificationsStringFromUI(_meshSpecificationFld, "<Mesh>");
  }

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  WEMPatch*           wemPatch = NULL;
  WEMTrianglePatch*   addedTrianglePatch = NULL;

  _statusFld->setStringValue("Analyzing mesh specification and input WEM.");

  const MLuint32 numberOfWEMPatches = _inWEM->getNumWEMPatches();

  for (MLuint32 i = 0; i < numberOfWEMPatches; i++)
  {
    U3DMeshInfoStruct   thisWEMMeshInfo;

    wemPatch = _inWEM->getWEMPatchAt(i); 
    addedTrianglePatch = NULL;
    const unsigned int newId = saveWEM->getCurrentWEMPatchId();

    if (wemPatch->getPatchType() != WEM_PATCH_TRIANGLES)
    {
      WEMTrianglePatch* triangulatedPatch = NULL;

      ML_CHECK_NEW(triangulatedPatch,WEMTrianglePatch());
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
      ModelBoundingBoxStruct newboundingBox;
      newboundingBox.start = thisWEMPatchBoundingBox->getMin();
      newboundingBox.end   = thisWEMPatchBoundingBox->getMax();
      UpdateBoundingBox(boundingBox, newboundingBox);

	    std::string wemDescription = addedTrianglePatch->getDescription();
      std::string wemLabel = addedTrianglePatch->getLabel();

      SpecificationParametersStruct thisSpecificationParameters;

      // Create an artificial meshSpecificationVector if only WEM label & description shall be used 
      if (simpleMode)
      {
        meshSpecificationsVector.clear();

        // Parse WEM label & description...
        std::string u3dModelName       = getSpecificParameterFromWEMDescription(wemDescription, "ModelName");
        std::string u3dGroupName       = getSpecificParameterFromWEMDescription(wemDescription, "GroupName");
        std::string u3dGroupPath       = "";
        if ("" != u3dModelName)
        {
          u3dGroupPath = "/" + u3dModelName + "/";
        }
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
          displayName = "Mesh " + intToString(i+1);
        }

        // ...and write data into meshSpecification string
        std::string meshSpecificationsString = "<U3DMesh>";
        meshSpecificationsString += "<WEMLabel>" + wemLabel + "</WEMLabel>";
        meshSpecificationsString += "<ObjectName>" + displayName + "</ObjectName>";
        meshSpecificationsString += "<GroupPath>" + u3dGroupPath + "</GroupPath>";
        meshSpecificationsString += "<Color>" + getSpecificParameterFromWEMDescription(wemDescription, "Color") + "</Color>";
        meshSpecificationsString += "<SpecularColor>" + getSpecificParameterFromWEMDescription(wemDescription, "SpecularColor") + "</SpecularColor>";
        meshSpecificationsString += "<ModelVisibility>3</ModelVisibility>";

        // Add meshSpecification string to meshSpecificationVector
        meshSpecificationsVector.push_back(meshSpecificationsString);
      }

      for (int thisSpecificationIndex = 0; thisSpecificationIndex < meshSpecificationsVector.size(); thisSpecificationIndex++)
      {
        thisSpecificationParameters = getAllSpecificationParametersFromString(meshSpecificationsVector[thisSpecificationIndex]);
        if (thisSpecificationParameters.WEMLabel == wemLabel)
        {
          U3DObjectInfoStruct thisU3DObjectInfo = CreateNewU3DObjectInfo(i, U3DOBJECTTYPE_MESH, thisSpecificationParameters.ObjectName, defaultValues);

          thisU3DObjectInfo.GroupPath = thisSpecificationParameters.GroupPath;
          thisU3DObjectInfo.DiffuseColor = getColorVec4(thisSpecificationParameters.Color, defaultValues.defaultMaterialDiffuseColorWithTransparency);
          thisU3DObjectInfo.SpecularColor = getColorVec3(thisSpecificationParameters.SpecularColor, defaultValues.defaultMaterialSpecularColor);

          thisU3DObjectInfo.Visibility = (MLuint32)stringToInt(thisSpecificationParameters.ModelVisibility);

          // Collect mesh info
          thisWEMMeshInfo.DiffuseColorCount    = 0;    // This is not really needed in this version
          thisWEMMeshInfo.SpecularColorCount   = 0;    // This is not really needed in this version
          thisWEMMeshInfo.TextureCoordCount    = 0;    // This is not really needed in this version
          thisWEMMeshInfo.DefaultAmbientColor  = defaultValues.defaultMaterialAmbientColor;
          thisWEMMeshInfo.DefaultSpecularColor = defaultValues.defaultMaterialSpecularColor;
          thisWEMMeshInfo.DefaultDiffuseColor  = defaultValues.defaultMaterialDiffuseColorWithTransparency;
          thisWEMMeshInfo.DefaultEmissiveColor = defaultValues.defaultMaterialEmissiveColor;            
          thisWEMMeshInfo.FaceCount = addedTrianglePatch->getNumFaces();
          thisWEMMeshInfo.NormalCount = addedTrianglePatch->getNumFaces();
          thisWEMMeshInfo.VertexCount = addedTrianglePatch->getNumNodes();            
          thisWEMMeshInfo.PatchID = addedTrianglePatch->getId();
          thisWEMMeshInfo.MeshAttributes = U3D_MESH_ATTRIBUTES_DEFAULT;
          thisWEMMeshInfo.MeshAttributes |= ( (thisWEMMeshInfo.NormalCount == 0) ? U3D_MESH_ATTRIBUTES_EXCLUDENORMALS : 0 );
          thisWEMMeshInfo.ShadingAttributes = U3D_SHADINGATTRIBUTES_NONE;  
          thisWEMMeshInfo.ShadingAttributes |= ( (thisWEMMeshInfo.DiffuseColorCount > 0) ? U3D_SHADINGATTRIBUTES_DIFFUSECOLORS : 0 );   // Should not happen in this version
          thisWEMMeshInfo.ShadingAttributes |= ( (thisWEMMeshInfo.SpecularColorCount > 0) ? U3D_SHADINGATTRIBUTES_SPECULARCOLORS : 0 ); // Should not happen in this version
          thisWEMMeshInfo.DisplayName  = thisU3DObjectInfo.DisplayName;
          thisWEMMeshInfo.ResourceName = thisU3DObjectInfo.ResourceName;
          thisWEMMeshInfo.MeshNumber = meshNumber++;

	        meshInfoVector.push_back(thisWEMMeshInfo);           
          _u3dObjectInfoVector.push_back(thisU3DObjectInfo);
        }
      }

    }

    wemPatch = NULL;
  }

}


//***********************************************************************************

// Add a mesh declaration modifier chain for each WEM patch
void SaveU3D::AddAllCLODMeshModifierChains(U3DMeshInfoVector meshInfoVector, WEMPtr saveWEM) const
{
  // Add a CLOD mesh declaration modifier chain for each WEM patch
  for (size_t thisMeshIndex = 0; thisMeshIndex < meshInfoVector.size(); thisMeshIndex++)
  {
    U3DMeshInfoStruct thisWEMMeshInfo = meshInfoVector[thisMeshIndex];
    _statusFld->setStringValue("Adding Mesh: " + thisWEMMeshInfo.DisplayName + ".");

    outU3DFile->addModifierChain_CLODMeshDeclaration(thisWEMMeshInfo);

    // Continuation Block shall not be part of modifier chain!
	  U3DDataBlockWriter continuationBlock = CreateCLODBaseMeshContinuationBlock(saveWEM, thisWEMMeshInfo);
    outU3DFile->addDataBlock(continuationBlock);  	
  }

  _progressFld->setFloatValue(0.9f);

}


//***********************************************************************************

U3DDataBlockWriter SaveU3D::CreateCLODBaseMeshContinuationBlock(WEMPtr saveWEM, U3DMeshInfoStruct meshInfo) const
{
  WEMPatch* wemPatch = NULL;
  WEMFace*  wemFace  = NULL;

  float progressStart = 0.3f; 
  float progressEnd   = 0.9f;
  float progressRangeforAllMeshes = progressEnd - progressStart; // 0.6

  float progressIntervalForOneWEMPatch = progressRangeforAllMeshes / _u3dObjectInfoVector.size();
  float progressStartForThisMesh = progressStart + progressIntervalForOneWEMPatch * meshInfo.MeshNumber;  
  _progressFld->setFloatValue(progressStartForThisMesh);
  _statusFld->setStringValue("Assembling data for Mesh: " + meshInfo.DisplayName + ".");

  wemPatch = saveWEM->getWEMPatchById(meshInfo.PatchID);

  U3DDataBlockWriter thisCLODBaseMeshContinuationBlock;
  thisCLODBaseMeshContinuationBlock.blockType = U3D_BLOCKTYPE_CLODBASEMESHCONTINUATION;

  thisCLODBaseMeshContinuationBlock.writeString(meshInfo.ResourceName);       // Write Mesh Name (9.6.1.2.1)
  thisCLODBaseMeshContinuationBlock.writeU32(U3D_ReservedZero);               // Write Chain Index (9.6.1.2.2) (shall be zero) 
   thisCLODBaseMeshContinuationBlock.writeU32(meshInfo.FaceCount);            // Write Base Mesh Description - Face Count (9.6.1.2.3.1) (# of faces)
   thisCLODBaseMeshContinuationBlock.writeU32(meshInfo.VertexCount);          // Write Base Mesh Description - Position Count (9.6.1.2.3.2) (# of vertices)
   thisCLODBaseMeshContinuationBlock.writeU32(meshInfo.NormalCount);          // Write Base Mesh Description - Normal Count (9.6.1.2.3.3) (# of normals)
   thisCLODBaseMeshContinuationBlock.writeU32(meshInfo.DiffuseColorCount);    // Write Base Mesh Description - Diffuse Color Count (9.6.1.2.3.4)
   thisCLODBaseMeshContinuationBlock.writeU32(meshInfo.SpecularColorCount);   // Write Base Mesh Description - Specular Color Count (9.6.1.2.3.5)
   thisCLODBaseMeshContinuationBlock.writeU32(0x00000001);                    // Write Base Mesh Description - Texture Coord Count (9.6.1.2.3.6)

  
  // Write all vertex positions (in U3D, vertices are called "positions")        
  for (MLuint32 thisVertex = 0; thisVertex < meshInfo.VertexCount; thisVertex++)
  {
    Vector3 wemPosition = wemPatch->getNodeAt(thisVertex)->getPosition();

    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[0]);             // Write Base Position - X (9.6.1.2.4.1.1)
    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[1]);             // Write Base Position - Y (9.6.1.2.4.1.2)
    thisCLODBaseMeshContinuationBlock.writeF32(wemPosition[2]);             // Write Base Position - Z (9.6.1.2.4.1.3)
  }

  for (MLuint32 thisNormal = 0; thisNormal < meshInfo.NormalCount; thisNormal++)
  {
    Vector3 faceNormal = wemPatch->getFaceAt(thisNormal)->getNormal();

    thisCLODBaseMeshContinuationBlock.writeF32(faceNormal[0]);              // Write Base Normal - X (9.6.1.2.4.2.1)
    thisCLODBaseMeshContinuationBlock.writeF32(faceNormal[1]);              // Write Base Normal - Y (9.6.1.2.4.2.2)
    thisCLODBaseMeshContinuationBlock.writeF32(faceNormal[2]);              // Write Base Normal - Z (9.6.1.2.4.2.3)
  }

  for (MLuint32 thisDiffuseColor = 0; thisDiffuseColor < meshInfo.DiffuseColorCount; thisDiffuseColor++)
  {
    thisCLODBaseMeshContinuationBlock.writeF32Color(meshInfo.DefaultDiffuseColor);   // Write Base Diffuse Color (9.6.1.2.4.3)
  }

  for (MLuint32 thisSpecularColor = 0; thisSpecularColor < meshInfo.SpecularColorCount; thisSpecularColor++)
  {
    thisCLODBaseMeshContinuationBlock.writeF32Color(meshInfo.DefaultSpecularColor);   // Write Base Specular Color (9.6.1.2.4.4)
  }

  // Write Write Base Texture Coord (9.6.1.2.4.5)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord U (9.6.1.2.4.5.1)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord V (9.6.1.2.4.5.2)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord S (9.6.1.2.4.5.3)
  thisCLODBaseMeshContinuationBlock.writeF32(0.0f);       // Write Base Texture Coord T (9.6.1.2.4.5.4)

  for (MLuint32 thisFace = 0; thisFace < meshInfo.FaceCount; thisFace++)
  {
    wemFace = wemPatch->getFaceAt(thisFace);
	thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3DContext_cShading, 0);       // Write Shading ID (9.6.1.2.4.6.1)
	  
	for (MLuint thisNode = 0; thisNode < 3; thisNode++)
    {
      MLuint32 VertexIndex = wemFace->getNodeAt(static_cast<unsigned int>(thisNode))->getEntryNumber();
      MLuint32 NormalIndex = thisFace;

      // Write Base Corner Info - Base Position Index (9.6.1.2.4.6.2.1)
      thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3DContext_StaticFull+meshInfo.VertexCount, VertexIndex);  
          
      // Write Base Corner Info - Base Normal Index (9.6.1.2.4.6.2.2)
      if (meshInfo.NormalCount > 0)
      {
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3DContext_StaticFull+meshInfo.NormalCount, NormalIndex);
      }

      // Write Base Corner Info - Base Diffuse Color Index (9.6.1.2.4.6.2.3)
      if (meshInfo.DiffuseColorCount > 0) 
      {
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3DContext_StaticFull+meshInfo.DiffuseColorCount, 0);
      }

      // Write Base Corner Info - Base Specular Color Index (9.6.1.2.4.6.2.4)
      if (meshInfo.SpecularColorCount > 0) 
      {
        thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3DContext_StaticFull+meshInfo.SpecularColorCount, 0);
      }
          
      // Write Base Corner Info - Base Texture Coord Index (9.6.1.2.4.6.2.5)
      //thisCLODBaseMeshContinuationBlock.writeCompressedU32(U3D_StaticFull+1, 0);  // No texture layers
    }

    if (0 == (thisFace % 100))   // Set progress field every 100 faces to save GUI update cost
    {
      float progressFldThisFaceValue = ( progressIntervalForOneWEMPatch / meshInfo.FaceCount) * (thisFace + 1);
      float progressFldValue = progressStartForThisMesh + progressFldThisFaceValue; 
      _progressFld->setFloatValue(progressFldValue);
    }

  }

  return thisCLODBaseMeshContinuationBlock;  

}

//***********************************************************************************

void SaveU3D::AddAllCLODBaseMeshContinuationBlocks(WEMPtr saveWEM, U3DMeshInfoVector meshInfoVector) const
{

  size_t meshCount = meshInfoVector.size();

	// Add priority update if necessary
  if (meshCount > 0)
  {
	  outU3DFile->addStandardBlock_PriorityUpdate(0x00000100);
  }

  // Add a CLOD base mesh continuation block for each WEM patch =================================
  for (size_t thisMeshIndex = 0; thisMeshIndex < meshCount; thisMeshIndex++)
  {
    U3DMeshInfoStruct thisWEMMeshInfo = meshInfoVector[thisMeshIndex];

	U3DDataBlockWriter continuationBlock = CreateCLODBaseMeshContinuationBlock(saveWEM, thisWEMMeshInfo);
	outU3DFile->addDataBlock(continuationBlock);  	
  }

}

//***********************************************************************************





ML_END_NAMESPACE