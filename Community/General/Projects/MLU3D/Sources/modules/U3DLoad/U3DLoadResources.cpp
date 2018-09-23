#include "U3DLoad.h"

ML_START_NAMESPACE

void U3DLoad::parseLitTextureShader(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 attributes = readU32();
	MLfloat alphaTestReference = readF32();
	MLuint32 alphaTestFunction = readU32();
	MLuint32 colorBlendFunction = readU32();
	MLuint32 renderPassEnabledFlags = readU32();
	MLuint32 shaderChannels = readU32();
	MLuint32 alphaTextureChannels = readU32();
	std::string materialName = readString();
	std::string textureName = "";
	if (shaderChannels != 0x0){
		textureName = readString();
	}

	litTextureShaderMap[name].resourceName = name;
	litTextureShaderMap[name].materialResourceName = materialName;
	litTextureShaderMap[name].textureResourceName = textureName;
}

void U3DLoad::parseMaterialResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 materialAttributes = readU32();
	
	MLuint32 ambientR = readF32();
	MLuint32 ambientG = readF32();
	MLuint32 ambientB = readF32();

	MLuint32 diffuseR = readF32();
	MLuint32 diffuseG = readF32();
	MLuint32 diffuseB = readF32();

	MLuint32 specularR = readF32();
	MLuint32 specularG = readF32();
	MLuint32 specularB = readF32();

	MLuint32 emissiveR = readF32();
	MLuint32 emissiveG = readF32();
	MLuint32 emissiveB = readF32();

	MLfloat reflectivity = readF32();
	MLfloat opacity = readF32();

	materialResourceMap[name].resourceName = name;
	materialResourceMap[name].ambientColor = Vector3(ambientR,ambientG,ambientB);
	materialResourceMap[name].diffuseColor = Vector4(diffuseR, diffuseG, diffuseB, 1);
	materialResourceMap[name].specularColor = Vector3(specularR, specularG, specularB);
	materialResourceMap[name].emissiveColor = Vector3(emissiveR, emissiveG, emissiveB);
	materialResourceMap[name].reflectivity = reflectivity;
}

void U3DLoad::parseCLODMeshDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 chainIndex = readU32();

	//Max Mesh Description
	MLuint32 meshAttributes = readU32();
	MLuint32 faceCount = readU32();
	MLuint32 positionCount = readU32();
	MLuint32 normalCount = readU32();
	MLuint32 diffuseColorCount = readU32();
	MLuint32 specularColorCount = readU32();
	MLuint32 textureCoordCount = readU32();
	MLuint32 shadingCount = readU32();
	for (MLuint32 isha = 0; isha < shadingCount; isha++){
		MLuint32 shadingDescription = readU32();
		MLuint32 textureLayerCount = readU32();
		for (MLuint32 itlc = 0; itlc < textureLayerCount; itlc++){
			MLuint32 textureCoordDimensions = readU32();
		}
		MLuint32 originalShadingId = readU32();
		meshMap[name].shadingAttributes = shadingDescription;
		meshMap[name].textureLayerCount = textureLayerCount;
	}

	//CLOD Description
	MLuint32 minimumResolution = readU32();
	MLuint32 finalMaximumResolution = readU32();

	//Resource Description
	MLuint32 positionQualityFactor = readU32();
	MLuint32 normalQualityFactor = readU32();
	MLuint32 textureCoordQualityFactor = readU32();

	MLfloat positionInverseQuant = readF32();
	MLfloat normalInverseQuant = readF32();
	MLfloat textureCoordInverseQuant = readF32();
	MLfloat diffuseColorInverseQuant = readF32();
	MLfloat specularColorInverseQuant = readF32();

	MLfloat normalCreaseParameter = readF32();
	MLfloat normalUpdateParameter = readF32();
	MLfloat normalToleranceParamter = readF32();

	//Skeleton Description

	MLuint32 boneCount = readU32();
	if (boneCount != 0){
		std::cerr << "MLU3D doesnt support bone structures" << std::endl;
	}


	meshMap[name].resourceName = name;
	meshMap[name].vertexCount = finalMaximumResolution;
	meshMap[name].faceCount = faceCount;
	meshMap[name].patchID = 0;
	meshMap[name].meshNumber = meshMap.size();
	meshMap[name].normalCount = normalCount;
	meshMap[name].specularColorCount = specularColorCount;
	meshMap[name].diffuseColorCount = diffuseColorCount;
	meshMap[name].textureCoordCount = textureCoordCount;
	meshMap[name].meshAttributes = meshAttributes;
}

void U3DLoad::parseLightResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 lightAttributes = readU32();
	MLuint8 lightType = readU8();

	//Light Color
	MLfloat lightColorR = readF32();
	MLfloat lightColorG = readF32();
	MLfloat lightColorB = readF32();
	MLfloat lightReversedParameter= readF32();

	//Light Attenuation
	MLfloat lightAttenuationC = readF32();
	MLfloat lightAttenuationL = readF32();
	MLfloat lightAttenuationQ = readF32();

	MLfloat lightSpotAngle = readF32(); 
	MLfloat lightIntensity = readF32();

	lightResourceMap[name].resourceName = name;
	lightResourceMap[name].lightType = lightType;
	lightResourceMap[name].lightColor = Vector3(lightColorR, lightColorG, lightColorB);
	lightResourceMap[name].lightAttenuation = Vector3(lightAttenuationC, lightAttenuationL, lightAttenuationQ);
	lightResourceMap[name].lightSpotAngle = lightSpotAngle;
	lightResourceMap[name].lightIntensity = lightIntensity;
}

void U3DLoad::parseViewResource(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	viewResourceMap[name].resourceName = name;
}

void U3DLoad::parseCLODMeshBaseContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 chainIndex = readU32();

	//Base Mesh Description
	MLuint32 baseFaceCount = readU32();
	MLuint32 basePositionCount = readU32();
	MLuint32 baseNormalCount = readU32();
	MLuint32 baseDiffuseColorCount = readU32();
	MLuint32 baseSpecularColorCount = readU32();
	MLuint32 baseTextureCoordCount = readU32();

	//Base Mesh Data
	WEMTrianglePatch* baseGeometry = new WEMTrianglePatch();
	std::vector<WEMNode*> nodeVector;
	std::vector<std::pair<MLfloat, MLfloat>> textureCoordinates;
	std::vector<ml::Vector3> normalVector;


	for (MLuint32 i = 0; i < basePositionCount; i++){
		MLfloat basePositionX = readF32();
		MLfloat basePositionY = readF32();
		MLfloat basePositionZ = readF32();
		WEMNode* position = baseGeometry->addNode();
		position->setPosition(basePositionX,basePositionY,basePositionZ);
		nodeVector.push_back(position);
	}

	for (MLuint32 i = 0; i < baseNormalCount; i++){
		MLfloat baseNormalX = readF32();
		MLfloat baseNormalY = readF32();
		MLfloat baseNormalZ = readF32();
		normalVector.push_back(ml::Vector3(baseNormalX, baseNormalY, baseNormalZ));
	}

	for (MLuint32 i = 0; i < baseDiffuseColorCount; i++){
		MLfloat baseDiffuseColorR = readF32();
		MLfloat baseDiffuseColorG = readF32();
		MLfloat baseDiffuseColorB = readF32();
		MLfloat baseDiffuseColorA = readF32();
	}

	for (MLuint32 i = 0; i < baseSpecularColorCount; i++){
		MLfloat baseSpecularColorR = readF32();
		MLfloat baseSpecularColorG = readF32();
		MLfloat baseSpecularColorB = readF32();
		MLfloat baseSpecularColorA = readF32();
	}

	for (MLuint32 i = 0; i < baseTextureCoordCount; i++){
		MLfloat baseTexCoordU = readF32();
		MLfloat baseTexCoordV = readF32();
		MLfloat baseTexCoordS = readF32();
		MLfloat baseTexCoordT = readF32();
		textureCoordinates.push_back(std::pair<MLfloat, MLfloat>(baseTexCoordU, baseTexCoordV));
	}

	for (MLuint32 i = 0; i < baseFaceCount; i++){
		MLuint32 shadingID = readCompressedU32(mlU3D::Context_cShading);
		WEMFace* currentFace = baseGeometry->addFace();
		for (MLuint32 j = 0; j < 3; j++){
			MLuint32 basePositionIndex = readCompressedU32(mlU3D::Context_StaticFull + basePositionCount);
			//std::cout << basePositionIndex << " ";
			if (meshMap[name].meshAttributes != 0x1){
				MLuint32 baseNormalIndex = readCompressedU32(mlU3D::Context_StaticFull + baseNormalCount);
				baseGeometry->getNodeAt(basePositionIndex)->setNormal(normalVector[baseNormalIndex]);
			}
			if (meshMap[name].shadingAttributes & 0x1){
				MLuint32 baseDiffuseColorIndex = readCompressedU32(mlU3D::Context_StaticFull + baseDiffuseColorCount);
			}
			if (meshMap[name].shadingAttributes & 0x2){
				MLuint32 baseSpecularColorIndex = readCompressedU32(mlU3D::Context_StaticFull + baseSpecularColorCount);
			}
			for (MLuint32 k = 0; k < meshMap[name].textureLayerCount; k++){
				MLuint32 baseTextureCoordIndex = readCompressedU32(mlU3D::Context_StaticFull + baseTextureCoordCount);
				std::pair<MLfloat, MLfloat> currentCoord = textureCoordinates[baseTextureCoordIndex];
				meshMap[name].textureCoordinates.push_back(currentCoord);
			}
			currentFace->setNode(j, nodeVector[basePositionIndex]);
		}
		//std::cout << std::endl;
	}
	meshMap[name].setMeshData(baseGeometry);

	/*std::cout << bitStreamReader.printState() << std::endl;

	if (dataSize % 4){
		for (MLint32 i = 0; i < 4 - dataSize % 4; i++){
			MLuint8 padding = readU8();
		}
	}*/
	//bitStreamReader.resetCompression();

	
}

void U3DLoad::parseTextureDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();

	//Texture Image Format
	MLuint32 imageHeight = readU32();
	MLuint32 imageWidth = readU32();
	MLuint8 imageType = readU8();
	if (imageType != 0x0F){
		std::cerr << "Texture Image Format not (yet) supported!" << std::endl;
	}

	textureResourceMap[name].resourceName = name;
	textureResourceMap[name].width = imageWidth;
	textureResourceMap[name].height = imageHeight;

	MLuint32 continuationImageCount = readU32();

	//Continueation Image Format
	for (MLuint32 iter = 0; iter < continuationImageCount; iter++){
		MLuint8 compressionType = readU8();
		if (compressionType != 0x02){
			std::cerr << "image file format not (yet) supported" << std::endl;
		}
		MLuint8 imageChannels = readU8();
		MLuint16 imageAttributes = readU16();
		if (imageAttributes != 0){
			std::cerr << "external continuation image file reference not supported!" << std::endl;
		}
		MLuint32 imageDataByteCount = readU32();
		textureResourceMap[name].pictureData.resize(imageDataByteCount);
	}


}

void U3DLoad::parseTextureContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 index = readU32();
	auto target = textureResourceMap.find(name);
	for (MLuint32 i = 0; i < target->second.pictureData.size(); i++){
		MLuint8 inputByte = readU8();
		target->second.pictureData[i] = inputByte;
	}
}

void U3DLoad::parsePointSetDeclaration(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 chainIndex = readU32();
	MLuint32 pointSetReserved = readU32();
	MLuint32 pointCount = readU32();
	MLuint32 positionCount = readU32();
	MLuint32 normalCount = readU32();
	MLuint32 diffuseColorCount = readU32();
	MLuint32 specularColorCount = readU32();
	MLuint32 textureCoordCount = readU32();
	MLuint32 shadingListCount = readU32();

	//Ressource Description

	pointSetMap[name].resourceName = name;
	pointSetMap[name].pointCount = pointCount;
	pointSetMap[name].normalCount = normalCount;
	pointSetMap[name].diffuseColorCount = diffuseColorCount;
	pointSetMap[name].specularColorCount = specularColorCount;
	pointSetMap[name].textureCoordCount = textureCoordCount;
}

void U3DLoad::parsePointSetContinuation(std::ifstream& fileStream, MLuint32 dataSize, MLuint32 MetaDataSize){
	std::string name = readString();
	MLuint32 chainIndex = readU32();

	//Point Resolution Range

	MLuint32 startRes = readU32();
	MLuint32 endRes = readU32();

	//Point Description
	auto& pointSet = pointSetMap.find(name)->second;
	pointSet.positions.resize(endRes - startRes);
	for (MLuint32 i = 0; i < endRes - startRes; i++){
		MLint32 splitPointIndex = 0; 
		if (i == 0){
			splitPointIndex = readCompressedU32(mlU3D::Context_StaticFull + 1);
		}
		else {
			splitPointIndex = readCompressedU32(mlU3D::Context_StaticFull + i);
		}
		MLuint8 signsXYZ = readCompressedU8(mlU3D::Context_cPosDiffSign);          
		MLuint32 diffX = readCompressedU32(mlU3D::Context_cPosDiffX);
		MLuint32 diffY = readCompressedU32(mlU3D::Context_cPosDiffY);
		MLuint32 diffZ = readCompressedU32(mlU3D::Context_cPosDiffZ);
		Vector3 posDiff;
		mlU3D::U3DTools::dequantizePosition(posDiff, signsXYZ, diffX, diffY, diffZ);
		Vector3 predictedPosition; 
		if (splitPointIndex >= 0){
			predictedPosition = pointSet.positions[splitPointIndex].position;
		}
		mlU3D::PositionStruct posstruct; 
		posstruct.position = predictedPosition + posDiff;
		pointSet.positions[i] = posstruct;
		MLuint32 newNormalCnt = readCompressedU32(mlU3D::Context_cNormlCnt);
		MLuint32 newPointCount = readCompressedU32(mlU3D::Context_cPointCnt_cLineCnt_cFaceCnt);
		for (MLuint32 newPointInfoNumber = 0; newPointInfoNumber < newPointCount; newPointInfoNumber++)
		{
			MLuint32 shadingID = readCompressedU32(mlU3D::Context_cShading);
			MLuint32 normalIndex = readCompressedU32(mlU3D::Context_cNormlIdx);
		}
	}
	
}

ML_END_NAMESPACE