//----------------------------------------------------------------------------------
// U3DAssemble module.
//
// \file    U3DAssemble.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Assembles a U3D object.
//----------------------------------------------------------------------------------


#ifndef _U3DAssemble_H
#define _U3DAssemble_H


// Local includes
#include "MLU3DSystem.h"
#include "../shared/MLU3D_Constants.h"
#include "../shared/MLU3D_DataTypes.h"
#include "../shared/U3DInternalFormat/U3D_Object.h"
#include "../shared/U3DFileFormat/U3D_FileWriter.h"

// Project includes
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>

// WEM includes
#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMInspector.h>


ML_START_NAMESPACE


struct DefaultValuesStruct
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

enum ModelType { MODELTYPE_POINTSET, MODELTYPE_LINESET, MODELTYPE_MESH /*, MODELTYPE_GLYPH*/ };

struct ObjectInfoStruct
{
  ModelType     Type;
  std::string          DisplayName;
  std::string          InternalName;
  std::string          GroupPath;
  std::string          ShaderName;
  std::string          MaterialName;
  std::string          ResourceName;
  Vector4              DiffuseColor;
  Vector3              SpecularColor;
  Vector3              AmbientColor;
  Vector3              EmissiveColor;
  bool                 UseVertexColors;
  MLuint32             Visibility;
};

typedef std::vector<ObjectInfoStruct> U3DObjectsVector;


//////////////////////////////////////////////////////////////////////////
//! Creates U3D object from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAssemble : public WEMInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DAssemble)

public:

  //! Constructor.
  U3DAssemble(std::string type = "U3DAssemble");

protected:

  //! Destructor.
  virtual ~U3DAssemble();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! _process()-routine for correct processing.
  virtual void _process();

private:

  /* FIELDS */
  BaseField *_outU3DObjectFld;

  //! A pointer to the output U3D object.
  mlU3D::U3DObjectPtr _outU3DObject;


  // Input fields for point sets and line sets
  BaseField *_inPointPositionsFld;
  BaseField *_inLinePositionsFld;
  BaseField *_inLineConnectionsFld;

  // Fields for selecting simple, straightforward mode
  BoolField *_simpleModePointSetFld;
  BoolField *_simpleModeLineSetFld;
  BoolField *_simpleModeMeshFld;

  // Fields for default model prefixes
  StringField  *_modelPrefixPointCloudsFld;
  StringField  *_modelPrefixLineSetsFld;
  StringField  *_modelPrefixMeshesFld;
  //StringField  *_modelPrefixGlyphsFld;         // Not supported by Acrobat.

  // Fields for geometry definition
  StringField  *_pointCloudSpecificationFld;
  StringField  *_lineSetSpecificationFld;
  StringField  *_meshSpecificationFld;
  //StringField  *_glyphSpecificationFld;        // Not supported by Acrobat.
  StringField  *_lightsSpecificationFld;
  StringField  *_viewsSpecificationFld;
  StringField  *_metaDataSpecificationFld;

  // Fields for U3D file block identifier names 
  StringField  *_defaultViewNameFld;
  StringField  *_defaultLightNameFld;

  // Fields for default material values
  ColorField   *_defaultMaterialAmbientColorFld;
  ColorField   *_defaultMaterialDiffuseColorFld;
  ColorField   *_defaultMaterialSpecularColorFld;
  ColorField   *_defaultMaterialEmissiveColorFld;
  FloatField   *_defaultMaterialAlphaFld;

  // Fields for default light values
  ColorField   *_defaultLightColorFld;
  DoubleField  *_defaultLightIntensityFld;

  // Other settings 
  BoolField    *_defaultBoundingBoxMetaDataFld;
  BoolField    *_addDefaultViewNodeFld;
  BoolField    *_addDefaultLightNodeFld;  
  
  //! Status & progress fields
  StringField   *_statusFld;
  ProgressField *_progressFld;
  BoolField     *_successFld;
  NotifyField   *_finishedFld;


  //! The XMarkerList input for point positions of point clouds
  ml::XMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  // This vector contains all object related data
  U3DObjectsVector _allU3DObjects;

  // This struct contains all default values
  DefaultValuesStruct defaultValues;

  /* METHODS */

  //! Save U3D (ECMA-363 Universal 3D Format) file into file stream.
  void _assembleU3DObject();


  // Standard U3D data methods =================================================

  void _addDefaultViewNode();
  void _addAllGroupNodes();
  void _addAllModelNodes();
  void _addDefaultLightNode();

  void _addDefaultLightResource();
  void _addDefaultViewResource();
  void _addMetaData();

  // Creates a new object info data struct
  ObjectInfoStruct _createNewU3DObjectInfo(int objectIndex, ModelType objectType, std::string objectName, DefaultValuesStruct defaultValues);

  // Makes sure that the internal name of each U3D object is unique
  void _makeInternalNameUnique(std::string& objectName, U3DObjectsVector& u3dObjects);

  // Re-create the full group path from components
  std::string _getFullGroupPathFromU3DGroupNodeStruct(mlU3D::GroupNodeStruct thisNode);

  // Makes sure that the names of the groups in the group paths are unique
  void _makeGroupPathNamesUnique(mlU3D::GroupNodeStructVector& groupNodes, U3DObjectsVector& u3dObjects);

  // Scans through all U3D object infos and assembles group nodes
  mlU3D::GroupNodeStructVector _assembleU3DGroupNodeInfo(U3DObjectsVector& u3dObjects);

  // Get default parameters
  DefaultValuesStruct _getDefaultValuesFromFields();

  // PointSet processing methods ================================================

  // Scan all data from input field and collect base info for point sets.
  void _preProcessPointSetData();

  // FiberSet processing methods ==============================================

  // Scan all data from input field and collect base info for line sets.
  void _preProcessLineSetData();


  // WEM processing methods ===================================================

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  void _preProcessMeshData(WEMPtr saveWEM);

  // Scan all node colors of a WEM patch and return number of different colors.
  MLuint32 _getNumberOfDifferentColorsFromWEMPatch(WEMTrianglePatch* patch) const;

  // Writes all vertex colors of a WEM patch to the continuation block
  mlU3D::ColorMap _writeVertexColors(WEMPatch* patch, U3DDataBlockWriter& continuationBlock) const;


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAssemble_H