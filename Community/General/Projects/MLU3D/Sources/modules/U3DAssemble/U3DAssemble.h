//----------------------------------------------------------------------------------
// U3DAssemble module.
//
// \file    U3DAssemble.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Assembles a U3D object from WEMs, Linesets and XMarkers.
//----------------------------------------------------------------------------------


#ifndef _U3DAssemble_H
#define _U3DAssemble_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_DataTypes.h"
#include "../../shared/U3DInternalFormat/U3D_Object.h"
#include "../../shared/U3DFileFormat/U3D_FileWriter.h"

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


//////////////////////////////////////////////////////////////////////////
//! Assembles a U3D object from WEMs, Linesets and XMarkers.
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

  // Fields for geometry definition
  StringField  *_pointCloudSpecificationFld;
  StringField  *_lineSetSpecificationFld;
  StringField  *_meshSpecificationFld;
  StringField  *_lightsSpecificationFld;
  StringField  *_viewsSpecificationFld;
  StringField  *_metaDataSpecificationFld;

  // Fields for U3D file block identifier names 
  StringField  *_defaultViewNameFld;
  StringField  *_defaultLightNameFld;

  // Fields for default material values
  StringField  *_defaultMaterialNameFld;
  ColorField   *_defaultMaterialAmbientColorFld;
  ColorField   *_defaultMaterialDiffuseColorFld;
  ColorField   *_defaultMaterialSpecularColorFld;
  ColorField   *_defaultMaterialEmissiveColorFld;
  DoubleField  *_defaultMaterialAlphaFld;
  DoubleField  *_defaultMaterialReflectivityFld;

  // Fields for default light values
  ColorField   *_defaultLightColorFld;
  DoubleField  *_defaultLightIntensityFld;

  // Other settings 
  BoolField    *_addDefaultBoundingBoxMetaDataFld;
  BoolField    *_addDefaultViewNodeFld;
  BoolField    *_addDefaultLightNodeFld;  
  
  //! Status & progress fields
  StringField   *_statusFld;
  StringField   *_networkPanelStatusFld;

  ProgressField *_progressFld;
  BoolField     *_successFld;
  NotifyField   *_finishedFld;


  //! The XMarkerList input for point positions of point clouds
  ml::XMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  /* METHODS */

  //! Save U3D (ECMA-363 Universal 3D Format) file into file stream.
  void _assembleU3DObject();


  // Standard U3D data methods =================================================

  void _addDefaultViewNode();
  void _addDefaultLightNode();

  void _addDefaultLightResource();
  void _addDefaultViewResource();
  void _addMetaData();

  // Tool methods =============================================================

  // Default parameters
  void _updateDefaultValues();

  // PointSet processing methods ================================================

  // Scan all data from input field and collect base info for point sets.
  void _addPointSetModelData();

  // FiberSet processing methods ==============================================

  // Scan all data from input field and collect base info for line sets.
  void _addLineSetModelData();


  // WEM processing methods ===================================================

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  void _addMeshModelData();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAssemble_H