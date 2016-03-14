//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    dicomDatasetParser.h
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#pragma once

#include <dcmtk/config/osconfig.h> //make sure OS specific configuration is included first

#include <exception>
#include <time.h>
#include <vector>
#include <boost/current_function.hpp>
#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>
#include <dcmtk/dcmdata/dctk.h>

#include "DICOMFileFormat/DCM_SharedTypes.h"

ML_START_NAMESPACE

class DicomDatasetParser {
public:
  DicomDatasetParser(SmartPtr_DCMDataSet dataset);

  std::string getStringAttribute(const DcmTagKey tag) const;
  std::string getMediaStorageSOPClassUID() const;

protected:
  SmartPtr_DCMDataSet _dataset; 

  DcmElement* searchFirstElement(DcmElement* ancestor, const DcmTag tag) const;
  char* searchFirstElementChar(DcmElement* ancestor, const DcmTag tag) const;
  boost::optional<unsigned short> searchFirstElementUnsignedShort(DcmElement* ancestor, const DcmTag tag) const;
  boost::optional<Uint32> searchFirstElementUnsignedLong(DcmElement* ancestor, const DcmTag tag) const;
  const char* getCharAttribute(const DcmTagKey tag) const;
  boost::shared_ptr<DcmStack> getElementsByTag(const DcmTag tag) const;
  std::vector<DcmItem*> getSequenceItems(const DcmTag tag) const;
  std::vector<DcmItem*> getSequenceItems(DcmItem* item, const DcmTag sequenceTag) const;
};

ML_END_NAMESPACE