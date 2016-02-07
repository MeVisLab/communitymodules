//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_MarkerListTools.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLPDF_DataTypes.h"
#include "MLPDF_Tools.h"
#include "MLPDF_MarkerListTools.h"

// Project includes
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

// ML includes
#include <mlXMarkerList.h>
#include <mlModuleIncludes.h>
#include <mlApplicationProperties.h>


ML_START_NAMESPACE

namespace mlPDF {

//***********************************************************************************

// Get all positions (vertices) from ColoredMarkerList
PositionsVector PDFMarkerListTools::getAllPositionsFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double pointSize)
{
  PositionsVector result;
  
  PositionStruct newPosition;

  StringVector allowedPositionTypesVector = PDFTools::stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  for (size_t i = 0; i < positionsListLength; i++)
  {
    ml::ColoredMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), MLintToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      newPosition.position.x = thisPosition.x();
      newPosition.position.y = thisPosition.y();
      newPosition.position.z = thisPosition.z();
      newPosition.color.x    = thisPosition.r();
      newPosition.color.y    = thisPosition.g();
      newPosition.color.z    = thisPosition.b();
      newPosition.alpha      = thisPosition.alpha();
      newPosition.size       = pointSize;

      result.push_back(newPosition);
    }

  }
  
  return result;
}

//***********************************************************************************

// Get all positions (vertices) from XMarkerList
PositionsVector PDFMarkerListTools::getAllPositionsFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double pointSize)
{
  PositionsVector result;
  
  PositionStruct newPosition;

  StringVector allowedPositionTypesVector = PDFTools::stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  for (size_t i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), intToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      newPosition.position.x = thisPosition.x();
      newPosition.position.y = thisPosition.y();
      newPosition.position.z = thisPosition.z();
      newPosition.color      = Vector3(0);       // Not defined by XMarkerList
      newPosition.alpha      = 1;                // Not defined by XMarkerList
      newPosition.size       = pointSize;

      result.push_back(newPosition);
    }

  }
  
  return result;
}

//***********************************************************************************

// Get all line connections from IndexPairList
LinesVector PDFMarkerListTools::getAllLinesFromIndexPairList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes, double lineWidth)
{
  LinesVector result;
  LineStruct thisLine;

  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning

  StringVector allowedConnectionTypesVector = PDFTools::stringSplit(allowedConnectionTypes, ",", false);

  size_t connectionsListLength = connectionsList.size();

  for (size_t i = 0; i < connectionsListLength; i++)
  {
    ml::IndexPair thisConnection = connectionsList[i];

    // If thisConnection is of allowed type
    if (  ("all" == allowedConnectionTypes) || ( std::find(allowedConnectionTypesVector.begin(), allowedConnectionTypesVector.end(), MLintToString(thisConnection.type)) != allowedConnectionTypesVector.end() )  )
    {
      thisLine.startIndex = static_cast<MLuint32>(thisConnection.index1);
      thisLine.endIndex = static_cast<MLuint32>(thisConnection.index2);
      thisLine.width = lineWidth;

      // Make sure start index is < endIndex
      if (thisLine.startIndex > thisLine.endIndex)
      {
        MLuint32 temp = thisLine.endIndex;
        thisLine.endIndex = thisLine.startIndex;
        thisLine.startIndex = temp;
      }

      result.push_back(thisLine);
    }

  }
  
  return result;
}

//***********************************************************************************

// Automatically calculate simple line connections from XMarkerList
LinesVector PDFMarkerListTools::getStandardLinesFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth)
{
  LinesVector result;
  LineStruct thisLine;
  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning
  thisLine.width = lineWidth;

  StringVector allowedPositionTypesVector = PDFTools::stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  int startPositionIndex = -2;
  int endPositionIndex = -1;

  for (size_t i = 0; i < positionsListLength; i++)
  {
    ml::XMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), intToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      startPositionIndex++;
      endPositionIndex++;

      if (startPositionIndex >= 0)
      {
        thisLine.startIndex = startPositionIndex;
        thisLine.endIndex = endPositionIndex;
        result.push_back(thisLine);
      }
      
    }

  }
  
  return result;
}

//***********************************************************************************

// Automatically calculate simple line connections from ColoredMarkerList
LinesVector PDFMarkerListTools::getStandardLinesFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth)
{
  LinesVector result;
  LineStruct thisLine;
  thisLine.startIndex = 0;  // Init to avoid warning
  thisLine.endIndex = 0;    // Init to avoid warning
  thisLine.width = lineWidth;


  StringVector allowedPositionTypesVector = PDFTools::stringSplit(allowedPositionTypes, ",", false);

  size_t positionsListLength = positionsList.size();

  int startPositionIndex = -2;
  int endPositionIndex = -1;

  for (size_t i = 0; i < positionsListLength; i++)
  {
    ml::ColoredMarker thisPosition = positionsList[i];

    // If thisPoint is of allowed type
    if (  ("all" == allowedPositionTypes) || ( std::find(allowedPositionTypesVector.begin(), allowedPositionTypesVector.end(), MLintToString(thisPosition.type)) != allowedPositionTypesVector.end() )  )
    {
      startPositionIndex++;
      endPositionIndex++;

      if (startPositionIndex >= 0)
      {
        thisLine.startIndex = startPositionIndex;
        thisLine.endIndex = endPositionIndex;
        result.push_back(thisLine);
      }
      
    }

  }
  
  return result;
}

//***********************************************************************************

// Get all lines that end at a given position
LinesVector PDFMarkerListTools::getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition)
{
  LinesVector result;
  LineStruct newLine;

  size_t numberOfLines = allLines.size();

  for (size_t i = 0; i < numberOfLines; i++)
  {
    if (allLines[i].endIndex == endPosition)
    {
      newLine.startIndex = allLines[i].startIndex;
      newLine.endIndex = allLines[i].endIndex;
      result.push_back(newLine);
    }
  }

  return result;
}

//***********************************************************************************

void PDFMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(FiberSetContainer& outFiberSetContainer, const XMarkerList& inLinePositions, const IndexPairList& inLineConnections)
{
  outFiberSetContainer.deleteAllFiberSets();

  std::set<int> typeIDsSet;
  std::map<int, int> numPositionsPerType;
  std::map<int, std::string> typeLabels;

  // Step 1: Scan all positions and get all type ID from them
  for (XMarkerList::const_iterator it = inLinePositions.cbegin(); it != inLinePositions.cend(); ++it)
  {
    XMarker thisMarker = *it;
    typeIDsSet.insert(thisMarker.type);
    numPositionsPerType[thisMarker.type]++;
    typeLabels[thisMarker.type] = thisMarker.name();
  }

  // Step 2: Now create fibers
  for (std::set<int>::const_iterator typeIdIterator = typeIDsSet.cbegin(); typeIdIterator != typeIDsSet.cend(); ++typeIdIterator)
  {
    int thisTypeID = *typeIdIterator;

    FiberSetContainer::FiberSet* newFiberSet = outFiberSetContainer.createFiberSet();
    newFiberSet->setColor(Vector3(0));
    newFiberSet->setLabel(typeLabels[thisTypeID]);

    // Add all connections to the temporary connections list if type ID matches
    XMarkerList  workPositions;
    for (XMarkerList::const_iterator outPositionsIterator = inLinePositions.cbegin(); outPositionsIterator != inLinePositions.cend(); ++outPositionsIterator)
    {
      XMarker thisMarker = *outPositionsIterator;

      if (thisMarker.type == thisTypeID)
      {
        workPositions.push_back(thisMarker);
      }
    }
    MLint workPositionsSize = (MLint)workPositions.size();

    // Add all connections to the temporary connections list if type ID matches
    IndexPairList workConnections;
    for (IndexPairList::const_iterator outConnectionsIterator = inLineConnections.cbegin(); outConnectionsIterator != inLineConnections.cend(); ++outConnectionsIterator)
    {
      IndexPair thisPair = *outConnectionsIterator;

      if (thisPair.type == thisTypeID)
      {
        workConnections.push_back(thisPair);
      }
    }

    // If temporary connections list is still empty at this point: create default list
    if (workConnections.size() == 0)
    {
      int numPositionsOfThisType = numPositionsPerType[thisTypeID];

      for (int p = 0; p < numPositionsOfThisType - 1; p++)
      {
        IndexPair thisPair(p, p + 1);
        workConnections.push_back(thisPair);
      }
    }

    // Now finally create fibers from connections
    for (IndexPairList::const_iterator workListIterator = workConnections.cbegin(); workListIterator != workConnections.cend(); ++workListIterator)
    {
      IndexPair thisWorkPair = *workListIterator;

      MLint startIndex = thisWorkPair.index1;
      MLint endIndex = thisWorkPair.index2;

      if ((startIndex < workPositionsSize) && (endIndex < workPositionsSize))
      {
        XMarker startMarker = workPositions[startIndex];
        XMarker endMarker = workPositions[endIndex];

        FiberSetContainer::FiberSet::Fiber* newFiber = newFiberSet->createFiber();

        FiberSetContainer::FiberSet::Fiber::FiberPoint startPoint;
        startPoint.setCoordinates(startMarker.x(), startMarker.y(), startMarker.z());
        FiberSetContainer::FiberSet::Fiber::FiberPoint endPoint;
        endPoint.setCoordinates(endMarker.x(), endMarker.y(), endMarker.z());

        newFiber->appendPoint(startPoint);
        newFiber->appendPoint(endPoint);
        newFiber->setLabel(1.0);
      }
    }

    workPositions.clearList();
    workConnections.clearList();
  }

}

//***********************************************************************************

} // end namespace mlPDF

ML_END_NAMESPACE