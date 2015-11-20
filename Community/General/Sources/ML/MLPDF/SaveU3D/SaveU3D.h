//----------------------------------------------------------------------------------
//! The ML module class SaveU3D.
/*!
// \file    SaveU3D.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
//
// Creates U3D file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


#ifndef _SaveU3D_H
#define _SaveU3D_H


// Local includes
#include "MLPDFSystem.h"
#include "U3DFileFormat/U3D_Constants.h"
#include "U3DFileFormat/U3D_FileWriter.h"
#include "../shared/MLPDF_DataTypes.h"

#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMInspector.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLPDF_EXPORT SaveU3D : public WEMInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SaveU3D)

public:

  //! Constructor.
  SaveU3D (std::string type="SaveU3D");

protected:

  //! Destructor.
  virtual ~SaveU3D();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! _process()-routine for correct processing.
  virtual void _process();

private:

  /* FIELDS */

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
  
  //! Field - Filename
  StringField   *_mlFileNameFld;
  //! Field - Save notification
  NotifyField   *_mlSaveFld;
  //! A status field.
  StringField   *_statusFld;
  //! Progress bar.
  ProgressField *_progressFld;

  //! Field for Specification Generator
  StringField   *_newSpecificationFld;
  IntField      *_newSpecificationSelectedTabFld;
  BoolField     *_newSpecificationOutputValidFld;
  NotifyField   *_newSpecificationAddFld;
  EnumField     *_newSpecificationTypeFld;
  StringField   *_newSpecificationObjectNameFld;
  StringField   *_newSpecificationGroupPathFld;
  BoolField     *_newSpecificationUseDefaultColorFld;
  BoolField     *_newSpecificationUseDefaultSpecularColorFld;
  ColorField    *_newSpecificationColorFld;
  FloatField    *_newSpecificationColorAlphaFld;
  ColorField    *_newSpecificationSpecularColorFld;
  EnumField     *_newSpecificationModelVisibilityFld;
  StringField   *_newSpecificationMetaDataKeyFld;
  StringField   *_newSpecificationMetaDataValueFld;
  StringField   *_newSpecificationWEMLabelFld;
  StringField   *_newSpecificationPositionTypesFld;
  StringField   *_newSpecificationConnectionTypesFld;

  //! The XMarkerList input for point positions of point clouds
	ml::XMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  // This vector contains all object related data
  U3DObjectInfoVector _u3dObjectInfoVector;

  // All point sets geometries
  PointSetsVector _pointSetsGeometryVector;

  // All line sets geometries
  LineSetsVector _lineSetsGeometryVector;

  // Pointer to the U3D FileWriter
  U3DFileWriter* outU3DFile;               

  // This struct contains all default values
  U3DDefaultValuesStruct defaultValues;  

  /* METHODS */

  //! Handler method for Save button.
  void saveButtonClicked();
  //! Save U3D (ECMA-363 Universal 3D Format) file into file stream.
  void saveU3DToFileStream(std::ofstream& ofstream);


  // Standard U3D data methods =================================================

  // Adds model nodes to U3D file stream
  std::string addAllModelGroupNodes(StringVector &modelNames) const;

  // Adds group nodes to U3D file stream
  void addAllGroupNodes(StringVector &groupNames, std::string modelGroupName, size_t modelGroupCount) const;
  void addAllGroupNodes(GroupNodeVector groupNodes) const;

  // Add a model node for each U3D object 
  void addU3DModelNodes() const;

  // Add default view node
  void addDefaultViewNode() const;

  // Add default light node
  void addDefaultLightNode() const;

  // Add shader & material resource for each U3D object 
  void addShaderAndMaterialResources() const;

  // Add light resources
  void addLightResources() const;

  // Add view resources
  void addViewResources() const;

  // Add meta data
  void addMetaData(MetaDataVector &metaDataVector, ModelBoundingBoxStruct boundingBox);

  // Creates a new object info data struct
  U3DObjectInfoStruct CreateNewU3DObjectInfo(int objectIndex, U3DObjectType objectType, std::string objectName, U3DDefaultValuesStruct defaultValues);

  // Makes sure that the internal name of each U3D object is unique
  void makeInternalNameUnique(std::string& objectName);

  // Re-create the full group path from components
  std::string getFullGroupPathFromU3DGroupNodeStruct(GroupNodeStruct thisNode);

  // Makes sure that the names of the groups in the group paths are unique
  void makeGroupPathNamesUnique(GroupNodeVector& groupNodes, U3DObjectInfoVector &_u3dObjectInfoVector);

  // Get default parameters
  U3DDefaultValuesStruct getDefaultValuesFromFields();

  // PointSet processing methods ================================================

  // Scan all data from input field and collect base info for point sets.
  void PreProcessPointSetData(U3DPointSetInfoVector &U3DPointSetInfoVector,
    	                        ModelBoundingBoxStruct& boundingBox);

  // Add a point set modifier chain for each point set
  void AddAllPointSetModifierChains(U3DPointSetInfoVector positionsInfoVector);

  // Create Point Set Continuation Block
  U3DDataBlockWriter CreatePointSetContinuationBlock(PointSetSpecificationStruct pointSetGeometry, U3DPointSetInfoStruct positionsInfo);



  // FiberSet processing methods ==============================================

  // Scan all data from input field and collect base info for line sets.
  void PreProcessLineSetData(U3DLineSetInfoVector &U3DLineSetInfoVector,
                             ModelBoundingBoxStruct& boundingBox);

  // Add a line set declaration modifier chain for each FiberSetContainer
  void AddAllLineSetModifierChains(U3DLineSetInfoVector U3DLineSetInfoVector);

  // Create Line Set Continuation Block
  U3DDataBlockWriter CreateLineSetContinuationBlock(LineSetSpecificationStruct lineSetGeometry, U3DLineSetInfoStruct lineSetInfo);


  // WEM processing methods ===================================================

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  void PreProcessMeshData(WEMPtr saveWEM, 	                                             
					                U3DMeshInfoVector &meshInfoVector,
                          ModelBoundingBoxStruct& boundingBox);

  // Add a CLOD mesh declaration modifier chain for each WEM patch
  void AddAllCLODMeshModifierChains(U3DMeshInfoVector meshInfoVector, WEMPtr saveWEM) const;

  // Add a CLOD base mesh continuation block for each WEM patch
  void AddAllCLODBaseMeshContinuationBlocks(WEMPtr saveWEM, U3DMeshInfoVector meshInfoVector) const; 

  // Create CLOD mesh Continuation Blocks
  U3DDataBlockWriter CreateCLODBaseMeshContinuationBlock(WEMPtr saveWEM, U3DMeshInfoStruct meshInfo) const;  
  //U3DDataBlockWriter CreateCLODProgressiveMeshContinuationBlock(WEMPtr saveWEM, U3DMeshInfoStruct meshInfo) const;  

  // Specification Generator methods =========================================

  void UpdateObjectTypeTabView();
  void AddNewSpecification();
  void UpdateNewSpecification();

  // Tool methods ============================================================




  // Debug, development & test methods =======================================


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _SaveU3D_H