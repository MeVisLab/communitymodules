//----------------------------------------------------------------------------------
// U3DLoad module.
//
// \file    U3DLoad.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Loads a U3D from disk.
//----------------------------------------------------------------------------------


#ifndef _U3DLoad_H
#define _U3DLoad_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DGenerator.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/U3DFileFormat/U3D_BitStream.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DLoad : public U3DGenerator
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DLoad)

public:

  //! Constructor.
  U3DLoad();

protected:

  //! Destructor.
  virtual ~U3DLoad();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

    /* FIELDS */
	ml::StringField* _fileNameFld;
	ml::Field* _applyFld;

	MLuint64 fileSize;

	// internal format, maps are accessed by blockname 
	std::map<std::string, mlU3D::ViewNode>  viewNodeMap;
	std::map<std::string, mlU3D::GroupNode> groupNodeMap;
	std::map<std::string, mlU3D::ModelNode> modelNodeMap;
	std::map<std::string, mlU3D::LightNode> lightNodeMap;
	std::map<std::string, mlU3D::PointSetGenerator> pointSetMap;
	std::map<std::string, mlU3D::LineSetGenerator>  lineSetMap;
	std::map<std::string, mlU3D::CLODMeshGenerator> meshMap;
	std::map<std::string, mlU3D::LightResource>    lightResourceMap;
	std::map<std::string, mlU3D::ViewResource>     viewResourceMap;
	std::map<std::string, mlU3D::LitTextureShader> litTextureShaderMap;
	std::map<std::string, mlU3D::MaterialResource> materialResourceMap;
	std::map<std::string, mlU3D::TextureResource>  textureResourceMap;

	// current modifier chain
	std::string currentChainNode;

	// local reader
	U3DBitStreamReader bitStreamReader;

    /* METHODS */

	// resets the internal format of the module 
	void clearMaps();

	//loads the given path and parses the file
	void loadU3DFile(std::string path);

	//identifies the next blocktype and parses the next block
	void parseBlock(std::ifstream& fileStream);

	//writes the internatl format data to the output
	void writeDataToOutput();

	/*helper methods, one for each supported blocktype, called by by parseBlock*/

	void parseFileHeader(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parsePriorityUpdate(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseModifierChain(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseLitTextureShader(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseModelNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseShadingModifier(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseLightNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseLightResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseViewNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseViewResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseGroupNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseMaterialResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseCLODMeshDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseCLODMeshBaseContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseTextureDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parseTextureContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parsePointSetDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);
	void parsePointSetContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize);

	/* helper methods for reading the various data-types */

	MLuint8 readU8();
	MLuint16 readU16();
	MLuint32 readU32();
	MLuint64 readU64();
	MLint16 readI16();
	MLint32 readI32();
	MLfloat readF32();
	MLdouble readF64();
	MLuint8 readCompressedU8(MLuint32 context);
	MLuint16 readCompressedU16(MLuint32 context);
	MLuint32 readCompressedU32(MLuint32 context);
	std::string readString();



//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DLoad_H