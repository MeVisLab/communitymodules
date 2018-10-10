//----------------------------------------------------------------------------------
// U3DLoad module.
//
// \file    U3DLoad.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DLoad.h"


ML_START_NAMESPACE
#pragma warning(push)
#pragma warning(disable:4189)
//Disabled due to yet incomplete internal format

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DLoad, U3DGenerator);


//***********************************************************************************


U3DLoad::U3DLoad() : U3DGenerator()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  (_fileNameFld = addString("filename"))->setStringValue("");
  _applyFld = addBool("apply");

  // Add output fields to the module and set allowed types.

  // Add and initialize parameter fields

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DLoad::~U3DLoad()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DLoad::handleNotification (Field* field)
{
  std::string filename = _fileNameFld->getStringValue();
  if (field == _applyFld && filename != "") 
  {
	  loadU3DFile(filename);
  } 
  

  // call parent class and handle apply/autoApply and in/outputs
  U3DGenerator::handleNotification(field);
}



//***********************************************************************************


void U3DLoad::loadU3DFile(std::string path){
	if (_outU3DObject){
		std::ifstream fileStream(path, std::ios::binary);
		fileSize = 0;
		clearMaps();
		bitStreamReader = U3DBitStreamReader();
		while (fileStream.tellg() < fileSize || fileSize == 0){
			parseBlock(fileStream);
		}
		fileStream.close();
		writeDataToOutput();
		_outU3DObjectFld->touch();
	}
}


//***********************************************************************************

static MLuint32 getNumPaddingBytes(const MLuint64 previousBytes) { return ((previousBytes % 4 != 0) ? 4 - previousBytes % 4 : 0); }

void U3DLoad::clearMaps(){
	viewNodeMap.clear();
	groupNodeMap.clear();
	modelNodeMap.clear();
	lightNodeMap.clear();
	pointSetMap.clear();
	lineSetMap.clear();
	meshMap.clear();
	lightResourceMap.clear();
	viewResourceMap.clear();
	litTextureShaderMap.clear();
	materialResourceMap.clear();
	textureResourceMap.clear();
}

void U3DLoad::parseBlock(std::ifstream& fileStream){
	std::streampos blockBegin = fileStream.tellg();
	bitStreamReader.setDataBlock(fileStream, 16);
	MLuint32 blocktype = readU32();
	MLuint32 dataSize = readU32();
	MLuint32 metaDataSize = readU32();
	MLuint64 blockSize = dataSize + getNumPaddingBytes(dataSize) + metaDataSize + getNumPaddingBytes(metaDataSize) + 12;
	fileStream.seekg(blockBegin.operator+(12));
	bitStreamReader.setDataBlock(fileStream, blockSize);
	fileStream.seekg(blockBegin.operator+(12));
	using namespace mlU3D;
	//std::cout << "parsing: " << std::hex << blocktype << " at: " << blockBegin << std::endl;
	switch (blocktype){
	case(BLOCKTYPE_FILEHEADER) :
		parseFileHeader(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_PRIORITYUPDATE) :
		parsePriorityUpdate(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_MODIFIERCHAIN) :
		parseModifierChain(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_LITTEXTURESHADER) : 
		parseLitTextureShader(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_MATERIALRESOURCE) :
		parseMaterialResource(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_MODELNODE) :
		parseModelNode(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_CLODMESHDECLARATION) :
		parseCLODMeshDeclaration(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_CLODBASEMESHCONTINUATION) :
		parseCLODMeshBaseContinuation(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_LIGHTNODE) :
		parseLightNode(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_LIGHTRESOURCE) :
		parseLightResource(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_VIEWRESOURCE) :
		parseViewResource(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_SHADINGMODIFIER):
		parseShadingModifier(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_GROUPNODE):
		parseGroupNode(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_VIEWNODE) :
		parseViewNode(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_TEXTUREDECLARATION) :
		parseTextureDeclaration(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_TEXTURECONTINUATION) :
		parseTextureContinuation(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_POINTSETDECLARATION) :
		parsePointSetDeclaration(fileStream, dataSize, metaDataSize);
		break;
	case(BLOCKTYPE_POINTSETCONTINUATION) :
		parsePointSetContinuation(fileStream, dataSize, metaDataSize);
		break;
	default:
		std::cerr << "unknown blocktype: " << std::hex << blocktype << " at: " << blockBegin << std::endl;
	}
	fileStream.seekg(blockBegin.operator+(blockSize));
}

void U3DLoad::writeDataToOutput(){
	this->_outU3DObject->clear();

	for (auto iter = this->viewNodeMap.begin(); iter != this->viewNodeMap.end(); iter++){
		this->_outU3DObject->viewNodes.push_back(iter->second);
	}
	for (auto iter = this->groupNodeMap.begin(); iter != this->groupNodeMap.end(); iter++){
		this->_outU3DObject->groupNodes.push_back(iter->second);
	}
	for (auto iter = this->modelNodeMap.begin(); iter != this->modelNodeMap.end(); iter++){
		this->_outU3DObject->modelNodes.push_back(iter->second);
	}
	for (auto iter = this->lightNodeMap.begin(); iter != this->lightNodeMap.end(); iter++){
		this->_outU3DObject->lightNodes.push_back(iter->second);
	}
	for (auto iter = this->pointSetMap.begin(); iter != this->pointSetMap.end(); iter++){
		this->_outU3DObject->pointSets.push_back(iter->second);
	}
	for (auto iter = this->lineSetMap.begin(); iter != this->lineSetMap.end(); iter++){
		this->_outU3DObject->lineSets.push_back(iter->second);
	}
	for (auto iter = this->meshMap.begin(); iter != this->meshMap.end(); iter++){
		this->_outU3DObject->meshes.push_back(iter->second);
	}
	for (auto iter = this->lightResourceMap.begin(); iter != this->lightResourceMap.end(); iter++){
		this->_outU3DObject->lightResources.push_back(iter->second);
	}
	for (auto iter = this->viewResourceMap.begin(); iter != this->viewResourceMap.end(); iter++){
		this->_outU3DObject->viewResources.push_back(iter->second);
	}
	for (auto iter = this->litTextureShaderMap.begin(); iter != this->litTextureShaderMap.end(); iter++){
		this->_outU3DObject->litTextureShaders.push_back(iter->second);
	}
	for (auto iter = this->materialResourceMap.begin(); iter != this->materialResourceMap.end(); iter++){
		this->_outU3DObject->materialResources.push_back(iter->second);
	}
	for (auto iter = this->textureResourceMap.begin(); iter != this->textureResourceMap.end(); iter++){
		this->_outU3DObject->textureResources.push_back(iter->second);
	}
}

void U3DLoad::parseFileHeader(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	MLint16 majorVersion = readI16();
	MLint16 minorVersion = readI16();
	MLuint32 profileIdentifier = readU32();
	MLuint32 declarationSize = readU32();
	MLuint64 newFileSize = readU64();
	MLuint32 characterEncoding = readU32();
	MLdouble unitsScalingFactor = readF64();
	this->fileSize = newFileSize;
}

void U3DLoad::parsePriorityUpdate(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	MLuint32 newPriority = readU32();
}

void U3DLoad::parseModifierChain(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	this->bitStreamReader.counter = 0;
	std::string chainName = readString();
	MLuint32 chainType = readU32();
	MLuint32 chainAttributes = readU32();
	if (chainAttributes & 0x1){
		MLfloat boundingSphereCenterX = readF32();
		MLfloat boundingSphereCenterY = readF32();
		MLfloat boundingSphereCenterZ = readF32();
		MLfloat boundingSphereRadius = readF32();
	}
	if (chainAttributes & 0x2){
		MLfloat alignedBoundingBoxMinX = readF32();
		MLfloat alignedBoundingBoxMinY = readF32();
		MLfloat alignedBoundingBoxMinz = readF32();
		MLfloat alignedBoundingBoxMaxX = readF32();
		MLfloat alignedBoundingBoxMaxY = readF32();
		MLfloat alignedBoundingBoxMaxZ = readF32();
	}
	/*fileStream.seekg(fileStream.tellg().operator+(2 + chainName.size() + 48));
	while (fileStream.tellg() % 4 != 0) fileStream.seekg(fileStream.tellg().operator+(1)); //padding
	*/
	while (this->bitStreamReader.counter % 4 != 0){
		readU8();
	}
	MLuint32 modifierCount = readU32();
	fileStream.seekg(fileStream.tellg().operator+(this->bitStreamReader.counter));
	for (int i = 0; i < modifierCount; i++){
		parseBlock(fileStream);
	}
}


//***********************************************************************************

void U3DLoad::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DGenerator::activateAttachments();
}


//***********************************************************************************


MLuint8 U3DLoad::readU8(){
	return this->bitStreamReader.readU8();
}

MLuint16 U3DLoad::readU16(){
	return this->bitStreamReader.readU16();
}

MLuint32 U3DLoad::readU32(){
	return this->bitStreamReader.readU32();
}

MLuint64 U3DLoad::readU64(){
	return this->bitStreamReader.readU64();
}

MLint16 U3DLoad::readI16(){
	return this->bitStreamReader.readI16();
}

MLint32 U3DLoad::readI32(){
	return this->bitStreamReader.readI32();
}

MLfloat U3DLoad::readF32(){
	return this->bitStreamReader.readF32();
}

MLdouble U3DLoad::readF64(){
	//return this->bitStreamReader.readUF64();
	return -1;
}

MLuint8 U3DLoad::readCompressedU8(MLuint32 context){
	return this->bitStreamReader.readCompressedU8(context);
}

MLuint16 U3DLoad::readCompressedU16(MLuint32 context){
	return this->bitStreamReader.readCompressedU16(context);
}

MLuint32 U3DLoad::readCompressedU32(MLuint32 context){
	return this->bitStreamReader.readCompressedU32(context);
}

std::string U3DLoad::readString(){
	std::string ret;
	MLuint16 length = readU16();
	for (MLuint16 i = 0; i < length; i++){
		MLuint8 buffer = readU8();
		ret.push_back(buffer);
	}
	return ret;
}
#pragma warning(pop)

ML_END_NAMESPACE