//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_Resources.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_Resources_H
#define _U3D_Object_Resources_H


// Local includes
#include "MLU3DSystem.h"

// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class ResourceBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT ResourceBase : public RefCountedBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    ResourceBase();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~ResourceBase();

    std::string resourceName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(ResourceBase);


  };

  //! Declares the smart pointer U3DObjectPtr from ResourceBase.
  ML_REFCOUNTED_PTR(ResourceBase);


  // ****************************************************************************************************
  //
  // Class LightResource: Represents a Light Resource
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT LightResource : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    LightResource();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~LightResource();

    MLuint8  lightType;
    Vector3  lightColor;
    Vector3  lightAttenuation;
    MLdouble lightSpotAngle;
    MLdouble lightIntensity;


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(LightResource);


  };

  //! Declares the smart pointer U3DObjectPtr from LightResource.
  ML_REFCOUNTED_PTR(LightResource);

  typedef std::vector<LightResource> LightResourceVector;


  // ****************************************************************************************************
  //
  // Class ViewResource: Represents a View Resource
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT ViewResource : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    ViewResource();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~ViewResource();


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(ViewResource);


  };

  //! Declares the smart pointer U3DObjectPtr from ViewResource.
  ML_REFCOUNTED_PTR(ViewResource);

  typedef std::vector<ViewResource> ViewResourceVector;


  // ****************************************************************************************************
  //
  // Class LitTextureShader: Represents a Lit Texture Shader
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT LitTextureShader : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    LitTextureShader();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~LitTextureShader();

    std::string materialResourceName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(LitTextureShader);


  };

  //! Declares the smart pointer U3DObjectPtr from LitTextureShader.
  ML_REFCOUNTED_PTR(LitTextureShader);

  typedef std::vector<LitTextureShader> LitTextureShaderVector;


  // ****************************************************************************************************
  //
  // Class MaterialResource: Represents a Material Resource
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT MaterialResource : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    MaterialResource();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~MaterialResource();

    Vector3 ambientColor;
    Vector4 diffuseColor;
    Vector3 specularColor;
    Vector3 emissiveColor;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(MaterialResource);


  };

  //! Declares the smart pointer U3DObjectPtr from MaterialResource.
  ML_REFCOUNTED_PTR(MaterialResource);

  typedef std::vector<MaterialResource> MaterialResourceVector;


  // ****************************************************************************************************
  //
  // Class TextureResource: Represents a Texture Resource
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT TextureResource : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    TextureResource();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~TextureResource();


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(TextureResource);


  };

  //! Declares the smart pointer U3DObjectPtr from TextureResource.
  ML_REFCOUNTED_PTR(TextureResource);

  typedef std::vector<TextureResource> TextureResourceVector;


  // ****************************************************************************************************
  //
  // Class MotionResource: Represents a Motion Resource
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT MotionResource : public ResourceBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    MotionResource();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~MotionResource();


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(MotionResource);


  };

  //! Declares the smart pointer U3DObjectPtr from MotionResource.
  ML_REFCOUNTED_PTR(MotionResource);

  typedef std::vector<MotionResource> MotionResourceVector;





} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_Resources_H
