//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_Resources.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Object_Resources.h"
#include "MLU3D_Constants.h"


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class ResourceBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::ResourceBase, RefCountedBase);

  //----------------------------------------------------------------------------------

  ResourceBase::ResourceBase() : RefCountedBase()
  {
    resourceName = "";
  }

  //----------------------------------------------------------------------------------

  ResourceBase::~ResourceBase()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class LightResource: Represents a Light Resource
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::LightResource, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  LightResource::LightResource() : ResourceBase()
  {
    lightType        = mlU3D::LIGHTTYPE_AMBIENT;
    lightColor       = Vector3(1.0f, 1.0f, 1.0f);
    lightAttenuation = Vector3(1.0f, 0.0f, 0.0f);
    lightSpotAngle   = 180.0f;
    lightIntensity   = 1.0f;
  }

  //----------------------------------------------------------------------------------

  LightResource::~LightResource()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class ViewResource: Represents a View Resource
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::ViewResource, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  ViewResource::ViewResource() : ResourceBase()
  {
  }

  //----------------------------------------------------------------------------------

  ViewResource::~ViewResource()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class LitTextureShader: Represents a Lit Texture Shader
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::LitTextureShader, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  LitTextureShader::LitTextureShader() : ResourceBase()
  {
    materialResourceName = "";
  }

  //----------------------------------------------------------------------------------

  LitTextureShader::~LitTextureShader()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class MaterialResource: Represents a Material Resource
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::MaterialResource, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  MaterialResource::MaterialResource() : ResourceBase()
  {
    ambientColor  = Vector3(1.0);
    diffuseColor  = Vector4(0.65f, 0.65f, 0.65f, 1.0f);
    specularColor = Vector3(0.75f);
    emissiveColor = Vector3(0.0f);

  }

  //----------------------------------------------------------------------------------

  MaterialResource::~MaterialResource()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class TextureResource: Represents a Texture Resource
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::TextureResource, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  TextureResource::TextureResource() : ResourceBase()
  {
  }

  //----------------------------------------------------------------------------------

  TextureResource::~TextureResource()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class MotionResource: Represents a Motion Resource
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::MotionResource, mlU3D::ResourceBase);

  //----------------------------------------------------------------------------------

  MotionResource::MotionResource() : ResourceBase()
  {
  }

  //----------------------------------------------------------------------------------

  MotionResource::~MotionResource()
  {
  }

  //----------------------------------------------------------------------------------




} // end namespace mlU3D

ML_END_NAMESPACE
