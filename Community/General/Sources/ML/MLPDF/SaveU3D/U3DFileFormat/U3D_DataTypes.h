//----------------------------------------------------------------------------------
//! This header files contains some data type definitions needed for file creation
/*!
// \file    U3D_DataTypes.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


#ifndef _U3D_DataTypes_H
#define _U3D_DataTypes_H

#include "../../shared/MLPDF_DataTypes.h"


ML_START_NAMESPACE

typedef MLuint32 U3DDataBlockFundamental;
typedef std::vector< U3DDataBlockFundamental > U3DDataVector;
typedef std::map<Vector4, MLuint> U3DColorMap;

enum  U3DObjectType {U3DOBJECTTYPE_POINTSET, U3DOBJECTTYPE_LINESET, U3DOBJECTTYPE_MESH /*, U3DOBJECTTYPE_GLYPH*/};

struct U3DObjectInfoStruct
{
  U3DObjectType Type; 
  std::string   DisplayName;
  std::string   InternalName;
  std::string   GroupPath;
  std::string   ShaderName;
  std::string   MaterialName;
  std::string   ResourceName;
  Vector4       DiffuseColor;
  Vector3       SpecularColor;
  Vector3       AmbientColor;
  Vector3       EmissiveColor;
  bool          UseVertexColors;
  MLuint32      Visibility;
};

typedef std::vector<U3DObjectInfoStruct> U3DObjectInfoVector;

struct U3DDefaultValuesStruct
{
  std::string defaultModelName;
  std::string defaultLightName;
  std::string defaultViewName;
  Vector3     defaultMaterialAmbientColor;
  Vector3     defaultMaterialDiffuseColor;
  Vector3     defaultMaterialSpecularColor;
  Vector3     defaultMaterialEmissiveColor;
  MLfloat     defaultMaterialAlpha;
  Vector4     defaultMaterialDiffuseColorWithTransparency;
};

struct U3DPointSetInfoStruct
{
  std::string DisplayName;
  std::string InternalName;
  std::string ResourceName;

  MLuint32    PointCount;
  MLuint32    NormalCount;            
  MLuint32    DiffuseColorCount;
  MLuint32    SpecularColorCount;
  MLuint32    TextureCoordCount;
  MLuint32    ShadingAttributes;
  
  Vector4     DefaultDiffuseColor;
  Vector3     DefaultSpecularColor;
  Vector3     DefaultAmbientColor;
  Vector3     DefaultEmissiveColor;
};

typedef std::vector<U3DPointSetInfoStruct> U3DPointSetInfoVector;

struct U3DLineSetInfoStruct
{
  std::string DisplayName;
  std::string InternalName;
  std::string ResourceName;

  MLuint32    LineCount;
  MLuint32    PointCount;
  MLuint32    NormalCount;            
  MLuint32    DiffuseColorCount;
  MLuint32    SpecularColorCount;
  MLuint32    TextureCoordCount;
  MLuint32    ShadingAttributes;
  //MLuint32    FiberSetID;
  
  Vector4     DefaultDiffuseColor;
  Vector3     DefaultSpecularColor;
  Vector3     DefaultAmbientColor;
  Vector3     DefaultEmissiveColor;
};

typedef std::vector<U3DLineSetInfoStruct> U3DLineSetInfoVector;

struct U3DMeshInfoStruct
{
  std::string DisplayName;
  std::string InternalName;
  std::string ResourceName;

  MLuint32    MeshAttributes;
  MLuint32    FaceCount;
  MLuint32    VertexCount;
  MLuint32    NormalCount;            
  MLuint32    DiffuseColorCount;
  MLuint32    SpecularColorCount;
  MLuint32    TextureCoordCount;
  MLuint32    ShadingAttributes;
  MLuint32    PatchID;
  MLuint32    MeshNumber; // Needed for progress calculation
  
  Vector4     DefaultDiffuseColor;
  Vector3     DefaultSpecularColor;
  Vector3     DefaultAmbientColor;
  Vector3     DefaultEmissiveColor;
};

typedef std::vector<U3DMeshInfoStruct> U3DMeshInfoVector;

struct U3DLightResourceDescriptionStruct
{
  std::string LightResourceName; 
  char        LightType;                          
  Vector3     LightColor;
  Vector3     LightAttenuation;
  MLdouble    LightSpotAngle;
  MLdouble    LightIntensity;
};

ML_END_NAMESPACE

#endif // _U3D_DataTypes_H
