//----------------------------------------------------------------------------------
//! The ML module class PDFUtils.
/*!
// \file    PDFUtils_Inventor.cpp
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

void PDFUtils::_calculateInventorPropertyFields()
{
  _calculateInventorVertexList();
  _calculateInventorCoordIndexList();
}

//----------------------------------------------------------------------------------

void PDFUtils::_calculateInventorVertexList()
{
  std::string vertexList = "[ ";

  size_t positionsListLength = _inPointPositions.size();

  for (int i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = _inPointPositions[i];

    vertexList += mlPDF::floatToString(thisPosition.x());
    vertexList += " ";
    vertexList += mlPDF::floatToString(thisPosition.y());
    vertexList += " ";
    vertexList += mlPDF::floatToString(thisPosition.z());

    if (i < positionsListLength - 1)
    {
      vertexList += ", ";
    }
  }

  vertexList += " ]";

  _vertexListFld->setStringValue(vertexList);
}

//----------------------------------------------------------------------------------

void PDFUtils::_calculateInventorCoordIndexList()
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
      for (int i = 0; i < positionsListLength; i++)
      {
        coordIndexList += mlPDF::intToString(i);

        if (i < positionsListLength - 1)
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