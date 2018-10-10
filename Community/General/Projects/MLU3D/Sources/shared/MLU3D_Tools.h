//----------------------------------------------------------------------------------
// Tools for U3D file handling.
//
// \file    MLU3D_Tools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _MLU3D_Tools_H
#define _MLU3D_Tools_H 

// Local includes
#include "../MLU3DSystem.h"
#include "MLU3D_DataTypes.h"
#include "U3DInternalFormat/U3D_Object.h"
#include "U3DInternalFormat/U3D_Object_DataTypes.h"

// Project includes
#include <IndexPairList.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <mlFiberSet.h>

// Inventor includes
#include <Inventor/nodes/SoNodes.h>  // for M_PI

// WEM includes
#include <WEMBase/WEM.h>

// Std includes
#include <sys/stat.h>

ML_START_NAMESPACE

namespace mlU3D {

  // Inlines ================================================================

  inline std::string floatToString(float floatValue)
  {
    std::ostringstream buffer;
    buffer << floatValue;
    return buffer.str();
  }

  inline std::string MLintToString(MLint intValue)
  {
    std::ostringstream buffer;
    buffer << intValue;
    return buffer.str();
  }

  inline std::string intToString(int intValue)
  {
    std::ostringstream buffer;
    buffer << intValue;
    return buffer.str();
  }

  inline MLdouble stringToDouble(std::string sourceString)
  {
    return atof(sourceString.c_str());
  }

  inline MLdouble stringToMLDouble(std::string sourceString)
  {
    return (MLdouble)stringToDouble(sourceString);
  }

  inline int stringToInt(std::string sourceString)
  {
    int result = atoi(sourceString.c_str());

    return result;
  }

  inline bool fileExists(std::string& filename)
  {
    struct stat fileCheckBuffer;

    return (stat (filename.c_str(), &fileCheckBuffer) == 0);
  }

  inline float degToRad(float degrees)
  {
    return (degrees/180.0f) * M_PI;
  }

  inline float radToDeg(float radians)
  {
    return (radians / M_PI * 180.0);
  }

  inline float roundWithPrecision(float value, int decimals)
  {
    return floor(value * pow(10, decimals)) / pow(10, decimals);
  }

  // U3DTools class ============================================================

  class MLU3D_EXPORT U3DTools
  {

  public:

    // Tool methods ============================================================

    // Quantize position (vertex)
    static void quantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);

    // Quantize normal
    static void quantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);

	// Reverse position Quantization
	static void dequantizePosition(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);

	// Reverse normal Quantization
	static void dequantizeNormal(Vector3& pos, MLuint8& u8Signs, MLuint32& udX, MLuint32& udY, MLuint32& udZ);

    // Split a string
    static StringVector stringSplit(const std::string& stringToSplit, const std::string& delimiter, const bool keepEmpty, const bool trimWhitespace = true);

    // Convert String to lower case
    static void stringLower(std::string &sourceString);

    // Converts a string to bool
    static bool stringToBool(std::string sourceString);

    // Trims whitespace characters from string
    static std::string stringTrimWhitespace(std::string sourceString);

    // Convert MeVisLab version number into string
    static std::string getMeVisLabVersionNumberString();

    // Return string with library's version number
    static std::string getLibraryVersionNumberString();

    // Get a file size
    static long getFileSize(std::string filename);

    //***********************************************************************************

    // Parses a string and try to get color values from it.
    static Vector4 getColorVec4FromString(std::string colorString, const Vector4 defaultColor = Vector4(0));
    static Vector3 getColorVec3FromString(std::string colorString, const Vector3 defaultColor = Vector3(0));
    static Vector3 getVec3FromString(std::string vectorString, const Vector3 defaultValue = Vector3(0));

    // Converts a (color) Vector to a color string.
    static std::string formatColorString(Vector3 colorValue);
    static std::string formatColorString(Vector3 colorValue, float alphaValue);
    static std::string formatVec3String(Vector3 vectorValue);

    // Converts float/double to string.
    static std::string formatDouble(double value);
    static std::string formatFloat(float value);
    static std::string formatUInt(unsigned int value, unsigned int length);

    // Converts std::tm to string.
    static std::string formatDate(std::tm value);

    //***********************************************************************************

    // Makes sure that the internal name of each U3D object is unique
    static void makeInternalNameUnique(std::string& objectName, U3DObjectPtr u3dObject);

    // Creates a new ModelNode
    static ModelNode createNewModelNode(const ObjectSpecificationMap& specification, U3DObjectPtr u3dObject);

    // Get model specs from fiberset container
    static StringVector getLineSetSpecificationsStringFromFiberSetContainer(FiberSetContainer& fiberSetContainer);

    // Get model specs from WEM attributes
    static StringVector getMeshSpecificationsStringFromWEM(ml::WEM* wem);

    // Get model specs from (point cloud, line set, mesh) specification fields
    static StringVector getModelSpecificationsStringFromUI(ml::StringField *inputField, std::string delimiter);

    // Parses the specification string from the UI and extracts a specific U3D property
    static std::string getSpecificParameterFromString(const std::string specificationString, const std::string parameterKeyword, const std::string failResult = "");

    // Parses input string from UI and extracts object specification parameters
    static ObjectSpecificationMap getAllSpecificationParametersFromString(const std::string specificationString);

    // Parses the description string of the WEM to set U3D properties.
    static std::string getSpecificParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult = "");

    // Get parent node name from the group name path
    static std::string getParentNameFromGroupPath(std::string groupPath);

    // Make sure each group path has a leading and a trailing "/"
    static std::string normalizeGroupPath(std::string groupPath);

    //***********************************************************************************

    // Update geometry generator mapping
    static std::string updateGeometryGeneratorMap(mlU3D::GeometryGeneratorMap& geometryGeneratorMap, std::string specificationString, std::string namePrefix, int startIndex = 0);

    // Add point set models and geometry generators according to specification vector
    static void addPointSetModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, const ml::XMarkerList& pointPositions);

    // Add line set models and geometry generators according to specification vector
    static void addLineSetModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, const ml::XMarkerList& linePositions, ml::IndexPairList& lineConnections);
    static void addLineSetModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, const FiberSetContainer& fibers);

	// Construct a WEM representing a single quad mesh with given coordinates
	static WEM* constructQuadWem(Vector3 point_a, Vector3 point_b, Vector3 point_c, Vector3 point_d);

    // Add mesh models and geometry generators according to specification vector
    static void addMeshModelAndGeometry(const mlU3D::StringVector& specificationsVector, U3DObjectPtr u3dObject, WEM* inWEM, std::string geometryGeneratorPrefix = "");

    // Scan all node colors of a WEM patch and return number of different colors
    static MLuint32 getNumberOfDifferentColorsFromWEMPatch(WEMTrianglePatch* patch);

    // Updates the model bounding box
    static void updateBoundingBox(ModelBoundingBoxStruct& boundingBox, ModelBoundingBoxStruct newCorners);

    // Get bounding box edges from positions
    static ModelBoundingBoxStruct getBoundingBoxFomPositions(PositionsVector positions);

	// Set the shader of a node to render the given material and texture
	static void setNodeShaderToTexture(U3DObjectPtr u3DObject, std::string nodeName, std::string textureName, std::vector<std::pair<MLfloat, MLfloat>> &textureMap);

    //***********************************************************************************



    //////////////////////////////////////////////

  };

} // end namespace mlU3D

ML_END_NAMESPACE

#endif // _MLU3D_Tools_H