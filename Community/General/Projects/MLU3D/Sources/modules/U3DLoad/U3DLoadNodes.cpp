#include "U3DLoad.h"

ML_START_NAMESPACE

void U3DLoad::parseModelNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string nodeName = readString();
	MLuint32 parentNodeCount = readU32();
	std::string parentNodeName;
	for (MLuint32 i = 0; i < parentNodeCount; i++){
		parentNodeName = readString();
		for (MLuint32 j = 0; j < 16; j++){
			MLfloat transformationMatrixMember = readF32();
		}
	}
	std::string modelRessourceName = readString();
	MLuint32 modelVisibility = readU32();
	modelNodeMap[nodeName].displayName = nodeName;
	modelNodeMap[nodeName].geometryGeneratorName = modelRessourceName;
	modelNodeMap[nodeName].internalName = nodeName;
	modelNodeMap[nodeName].nodeName = nodeName;
	modelNodeMap[nodeName].visibility = modelVisibility;
	modelNodeMap[nodeName].groupPath = parentNodeName;
	//std::cout << "node: " << nodeName << " res: " << modelRessourceName << " parent: " << parentNodeName<< std::endl;
	currentChainNode = nodeName;
}

void U3DLoad::parseLightNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string nodeName = readString();
	MLuint32 parentNodeCount = readU32();
	std::string parentNodeName;
	for (MLuint32 i = 0; i < parentNodeCount; i++){
		parentNodeName = readString();
		for (MLuint32 j = 0; j < 16; j++){
			MLfloat transformationMatrixMember = readF32();
		}
	}
	std::string resourceName = readString();
	lightNodeMap[nodeName].displayName = nodeName;
	lightNodeMap[nodeName].nodeName = nodeName;
	lightNodeMap[nodeName].resourceName = resourceName;
	currentChainNode = nodeName;
}

void U3DLoad::parseViewNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string nodeName = readString();
	MLuint32 parentNodeCount = readU32();
	std::string parentNodeName;
	for (MLuint32 i = 0; i < parentNodeCount; i++){
		parentNodeName = readString();
		for (MLuint32 j = 0; j < 16; j++){
			MLfloat transformationMatrixMember = readF32();
		}
	}
	std::string viewResourceName = readString();
	MLuint32 viewNodeAttributes = readU32();
	MLfloat viewNearClip = readF32();
	MLfloat viewFarClip = readF32();

	if (viewNodeAttributes & 0x4){
		MLfloat viewProjectionVectorX = readF32();
		MLfloat viewProjectionVectorY = readF32();
		MLfloat viewProjectionVectorZ = readF32();
	}
	else if (viewNodeAttributes & 0x2){
		MLfloat viewOrthographicHeight = readF32();
	}
	else {
		MLfloat viewProjection = readF32();
	}

	viewNodeMap[nodeName].displayName = nodeName;
	viewNodeMap[nodeName].resourceName = nodeName;
	viewNodeMap[nodeName].resourceName = viewResourceName;
}

void U3DLoad::parseGroupNode(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string nodeName = readString();
	MLuint32 parentNodeCount = readU32();
	std::string parentNodeName;
	for (MLuint32 i = 0; i < parentNodeCount; i++){
		parentNodeName = readString();
		for (MLuint32 j = 0; j < 16; j++){
			MLfloat transformationMatrixMember = readF32();
		}
	}
	groupNodeMap[nodeName].displayName = nodeName;
	groupNodeMap[nodeName].nodeName = nodeName;
	groupNodeMap[nodeName].parentName = parentNodeName;
}

void U3DLoad::parseShadingModifier(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string modName = readString();
	MLuint32 chainIndex = readU32();
	MLuint32 shadingAttributes = readU32();
	MLuint32 shaderListCount = readU32();
	MLuint32 shaderCount = 0;
	std::string shaderName;
	for (MLuint32 i = 0; i < shaderListCount; i++){
		shaderCount = readU32();
		for (MLuint32 j = 0; j < shaderCount; j++){
			shaderName = readString();
		}
	}
	modelNodeMap[currentChainNode].shaderName = shaderName;
}


ML_END_NAMESPACE