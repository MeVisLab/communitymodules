//----------------------------------------------------------------------------------
//! Methods for processing WEMs for a PRC file.
/*!
// \file    SavePRC_ProcessMeshes.cpp
// \author  Axel Newe
// \date    2015-08-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "PRCFileFormat/PRC_DataTypes.h"
#include "PRCFileFormat/PRC_Tools.h"
#include "shared/MLPDF_Tools.h"

ML_START_NAMESPACE


//***********************************************************************************


void SavePRC::PreProcessMeshData(WEMPtr saveWEM, 
							                   PRCMeshInfoVector &meshInfoVector,
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
    PRCMeshInfoStruct   thisWEMMeshInfo;

    wemPatch = _inWEM->getWEMPatchAt(i); 
    addedTrianglePatch = NULL;
    const unsigned int newId = saveWEM->getCurrentWEMPatchId();

    if (wemPatch->getPatchType() != WEM_PATCH_TRIANGLES)
    {
      WEMTrianglePatch* triangulatedPatch = NULL;

      ML_CHECK_NEW(triangulatedPatch,WEMTrianglePatch());
      wemPatch->triangulate(triangulatedPatch, WEM_TRIANGULATION_STRIP);
      addedTrianglePatch = saveWEM->addWEMPatchCopy(triangulatedPatch);
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
          PRCObjectInfoStruct thisPRCObjectInfo = CreateNewPRCObjectInfo(i,PRCOBJECTTYPE_MESH, thisSpecificationParameters.ObjectName, defaultValues);
          thisPRCObjectInfo.GroupPath        = thisSpecificationParameters.GroupPath;
          thisPRCObjectInfo.ParentTreeNodeID = -1;
          thisPRCObjectInfo.RGBAColor        = getColorVec4(thisSpecificationParameters.Color, Vector4(0));  // If alpha = 0 -> Adobe doesn't render;
          _prcObjectInfoVector.push_back(thisPRCObjectInfo);



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
//          thisWEMMeshInfo.MeshAttributes = U3D_MESH_ATTRIBUTES_DEFAULT;
//          thisWEMMeshInfo.MeshAttributes |= ( (thisWEMMeshInfo.NormalCount == 0) ? U3D_MESH_ATTRIBUTES_EXCLUDENORMALS : 0 );
//          thisWEMMeshInfo.ShadingAttributes = U3D_SHADINGATTRIBUTES_NONE;  
//          thisWEMMeshInfo.ShadingAttributes |= ( (thisWEMMeshInfo.DiffuseColorCount > 0) ? U3D_SHADINGATTRIBUTES_DIFFUSECOLORS : 0 );   // Should not happen in this version
//          thisWEMMeshInfo.ShadingAttributes |= ( (thisWEMMeshInfo.SpecularColorCount > 0) ? U3D_SHADINGATTRIBUTES_SPECULARCOLORS : 0 ); // Should not happen in this version
//          thisWEMMeshInfo.ResourceName = thisPRCObjectInfo.ResourceName;
          thisWEMMeshInfo.MeshNumber = meshNumber++;

	        meshInfoVector.push_back(thisWEMMeshInfo);           
        }
      }

    }

    wemPatch = NULL;
  }

}

//***********************************************************************************

void SavePRC::WriteMeshDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode)
{
}

//***********************************************************************************
/*
std::string SavePRC::getPRCParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult) const
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
*/
//***********************************************************************************




ML_END_NAMESPACE