//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_MarkerListTools.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_MarkerListTools_H
#define _PDF_MarkerListTools_H 

#include "MLPDFSystem.h"
#include "MLPDF_DataTypes.h"

#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>
#include <mlFiberSet.h>

#include "mlModuleIncludes.h"

// Inventor includes
#include <Inventor/nodes/SoNodes.h>  // for M_PI

#include <sys/stat.h>


ML_START_NAMESPACE

namespace mlPDF {

class MLPDF_EXPORT PDFMarkerListTools
{

public:

  //////////////////////////////////////////////

  // Get all positions (vertices) from ColoredMarkerList
  static PositionsVector getAllPositionsFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

  // Get all positions (vertices) from XMarkerList
  static PositionsVector getAllPositionsFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double pointSize);

  //***********************************************************************************

  // Get all line connections from IndexPairList
  static LinesVector getAllLinesFromIndexPairList(const ml::IndexPairList connectionsList, const std::string allowedConnectionTypes, double lineWidth);

  // Automatically calculate simple line connections from XMarkerList
  static LinesVector getStandardLinesFromXMarkerList(const ml::XMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

  // Automatically calculate simple line connections from  ColoredMarkerList
  static LinesVector getStandardLinesFromColoredMarkerList(const ml::ColoredMarkerList positionsList, const std::string allowedPositionTypes, double lineWidth);

  // Get all lines that end at a given position
  static LinesVector getNewLinesFromAllLines(LinesVector allLines, MLuint endPosition);

  //***********************************************************************************

  // Fill a FiberSetContainer from line positions and line connections
  static void fillFiberSetContainerFromPositionsAndConnections(ml::FiberSetContainer& outFiberSetContainer, const ml::XMarkerList& inLinePositions, const ml::IndexPairList& inLineConnections);

  //////////////////////////////////////////////

};

} // end namespace mlPDF

ML_END_NAMESPACE

#endif // _PDF_MarkerListTools_H