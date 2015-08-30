//----------------------------------------------------------------------------------
//! The ML module class SavePRC.
/*!
// \file    SavePRC.h
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PRC file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


#ifndef _SavePRC_H
#define _SavePRC_H


// Local includes
#include "MLPDFSystem.h"
#include "PRCFileFormat/PRC_DataTypes.h"
#include "PRCFileFormat/PRC_File.h"
#include "shared/MLPDF_DataTypes.h"

#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMInspector.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <ColoredMarkerList.h>
#include <IndexPairList.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT SavePRC : public WEMInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePRC)

public:

  //! Constructor.
  SavePRC (std::string type="SavePRC");

protected:

  //! Destructor.
  virtual ~SavePRC();

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

  // Fields for geometry definition
  StringField  *_pointCloudSpecificationFld;
  StringField  *_lineSetSpecificationFld;
  StringField  *_meshSpecificationFld;
  //StringField  *_lightsSpecificationFld;
  //StringField  *_viewsSpecificationFld;
  //StringField  *_metaDataSpecificationFld;

  // Fields for PRC file block identifier names 
  //StringField  *_defaultViewNameFld;
  //StringField  *_defaultLightNameFld;

  // Fields for default material values
  //ColorField   *_defaultMaterialAmbientColorFld;
  //ColorField   *_defaultMaterialDiffuseColorFld;
  //ColorField   *_defaultMaterialSpecularColorFld;
  //ColorField   *_defaultMaterialEmissiveColorFld;
  //FloatField   *_defaultMaterialAlphaFld;

  // Fields for default light values
  //ColorField   *_defaultLightColorFld;
  //DoubleField  *_defaultLightIntensityFld;

  // Other settings 
  //BoolField    *_defaultBoundingBoxMetaDataFld;
  //BoolField    *_addDefaultViewNodeFld;
  //BoolField    *_addDefaultLightNodeFld;  
  
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
	ml::ColoredMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::ColoredMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  // This vector contains all object related data
  PRCObjectInfoVector _prcObjectInfoVector;

  // All point sets geometries
  PointSetsVector _pointSetsGeometryVector;

  // All line sets geometries
  LineSetsVector _lineSetsGeometryVector;

  // The model tree
  PRCModelTreeNodeVector _modelTree;

  // This struct contains all default values
  PRCDefaultValuesStruct defaultValues;  


  /* METHODS */

  //! Handler method for Save button.
  void saveButtonClicked();
  //! Save PRC file into file stream.
  void savePRCToFileStream(std::ofstream& ofstream);


  // Standard PRC data methods =================================================

  // Add meta data
  //void addMetaData(PRCMetaDataVector &metaDataVector, PRCModelBoundingBoxStruct boundingBox);

  // Write model data of current tree node to file and continue with children nodes
  void WriteNodeModelsToPRCFile(PRCFile &outPRCFile, PRCModelTreeNode modelTreeNode);

  // Creates a new object info data struct
  PRCObjectInfoStruct CreateNewPRCObjectInfo(int objectIndex, PRCObjectType objectType, std::string objectName, PRCDefaultValuesStruct defaultValues);

  // Makes sure that the internal name of each PRC object is unique
  void makeInternalNameUnique(std::string& objectName);

  // Get default parameters
  //PRCDefaultValuesStruct getDefaultValuesFromFields();

  // PointSet processing methods ================================================

  // Scan all data from input field and collect base info for point sets.
  void PreProcessPointSetData(ModelBoundingBoxStruct& boundingBox);

  // Write all point sets of a model tree node to PRC file
  void WritePointSetDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode);


  // FiberSet processing methods ==============================================

  // Scan all data from input field and collect base info for line sets.
  void PreProcessLineSetData(ModelBoundingBoxStruct& boundingBox);

  // Write all line sets of a model tree node to PRC file
  void WriteLineSetDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode);


  // WEM processing methods ===================================================

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  void PreProcessMeshData(WEMPtr saveWEM, 
                          PRCMeshInfoVector &meshInfoVector,
	                        ModelBoundingBoxStruct& boundingBox);

  // Write all meshes of a model tree node to PRC file
  void WriteMeshDataToPRCFile(PRCFile &outPRCFile, MLint modelTreeNode);

  // Specification Generator methods =========================================

  void UpdateObjectTypeTabView();
  void AddNewSpecification();
  void UpdateNewSpecification();

  // Tool methods ============================================================

  // Parses the description string of the WEM to set PRC properties.
  //std::string getPRCParameterFromWEMDescription(const std::string wemDescription, const std::string parameter, const std::string failResult = "") const;

  // Get all PRC line connections from input connections field
  //LinesVector getAllPRCLinesFromConnectionsList(const ml::XMarkerList connectionsList, const std::string allowedConnectionTypes) const;

  // Automatically calculate simple line connections from input positions field
  //LinesVector getStandardPRCLinesFromPositionsList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes) const;

  // Get all PRC lines that end at a given position
  //LinesVector getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition) const;

  
};

ML_END_NAMESPACE

#endif // __mlSavePRC_H