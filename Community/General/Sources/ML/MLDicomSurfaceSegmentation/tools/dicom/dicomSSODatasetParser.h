//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    dicomSSODatasetParser.h
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <limits>
#include <string>
#include <boost/foreach.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "dicomDatasetParser.h"
#include "element3D.h"
#include "coordinates3d.h"

#include <mlTypeDefs.h> //for macro ML_NAMESPACE
#include <mlVector3.h>

#include "DICOMFileFormat/DCM_SharedTypes.h"

ML_START_NAMESPACE

class DicomSSODatasetParser : public DicomDatasetParser {

public:
  DicomSSODatasetParser(SmartPtr_DCMDataSet);
  std::vector<Element3D> parseDataset(const bool isParsingNormalsEnabled) const;

private:
  std::vector<Element3D> parseSegmentSequenceItems(std::vector<DcmItem*> segmentSequenceItems) const;
  std::vector<Coordinates3D> parseSurfaceSequenceItems(std::vector<DcmItem*> surfaceSequenceItems, const bool isParsingNormalsEnabled) const;

  const float* getCoordinates(DcmItem* surfaceSequence, Uint32& numberOfCoordinates) const;
  static bool addCoordinatesToCoordinates3dInstance(DcmItem* surfaceMeshSequenceItem, const DcmTag pointIndexListTag, const float* coordinates, Coordinates3D& coordinates3d);
  static bool addCoordinatesToCoordiantes3DInstanceUsingIndices(DcmItem* surfaceMeshSequenceItem, DcmTag pointIndexListTag, const float* coordinates, Coordinates3D& coordinates3d, const unsigned long numOfCoordinates);
  static bool setNormalOfCoordinates3dInstance(DcmItem* surfacePointsNormalsSequenceItem, Coordinates3D& coordinates3d);
  bool addReferencedSeriesSequenceInformationToElements(std::vector<Element3D>& surfaceSequenceItems, DcmItem* referencedSeriesSequenceItem) const;
  static void matchElementsWithCoordinates(std::vector<Element3D>& surfaceSequenceItems, std::vector<Coordinates3D>& coordinates3d);
  static unsigned short getLowestValueInList(const unsigned short* list, const unsigned long numElements);
};

ML_END_NAMESPACE