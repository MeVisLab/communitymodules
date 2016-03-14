//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_Utils.
/*!
// \file    PDF3DFigurePage_Utils.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDF3DFigurePage_Utils.h"

// Project includes
#include <MLPDF_MarkerListTools.h>


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_getMaxTypeIDFromList(ml::XMarkerList& list, int& maxTypeID)
{
  size_t listLength = list.size();

  for (size_t i = 0; i < listLength; i++)
  {
    ml::XMarker thisListEntry = list[i];

    if (thisListEntry.type > maxTypeID)
    {
      maxTypeID = thisListEntry.type;
    }
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_getMaxTypeIDFromList(ml::IndexPairList& list, int& maxTypeID)
{
  size_t listLength = list.size();

  for (size_t i = 0; i < listLength; i++)
  {
    ml::IndexPair thisListEntry = list[i];

    if (thisListEntry.type > maxTypeID)
    {
      maxTypeID = thisListEntry.type;
    }
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_calculateListPropertyFields()
{
  int maxPointPositionsTypeID    = -1;
  int maxLinePositionsMaxTypeID   = -1;
  int maxLineConnectionsMaxTypeID = -1;

  _getMaxTypeIDFromList(_inPointPositions, maxPointPositionsTypeID);
  _getMaxTypeIDFromList(_inLinePositions, maxLinePositionsMaxTypeID);
  _getMaxTypeIDFromList(_inLineConnections, maxLineConnectionsMaxTypeID);

  int maxLineDefinitionsMaxTypeID = maxLinePositionsMaxTypeID;

  if (maxLineConnectionsMaxTypeID > maxLineDefinitionsMaxTypeID)
  {
    maxLineDefinitionsMaxTypeID = maxLineConnectionsMaxTypeID;
  }

  _pointPositionsMaxTypeIDFld->setIntValue(maxPointPositionsTypeID);
  _linePositionsMaxTypeIDFld->setIntValue(maxLinePositionsMaxTypeID);
  _lineConnectionsMaxTypeIDFld->setIntValue(maxLineConnectionsMaxTypeID);
  _lineDefinitionsMaxTypeIDFld->setIntValue(maxLineDefinitionsMaxTypeID);

  _pointPositionsNextTypeIDFld->setIntValue(maxPointPositionsTypeID+1);
  _linePositionsNextTypeIDFld->setIntValue(maxLinePositionsMaxTypeID+1);
  _lineConnectionsNextTypeIDFld->setIntValue(maxLineConnectionsMaxTypeID+1);
  _lineDefinitionsNextTypeIDFld->setIntValue(maxLineDefinitionsMaxTypeID+1);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updatePointSetMap()
{
  _pointSetMap.clear();

  const int numPoints = (int)_outPointPositions.size();

  for (int p = 0; p < numPoints; p++)
  {
    ml::XMarker thisPoint = _outPointPositions[p];
    int thisPointType = thisPoint.type;

    PointSetMapType::iterator findIt = _pointSetMap.find(thisPointType);

    if (findIt == _pointSetMap.end())
    {
      // inMarkerType has not yet been added to map, so add it now
      PointSetProperties thisPointSetProperties;
      thisPointSetProperties.id = thisPointType;
      thisPointSetProperties.numPoints = 1;
      thisPointSetProperties.name = thisPoint.name();
      thisPointSetProperties.groupPath = "";
      _pointSetMap[thisPointType] = thisPointSetProperties;
    }
    else
    {
      PointSetProperties thisPointSetProperties = _pointSetMap[thisPointType];
      thisPointSetProperties.numPoints += 1;
      _pointSetMap[thisPointType] = thisPointSetProperties;
    }
  }

}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updatePointSetOutputs()
{
  _outPointPositions.clearList();
  _outPointPositions.selectItemAt(-1);

  const int numPoints = (int)_inPointPositions.size();

  for (int p = 0; p < numPoints; p++)
  {
    ml::XMarker thisPoint = _inPointPositions[p];

    _outPointPositions.appendItem(thisPoint);
  }

  _outPointPositionsFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateLineSetMap()
{
  _lineSetMap.clear();

  const int numLinesPositions = (int)_outLinePositions.size();

  for (int p = 0; p < numLinesPositions; p++)
  {
    ml::XMarker thisLinePosition = _outLinePositions[p];
    int thisLinePositionType = thisLinePosition.type;

    LineSetMapType::iterator findIt = _lineSetMap.find(thisLinePositionType);

    if (findIt == _lineSetMap.end())
    {
      // inMarkerType has not yet been added to map, so add it now
      LineSetProperties thisLineSetProperties;
      thisLineSetProperties.id = thisLinePositionType;
      thisLineSetProperties.numPoints = 1;
      thisLineSetProperties.name = thisLinePosition.name();
      thisLineSetProperties.groupPath = "";
      thisLineSetProperties.useDefaultColor = true;
      thisLineSetProperties.color = Vector3(0);
      _lineSetMap[thisLinePositionType] = thisLineSetProperties;
    }
    else
    {
      LineSetProperties thisLineSetProperties = _lineSetMap[thisLinePositionType];
      thisLineSetProperties.numPoints += 1;
      _lineSetMap[thisLinePositionType] = thisLineSetProperties;
    }
  }

}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateLineSetOutputs()
{
  _outLinePositions.clearList();
  _outLinePositions.selectItemAt(-1);

  const int numLinePoints = (int)_inLinePositions.size();

  for (int p = 0; p < numLinePoints; p++)
  {
    ml::XMarker thisPoint = _inLinePositions[p];

    _outLinePositions.appendItem(thisPoint);
  }

  _outLineConnections.clearList();
  _outLineConnections.selectItemAt(-1);

  const int numLineConnections = (int)_inLineConnections.size();

  for (int c = 0; c < numLineConnections; c++)
  {
    ml::IndexPair thisConnection = _inLineConnections[c];

    _outLineConnections.appendItem(thisConnection);
  }

  _outLinePositionsFld->touch();
  _outLineConnectionsFld->touch();
  _createFibers();
}


//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createFibers()
{
  PDFMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(_outFiberSetContainer, _outLinePositions, _outLineConnections);

  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createSelectedFibers()
{
  const int lineSetId = _selectedLineSetIdFld->getIntValue();

  XMarkerList   _outSelectedLinePositions;
  IndexPairList _outSelectedLineConnections;

  for (XMarkerList::const_iterator it = _outLinePositions.cbegin(); it != _outLinePositions.cend(); ++it)
  {
    XMarker thisPosition = *it;

    if (thisPosition.type == lineSetId)
    {
      _outSelectedLinePositions.appendItem(thisPosition);
    }
  }

  for (IndexPairList::const_iterator it = _outLineConnections.cbegin(); it != _outLineConnections.cend(); ++it)
  {
    IndexPair thisConnection = *it;

    if (thisConnection.type == lineSetId)
    {
      _outSelectedLineConnections.appendItem(thisConnection);
    }
  }

  PDFMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(_outSelectedFiberSetContainer, _outSelectedLinePositions, _outSelectedLineConnections);

  _outSelectedFibersFld->touch();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createPointCloudSpecification()
{
  std::string pointCloudSpecification = "";

  for (PointSetMapType::const_iterator it = _pointSetMap.cbegin(); it != _pointSetMap.cend(); ++it)
  {
    PointSetProperties thisPointSet = it->second;

    std::string thisPointCloudSpecification = "";
    thisPointCloudSpecification += "<PointSet>\n";
    thisPointCloudSpecification += "<PositionTypes>" + intToString(thisPointSet.id) + "</PositionTypes>\n";
    thisPointCloudSpecification += "<ObjectName>" + thisPointSet.name + "</ObjectName>\n";
    thisPointCloudSpecification += "<GroupPath>" + thisPointSet.groupPath + "</GroupPath>\n";
    thisPointCloudSpecification += "<ModelVisibility>3</ModelVisibility>\n";
    thisPointCloudSpecification += "</PointSet>\n";

    if (pointCloudSpecification != "")
    {
      pointCloudSpecification += "\n";
    }

    pointCloudSpecification += thisPointCloudSpecification;
  }

  _pointCloudSpecificationFld->setStringValue(pointCloudSpecification);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_createLineSetSpecification()
{
  std::string lineSetSpecification = "";

  for (LineSetMapType::const_iterator it = _lineSetMap.cbegin(); it != _lineSetMap.cend(); ++it)
  {
    LineSetProperties thisLineSet = it->second;

    std::string thisLineSetSpecification = "";
    thisLineSetSpecification += "<LineSet>\n";
    thisLineSetSpecification += "<PositionTypes>" + intToString(thisLineSet.id) + "</PositionTypes>\n";
    thisLineSetSpecification += "<ConnectionTypes>" + intToString(thisLineSet.id) + "</ConnectionTypes>\n";
    thisLineSetSpecification += "<ObjectName>" + thisLineSet.name + "</ObjectName>\n";
    thisLineSetSpecification += "<GroupPath>" + thisLineSet.groupPath + "</GroupPath>\n";

    if (!thisLineSet.useDefaultColor)
    {
      thisLineSetSpecification += "<Color>" + PDFTools::FormatColorString(thisLineSet.color) + "</Color>\n";
    }

    thisLineSetSpecification += "<ModelVisibility>3</ModelVisibility>\n";
    thisLineSetSpecification += "</LineSet>\n";

    if (lineSetSpecification != "")
    {
      lineSetSpecification += "\n";
    }

    lineSetSpecification += thisLineSetSpecification;
  }

  _lineSetSpecificationFld->setStringValue(lineSetSpecification);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_selectedPointSetChanged()
{
  int pointSetId = _getModelIDFromString(_selectedPointSetFld->getStringValue());

  if (pointSetId != ML_INT16_MIN)
  {
    _selectedPointSetIdFld->setIntValue(pointSetId);
    _selectedPointSetIdChanged();
  }

}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_selectedPointSetIdChanged()
{
  std::string newLabel = NOLABELSPECIFIED;
  std::string groupPath = "";

  const int pointSetId = _selectedPointSetIdFld->getIntValue();

  if (pointSetId != ML_INT16_MIN)
  {
    PointSetProperties thisPointSet = _pointSetMap[pointSetId];

    newLabel = thisPointSet.name;
    groupPath = thisPointSet.groupPath;

    if (newLabel == NOLABELSPECIFIED)
    {
      newLabel = "";
    }

    _selectedPointSetNewLabelFld->setStringValue(newLabel);
    _selectedPointSetGroupPathFld->setStringValue(groupPath);
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateAvailablePointSetsFld(std::string defaultEntry)
{
  std::string availablePointSets = "";
  std::string firstEntry = "";

  for (PointSetMapType::const_iterator it = _pointSetMap.cbegin(); it != _pointSetMap.cend(); ++it)
  {
    PointSetProperties thisPointSet = it->second;

    int thisPointSetID = thisPointSet.id;
    std::string thisPointSetName = thisPointSet.name;

    if (thisPointSetName == "")
    {
      thisPointSetName = NOLABELSPECIFIED;
    }

    if (availablePointSets != "")
    {
      availablePointSets.append("|");
    }

    availablePointSets.append(thisPointSetName + " {ID=" + intToString(thisPointSetID) + "}");

    if (firstEntry == "")
    {
      firstEntry = availablePointSets;
    }

  }

  _availablePointSetsFld->setStringValue(availablePointSets);

  if (defaultEntry != "")
  {
    _selectedPointSetFld->setStringValue(defaultEntry);
    _selectedPointSetChanged();
  }
  else
  {
    _selectedPointSetFld->setStringValue(firstEntry);
    _selectedPointSetChanged();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedPointSetLabel()
{
  std::string newLabel = _selectedPointSetNewLabelFld->getStringValue();

  if (newLabel == "")
  {
    newLabel = NOLABELSPECIFIED;
  }

  const int pointSetId = _selectedPointSetIdFld->getIntValue();

  if (pointSetId != ML_INT16_MIN)
  {
    _pointSetMap[pointSetId].name = newLabel;
    std::string defaultSelection = newLabel + " {ID=" + intToString(pointSetId) + "}";

    _updateAvailablePointSetsFld(defaultSelection);
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedPointSethGroupPath()
{
  const int pointSetId = _selectedPointSetIdFld->getIntValue();

  if (pointSetId != ML_INT16_MIN)
  {
    _pointSetMap[pointSetId].groupPath = _selectedPointSetGroupPathFld->getStringValue();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_selectedLineSetChanged()
{
  int lineSetId = _getModelIDFromString(_selectedLineSetFld->getStringValue());

  if (lineSetId != ML_INT16_MIN)
  {
    _selectedLineSetIdFld->setIntValue(lineSetId);
    _createSelectedFibers();
    _selectedLineSetIdChanged();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_selectedLineSetIdChanged()
{
  std::string newLabel = NOLABELSPECIFIED;
  std::string groupPath = "";
  bool useDefaultColor = true;
  Vector3 color(0);

  const int lineSetId = _selectedLineSetIdFld->getIntValue();

  if (lineSetId != ML_INT16_MIN)
  {
    LineSetProperties thisLineSet = _lineSetMap[lineSetId];

    newLabel = thisLineSet.name;
    groupPath = thisLineSet.groupPath;
    useDefaultColor = thisLineSet.useDefaultColor;
    color = thisLineSet.color;

    if (newLabel == NOLABELSPECIFIED)
    {
      newLabel = "";
    }

    _selectedLineSetNewLabelFld->setStringValue(newLabel);
    _selectedLineSetGroupPathFld->setStringValue(groupPath);
    _selectedLineSetUseDefaultColorFld->setBoolValue(useDefaultColor);
    _selectedLineSetColorFld->setVectorValue(color);
    _updateSelectedLineSetColor();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateAvailableLineSetsFld(std::string defaultEntry)
{
  std::string availableLineSets = "";
  std::string firstEntry = "";

  for (LineSetMapType::const_iterator it = _lineSetMap.cbegin(); it != _lineSetMap.cend(); ++it)
  {
    LineSetProperties thisLineSet = it->second;

    int thisLineSetID = thisLineSet.id;
    std::string thisLineSetName = thisLineSet.name;

    if (thisLineSetName == "")
    {
      thisLineSetName = NOLABELSPECIFIED;
    }

    if (availableLineSets != "")
    {
      availableLineSets.append("|");
    }

    availableLineSets.append(thisLineSetName + " {ID=" + intToString(thisLineSetID) + "}");

    if (firstEntry == "")
    {
      firstEntry = availableLineSets;
    }

  }

  _availableLineSetsFld->setStringValue(availableLineSets);

  if (defaultEntry != "")
  {
    _selectedLineSetFld->setStringValue(defaultEntry);
    _selectedLineSetChanged();
  }
  else
  {
    _selectedLineSetFld->setStringValue(firstEntry);
    _selectedLineSetChanged();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedLineSetLabel()
{
  std::string newLabel = _selectedLineSetNewLabelFld->getStringValue();

  if (newLabel == "")
  {
    newLabel = NOLABELSPECIFIED;
  }

  const int lineSetId = _selectedLineSetIdFld->getIntValue();

  if (lineSetId != ML_INT16_MIN)
  {
    _lineSetMap[lineSetId].name = newLabel;
    std::string defaultSelection = newLabel + " {ID=" + intToString(lineSetId) + "}";

    _updateAvailableLineSetsFld(defaultSelection);
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedLineSethGroupPath()
{
  const int lineSetId = _selectedLineSetIdFld->getIntValue();

  if (lineSetId != ML_INT16_MIN)
  {
    _lineSetMap[lineSetId].groupPath = _selectedLineSetGroupPathFld->getStringValue();
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateSelectedLineSetColor()
{
  const int lineSetId = _selectedLineSetIdFld->getIntValue();

  if (lineSetId != ML_INT16_MIN)
  {
    bool useDefaultColor = _selectedLineSetUseDefaultColorFld->getBoolValue();
    _lineSetMap[lineSetId].useDefaultColor = useDefaultColor;

    if (!useDefaultColor)
    {
      _lineSetMap[lineSetId].color = _selectedLineSetColorFld->getVectorValue();
    }

    _updateFiberColor(_lineSetMap[lineSetId]);
  }
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_updateFiberColor(LineSetProperties lineSet)
{
  // Set color of result fiberset
  FiberSetContainer::FiberSet* selectedFiberSet = _outFiberSetContainer.getFiberSet(lineSet.id-1);

  if (selectedFiberSet)
  {
    if (lineSet.useDefaultColor)
    {
      selectedFiberSet->setColor(Vector3(0));
    }
    else
    {
      selectedFiberSet->setColor(lineSet.color);
    }
  }

  _outFibersFld->touch();

  // Set color of preview fiberset
  selectedFiberSet = _outSelectedFiberSetContainer.getFiberSet(0);

  if (selectedFiberSet)
  {
    if (lineSet.useDefaultColor)
    {
      selectedFiberSet->setColor(Vector3(0));
    }
    else
    {
      selectedFiberSet->setColor(lineSet.color);
    }
  }

  _outSelectedFibersFld->touch();
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE