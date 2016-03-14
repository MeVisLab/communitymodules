//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_Utils.
/*!
// \file    PDF3DFigurePage_Utils.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------


#pragma once


#ifndef _PDFUtils_H
#define _PDFUtils_H


// Local includes
#include "PDF3DFigurePageSystem.h"

// Project includes
#include <MLPDF_Tools.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <mlFiberSet.h>

// WEM includes
#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMProcessor.h>


ML_START_NAMESPACE

struct PointSetProperties
{
  int id;
  int numPoints;
  std::string name;
  std::string groupPath;
};

struct LineSetProperties
{
  int id;
  int numPoints;
  int numConnections;
  std::string name;
  std::string groupPath;
  bool useDefaultColor;
  Vector3 color;
};

typedef std::map<int, PointSetProperties> PointSetMapType;
typedef std::map<int, LineSetProperties>  LineSetMapType;

#define NOLABELSPECIFIED "[no label specified]"


//////////////////////////////////////////////////////////////////////////
//! Utilities for PDF creation.
class PDF3DFIGUREPAGE_EXPORT PDF3DFigurePage_Utils : public WEMProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDF3DFigurePage_Utils)

public:

  //! Constructor.
  PDF3DFigurePage_Utils (std::string type="PDF3DFigurePage_Utils");

protected:

  //! Destructor.
  virtual ~PDF3DFigurePage_Utils();

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
  //BaseField *_inRawWEMFld;


  // Output fields (WEM output is for free...)
  BaseField*   _outPointPositionsFld;
  BaseField*   _outLinePositionsFld;
  BaseField*   _outLineConnectionsFld;
  BaseField*   _outFibersFld;
  BaseField*   _outSelectedFibersFld;
  //BaseField*   _outOptimizedWEMFld;

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  NotifyField*   _calculateCameraFromInventorSceneFld;
  BoolField*     _autoCalculateCameraFromInventorSceneFld;
  Vector3Field*  _inventorCameraPositionFld;
  Vector4Field*  _inventorCameraOrientationFld;
  FloatField*    _inventorCameraFocalDistanceFld;
  FloatField*    _inventorCameraHeightFld;

  //! Fields for SoVertexProperty and SoIndexedLineSet
  StringField*   _vertexListFld;
  StringField*   _coordIndexListFld;

  //! PDF view camera fields
  Vector3Field*  _cameraCenterOfOrbitFld;
  Vector3Field*  _cameraCenterToCameraFld;
  FloatField*    _cameraRadiusOfOrbitFld;
  FloatField*    _cameraRollAngleFld;
  FloatField*    _cameraFOVAngleFld;

  //! Fields for creating views
  StringField*   _viewSpecificationsFld;
  StringField*   _newViewDisplayNameFld;
  ColorField*    _newViewBackgroundColorFld;
  EnumField*     _newViewLightingSchemeFld;
  EnumField*     _newViewRenderModeFld;
  NotifyField*   _addNewViewFld;
  NotifyField*   _clearViewsFld;

  //! Fields for WEM/mesh editing
  StringField*   _selectedWEMPatchFld;
  IntField*      _selectedWEMPatchIdFld;
  StringField*   _availableWEMPatchesFld;
  StringField*   _selectedWEMPatchNewLabelFld;
  StringField*   _selectedWEMPatchGroupPathFld;
  BoolField*     _selectedWEMPatchUseDefaultColorFld;
  ColorField*    _selectedWEMPatchColorFld;
  FloatField*    _selectedWEMPatchColorAlphaFld;

  //! Fields for line set editing
  StringField*   _selectedLineSetFld;
  IntField*      _selectedLineSetIdFld;
  StringField*   _availableLineSetsFld;
  StringField*   _selectedLineSetNewLabelFld;
  StringField*   _selectedLineSetGroupPathFld;
  BoolField*     _selectedLineSetUseDefaultColorFld;
  ColorField*    _selectedLineSetColorFld;

  //! Fields for point set editing
  StringField*   _selectedPointSetFld;
  IntField*      _selectedPointSetIdFld;
  StringField*   _availablePointSetsFld;
  StringField*   _selectedPointSetNewLabelFld;
  StringField*   _selectedPointSetGroupPathFld;

  //! Fields for PointSet/LineSet properties
  IntField*      _pointPositionsMaxTypeIDFld;
  IntField*      _linePositionsMaxTypeIDFld;
  IntField*      _lineConnectionsMaxTypeIDFld;
  IntField*      _lineDefinitionsMaxTypeIDFld;
  IntField*      _pointPositionsNextTypeIDFld;
  IntField*      _linePositionsNextTypeIDFld;
  IntField*      _lineConnectionsNextTypeIDFld;
  IntField*      _lineDefinitionsNextTypeIDFld;

  //! Fields for point/line set specification strings
  StringField*   _pointCloudSpecificationFld;
  StringField*   _lineSetSpecificationFld;
  StringField*   _metaDataSpecificationFld;
  NotifyField*   _createPointCloudSpecificationFld;
  NotifyField*   _createLineSetSpecificationFld;
  NotifyField*   _createMetaDataSpecificationFld;


  //! The XMarkerList input for point positions of point clouds
  ml::XMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  //! The XMarkerList output for point positions of point clouds
  ml::XMarkerList _outPointPositions;  

  //! The XMarkerList output for node positions of line sets
  ml::XMarkerList _outLinePositions;

  //! The StringList output for connections of line sets
  ml::IndexPairList _outLineConnections;

  //! The fiberset containers used for visualizing the line sets
  FiberSetContainer _outFiberSetContainer;
  FiberSetContainer _outSelectedFiberSetContainer;

  //! Pointer to second input & output WEM.
  //WEM* _inRawWEM;
  //WEM* _outOptimizedWEM;

  PointSetMapType _pointSetMap;
  LineSetMapType  _lineSetMap;

  /* METHODS */

  // Camera methods ==========================================================

  // Calculate PDF camera properties
  void _calculateCameraPropertiesFromInventorCamera();

  // Methods for SoVertexProperty and SoIndexedLineSet =======================

  void _calculateInventorPropertyFields();
  void _calculateInventorVertexList();
  void _calculateInventorCoordIndexList();

  // Methods for creating views ==============================================

  void _createNewView();
  void _clearViews();
  void _createMetaDataSpecification();

  // Methods for handling meshes =============================================

  void _updateAvailableWEMPatchesFld(WEMPtr wem, std::string defaultEntry = "");
  void _selectedWEMPatchChanged(WEMPtr wem);
  void _selectedWEMPatchIdChanged(WEMPtr wem);
  void _updateSelectedWEMPatchLabel();
  void _updateSelectedWEMPatchDescription();
  void _updateWEMPatchNodesColor();
  void _processPatch(unsigned int patchIndex);


  // Methods for PointSet/LineSet properties =================================

  void _getMaxTypeIDFromList(ml::XMarkerList& list, int& maxTypeID);
  void _getMaxTypeIDFromList(ml::IndexPairList& list, int& maxTypeID);
  void _calculateListPropertyFields();

  // Methods for Point Sets===================================================

  void _updatePointSetMap();
  void _selectedPointSetChanged();
  void _selectedPointSetIdChanged();
  void _updateSelectedPointSetLabel();
  void _updateSelectedPointSethGroupPath();
  void _updatePointSetOutputs();
  void _updateAvailablePointSetsFld(std::string defaultEntry = "");
  void _createPointCloudSpecification();


  // Methods for Line Sets====================================================

  void _updateLineSetMap();
  void _selectedLineSetChanged();
  void _selectedLineSetIdChanged();
  void _updateSelectedLineSetLabel();
  void _updateSelectedLineSethGroupPath();
  void _updateSelectedLineSetColor();
  void _updateLineSetOutputs();
  void _createFibers();
  void _createSelectedFibers();
  void _updateFiberColor(LineSetProperties lineSet);
  void _updateAvailableLineSetsFld(std::string defaultEntry = "");
  void _createLineSetSpecification();

  // Tool Methods ============================================================

  int _getModelIDFromString(std::string idString);

  // Debug, development & test methods =======================================


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _PDFUtils_H