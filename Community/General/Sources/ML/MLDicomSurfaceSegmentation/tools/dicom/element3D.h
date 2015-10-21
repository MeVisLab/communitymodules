//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    element3D.h
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#pragma once

#include <dcmtk/config/osconfig.h> //make sure OS specific configuration is included first

#include <boost/current_function.hpp>
#include <boost/optional.hpp>
#include <dcmtk/dcmdata/dctk.h>
#include "coordinates3d.h"

class Element3D {

public:
	Element3D(DcmElement*);
	DcmElement* getSegmentSequence() const;
	void setSegmentNumber(const boost::optional<unsigned short>);
	boost::optional<unsigned short> getSegmentNumber() const;
	void setSegmentLabel(char*);
	char* getSegmentLabel() const;
	void setAnatomicRegion(char*);
	char* getAnatomicRegion() const;
	void setReferencedSurfaceNumber(const boost::optional<unsigned long>);
	boost::optional<unsigned long> getReferencedSurfaceNumber() const;
	void setReferencedSurface(Coordinates3D);
	Coordinates3D getReferencedSurface() const;
	void setReferencedSeriesInstanceId(char*);
	char* getReferencedSeriesInstanceId() const;

private:
	DcmElement* _segmentSequence; ///< The segment-sequence pertaining to the 3D-element.
	boost::optional<unsigned short> _segmentNumber; //< The segment-number pertaining to the 3D-element.
	char* _segmentLabel; ///< The segment-label pertaining to the 3D-element.
	char* _anatomicRegion; ///< The anatomic region pertaining to the 3D-element.
	boost::optional<unsigned long> _referencedSurfaceNumber; ///< The surface number referenced by the 3D-element.
	Coordinates3D _coordinates; ///< The surface referenced by the 3D-element.
	char* _referencedSeriesInstanceId; ///< The id of the series instance referenced by the 3D-element.

};