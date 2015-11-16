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


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_getMaxTypeIDFromList(ml::XMarkerList& list, int& maxTypeID)
{
  size_t listLength = list.size();

  for (int i = 0; i < listLength; i++)
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

  for (int i = 0; i < listLength; i++)
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
  PDFTools::fillFiberSetContainerFromPositionsAndConnections(_outFiberSetContainer, _outLinePositions, _outLineConnections);

  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE