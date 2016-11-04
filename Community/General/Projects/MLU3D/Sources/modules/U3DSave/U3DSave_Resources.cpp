//----------------------------------------------------------------------------------
// U3DSave module.
//
// \file    U3DSave_Resources.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Saves a U3D to disk.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DSave.h"
#include "shared/U3DInternalFormat/U3D_Object_Resources.h"


ML_START_NAMESPACE


//***********************************************************************************


void U3DSave::_addLightResources()
{
  mlU3D::LightResourceVector allLightResources = _inU3DObject->lightResources;

  for (size_t thisResourceListIndex = 0; thisResourceListIndex < allLightResources.size(); thisResourceListIndex++)
  {
    mlU3D::LightResource thisLightResource = allLightResources[thisResourceListIndex];
    _statusFld->setStringValue("Adding Light Resource: " + thisLightResource.resourceName + ".");

    _outU3DFile->addStandardBlock_LightResource(thisLightResource);
  }
}


//***********************************************************************************


void U3DSave::_addViewResources()
{
  mlU3D::ViewResourceVector allViewResources = _inU3DObject->viewResources;

  for (size_t thisResourceListIndex = 0; thisResourceListIndex < allViewResources.size(); thisResourceListIndex++)
  {
    mlU3D::ViewResource thisViewResource = allViewResources[thisResourceListIndex];
    _statusFld->setStringValue("Adding View Resource: " + thisViewResource.resourceName + ".");

    _outU3DFile->addStandardBlock_ViewResource(thisViewResource);
  }
}


//***********************************************************************************


void U3DSave::_addShaders()
{
  mlU3D::LitTextureShaderVector allShaders = _inU3DObject->litTextureShaders;

  for (size_t thisResourceListIndex = 0; thisResourceListIndex < allShaders.size(); thisResourceListIndex++)
  {
    mlU3D::LitTextureShader thisShader = allShaders[thisResourceListIndex];
    _statusFld->setStringValue("Adding Shader: " + thisShader.resourceName + ".");

    _outU3DFile->addStandardBlock_LitTextureShader(thisShader);
  }
}


//***********************************************************************************


void U3DSave::_addMaterialResources()
{
  mlU3D::MaterialResourceVector allMaterialResources = _inU3DObject->materialResources;

  for (size_t thisResourceListIndex = 0; thisResourceListIndex < allMaterialResources.size(); thisResourceListIndex++)
  {
    mlU3D::MaterialResource thisMaterialResource = allMaterialResources[thisResourceListIndex];
    _statusFld->setStringValue("Adding Material Resource: " + thisMaterialResource.resourceName + ".");

    _outU3DFile->addStandardBlock_MaterialResource(thisMaterialResource);
  }
}



//***********************************************************************************


void U3DSave::_addMotionResources()
{
  // Motion resources are not supported in this version.
}



//***********************************************************************************


ML_END_NAMESPACE