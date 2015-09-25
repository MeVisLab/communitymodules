//----------------------------------------------------------------------------------
//! The ML module class VCGCoordinateSystem.
/*!
// \file    VCGCoordinateSystem.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "VCGCoordinateSystem.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(VCGCoordinateSystem, Module);

//----------------------------------------------------------------------------------

VCGCoordinateSystem::VCGCoordinateSystem() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  ML_CHECK_NEW(_outGridPlaneWEM, WEM());

  static const char* axesTypeOptions[2] = {"Axes",  "Grid"};

  // Add input/output fields
  (_inPositionsFld         = addBase("inPositions"))->setBaseValueAndAddAllowedType(&_inPositions);

  (_outGridPlaneWEMFld     = addBase("outGridPlaneWEM"))->setBaseValueAndAddAllowedType(_outGridPlaneWEM);
  (_outLinePositionsFld    = addBase("outLinePositions"))->setBaseValueAndAddAllowedType(&_outLinePositions);
  (_outLineConnectionsFld  = addBase("outLineConnections"))->setBaseValueAndAddAllowedType(&_outLineConnections);

  // Add UI fields
   _lineTypeStartIDFld          = addInt("lineTypeStartID", 1000);
   _lineTypeXAxisIDFld          = addInt("lineTypeXAxisID", 1000);
   _lineTypeYAxisIDFld          = addInt("lineTypeYAxisID", 1001);
   _lineTypeZAxisIDFld          = addInt("lineTypeZAxisID", 1002);
   _lineTypeXTicksIDFld         = addInt("lineTypeXTicks", 1003);
   _lineTypeYTicksIDFld         = addInt("lineTypeYTicks", 1004);
   _lineTypeZTicksIDFld         = addInt("lineTypeZTicks", 1005);
   _lineTypeEndMarkerIDFld      = addInt("lineTypeEndMarkerID", 1006);

   _xyPlaneWEMLabelFld           = addString("xyPlaneWEMLabel", "VCGCoordinateSystemXYPlane");
   _yzPlaneWEMLabelFld           = addString("yzPlaneWEMLabel", "VCGCoordinateSystemYZPlane");
   _zxPlaneWEMLabelFld           = addString("zxPlaneWEMLabel", "VCGCoordinateSystemZXPlane");

  (_axesTypeFld                 = addEnum("axesType", axesTypeOptions, 2))->setEnumValue(0);
   _axesOverlengthPercentFld    = addInt("axesOverlengthPercent", 10);
   _axesMarkerAtAbsoluteEndFld  = addBool("axesMarkerAtAbsoluteEnd", true);
   _axesTicksFld                = addBool("axesTicks", true);
   _axesTicksLengthPercentFld   = addInt("axesTicksLengthPercent", 5);

   _linesetDefinitionFld        = addString("linesetDefinition", "");
   _meshDefinitionFld           = addString("meshDefinition", "");
   _xAxisColorFld               = addColor("xAxisColor", Vector3(0.7,0.7,0.7));
   _xAxisColorFld               = addColor("yAxisColor", Vector3(0.75,0.75,0.75));
   _zAxisColorFld               = addColor("zAxisColor", Vector3(0.8,0.8,0.8));
   _axisMarkerColorFld          = addColor("axisMarkerColor", Vector3(1,0,0));


  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

}

//----------------------------------------------------------------------------------

VCGCoordinateSystem::~VCGCoordinateSystem()
{
  if (_outGridPlaneWEM) 
  {
    _outGridPlaneWEM->removeAll(); 
    _outGridPlaneWEM->clear(); 
  }

  _outGridPlaneWEM = NULL;
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::handleNotification(Field* field)
{
  // Always immediately update output
  if (field)
  {  
    if (field == _lineTypeStartIDFld)
    {
      _updateLineIDs();
    }

    _updateOutput();
  }
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_updateLineIDs()
{
  int lineTypeStartID = (int)_lineTypeStartIDFld->getIntValue();

  _lineTypeXAxisIDFld     ->setIntValue(lineTypeStartID);
  _lineTypeYAxisIDFld     ->setIntValue(lineTypeStartID+1);
  _lineTypeZAxisIDFld     ->setIntValue(lineTypeStartID+2);
  _lineTypeXTicksIDFld    ->setIntValue(lineTypeStartID+3);
  _lineTypeYTicksIDFld    ->setIntValue(lineTypeStartID+4);
  _lineTypeZTicksIDFld    ->setIntValue(lineTypeStartID+5);
  _lineTypeEndMarkerIDFld ->setIntValue(lineTypeStartID+6);
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_addLine(XMarker startPosition, XMarker endPosition, const int lineNumber, const int lineID)
{ 
  startPosition.type        = lineID;
  endPosition.type          = lineID;

  IndexPair newLineConnection;
  newLineConnection.type    = lineID;
  newLineConnection.index1  = lineNumber*2;
  newLineConnection.index2  = lineNumber*2 + 1;

  _outLinePositions.appendItem(startPosition);
  _outLinePositions.appendItem(endPosition);

  _outLineConnections.appendItem(newLineConnection);
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_addTick(const MLdouble position, const MLdouble start1, const MLdouble end1, const MLdouble start2, const MLdouble end2, const int axis, int& lineNumber, const int tickID)
{
  XMarker startPosition1, endPosition1 ,startPosition2, endPosition2;

  if (0 == axis)
  {
    startPosition1.pos = Vector6(position, start1, 0, 0, 0, 0);
    endPosition1.pos   = Vector6(position, end1, 0, 0, 0, 0);
    startPosition2.pos = Vector6(position, 0, start2, 0, 0, 0);
    endPosition2.pos   = Vector6(position, 0, end2, 0, 0, 0);
  }
  else if (1 == axis)
  {
    startPosition1.pos = Vector6(start1, position, 0, 0, 0, 0);
    endPosition1.pos   = Vector6(end1, position, 0, 0, 0, 0);
    startPosition2.pos = Vector6(0, position, start2, 0, 0, 0);
    endPosition2.pos   = Vector6(0, position, end2, 0, 0, 0);
  }
  else if (2 == axis)
  {
    startPosition1.pos = Vector6(start1, 0, position, 0, 0, 0);
    endPosition1.pos   = Vector6(end1, 0, position, 0, 0, 0);
    startPosition2.pos = Vector6(0, start2, position, 0, 0, 0);
    endPosition2.pos   = Vector6(0, end2, position, 0, 0, 0);
  }

  _addLine(startPosition1, endPosition1, lineNumber, tickID);
  lineNumber++;

  _addLine(startPosition2, endPosition2, lineNumber, tickID);
  lineNumber++;
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_createAxes(bool gridMode)
{
  const int lineTypeStartID = (int)_lineTypeStartIDFld->getIntValue();\
  const int xAxisID         = lineTypeStartID;\
  const int yAxisID         = lineTypeStartID + 1;\
  const int zAxisID         = lineTypeStartID + 2;\
  const int xTicksID        = lineTypeStartID + 3;\
  const int yTicksID        = lineTypeStartID + 4;\
  const int zTicksID        = lineTypeStartID + 5;\
  const int endMarkerID     = lineTypeStartID + 6;\
  const int axesOverlengthPercent    = (int)_axesOverlengthPercentFld->getIntValue();\
  const bool axesMarkerAtAbsoluteEnd = _axesMarkerAtAbsoluteEndFld->getBoolValue();\
  const bool axesTicks               = _axesTicksFld->getBoolValue();\
  const int axesTicksLengthPercent   = (int)_axesTicksLengthPercentFld->getIntValue();

  Vector3 boundingBoxMin,boundingBoxMax;
  _getBoundingBox(boundingBoxMin, boundingBoxMax);

  XMarker   newLineStartPosition, newLineEndPosition;

  int lineNumber = 0;
  MLdouble xValuesLength  = MLfabs(boundingBoxMin.x - boundingBoxMax.x);
  MLdouble yValuesLength  = MLfabs(boundingBoxMin.y - boundingBoxMax.y);
  MLdouble zValuesLength  = MLfabs(boundingBoxMin.z - boundingBoxMax.z);

  // Get length of longest axis
  MLdouble longestAxisLength = xValuesLength;

  if (yValuesLength > longestAxisLength)
  {
    longestAxisLength = yValuesLength;
  }

  if (zValuesLength > longestAxisLength)
  {
    longestAxisLength = zValuesLength;
  }

  MLdouble shortTicksLength = (longestAxisLength * axesTicksLengthPercent / 100.) / 2.;

  double decimalPower = floor(log10(longestAxisLength));
  int tickStep = (int)(pow(10, decimalPower-1));

  MLdouble xLineElongation = xValuesLength * axesOverlengthPercent / 100.;
  MLdouble yLineElongation = yValuesLength * axesOverlengthPercent / 100.;
  MLdouble zLineElongation = zValuesLength * axesOverlengthPercent / 100.;

  MLdouble xAxisStart = ceil ((boundingBoxMin.x-xLineElongation) / tickStep) * tickStep;
  MLdouble xAxisEnd   = floor((boundingBoxMax.x+xLineElongation) / tickStep) * tickStep;
  MLdouble yAxisStart = ceil ((boundingBoxMin.y-yLineElongation) / tickStep) * tickStep;
  MLdouble yAxisEnd   = floor((boundingBoxMax.y+yLineElongation) / tickStep) * tickStep;
  MLdouble zAxisStart = ceil ((boundingBoxMin.z-zLineElongation) / tickStep) * tickStep;
  MLdouble zAxisEnd   = floor((boundingBoxMax.z+zLineElongation) / tickStep) * tickStep;

  // Create X-Axis
  newLineStartPosition.pos = Vector6(xAxisStart, 0, 0, 0, 0, 0);
  newLineEndPosition.pos   = Vector6(xAxisEnd, 0, 0, 0, 0, 0);
  _addLine(newLineStartPosition, newLineEndPosition, lineNumber, xAxisID);

  // Create Y-Axis
  lineNumber = 0;
  newLineStartPosition.pos = Vector6(0, yAxisStart, 0, 0, 0, 0);
  newLineEndPosition.pos   = Vector6(0, yAxisEnd, 0, 0, 0, 0);
  _addLine(newLineStartPosition, newLineEndPosition, lineNumber, yAxisID);

  // Create Z-Axis
  lineNumber = 0;
  newLineStartPosition.pos = Vector6(0, 0, zAxisStart, 0, 0, 0);
  newLineEndPosition.pos   = Vector6(0, 0, zAxisEnd, 0, 0, 0);
  _addLine(newLineStartPosition, newLineEndPosition, lineNumber, zAxisID);

  // Create Ticks
  if (axesTicks)
  {
    // Create X-Ticks
    lineNumber = 0;

    for (int xTickPos = (int)xAxisStart; xTickPos <= (int)xAxisEnd; xTickPos += tickStep)
    {
      if (gridMode)
      {
        _addTick(xTickPos, yAxisStart, yAxisEnd, zAxisStart, zAxisEnd, 0, lineNumber, xTicksID);
      }
      else
      {
        _addTick(xTickPos, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 0, lineNumber, xTicksID);
      }
    }

    // Create Y-Ticks
    lineNumber = 0;

    for (int yTickPos = (int)yAxisStart; yTickPos <= (int)yAxisEnd; yTickPos += tickStep)
    {
      if (gridMode)
      {
        _addTick(yTickPos, xAxisStart, xAxisEnd, zAxisStart, zAxisEnd, 1, lineNumber, yTicksID);
      }
      else
      {
        _addTick(yTickPos, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 1, lineNumber, yTicksID);
      }
    }

    // Create Z-Ticks
    lineNumber = 0;

    for (int zTickPos = (int)zAxisStart; zTickPos <= (int)zAxisEnd; zTickPos += tickStep)
    {
      if (gridMode)
      {
        _addTick(zTickPos, xAxisStart, xAxisEnd, yAxisStart, yAxisEnd, 2, lineNumber, zTicksID);
      }
      else
      {
        _addTick(zTickPos, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 2, lineNumber, zTicksID);
      }
    }

  }

  // Create End Markers
  if (axesMarkerAtAbsoluteEnd)
  {
    lineNumber = 0;
      if (gridMode)
      {
        _addTick(boundingBoxMin.x, yAxisStart, yAxisEnd, zAxisStart, zAxisEnd, 0, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.x, yAxisStart, yAxisEnd, zAxisStart, zAxisEnd, 0, lineNumber, endMarkerID);
        _addTick(boundingBoxMin.y, xAxisStart, xAxisEnd, zAxisStart, zAxisEnd, 1, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.y, xAxisStart, xAxisEnd, zAxisStart, zAxisEnd, 1, lineNumber, endMarkerID);
        _addTick(boundingBoxMin.z, xAxisStart, xAxisEnd, yAxisStart, yAxisEnd, 2, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.z, xAxisStart, xAxisEnd, yAxisStart, yAxisEnd, 2, lineNumber, endMarkerID);
      }
      else
      {
        _addTick(boundingBoxMin.x, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 0, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.x, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 0, lineNumber, endMarkerID);
        _addTick(boundingBoxMin.y, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 1, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.y, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 1, lineNumber, endMarkerID);
        _addTick(boundingBoxMin.z, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 2, lineNumber, endMarkerID);
        _addTick(boundingBoxMax.z, 0-shortTicksLength, 0+shortTicksLength, 0-shortTicksLength, 0+shortTicksLength, 2, lineNumber, endMarkerID);
      }
  }

  // Create WEM planes
  if (gridMode)
  {
    Vector3 xyPlaneStart(xAxisStart, yAxisStart, 0);
    Vector3 xyPlaneEnd(xAxisEnd, yAxisEnd, 0);
    _addWEMPlane(xyPlaneStart, xyPlaneEnd, _xyPlaneWEMLabelFld->getStringValue());

    Vector3 yzPlaneStart(0, yAxisStart, zAxisStart);
    Vector3 yzPlaneEnd(0, yAxisEnd, zAxisEnd);
    _addWEMPlane(yzPlaneStart, yzPlaneEnd, _yzPlaneWEMLabelFld->getStringValue());

    Vector3 zxPlaneStart(xAxisStart, 0, zAxisStart);
    Vector3 zxPlaneEnd(xAxisEnd, 0, zAxisEnd);
    _addWEMPlane(zxPlaneStart, zxPlaneEnd, _zxPlaneWEMLabelFld->getStringValue());
  }

  _outGridPlaneWEM->notifyObservers(WEM_NOTIFICATION_REPAINT);
}
 
//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_addWEMPlane(Vector3 start, Vector3 end, std::string label)
{
  WEMTrianglePatch* planePatch = NULL;
  ML_CHECK_NEW(planePatch, WEMTrianglePatch());

  WEMNode* vertex0 = planePatch->addNode();
  WEMNode* vertex1 = planePatch->addNode();
  WEMNode* vertex2 = planePatch->addNode();
  WEMNode* vertex3 = planePatch->addNode();

  WEMTriangle* face0 = planePatch->addTriangle();
  WEMTriangle* face1 = planePatch->addTriangle();

  vertex0->setPosition(Vector3(start));
  vertex2->setPosition(Vector3(end));

  if (start.z == 0) // xy plane
  {
    vertex1->setPosition(Vector3(start.x, end.y, 0));
    vertex3->setPosition(Vector3(end.x, start.y, 0));
  }
  else if (start.x == 0) // yz plane
  {
    vertex1->setPosition(Vector3(0, start.y, end.z));
    vertex3->setPosition(Vector3(0, end.y, start.z));
  }
  else // zx plane
  {
    vertex1->setPosition(Vector3(end.x, 0, start.z));
    vertex3->setPosition(Vector3(start.x, 0, end.z));
  }

  vertex0->addFace(face0); vertex0->addFace(face1);
  vertex1->addFace(face0); 
  vertex2->addFace(face1); vertex2->addFace(face0); 
  vertex3->addFace(face1); 

  face0->setNodes(vertex0, vertex1, vertex2);
  face1->setNodes(vertex0, vertex2, vertex3);

  //xyPlanePatch->setType(_type);
  planePatch->setCreatorId(0);  
    
  planePatch->setLabel(label);
  planePatch->setDescription("");

  _outGridPlaneWEM->addWEMPatch(planePatch); 

  planePatch->setId(_outGridPlaneWEM->getCurrentWEMPatchId());

  //xyPlanePatch->mirror(); 
  planePatch->buildEdgeConnectivity(); 
  planePatch->computeNormals();
  planePatch->computeBoundingBox();
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_createLinesetDefinition()
{
  std::string xAxisColor  = _xAxisColorFld->getStringValue();
  std::string yAxisColor  = _xAxisColorFld->getStringValue();
  std::string zAxisColor  = _zAxisColorFld->getStringValue();
  std::string markerColor = _axisMarkerColorFld->getStringValue();
  std::string xAxisID     = _lineTypeXAxisIDFld->getStringValue();
  std::string yAxisID     = _lineTypeYAxisIDFld->getStringValue();
  std::string zAxisID     = _lineTypeZAxisIDFld->getStringValue();
  std::string xTicksID    = _lineTypeXTicksIDFld->getStringValue();
  std::string yTicksID    = _lineTypeYTicksIDFld->getStringValue();
  std::string zTicksID    = _lineTypeZTicksIDFld->getStringValue();
  std::string endMarkerID = _lineTypeEndMarkerIDFld->getStringValue();

  std::string linesetDefinition = "";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/X-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Axis</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + xAxisID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + xAxisID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + xAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/Y-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Axis</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + yAxisID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + yAxisID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + yAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/Z-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Axis</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + zAxisID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + zAxisID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + zAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/X-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Ticks</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + xTicksID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + xTicksID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + xAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/Y-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Ticks</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + yTicksID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + yTicksID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + yAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/Z-Axis/</GroupPath>\n";
  linesetDefinition += "<ObjectName>Ticks</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + zTicksID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + zTicksID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + zAxisColor + "</Color>\n";
  linesetDefinition += "\n";

  linesetDefinition += "<LineSet>\n";
  linesetDefinition += "<GroupPath>/Coordinate System/</GroupPath>\n";
  linesetDefinition += "<ObjectName>End Markers</ObjectName>\n";
  linesetDefinition += "<PositionTypes>" + endMarkerID + "</PositionTypes>\n";
  linesetDefinition += "<ConnectionTypes>" + endMarkerID + "</ConnectionTypes>\n";
  linesetDefinition += "<Color>" + markerColor + "</Color>\n";
  linesetDefinition += "\n";

  _linesetDefinitionFld->setStringValue(linesetDefinition);
}
 
//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_createMeshDefinition(bool gridMode)
{
  std::string meshDefinition = "";

  if (gridMode)
  {
    std::string xyPlaneWEMLabel = _xyPlaneWEMLabelFld->getStringValue();
    std::string yzPlaneWEMLabel = _yzPlaneWEMLabelFld->getStringValue();
    std::string zxPlaneWEMLabel = _zxPlaneWEMLabelFld->getStringValue();

    std::string xAxisColor  = _xAxisColorFld->getStringValue();
    std::string yAxisColor  = _xAxisColorFld->getStringValue();
    std::string zAxisColor  = _zAxisColorFld->getStringValue();
    std::string markerColor = _axisMarkerColorFld->getStringValue();

    meshDefinition += "<Mesh>\n";
    meshDefinition += "<GroupPath>/Coordinate System/</GroupPath>\n";
    meshDefinition += "<ObjectName>XY Plane</ObjectName>\n";
    meshDefinition += "<WEMLabel>" + xyPlaneWEMLabel + "</WEMLabel>\n";
    meshDefinition += "<Color>" + xAxisColor + "</Color>\n";
    meshDefinition += "\n";

    meshDefinition += "<Mesh>\n";
    meshDefinition += "<GroupPath>/Coordinate System/</GroupPath>\n";
    meshDefinition += "<ObjectName>YZ Plane</ObjectName>\n";
    meshDefinition += "<WEMLabel>" + yzPlaneWEMLabel + "</WEMLabel>\n";
    meshDefinition += "<Color>" + yAxisColor + "</Color>\n";
    meshDefinition += "\n";

    meshDefinition += "<Mesh>\n";
    meshDefinition += "<GroupPath>/Coordinate System/</GroupPath>\n";
    meshDefinition += "<ObjectName>ZX Plane</ObjectName>\n";
    meshDefinition += "<WEMLabel>" + zxPlaneWEMLabel + "</WEMLabel>\n";
    meshDefinition += "<Color>" + zAxisColor + "</Color>\n";
    meshDefinition += "\n";

  }

  _meshDefinitionFld->setStringValue(meshDefinition);

}
 
//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_updateOutput()
{
  handleNotificationOff();

  _outLinePositions.clear();
  _outLineConnections.clear();
  _outGridPlaneWEM->removeAll();
  _outLinePositions.resetId();
  _outLineConnections.resetId();
  _inPositions.clear();

  XMarkerList* inList = static_cast<XMarkerList*>(_inPositionsFld->getBaseValue());

  if (inList) 
  {
    _inPositions = *inList;

    if (_inPositions.size() > 0)
    {

      const int axesType = _axesTypeFld->getEnumValue();

      if (0 == axesType)
      {
        _createAxes(GRIDMODE_OFF);
        _createMeshDefinition(GRIDMODE_OFF);
      }
      else if (1 == axesType)
      {
        _createAxes(GRIDMODE_ON);
        _createMeshDefinition(GRIDMODE_ON);
      }

      _createLinesetDefinition();

    }
  }
  handleNotificationOn();

  // Notify obervers
  _outLinePositionsFld->touch();
  _outLineConnectionsFld->touch();
}

//----------------------------------------------------------------------------------

void VCGCoordinateSystem::_getBoundingBox(Vector3& boundingBoxMin, Vector3& boundingBoxMax)
{
  double xmin = 0;
  double ymin = 0;
  double zmin = 0;
  double xmax = 0;
  double ymax = 0;
  double zmax = 0;

  for (size_t i = 0; i < _inPositions.size(); i++) 
  {
    XMarker inListItem = _inPositions[i];

    double x = inListItem.x();
    double y = inListItem.y();
    double z = inListItem.z();

    if (x < xmin)
    {
      xmin = x;
    }

    if (x > xmax)
    {
      xmax = x;
    }

    if (y < ymin)
    {
      ymin = y;
    }

    if (y > ymax)
    {
      ymax = y;
    }

    if (z < zmin)
    {
      zmin = z;
    }

    if (z > zmax)
    {
      zmax = z;
    }

  }

  boundingBoxMin.x = xmin;
  boundingBoxMin.y = ymin;
  boundingBoxMin.z = zmin;

  boundingBoxMax.x = xmax;
  boundingBoxMax.y = ymax;
  boundingBoxMax.z = zmax;
}



ML_END_NAMESPACE