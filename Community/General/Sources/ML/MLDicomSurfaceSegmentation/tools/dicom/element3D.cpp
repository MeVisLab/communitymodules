//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    element3D.cpp
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#include "element3D.h"

/**
 * \brief  Standard-constructor.
 *
 * \throws std::exception If the segmentSequence-parameter is NULL.
 */
Element3D::Element3D(DcmElement* segmentSequence)
{
	if(segmentSequence == NULL) {
		std::cerr << "Segment sequence is NULL.";
		throw std::exception("Segment sequence is NULL.");
	}

	_segmentSequence = segmentSequence;
	_segmentNumber = boost::none;
	_segmentLabel = "";
	_anatomicRegion = "";
	_referencedSurfaceNumber = boost::none;
	_coordinates = Coordinates3D();
	_referencedSeriesInstanceId = "";
}

/**
 * \brief  Returns the segment-sequence pertaining to the 3D-element.
 *
 * cf. DICOM tag 0062,0002 SegmentSequence SQ
 *
 * \return The segment-sequence.
 */
DcmElement* Element3D::getSegmentSequence() const
{
	return _segmentSequence;
}

/**
 * \brief  Sets the segment-number pertaining to the 3D-element.
 *
 * cf. DICOM tag 0062,0004 SegmentLabel US
 *
 * \param  [in] segmentNumber The segment-number.
 */
void Element3D::setSegmentNumber(const boost::optional<unsigned short> segmentNumber)
{
	_segmentNumber = segmentNumber;
}

/**
 * \brief  Returns the segment-number pertaining to the 3D-element.
 *
 * cf. DICOM tag 0062,0004 SegmentNumber US
 * 
 * \return The segment-number. If the number is not set, boost::none is returned.
 */
boost::optional<unsigned short> Element3D::getSegmentNumber() const
{
	return _segmentNumber;
}

/**
 * \brief  Sets the segment-label pertaining to the 3D-element.
 *
 * cf. DICOM tag 0062,0005 SegmentLabel LO
 *
 * \param  [in] segmentLabel The segment-label. If the label is NULL,
 *         the method returns without having any effect.
 */
void Element3D::setSegmentLabel(char* segmentLabel)
{
	if(segmentLabel == NULL) {
		std::cerr << "Segment label is NULL. Previous label retained.";
		return;
	}

	_segmentLabel = segmentLabel;
}

/**
 * \brief Returns the segment-label pertaining to the 3D-element.
 *
 * cf. DICOM tag 0062,0005 SegmentLabel LO
 * 
 * \return The segment-label. If the label is not set, an empty
 *         string is returned.
 */
char* Element3D::getSegmentLabel() const
{
	return _segmentLabel;
}

/**
 * \brief  Sets the anatomic region pertaining to the 3D-element.
 *
 * cf. DICOM tags 0008,2218 AnatomicRegionSequence SQ and 0008,0104 CodeMeaning LO
 *
 * \param  [in] anatomicRegion The anatomic region. If the region is NULL,
 *         the method returns without having any effect.
 */
void Element3D::setAnatomicRegion(char* anatomicRegion)
{
	if(anatomicRegion == NULL) {
		std::cerr << "Anatomic region is NULL. Previous region retained.";
		return;
	}

	_anatomicRegion = anatomicRegion;
}

/**
 * \brief  Returns the anatomicRegion pertaining to the 3D-element.
 *
 * cf. DICOM tags 0008,2218 AnatomicRegionSequence SQ and 0008,0104 CodeMeaning LO
 * 
 * \return The anatomic region. If the region is not set, an empty string
 *         is returned.
 */
char* Element3D::getAnatomicRegion() const
{
	return _anatomicRegion;
}

/**
 * \brief  Sets the surface number referenced by the 3D-element.
 *
 * cf. DICOM tag 0066,002c ReferencedSurfaceNumber UL
 *
 * \param  [in] referencedSurfaceNumber The referenced surface number.
 */
void Element3D::setReferencedSurfaceNumber(const boost::optional<unsigned long> referencedSurfaceNumber)
{
	_referencedSurfaceNumber = referencedSurfaceNumber;
}

/**
 * \brief Returns the surface number referenced by the 3D-element.
 *
 * cf. DICOM tag 0066,002c ReferencedSurfaceNumber UL
 * 
 * \return The referenced surface number. If the number is not set,
 *         boost::none is returned.
 */
boost::optional<unsigned long> Element3D::getReferencedSurfaceNumber() const
{
	return _referencedSurfaceNumber;
}

/**
 * \brief  Sets the surface referenced by the 3D-element.
 *
 * \param  [in] referencedSurface The referenced surface (i.e. the coordinates).
 */
void Element3D::setReferencedSurface(Coordinates3D referencedSurface)
{
	_coordinates = referencedSurface;
}

/**
 * \brief   Returns the surface referenced by the 3D-element.
 *
 * \return  referencedSurface The referenced surface (i.e. the coordinates).
 *          If the surface is not set, an empty Coordinates3D-instance
 *          is returned.
 */
Coordinates3D Element3D::getReferencedSurface() const
{
	return _coordinates;
}

/**
 * \brief  Sets the id of the series instance referenced by the 3D-element.
 *
 * cf. DICOM tags 0008,1115 ReferencedSeriesSequence SQ and 0020,000E SeriesInstanceUID LO
 *
 * \param  [in] referencedSeriesInstanceId The id of the referenced surface. If the id is NULL,
 *         the method returns without having any effect.
 */
void Element3D::setReferencedSeriesInstanceId(char* referencedSeriesInstanceId)
{
	if(referencedSeriesInstanceId == NULL) {
		std::cerr << "Referenced series instance id is NULL. Previous id retained.";
		return;
	}

	_referencedSeriesInstanceId = referencedSeriesInstanceId;
}

/**
 * \brief  Returns the id of the series instance referenced by the 3D-element.
 *
 * cf. DICOM tags 0008,1115 ReferencedSeriesSequence SQ and 0020,000E SeriesInstanceUID LO
 * 
 * \return The id of the referenced series instance. If the id is not set, an empty string
 *         is returned.
 */
char* Element3D::getReferencedSeriesInstanceId() const
{
	return _referencedSeriesInstanceId;
}
