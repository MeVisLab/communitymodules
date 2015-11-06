//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils_Lists.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Utilities for PDF creation.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDFUtils.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFUtils::_getMaxTypeIDFromList(ml::XMarkerList& list, int& maxTypeID)
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

void PDFUtils::_getMaxTypeIDFromList(ml::IndexPairList& list, int& maxTypeID)
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

void PDFUtils::_calculateListPropertyFields()
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


//----------------------------------------------------------------------------------


ML_END_NAMESPACE