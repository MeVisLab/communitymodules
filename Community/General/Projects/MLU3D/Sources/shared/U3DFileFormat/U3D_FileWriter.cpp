//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data 
// block as specified in Standard ECMA-363 (Universal 3D File Format), 4th edition 
// (June 2007) (referred below as "the Standard")
//
// \file    U3D_FileWriter.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_FileWriter.h"
#include "../MLU3D_Tools.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! Standard constructor
U3DFileWriter::U3DFileWriter()
{
  // Init dataBlocks with first (and always present) "File header" data block
  U3DDataBlockWriter FileHeaderBlock;
  FileHeaderBlock.blockType = mlU3D::BLOCKTYPE_FILEHEADER;
  addDataBlock(FileHeaderBlock);
}


U3DFileWriter::~U3DFileWriter()
{
  _dataBlocks.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Standard blocks
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! Adds a given data block to the chain of all data blocks.
//! Returns the last index of data blocks in the data block chain.
size_t U3DFileWriter::addDataBlock(U3DDataBlockWriter& dataBlock)
{
  size_t dataBlockCount = _dataBlocks.size();

  _dataBlocks.push_back(dataBlock);

  return dataBlockCount;
}


//! Adds a given data block to the chain of all data blocks.
//! Allows to set block type.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addDataBlock(U3DDataBlockWriter& dataBlock, const MLuint32 blockType)
{
  dataBlock.blockType = blockType;

  return addDataBlock(dataBlock);
}


//! Adds a standard data block (type: priority update) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_PriorityUpdate(const MLuint32 newPriority)
{
  U3DDataBlockWriter PriorityUpdateBlock;
  PriorityUpdateBlock.blockType = mlU3D::BLOCKTYPE_PRIORITYUPDATE;
  PriorityUpdateBlock.writeU32(newPriority);

  return addDataBlock(PriorityUpdateBlock);
}


//! Adds a standard data block (type: modifier chain) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_ModifierChain(const std::string& modifierChainName, 
                                                     const MLuint32 modifierChainType, 
                                                     const MLuint32 modifierChainAttributes, 
                                                     const MLuint32 modifierCount)
{
  U3DDataBlockWriter ModifierChainBlock;
  ModifierChainBlock.blockType = mlU3D::BLOCKTYPE_MODIFIERCHAIN;

  ModifierChainBlock.writeString(modifierChainName);
  ModifierChainBlock.writeU32(modifierChainType);
  ModifierChainBlock.writeU32(modifierChainAttributes);
  ModifierChainBlock.writePaddingBytes(U3DDataBlockWriter::getNumPaddingBytes( 2 + (MLuint32)(modifierChainName.length()) ));
  ModifierChainBlock.writeU32(modifierCount);

  return addDataBlock(ModifierChainBlock);
}


//! Adds a standard data block (type: group node) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_GroupNode(const std::string& groupNodeName, const std::string& parentNodeName)
{
  U3DDataBlockWriter GroupNodeBlock;
  GroupNodeBlock.blockType = mlU3D::BLOCKTYPE_GROUPNODE;

  GroupNodeBlock.writeString(groupNodeName);              // Write Group Node Name (9.5.1.1)
  _writeParentNodeData(GroupNodeBlock, parentNodeName);   // Write Parent Node Data (9.5.1.2) (Parent Node Name = empty --> = world)

  return addDataBlock(GroupNodeBlock);
}


//! Adds a standard data block (type: view node) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_ViewNode(const std::string& viewNodeName, const std::string& viewResourceName)
{
  U3DDataBlockWriter ViewNodeBlock;
  ViewNodeBlock.blockType = mlU3D::BLOCKTYPE_VIEWNODE;

  ViewNodeBlock.writeString(viewNodeName);                   // Write View Node Name (9.5.4.1)
  _writeParentNodeData(ViewNodeBlock, "");                   // Write Parent Node Data (9.5.4.2) (Parent Node Name = empty --> = world)
  ViewNodeBlock.writeString(viewResourceName);               // Write View Resource Name (9.5.4.3) (Same as View Node Name) [UNUSED BY ACROBAT]
  ViewNodeBlock.writeU32(mlU3D::VIEWNODEATTRIBUTE_DEFAULT);  // Write View Node Attributes (9.5.4.4) 
  ViewNodeBlock.writeF32(0.0f);                              // Write View Near Clip (9.5.4.5.1)
  ViewNodeBlock.writeF32(0.0f);                              // Write View Far Clip (9.5.4.5.2)
  ViewNodeBlock.writeF32(0.0f);                              // Write View Projection (9.5.4.6.1)
  //ViewNodeBlock.writeF32(0.0f);                            // Write View Orthographic Height (9.5.4.6.2) (only for orthographic projection) [UNUSED BY ACROBAT]
  ViewNodeBlock.writeF32(100.0f);                            // Write View Port Width (9.5.4.7.1)
  ViewNodeBlock.writeF32(100.0f);                            // Write View Port Height (9.5.4.7.2)
  ViewNodeBlock.writeF32(50.0f);                             // Write View Port Horizontal Position (9.5.4.7.3)
  ViewNodeBlock.writeF32(50.0f);                             // Write View Port Vertical Position (9.5.4.7.4)
  ViewNodeBlock.writeU32(0x00000000);                        // Write Backdrop Count (9.5.4.8) [UNUSED BY ACROBAT]
  ViewNodeBlock.writeU32(0x00000000);                        // Write Overlay Count (9.5.4.10) [UNUSED BY ACROBAT]

  return addDataBlock(ViewNodeBlock);
}


//! Adds a standard data block (type: light node) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_LightNode(const std::string& lightNodeName, const std::string& lightResourceName)
{
  U3DDataBlockWriter LightNodeBlock;
  LightNodeBlock.blockType = mlU3D::BLOCKTYPE_LIGHTNODE;

  LightNodeBlock.writeString(lightNodeName);      // Write Light Node Name (9.5.3.1)
  _writeParentNodeData(LightNodeBlock, "");       // Write Parent Node Data (9.5.3.2) (Parent Node Name = empty --> = world)
  LightNodeBlock.writeString(lightResourceName);  // Write Light Resource Name (9.5.3.1) (Same as Light Node Name)

  return addDataBlock(LightNodeBlock);
}


//! Adds a standard data block (type: Lit texture shader) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_LitTextureShader(const mlU3D::LitTextureShader& shader)
{
  U3DDataBlockWriter LitTextureShaderBlock;
  LitTextureShaderBlock.blockType = mlU3D::BLOCKTYPE_LITTEXTURESHADER;
  LitTextureShaderBlock.writeString(shader.resourceName);                         // Write Lit Texture Shader Name (9.8.3.1)
  LitTextureShaderBlock.writeU32(mlU3D::LITTEXTURESHADERATTRIBUTES_ALL);          // Write Lit Texture Shader Attributes (9.8.3.2)  [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeF32(0.0f);                                           // Write Alpha Test Reference (9.8.3.3)           [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeU32(mlU3D::ALPHATESTFUCTION_ALWAYS);                 // Write Alpha Test Function (9.8.3.4)            [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeU32(mlU3D::COLORBLEND_ALPHABLEND);                   // Write Color Blend Function (9.8.3.5)           [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeU32(0x00000001);                                     // Write Render Pass Flags (9.8.3.6)              [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeU32(0x00000000);                                     // Write Shader Channels (9.8.3.7)
  LitTextureShaderBlock.writeU32(0x00000000);                                     // Write Alpha Texture Channels (9.8.3.8)         [UNUSED BY ACROBAT]
  LitTextureShaderBlock.writeString(shader.materialResourceName);                 // Write Material Name (9.8.3.9)
  // No texture information written since no Shader Channels are activated

  return addDataBlock(LitTextureShaderBlock);
}


//! Adds a standard data block (type: material resource) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_MaterialResourceWithDefaultLight(const std::string& materialResourceName)
{
  U3DDataBlockWriter MaterialResourceBlock;
  MaterialResourceBlock.blockType = mlU3D::BLOCKTYPE_MATERIALRESOURCE;
  MaterialResourceBlock.writeString(materialResourceName);                        // Write Material Resource Name (9.8.4.1)
  MaterialResourceBlock.writeU32(mlU3D::MATERIALRESOURCEATTRIBUTES_ALL);          // Write Material Attributes (9.8.4.2)

  MaterialResourceBlock.writeF32(1.0f);                                           // Ambient Color - Red (9.8.4.3.1)
  MaterialResourceBlock.writeF32(1.0f);                                           // Ambient Color - Green (9.8.4.3.2)
  MaterialResourceBlock.writeF32(1.0f);                                           // Ambient Color - Blue (9.8.4.3.3)
  MaterialResourceBlock.writeF32(0.65f);                                          // Diffuse Color - Red (9.8.4.4.1)
  MaterialResourceBlock.writeF32(0.65f);                                          // Diffuse Color - Green (9.8.4.4.2)
  MaterialResourceBlock.writeF32(0.65f);                                          // Diffuse Color - Blue (9.8.4.4.3)
  MaterialResourceBlock.writeF32(0.75f);                                          // Specular Color - Red (9.8.4.5.1)
  MaterialResourceBlock.writeF32(0.75f);                                          // Specular Color - Green (9.8.4.5.2)
  MaterialResourceBlock.writeF32(0.75f);                                          // Specular Color - Blue (9.8.4.5.3)
  MaterialResourceBlock.writeF32(0.0f);                                           // Emissive Color - Red (9.8.4.6.1)
  MaterialResourceBlock.writeF32(0.0f);                                           // Emissive Color - Green (9.8.4.6.2)
  MaterialResourceBlock.writeF32(0.0f);                                           // Emissive Color - Blue (9.8.4.6     
  MaterialResourceBlock.writeF32(0.5f);                                           // Write Reflectivity (9.8.4.7)
  MaterialResourceBlock.writeF32(1.0f);                                           // Write Opacity (9.8.4.8)

  return addDataBlock(MaterialResourceBlock);
}


//! Adds a standard data block (type: material resource) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_MaterialResource(const mlU3D::MaterialResource& materialResource)
{
  U3DDataBlockWriter MaterialResourceBlock;
  MaterialResourceBlock.blockType = mlU3D::BLOCKTYPE_MATERIALRESOURCE;
  MaterialResourceBlock.writeString(materialResource.resourceName);                 // Write Material Resource Name (9.8.4.1)
  MaterialResourceBlock.writeU32(mlU3D::MATERIALRESOURCEATTRIBUTES_ALL);            // Write Material Attributes (9.8.4.2)     [UNUSED BY ACROBAT]
  MaterialResourceBlock.writeF32(materialResource.ambientColor[0]);                 // Ambient Color - Red (9.8.4.3.1)
  MaterialResourceBlock.writeF32(materialResource.ambientColor[1]);                 // Ambient Color - Green (9.8.4.3.2)
  MaterialResourceBlock.writeF32(materialResource.ambientColor[2]);                 // Ambient Color - Blue (9.8.4.3.3)
  MaterialResourceBlock.writeF32(materialResource.diffuseColor[0]);                 // Diffuse Color - Red (9.8.4.4.1)
  MaterialResourceBlock.writeF32(materialResource.diffuseColor[1]);                 // Diffuse Color - Green (9.8.4.4.2)
  MaterialResourceBlock.writeF32(materialResource.diffuseColor[2]);                 // Diffuse Color - Blue (9.8.4.4.3)
  MaterialResourceBlock.writeF32(materialResource.specularColor[0]);                // Specular Color - Red (9.8.4.5.1)
  MaterialResourceBlock.writeF32(materialResource.specularColor[1]);                // Specular Color - Green (9.8.4.5.2)
  MaterialResourceBlock.writeF32(materialResource.specularColor[2]);                // Specular Color - Blue (9.8.4.5.3)
  MaterialResourceBlock.writeF32(materialResource.emissiveColor[0]);                // Emissive Color - Red (9.8.4.6.1)
  MaterialResourceBlock.writeF32(materialResource.emissiveColor[1]);                // Emissive Color - Green (9.8.4.6.2)
  MaterialResourceBlock.writeF32(materialResource.emissiveColor[2]);                // Emissive Color - Blue (9.8.4.6     

  MaterialResourceBlock.writeF32(materialResource.reflectivity);                    // Write Reflectivity (9.8.4.7)
  MaterialResourceBlock.writeF32(materialResource.diffuseColor[3]);                 // Write Opacity (9.8.4.8)

  return addDataBlock(MaterialResourceBlock);
}


//! Adds a standard data block (type: view resource) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_ViewResource(const mlU3D::ViewResource& viewResource)
{
  U3DDataBlockWriter ViewResourceBlock;
  ViewResourceBlock.blockType = mlU3D::BLOCKTYPE_VIEWRESOURCE;
  ViewResourceBlock.writeString(viewResource.resourceName);      // Write View Resource Name (9.8.2.1)
  ViewResourceBlock.writeU32(0x00000000);                        // Write Pass Count (9.8.2.2)

  return addDataBlock(ViewResourceBlock);
}


//! Adds a standard data block (type: light resource) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_LightResource(const mlU3D::LightResource& lightResource)
{
  U3DDataBlockWriter LightResourceBlock;
  LightResourceBlock.blockType = mlU3D::BLOCKTYPE_LIGHTRESOURCE;
  LightResourceBlock.writeString(lightResource.resourceName);      // Write Light Resource Name (9.8.1.1)
  LightResourceBlock.writeU32(mlU3D::LIGHTRESOURCEATTRIBUTES_ALL); // Write Light Attributes (9.8.1.2)  [UNUSED BY ACROBAT]
  LightResourceBlock.writeU8(lightResource.lightType);             // Write Light Type (9.8.1.3)
  LightResourceBlock.writeF32(lightResource.lightColor[0]);        // Write Light Color - Red (9.8.1.4.1)
  LightResourceBlock.writeF32(lightResource.lightColor[1]);        // Write Light Color - Green (9.8.1.4.2)
  LightResourceBlock.writeF32(lightResource.lightColor[2]);        // Write Light Color - Blue (9.8.1.4.3)
  LightResourceBlock.writeF32(1.0f);                               // Write Light Color - Light Reserved Parameter (9.8.1.4.4) (shall be 1.0)
  LightResourceBlock.writeF32(lightResource.lightAttenuation[0]);  // Write Light Attenuation - Constant Factor (9.8.1.5.1) 
  LightResourceBlock.writeF32(lightResource.lightAttenuation[1]);  // Write Light Attenuation - Linear Factor (9.8.1.5.2) 
  LightResourceBlock.writeF32(lightResource.lightAttenuation[2]);  // Write Light Attenuation - Quadric Factor (9.8.1.5.3) 
  LightResourceBlock.writeF32(lightResource.lightSpotAngle);       // Write Light Spot Angle (9.8.1.6) 
  LightResourceBlock.writeF32(lightResource.lightIntensity);       // Write Light Intensity (9.8.1.7) 

  return addDataBlock(LightResourceBlock);
}


//! Adds a standard data block (type: model node) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_ModelNode(const mlU3D::ModelNode& modelNode)
{
  U3DDataBlockWriter ModelNodeBlock;
  ModelNodeBlock.blockType = mlU3D::BLOCKTYPE_MODELNODE;
  std::string parentName = mlU3D::U3DTools::getParentNameFromGroupPath(modelNode.groupPath);

  ModelNodeBlock.writeString(modelNode.internalName);            // Write Model Node Name (9.5.2.1)

  _writeParentNodeData(ModelNodeBlock, parentName);              // Write Parent Node Data (9.5.2.2)

  ModelNodeBlock.writeString(modelNode.geometryGeneratorName);   // Write Model Resource Name (9.5.2.3) 
  ModelNodeBlock.writeU32(modelNode.visibility);                 // Write Model Visibility (9.5.2.4);

  return addDataBlock(ModelNodeBlock);
}


//! Adds a standard data block (type: shading modifier) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_ShadingModifier(const std::string& shadingModifierName, const std::string& shaderName)
{
  U3DDataBlockWriter ShadingModifierBlock;
  ShadingModifierBlock.blockType = mlU3D::BLOCKTYPE_SHADINGMODIFIER;

  ShadingModifierBlock.writeString(shadingModifierName);                         // Write Shading Modifier Name (9.7.5.1)
  ShadingModifierBlock.writeU32(0x00000001);                                     // Write Chain Index (9.7.5.2)
  ShadingModifierBlock.writeU32(mlU3D::SHADINGMODIFIER_SHADINGATTRIBUTES_ALL);   // Write Shading Attributes (9.7.5.3)  (all = meshes, points, lines & glyphs) [UNUSED BY ACROBAT]
  ShadingModifierBlock.writeU32(0x00000001);                                     // Write Shader List Count (9.7.5.4)
  ShadingModifierBlock.writeU32(0x00000001);                                     // Write Shader Count (9.7.5.5)
  ShadingModifierBlock.writeString(shaderName);                                  // Write Shader Name (9.7.5.6)

  return addDataBlock(ShadingModifierBlock);
}


//! Adds a standard data block (type: CLOD mesh declaration) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_CLODMeshDeclaration(const mlU3D::CLODMeshGenerator& meshGenerator)
{
  U3DDataBlockWriter CLODMeshDeclarationBlock;
  CLODMeshDeclarationBlock.blockType = mlU3D::BLOCKTYPE_CLODMESHDECLARATION;

  CLODMeshDeclarationBlock.writeString(meshGenerator.resourceName);     // Write Mesh Name (9.6.1.1.1)
  CLODMeshDeclarationBlock.writeU32(mlU3D::ReservedZero);               // Write Chain Index (9.6.1.1.2) (shall be zero) 

  CLODMeshDeclarationBlock.writeU32(meshGenerator.meshAttributes);      // Write Max Mesh Description - Mesh Attributes (9.6.1.1.3.1)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.faceCount);           // Write Max Mesh Description - Face Count (9.6.1.1.3.2) (# of faces)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.vertexCount);         // Write Max Mesh Description - Position Count (9.6.1.1.3.3) (# of vertices)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.normalCount);         // Write Max Mesh Description - Normal Count (9.6.1.1.3.4) (# of normals)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.diffuseColorCount);   // Write Max Mesh Description - Diffuse Color Count (9.6.1.1.3.5)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.specularColorCount);  // Write Max Mesh Description - Specular Color Count (9.6.1.1.3.6)
  CLODMeshDeclarationBlock.writeU32(0x00000001);                        // Write Max Mesh Description - Texture Coord Count (9.6.1.1.3.7)
  CLODMeshDeclarationBlock.writeU32(0x00000001);                        // Write Max Mesh Description - Shading Count (9.6.1.1.3.8) (shall be 1 since only one shader is supported by this version)
  CLODMeshDeclarationBlock.writeU32(meshGenerator.shadingAttributes);   // Write Max Mesh Description - Shading Description - Shading Attributes (9.6.1.1.3.9.1)
    CLODMeshDeclarationBlock.writeU32(0x00000000);                      // Write Max Mesh Description - Shading Description - Texture Layer Count (9.6.1.1.3.9.2) (shall be zero since textures are not supported by this version)
      //CLODMeshDeclarationBlock.writeU32(0x00000002);                    // Write Max Mesh Description - Shading Description - Texture Coord Dimensions (9.6.1.1.3.9.3)
    CLODMeshDeclarationBlock.writeU32(0x00000000);                      // Write Max Mesh Description - Shading Description - Original Shading ID (9.6.1.1.3.9.4)

    CLODMeshDeclarationBlock.writeU32(meshGenerator.vertexCount);       // Write CLOD Description - Minimum Resolution (9.6.1.1.4.1)  // Min resolution = max resolution -> base mesh only!
  //CLODMeshDeclarationBlock.writeU32(0);                                 // Write CLOD Description - Minimum Resolution (9.6.1.1.4.1) 
    CLODMeshDeclarationBlock.writeU32(meshGenerator.vertexCount);       // Write CLOD Description - Final Maximum Resolution (9.6.1.1.4.2)

  CLODMeshDeclarationBlock.writeU32(0x000003E8);                        // Write Resource Description - Quality Factors - Position Quality Factor (9.6.1.1.5.1.1)
  CLODMeshDeclarationBlock.writeU32(0x000003E8);                        // Write Resource Description - Quality Factors - Normal Quality Factor (9.6.1.1.5.1.2)
  CLODMeshDeclarationBlock.writeU32(0x000003E8);                        // Write Resource Description - Quality Factors - Texture Coord Quality Factor (9.6.1.1.5.1.3)
  CLODMeshDeclarationBlock.writeF32(1.f/mlU3D::Quant_Position);         // Write Resource Description - Inverse Quantization - Position Inverse Quant (9.6.1.1.5.2.1)
  CLODMeshDeclarationBlock.writeF32(1.f/mlU3D::Quant_Normal);           // Write Resource Description - Inverse Quantization - Normal Inverse Quant (9.6.1.1.5.2.2)
  CLODMeshDeclarationBlock.writeF32(1.f/mlU3D::Quant_TextureCoord);     // Write Resource Description - Inverse Quantization - Texture Coord Inverse Quant (9.6.1.1.5.2.3)
  CLODMeshDeclarationBlock.writeF32(1.f/mlU3D::Quant_DiffuseColor);     // Write Resource Description - Inverse Quantization - Diffuse Color Inverse Quant (9.6.1.1.5.2.4)
  CLODMeshDeclarationBlock.writeF32(1.f/mlU3D::Quant_SpecularColor);    // Write Resource Description - Inverse Quantization - Specular Color Inverse Quant (9.6.1.1.5.2.5)
  CLODMeshDeclarationBlock.writeF32(0.9f);                              // Write Resource Description - Resource Parameters - Normal Crease Parameter (9.6.1.1.5.3.1)
  CLODMeshDeclarationBlock.writeF32(0.5f);                              // Write Resource Description - Resource Parameters - Normal Update Parameter (9.6.1.1.5.3.2)
  CLODMeshDeclarationBlock.writeF32(0.985f);                            // Write Resource Description - Resource Parameters - Normal Tolerance Parameter (9.6.1.1.5.3.3)

  CLODMeshDeclarationBlock.writeU32(0x00000000);                        // Write Skeleton Description - Bone Count (9.6.1.1.6.1) (shall be zero since skeltons are not supported by this version) [UNUSED BY ACROBAT]

  return addDataBlock(CLODMeshDeclarationBlock);
}


//! Adds a standard data block (type: point set declaration) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_PointSetDeclaration(const mlU3D::PointSetGenerator& pointSetGenerator)
{
  U3DDataBlockWriter PointSetDeclarationBlock;
  PointSetDeclarationBlock.blockType = mlU3D::BLOCKTYPE_POINTSETDECLARATION;

  PointSetDeclarationBlock.writeString(pointSetGenerator.resourceName);    // Write Point Set Name (9.6.2.1.1)
  PointSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Chain Index (9.6.2.1.1) (shall be zero) 

  PointSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Point Set Description - Point Set Reserved (9.6.2.1.3.1) (shall be zero)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.pointCount);         // Write Point Set Description - Point Count (9.6.2.1.3.2) (# of points)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.pointCount);         // Write Point Set Description - Position Count (9.6.2.1.3.3) (# of positions - equals # of points)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.normalCount);        // Write Point Set Description - Normal Count (9.6.2.1.3.4) (# of normals)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.diffuseColorCount);  // Write Point Set Description - Diffuse Color Count (9.6.2.1.3.5)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.specularColorCount); // Write Point Set Description - Specular Color Count (9.6.2.1.3.6)
  PointSetDeclarationBlock.writeU32(pointSetGenerator.textureCoordCount);  // Write Point Set Description - Texture Coord Count (9.6.2.1.3.7)
  PointSetDeclarationBlock.writeU32(0x00000001);                           // Write Point Set Description - Shading Count (9.6.2.1.3.8)

  PointSetDeclarationBlock.writeU32(pointSetGenerator.shadingAttributes);  // Write Point Set Description - Shading Description - Shading Attributes (9.6.1.1.3.9.1) 
  PointSetDeclarationBlock.writeU32(0x00000000);                           // Write Point Set Description - Shading Description - Texture Layer Count (9.6.1.1.3.9.2)
  //PointSetDeclarationBlock.writeU32(0x00000000);                           // Write Point Set Description - Shading Description - Texture Coord Dimensions (9.6.1.1.3.9.3) - only if Texture Layer Count > 0
  PointSetDeclarationBlock.writeU32(0x00000000);                           // Write Point Set Description - Shading Description - Original Shading ID (9.6.1.1.3.9.4)

  PointSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Position Quality Factor (9.6.1.1.5.1.1)       [UNUSED BY ACROBAT]
  PointSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Normal Quality Factor (9.6.1.1.5.1.2)         [UNUSED BY ACROBAT]
  PointSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Texture Coord Quality Factor (9.6.1.1.5.1.3)  [UNUSED BY ACROBAT]
  PointSetDeclarationBlock.writeF32(1.f / mlU3D::Quant_Position);          // Write Resource Description - Inverse Quantization - Position Inverse Quant (9.6.1.1.5.2.1)
  PointSetDeclarationBlock.writeF32(1.f / mlU3D::Quant_Normal);            // Write Resource Description - Inverse Quantization - Normal Inverse Quant (9.6.1.1.5.2.2)
  PointSetDeclarationBlock.writeF32(1.f / mlU3D::Quant_TextureCoord);      // Write Resource Description - Inverse Quantization - Texture Coord Inverse Quant (9.6.1.1.5.2.3)
  PointSetDeclarationBlock.writeF32(1.f / mlU3D::Quant_DiffuseColor);      // Write Resource Description - Inverse Quantization - Diffuse Color Inverse Quant (9.6.1.1.5.2.4)
  PointSetDeclarationBlock.writeF32(1.f / mlU3D::Quant_SpecularColor);     // Write Resource Description - Inverse Quantization - Specular Color Inverse Quant (9.6.1.1.5.2.5)
  PointSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Point Set Parameter 1 (9.6.2.1.4.3.1) (shall be zero)
  PointSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Point Set Parameter 2 (9.6.2.1.4.3.2) (shall be zero)
  PointSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Point Set Parameter 3 (9.6.2.1.4.3.3) (shall be zero)

  PointSetDeclarationBlock.writeU32(0x00000000);                           // Write Skeleton Description - Bone Count (9.6.2.1.5) (shall be zero since skeltons are not supported by this version) [UNUSED BY ACROBAT]

  return addDataBlock(PointSetDeclarationBlock);
}


//! Adds a standard data block (type: line set declaration) to the chain of all data blocks.
//! Returns the total # of data blocks in the data block chain.
size_t U3DFileWriter::addStandardBlock_LineSetDeclaration(const mlU3D::LineSetGenerator& lineSetGenerator)
{
  U3DDataBlockWriter LineSetDeclarationBlock;
  LineSetDeclarationBlock.blockType = mlU3D::BLOCKTYPE_LINESETDECLARATION;

  LineSetDeclarationBlock.writeString(lineSetGenerator.resourceName);     // Write Line Set Name (9.6.1.1.1)
  LineSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Chain Index (9.6.1.1.2) (shall be zero) 

  LineSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Line Set Description - Line Set Reserved (9.6.3.1.3.1) (shall be zero)
  LineSetDeclarationBlock.writeU32(lineSetGenerator.lineCount);           // Write Line Set Description - Line Count (9.6.3.1.3.2) (# of lines)
  LineSetDeclarationBlock.writeU32(lineSetGenerator.pointCount);          // Write Line Set Description - Position Count (9.6.3.1.3.3) (# of positions - one point more than lines)
  LineSetDeclarationBlock.writeU32(lineSetGenerator.normalCount);         // Write Line Set Description - Normal Count (9.6.3.1.3.4) (# of normals)
  //LineSetDeclarationBlock.writeU32(lineSetGenerator.DiffuseColorCount);   // Write Line Set Description - Diffuse Color Count (9.6.3.1.3.5)
  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Diffuse Color Count (9.6.3.1.3.5)
  //LineSetDeclarationBlock.writeU32(lineSetGenerator.specularColorCount);  // Write Line Set Description - Specular Color Count (9.6.3.1.3.6)
  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Specular Color Count (9.6.3.1.3.6)
  //LineSetDeclarationBlock.writeU32(lineSetGenerator.textureCoordCount);   // Write Line Set Description - Texture Coord Count (9.6.3.1.3.7)
  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Texture Coord Count (9.6.3.1.3.7)
  LineSetDeclarationBlock.writeU32(0x00000001);                           // Write Line Set Description - Shading Count (9.6.3.1.3.8)

  LineSetDeclarationBlock.writeU32(lineSetGenerator.shadingAttributes);   // Write Line Set Description - Shading Description - Shading Attributes (9.6.1.1.3.9.1)
  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Shading Description - Texture Layer Count (9.6.1.1.3.9.2)
  //LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Shading Description - Texture Coord Dimensions (9.6.1.1.3.9.3) - only if Texture Layer Count > 0
  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Line Set Description - Shading Description - Original Shading ID (9.6.1.1.3.9.4)

  LineSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Position Quality Factor (9.6.1.1.5.1.1)
  LineSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Normal Quality Factor (9.6.1.1.5.1.2)
  LineSetDeclarationBlock.writeU32(0x000003E8);                           // Write Resource Description - Quality Factors - Texture Coord Quality Factor (9.6.1.1.5.1.3)
  LineSetDeclarationBlock.writeF32(1.f/mlU3D::Quant_Position);            // Write Resource Description - Inverse Quantization - Position Inverse Quant (9.6.1.1.5.2.1)
  LineSetDeclarationBlock.writeF32(1.f/mlU3D::Quant_Normal);              // Write Resource Description - Inverse Quantization - Normal Inverse Quant (9.6.1.1.5.2.2)
  LineSetDeclarationBlock.writeF32(1.f/mlU3D::Quant_TextureCoord);        // Write Resource Description - Inverse Quantization - Texture Coord Inverse Quant (9.6.1.1.5.2.3)
  LineSetDeclarationBlock.writeF32(1.f/mlU3D::Quant_DiffuseColor);        // Write Resource Description - Inverse Quantization - Diffuse Color Inverse Quant (9.6.1.1.5.2.4)
  LineSetDeclarationBlock.writeF32(1.f/mlU3D::Quant_SpecularColor);       // Write Resource Description - Inverse Quantization - Specular Color Inverse Quant (9.6.1.1.5.2.5)
  LineSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Line Set Parameter 1 (9.6.3.1.4.3.1) (shall be zero)
  LineSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Line Set Parameter 2 (9.6.3.1.4.3.2) (shall be zero)
  LineSetDeclarationBlock.writeU32(mlU3D::ReservedZero);                  // Write Resource Description - Resource Parameters - Reserved Line Set Parameter 3 (9.6.3.1.4.3.3) (shall be zero)

  LineSetDeclarationBlock.writeU32(0x00000000);                           // Write Skeleton Description - Bone Count (9.6.1.1.6.1) (shall be zero since skeltons are not supported by this version) [UNUSED BY ACROBAT]

  return addDataBlock(LineSetDeclarationBlock);
}


//! Write parent node data to the respective position of a (child) node block. (private)
void U3DFileWriter::_writeParentNodeData(U3DDataBlockWriter& dataBlock, const std::string& parentNodeName)
{
  dataBlock.writeU32(0x00000001);         // Write Parent Node Count (9.5.1.2.1)
  dataBlock.writeString(parentNodeName);  // Write empty Parent Node Name (9.5.1.2.2)
  dataBlock.writeIdentityMatrix();        // Write Parent Node Transform Matrix (9.5.1.2.3) (identity matrix)    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Standard Modifer Chains
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Important note from "Adobe® Acrobat® SDK - U3D Supported Elements" document:
//
// Certain blocks may only be read if presented as declarations for object modifiers. 
// As a result, such blocks must reside within a modifier chain. 
// The following sections specify such object modifiers:
//
// * 9.5.1 Group Node (blocktype: 0xFFFFFF21)
// * 9.5.2 Model Node (blocktype: 0xFFFFFF22)
// * 9.5.3 Light Node (blocktype: 0xFFFFFF23)
// * 9.5.4 View Node (blocktype: 0xFFFFFF24)
// * 9.6.1.1 CLOD Mesh Declaration (blocktype: 0xFFFFFF31)
// * 9.6.2.1 Point Set Declaration (blocktype: 0xFFFFFF36)
// * 9.6.2.2 Point Set Continuation (blocktype: 0xFFFFFF3E)   <-- continuation block within modifier chain!
// * 9.6.3.1 Line Set Declaration (blocktype: 0xFFFFFF37)
// * 9.6.3.2 Line Set Continuation (blocktype: 0xFFFFFF3F)    <-- continuation block within modifier chain!
// * 9.7.2 Subdivision Modifier (blocktype: 0xFFFFFF42)
// * 9.7.3 Animation Modifier (blocktype: 0xFFFFFF43)
// * 9.7.5 Shading Modifier (blocktype: 0xFFFFFF45)
// * 9.8.5.1 Texture Declaration (blocktype: 0xFFFFFF55)


//! Adds a standard modifier chain (type: group node modifier chain) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_GroupNode(const std::string& groupNodeName, const std::string& parentNodeName)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(groupNodeName,mlU3D::MODIFIERCHAINTYPE_NODEMODIFIERCHAIN, 0x00000000, 1);
  
  size_t GroupNodeBlockIndex = addStandardBlock_GroupNode(groupNodeName, parentNodeName);
  _dataBlocks[GroupNodeBlockIndex].close();
  
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[GroupNodeBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: light node modifier chain) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_ViewNode(const std::string& viewNodeName, const std::string& viewResourceName)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(viewNodeName, mlU3D::MODIFIERCHAINTYPE_NODEMODIFIERCHAIN, 0x00000000, 1);
  
  size_t ViewNodeBlockIndex = addStandardBlock_ViewNode(viewNodeName, viewResourceName);
  _dataBlocks[ViewNodeBlockIndex].close();
  
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ViewNodeBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: view node modifier chain) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_LightNode(const std::string& lightNodeName, const std::string& lightResourceName)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(lightNodeName,mlU3D::MODIFIERCHAINTYPE_NODEMODIFIERCHAIN, 0x00000000, 1);
  
  size_t LightNodeBlockIndex = addStandardBlock_LightNode(lightNodeName, lightResourceName);
  _dataBlocks[LightNodeBlockIndex].close();
  
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[LightNodeBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: model node modifier chain) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_ModelNode(const mlU3D::ModelNode& modelNode)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(modelNode.internalName, mlU3D::MODIFIERCHAINTYPE_NODEMODIFIERCHAIN, 0x00000000, 1);

  size_t ModelNodeBlockIndex = addStandardBlock_ModelNode(modelNode);
  _dataBlocks[ModelNodeBlockIndex].close();

  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ModelNodeBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: model node modifier chain) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_ModelNodeWithShadingModifier(const mlU3D::ModelNode& modelNode)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(modelNode.internalName, mlU3D::MODIFIERCHAINTYPE_NODEMODIFIERCHAIN, 0x00000000, 2);

  size_t ModelNodeBlockIndex = addStandardBlock_ModelNode(modelNode);
  _dataBlocks[ModelNodeBlockIndex].close();

  size_t ShadingModifierBlockIndex = addStandardBlock_ShadingModifier(modelNode.internalName, modelNode.shaderName);
  _dataBlocks[ShadingModifierBlockIndex].close();

  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ModelNodeBlockIndex].getNumDataBytes());
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ShadingModifierBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: point set declaration & continuation) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_PointSet(const mlU3D::PointSetGenerator& pointSetGenerator, U3DDataBlockWriter continuationBlock)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(pointSetGenerator.resourceName, mlU3D::MODIFIERCHAINTYPE_MODELRESOURCEMODIFIERCHAIN, 0x00000000, 2);

  size_t PointSetDeclarationBlockIndex = addStandardBlock_PointSetDeclaration(pointSetGenerator);
  _dataBlocks[PointSetDeclarationBlockIndex].close();

  size_t ContinuationBlockIndex = addDataBlock(continuationBlock);
  _dataBlocks[ContinuationBlockIndex].close();

  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[PointSetDeclarationBlockIndex].getNumDataBytes());
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ContinuationBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: line set declaration & continuation) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_LineSet(const mlU3D::LineSetGenerator& lineSetGenerator, U3DDataBlockWriter continuationBlock)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(lineSetGenerator.resourceName, mlU3D::MODIFIERCHAINTYPE_MODELRESOURCEMODIFIERCHAIN, 0x00000000, 2);
  
  size_t LineSetDeclarationBlockIndex = addStandardBlock_LineSetDeclaration(lineSetGenerator);
  _dataBlocks[LineSetDeclarationBlockIndex].close();
  
  size_t ContinuationBlockIndex = addDataBlock(continuationBlock);
  _dataBlocks[ContinuationBlockIndex].close();

  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[LineSetDeclarationBlockIndex].getNumDataBytes());
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[ContinuationBlockIndex].getNumDataBytes());
}


//! Adds a standard modifier chain (type: CLOD mesh declaration) to the chain of all data blocks.
void U3DFileWriter::addModifierChain_CLODMeshDeclaration(const mlU3D::CLODMeshGenerator& meshGenerator)
{
  size_t ModifierChainBlockIndex = addStandardBlock_ModifierChain(meshGenerator.resourceName, mlU3D::MODIFIERCHAINTYPE_MODELRESOURCEMODIFIERCHAIN, 0x00000000, 1);
  
  size_t CLODMeshDeclarationBlockIndex = addStandardBlock_CLODMeshDeclaration(meshGenerator);
  _dataBlocks[CLODMeshDeclarationBlockIndex].close();
  
  _dataBlocks[ModifierChainBlockIndex].addChildDataBytes(_dataBlocks[CLODMeshDeclarationBlockIndex].getNumDataBytes());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Write to file stream
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//! Write all data to file stream (no meta data).
bool U3DFileWriter::writeToFileStream(std::ofstream& ofstream)
{  
  mlU3D::MetaDataVector metaData;
  return writeToFileStream(ofstream, metaData);
}

//! Write all data to file stream (including meta data).
bool U3DFileWriter::writeToFileStream(std::ofstream& ofstream, const mlU3D::MetaDataVector metaData)
{
  bool Success = false;

  size_t dataBlockCount = _dataBlocks.size();
  MLuint32 ByteSizeOfAllBlocks = 0;

  // Close all data blocks but the first (we still need to write some info there)
  for (size_t i = 1; i < dataBlockCount; i++)
  {
    _dataBlocks[i].close();
    ByteSizeOfAllBlocks += _dataBlocks[i].getNumTotalBytes();
  }

  // Write header block data
  MLuint32 ByteSizeOfHeaderMetaData = 0;
  MLuint32 PadBytes = U3DDataBlockWriter::getNumPaddingBytes(ByteSizeOfHeaderMetaData);  
  
  ByteSizeOfHeaderMetaData += PadBytes; 

  MLuint32 ByteSizeOfHeaderData = 24;
  MLuint32 ByteSizeOfHeader = mlU3D::BLOCKDECLARATIONSIZE + ByteSizeOfHeaderData + ByteSizeOfHeaderMetaData;

  _dataBlocks[0].writeI16(0x0100);                                    // Write major file version (9.4.1.1.1)
  _dataBlocks[0].writeI16(0x0000);                                    // Write minor file version (9.4.1.1.2)
  _dataBlocks[0].writeU32(mlU3D::PROFILEIDENTIFIER_BASEPROFILE);      // Write profile identifier (9.4.1.2)
  _dataBlocks[0].writeU32(ByteSizeOfHeader);                          // Write declaration size (9.4.1.3)
  _dataBlocks[0].writeU64(ByteSizeOfHeader + ByteSizeOfAllBlocks);    // Write file size (9.4.1.4)
  _dataBlocks[0].writeU32(0x0000006A);                                // Write character encoding (9.4.1.5)

  // Write header block meta data
  MLuint32 metaDataPairCount = MLuint32(metaData.size()); 

  if (metaDataPairCount > 0)
  {
    _dataBlocks[0].writeMetaU32(metaDataPairCount);                     // Write meta data Key/Value Pair Count  
  
    for (size_t i = 0; i < metaData.size(); i++)
    {
      _dataBlocks[0].writeMetaKeyValuePair(0x00000000, metaData[i].key, metaData[i].value);  // Write meta data Key/Value Pairs 
    }
  }

  // Close header block
  _dataBlocks[0].close();

  // Write all data blocks to output stream
  for (MLuint32 i = 0; i < dataBlockCount; i++)
  {
    _writeBlockToFileStream(_dataBlocks[i], ofstream);
  }

  Success = true;

  return Success;
}

//! Write one block to file stream. (private)
bool U3DFileWriter::_writeBlockToFileStream(U3DDataBlockWriter& block, std::ofstream& ofstream)
{
  bool Success = false;

  MLuint32 flushBufferSize = block.getNumTotalBytes();

  mlU3D::DataBlockFundamental* flushBuffer = NULL;
  ML_CHECK_NEW(flushBuffer, mlU3D::DataBlockFundamental[flushBufferSize]);
  memset(flushBuffer, 0, flushBufferSize);

  if (NULL != flushBuffer)
  {
    // Add Block type, Data size & Meta data Size
    mlU3D::DataBlockFundamental flushBufferIndex = 0;
    flushBuffer[flushBufferIndex] = block.blockType;

    flushBufferIndex++;
    flushBuffer[flushBufferIndex] = block.getDataSizeWithChildDataBytes();

    flushBufferIndex++;
    flushBuffer[flushBufferIndex] = block.getMetaDataSizeWithoutPadding();

    flushBufferIndex++;


    // Add Data
    mlU3D::DataVector data = block.getData();
    for (mlU3D::DataBlockFundamental i = 0; i < block.getDataSize(); i++)
    {
      flushBuffer[flushBufferIndex] = data[i];
      flushBufferIndex++;
    }

    // Add Meta data
    mlU3D::DataVector metaData = block.getMetaData();
    for (mlU3D::DataBlockFundamental i = 0; i < block.getMetaDataSize(); i++)
    {
      flushBuffer[flushBufferIndex] = metaData[i];
      flushBufferIndex++;
    }
  }
  else
  {
    flushBufferSize = 0;
  }

  if ((0 != flushBufferSize) && (block.blockType != 0))
  {
    try
    {
      ofstream.write(reinterpret_cast<char*>(flushBuffer), flushBufferSize);
      ofstream.flush();
      Success = true;
    }
    catch (...)
    {
      // Ignore errors
    }
  }

  ML_DELETE_ARRAY(flushBuffer);

  return Success;
}



ML_END_NAMESPACE

