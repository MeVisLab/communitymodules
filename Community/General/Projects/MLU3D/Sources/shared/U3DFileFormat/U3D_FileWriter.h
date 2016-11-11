//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data 
// block as specified in Standard ECMA-363 (Universal 3D File Format), 4th edition 
// (June 2007) (referred below as "the Standard")
//
// \file    U3D_FileWriter.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_FileWriter_H
#define _U3D_FileWriter_H

// Local includes  
#include "../MLU3D_Constants.h"
#include "U3D_DataBlockWriter.h"
#include "../U3DInternalFormat/U3D_Object_Nodes.h"
#include "../U3DInternalFormat/U3D_Object_DataTypes.h"
#include "../U3DInternalFormat/U3D_Object_Resources.h"
#include "../U3DInternalFormat/U3D_Object_GeometryGenerators.h"


ML_START_NAMESPACE


class U3DFileWriter
{
public:

  //! Standard constructor
  U3DFileWriter();

  //! Standard destructor
  ~U3DFileWriter();

  size_t addDataBlock(U3DDataBlockWriter& dataBlock);
  size_t addDataBlock(U3DDataBlockWriter& dataBlock, MLuint32 blockType);

  size_t addStandardBlock_PriorityUpdate(const MLuint32 newPriority);
  size_t addStandardBlock_ModifierChain(const std::string& modifierChainName, const MLuint32 modifierChainType, 
                                        const MLuint32 modifierChainAttributes, const MLuint32 modifierCount);
  size_t addStandardBlock_GroupNode(const std::string& groupNodeName, const std::string& parentNodeName = "");
  size_t addStandardBlock_ViewNode(const std::string& viewNodeName, const std::string& viewResourceName);
  size_t addStandardBlock_LightNode(const std::string& lightNodeName, const std::string& lightResourceName);
  size_t addStandardBlock_LitTextureShader(const mlU3D::LitTextureShader& shader);
  size_t addStandardBlock_MaterialResource(const mlU3D::MaterialResource& materialResource);
  size_t addStandardBlock_MaterialResourceWithDefaultLight(const std::string& materialResourceName);
  size_t addStandardBlock_ViewResource(const mlU3D::ViewResource& viewResource);
  size_t addStandardBlock_LightResource(const mlU3D::LightResource& lightResource);
  size_t addStandardBlock_ModelNode(const mlU3D::ModelNode& modelNode);
  size_t addStandardBlock_ShadingModifier(const std::string& shadingModifierName, const std::string& shaderName);
  size_t addStandardBlock_PointSetDeclaration(const mlU3D::PointSetGenerator& pointSetGenerator);
  size_t addStandardBlock_LineSetDeclaration(const mlU3D::LineSetGenerator& lineSetGenerator);
  size_t addStandardBlock_CLODMeshDeclaration(const mlU3D::CLODMeshGenerator& meshGenerator);
  
  void addModifierChain_GroupNode(const std::string& groupNodeName, const std::string& parentNodeName = "");
  void addModifierChain_ViewNode(const std::string& viewNodeName, const std::string& viewResourceName);
  void addModifierChain_LightNode(const std::string& lightNodeName, const std::string& lightResourceName);
  void addModifierChain_ModelNode(const mlU3D::ModelNode& modelNode);
  void addModifierChain_ModelNodeWithShadingModifier(const mlU3D::ModelNode& modelNode);
  void addModifierChain_PointSet(const mlU3D::PointSetGenerator& pointSetGenerator, U3DDataBlockWriter continuationBlock);
  void addModifierChain_LineSet(const mlU3D::LineSetGenerator& lineSetGenerator, U3DDataBlockWriter continuationBlock);
  void addModifierChain_CLODMeshDeclaration(const mlU3D::CLODMeshGenerator& meshGenerator);

  bool writeToFileStream(std::ofstream& ofstream);
  bool writeToFileStream(std::ofstream& ofstream, mlU3D::MetaDataVector metaData);

private:

  bool _writeBlockToFileStream(U3DDataBlockWriter& block, std::ofstream& ofstream);
  void _writeParentNodeData(U3DDataBlockWriter& dataBlock, const std::string& parentNodeName);

  std::vector< U3DDataBlockWriter > _dataBlocks; // Contains all data blocks to be written into the file    

};

ML_END_NAMESPACE

#endif // _U3D_FileWriter_H
