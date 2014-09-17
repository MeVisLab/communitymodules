//----------------------------------------------------------------------------------
//! This header file contains definitions for a class that writes a U3D file according to
//! Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
//! 
/*!
// \file    U3D_FileWriter.h
// \author  Axel Newe
// \date    2014-01-20
*/


#ifndef _U3D_FileWriter_H
#define _U3D_FileWriter_H

// Local includes  
#include "U3D_Constants.h"
#include "U3D_DataTypes.h"
#include "U3D_DataBlockWriter.h"

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
  size_t addStandardBlock_LitTextureShader(const std::string& litTextureShaderName, const std::string& materialResourceName);
  size_t addStandardBlock_MaterialResource(const U3DObjectInfoStruct& objectInfo);
  size_t addStandardBlock_MaterialResourceWithDefaultLight(const std::string& materialResourceName);
  size_t addStandardBlock_ViewResource(const std::string& viewResourceName);
  size_t addStandardBlock_LightResource(const U3DLightResourceDescriptionStruct lightResourceDescription);
  size_t addStandardBlock_ModelNode(const U3DObjectInfoStruct& objectInfo);
  size_t addStandardBlock_ShadingModifier(const std::string& shadingModifierName, const std::string& shaderName);
  size_t addStandardBlock_PointSetDeclaration(const U3DPointSetInfoStruct& pointSetInfo);
  size_t addStandardBlock_LineSetDeclaration(const U3DLineSetInfoStruct& lineSetInfo);
  size_t addStandardBlock_CLODMeshDeclaration(const U3DMeshInfoStruct& meshInfo);
  
  void addModifierChain_GroupNode(const std::string& groupNodeName, const std::string& parentNodeName = "");
  void addModifierChain_ViewNode(const std::string& viewNodeName, const std::string& viewResourceName);
  void addModifierChain_LightNode(const std::string& lightNodeName, const std::string& lightResourceName);
  void addModifierChain_ModelNode(const U3DObjectInfoStruct& objectInfo);
  void addModifierChain_ModelNodeWithShadingModifier(const U3DObjectInfoStruct& objectInfo);
  void addModifierChain_PointSet(const U3DPointSetInfoStruct& pointSetInfo, U3DDataBlockWriter continuationBlock);
  void addModifierChain_LineSet(const U3DLineSetInfoStruct& lineSetInfo, U3DDataBlockWriter continuationBlock);
  void addModifierChain_CLODMeshDeclaration(const U3DMeshInfoStruct& meshInfo);

  bool writeToFileStream(std::ofstream& ofstream);
  bool writeToFileStream(std::ofstream& ofstream, MetaDataVector metaData);

private:

  bool _writeBlockToFileStream(U3DDataBlockWriter& block, std::ofstream& ofstream);
  void _writeParentNodeData(U3DDataBlockWriter& dataBlock, const std::string& parentNodeName);

  std::vector< U3DDataBlockWriter >   _dataBlocks;         // Contains all data blocks to be written into the file    

};



ML_END_NAMESPACE

#endif // _U3D_FileWriter_H
