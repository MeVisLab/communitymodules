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

void PDF3DFigurePage_Utils::_calculateInventorPropertyFields()
{
  _calculateInventorVertexList();
  _calculateInventorCoordIndexList();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_calculateInventorVertexList()
{
  std::string vertexList = "[ ";

  size_t positionsListLength = _inPointPositions.size();

  for (size_t i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = _inPointPositions[i];

    vertexList += floatToString(thisPosition.x());
    vertexList += " ";
    vertexList += floatToString(thisPosition.y());
    vertexList += " ";
    vertexList += floatToString(thisPosition.z());

    if (i + 1 < positionsListLength)
    {
      vertexList += ", ";
    }
  }

  vertexList += " ]";

  _vertexListFld->setStringValue(vertexList);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_Utils::_calculateInventorCoordIndexList()
{
  std::string coordIndexList = "[ ";

  size_t positionsListLength = _inLinePositions.size();

  if (positionsListLength > 1)
  {
    size_t connectionsListLength = _inLineConnections.size();

    if (connectionsListLength > 1)
    {
      // Line connections are given
    }
    else
    {
      // No connections are defined -> create default single line connection
      for (size_t i = 0; i < positionsListLength; i++)
      {
        coordIndexList += intToString(i);

        if (i + 1 < positionsListLength)
        {
          coordIndexList += ", ";
        }
      }

    }

  }

  coordIndexList += " ]";

  _coordIndexListFld->setStringValue(coordIndexList);
}

//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


ML_END_NAMESPACE