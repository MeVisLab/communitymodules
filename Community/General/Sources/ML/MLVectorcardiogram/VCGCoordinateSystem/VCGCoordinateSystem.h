//----------------------------------------------------------------------------------
//! The ML module class VCGCoordinateSystem.
/*!
// \file    VCGCoordinateSystem.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __VCGCoordinateSystem_H
#define __VCGCoordinateSystem_H


// Local includes
#include "MLVectorcardiogramSystem.h"

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include "IndexPairList.h"
#include "WEMBase/WEM.h"

#define GRIDMODE_ON true
#define GRIDMODE_OFF false


ML_START_NAMESPACE


//! 
class MLVECTORCARDIOGRAM_EXPORT VCGCoordinateSystem : public Module
{
public:

  //! Constructor.
  VCGCoordinateSystem();

  //! Destructor.
  ~VCGCoordinateSystem();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // Input/output fields
  BaseField* _inPositionsFld;
  BaseField* _outLinePositionsFld;
  BaseField* _outLineConnectionsFld;
  BaseField* _outGridPlaneWEMFld;


  // UI fields
  IntField*  _lineTypeStartIDFld;
  IntField*  _lineTypeXAxisIDFld;
  IntField*  _lineTypeYAxisIDFld;
  IntField*  _lineTypeZAxisIDFld;
  IntField*  _lineTypeXTicksIDFld;
  IntField*  _lineTypeYTicksIDFld;
  IntField*  _lineTypeZTicksIDFld;
  IntField*  _lineTypeEndMarkerIDFld;

  StringField*  _xyPlaneWEMLabelFld;
  StringField*  _yzPlaneWEMLabelFld;
  StringField*  _zxPlaneWEMLabelFld;

  EnumField* _axesTypeFld;
  IntField*  _axesOverlengthPercentFld;
  BoolField* _axesMarkerAtAbsoluteEndFld;
  BoolField* _axesTicksFld;
  IntField*  _axesTicksLengthPercentFld;

  StringField*  _linesetDefinitionFld;
  StringField*  _meshDefinitionFld;
  ColorField*   _xAxisColorFld;
  ColorField*   _yAxisColorFld;
  ColorField*   _zAxisColorFld;
  ColorField*   _axisMarkerColorFld;

  // Globals
  XMarkerList   _inPositions;
  XMarkerList   _outLinePositions;
  IndexPairList _outLineConnections;
  WEMPtr        _outGridPlaneWEM;


  // Methods
  void _updateOutput();
  void _updateLineIDs();
  void _createAxes(bool gridMode);
  void _createLinesetDefinition();
  void _createMeshDefinition(bool gridMode);
  void _addLine(XMarker startPosition, XMarker endPosition, const int lineNumber, const int lineID);
  void _addTick(const MLdouble position, const MLdouble start1, const MLdouble end1, const MLdouble start2, const MLdouble end2, const int axis, int& lineNumber, const int tickID);
  void _addWEMPlane(Vector3 start, Vector3 end, std::string label);

  void _getBoundingBox(Vector3& boundingBoxMin, Vector3& boundingBoxMax);


  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(VCGCoordinateSystem)
};


ML_END_NAMESPACE

#endif // __VCGCoordinateSystem_H