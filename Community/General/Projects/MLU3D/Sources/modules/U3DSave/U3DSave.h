//----------------------------------------------------------------------------------
// U3DSave module.
//
// \file    U3DSave.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Saves a U3D to disk.
//----------------------------------------------------------------------------------

#ifndef _U3DSave_H
#define _U3DSave_H


// Local includes
#include "MLU3DSystem.h"
#include "abstract/BaseModules/U3DInspector.h"
#include "../shared/U3DFileFormat/U3D_FileWriter.h"

// WEM includes
#include <WEMBase/WEM.h>


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DSave : public U3DInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DSave)

public:

  //! Constructor.
  U3DSave();

protected:

  //! Destructor.
  virtual ~U3DSave();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification(Field* field);

private:

  /* FIELDS */

  //! Field - Filename
  StringField   *_fileNameFld;

  //! Field - Save notification
  NotifyField   *_saveFld;

  //! Status & progress fields
  StringField   *_statusFld;
  ProgressField *_progressFld;
  BoolField     *_successFld;
  NotifyField   *_finishedFld;

  /* VARIABLES */

  // Pointer to the U3D FileWriter
  U3DFileWriter* _outU3DFile;


  /* METHODS */

  //! Handler method for Save button.
  void saveButtonClicked();

  //! Save U3D (ECMA-363 Universal 3D Format) file into file stream.
  bool saveU3DToFileStream(std::ofstream& ofstream);

  // +++++++++++++++++++++++++++++++++++++++++++++++++
  // Node blocks
  // +++++++++++++++++++++++++++++++++++++++++++++++++

  //! Add all View Node Blocks
  void _addViewNodes();

  //! Add all Group Node Blocks
  void _addGroupNodes();

  //! Add all Model Node Blocks
  void _addModelNodes();

  //! Add all Light Node Blocks
  void _addLightNodes();

  // +++++++++++++++++++++++++++++++++++++++++++++++++
  // Geometry generator blocks
  // +++++++++++++++++++++++++++++++++++++++++++++++++

  // Create blocks
  U3DDataBlockWriter _createPointSetContinuationBlock(mlU3D::PointSetGenerator &pointSetGenerator) const;
  U3DDataBlockWriter _createLineSetContinuationBlock(mlU3D::LineSetGenerator &lineSetGenerator) const;
  U3DDataBlockWriter _createCLODBaseMeshContinuationBlock(WEMPtr saveWEM, mlU3D::CLODMeshGenerator& meshGenerator) const;
  mlU3D::ColorMap _writeVertexColors(WEMPatch* patch, U3DDataBlockWriter& continuationBlock) const;

  //! Add all View Node Blocks
  void _addPointSetGeometry();

  //! Add all Group Node Blocks
  void _addLineSetGeometry();

  //! Add all Model Node Blocks
  void _addCLODMeshGeometry();

  // +++++++++++++++++++++++++++++++++++++++++++++++++
  // Resource blocks
  // +++++++++++++++++++++++++++++++++++++++++++++++++

  // Add light resources
  void _addLightResources();

  // Add view resources
  void _addViewResources();

  // Add shaders
  void _addShaders();

  // Add material resources
  void _addMaterialResources();

  // Add motion resources
  void _addMotionResources();

  //////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DSave_H