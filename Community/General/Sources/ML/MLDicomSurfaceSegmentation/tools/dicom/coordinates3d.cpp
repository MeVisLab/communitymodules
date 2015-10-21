//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    coordinates3D.cpp
// \author  Christian Hoffmann
// \date    2013-11
//
// A class representing 3D-coordinates of e.g. a vertex or a mesh.
*/
//----------------------------------------------------------------------------------

#include "coordinates3D.h"

/**
 * \brief  Standard-constructor.
 */
Coordinates3D::Coordinates3D()
{
	_surfaceNumber = boost::none;
	_surfaceComment = "";
	_elementType = ElementType_Unknown;
	_isVolumeFinite = false;
	_coordinates.clear();
	_normals.clear();
	_coordinateIndices.clear();
}

/**
 * \brief  Sets the number of the surface defined by the coordinates.
 *		   
 * cf. DICOM tag 0066,0003 SurfaceNumber UL
 *
 * \param  [in] surfaceNumber The surface number.
 */
void Coordinates3D::setSurfaceNumber(const boost::optional<unsigned long> surfaceNumber)
{
	_surfaceNumber = surfaceNumber;
}

/**
 * \brief Returns the number of the surface defined by the coordinates.
 *
 * cf. DICOM tag 0066,0003 SurfaceNumber UL
 * 
 * \return The surface number. If the number is not set, boost::none is returned.
 */
boost::optional<unsigned long> Coordinates3D::getSurfaceNumber() const
{
	return _surfaceNumber;
}

/**
 * \brief  Sets the comment pertaining to the coordinates.
 * 
 *  This function also sets the type of geometrical element the 
 *  coordinates relate to. Therefore, the comment is regarded as 
 *  a string-representation of an Element3DType; if the comment 
 *  cannot case-insensitively be parsed into an element of this 
 *  enum, the element-type is set to Unknown.<br />
 *  (cf. DICOM tag 0066,0004 SurfaceComments UL)
 *
 * \param  [in] surfaceComment The surface comment. If the parameter is NULL,
 *         the surface comment is not changed.
 */
void Coordinates3D::setSurfaceComment(char* surfaceComment)
{
	if(surfaceComment == NULL) {
		std::cerr << "Surface-comment is NULL. Previous comment retained.";
		return;
	}
	_surfaceComment = surfaceComment;
	_elementType = parseElementType(surfaceComment);
}

/**
 * \brief  Returns the type of geometrical element the coordinates relate to.
 * 
 * For setting the element-type, see the setSurfaceComment-method.
 *
 * \return The type of element the coordinates relate to. If the 
 *         type is not set, Element3DType::Unknown is returned.
 */
Coordinates3D::ElementType Coordinates3D::getElementType() const
{
	return _elementType;
}

/**
 * \brief  Indicates wether the coordinates represent a solid ("waterproof")
 *         object with and outside and an inside.
 *
 * cf. DICOM tag 0066,000e Finite Volume CS
 * 
 * \param [in] isVolumeFinite True if the volume is finite, false otherwise.
 */
void Coordinates3D::setIsVolumeFinite(const bool isVolumeFinite)
{
	_isVolumeFinite = isVolumeFinite;
}

/**
 * \brief  Indicates wether the coordinates represent a solid ("waterproof")
 *         object with and outside and an inside.
 *
 * cf. DICOM tag 0066,000e Finite Volume CS
 *
 * \return True if the volume is finite, false otherwise.
 */
bool Coordinates3D::isVolumeFinite() const
{
	return _isVolumeFinite;
}

/**
 * \brief  Adds a new coordinate.
 *
 * \param  [in] x The x-position. 
 * \param  [in] y The y-position.
 * \param  [in] z The z-position.
 */
void Coordinates3D::addCoordinate(const float x, const float y, const float z)
{
	_coordinates.push_back(ML_NAMESPACE::Vector3(x, y, z));
}

/**
 * \brief  Returns all coordinates.
 *
 * \return All coordinates.<br />
 *         If the coordinate numbers are to be reordered, the order information is stored in the 
 *         coordinateIndices-vector. Otherwise, this vector is empty.
 */
std::vector<ML_NAMESPACE::Vector3> Coordinates3D::getCoordinates() const
{
	return _coordinates;
}

/**
 * \brief  Adds a new normal
 *
 * \param  [in] x The x-position.
 * \param  [in] y The y-position.
 * \param  [in] z The z-position.
 */
void Coordinates3D::addNormal(const float x, const float y, const float z)
{
	_normals.push_back(ML_NAMESPACE::Vector3(x, y, z));
}

/**
 * \brief  Returns all normals.
 *
 * \return All normals (or an empty vector in case the normals are not set).<br />
 *		   If the normals are to be reordered, the order information is stored in the 
 *		   coordinateIndices-vector. Otherwise, this vector is empty.
 */
std::vector<ML_NAMESPACE::Vector3> Coordinates3D::getNormals() const
{
	return _normals;
}

/**
 * \brief  Adds a new index-ordering information.
 *
 * \param  [in] index The index to be added at the end of the ordering
 *         information vector.
 */
void Coordinates3D::addCoordinateIndex(const unsigned long index)
{
	_coordinateIndices.push_back(index);
}

/**
 * \brief  Returns index-ordering information for the coordinate points and normals.
 *
 * \return The index-ordering information: If the points described by the 
 *         coordinates-array and the normals-array are to be reordered, the
 *         returned vector holds the vector indices in the correct order. If the 
 *		   aforementioned vectors do already contain the coordinate points in correct
 *         order, the returned vector is empty.
 */
std::vector<unsigned long> Coordinates3D::getCoordinateIndices() const
{
	return _coordinateIndices;
}

Coordinates3D::ElementType Coordinates3D::parseElementType(char* elementName)
{
	if(elementName == NULL) {
		std::cerr << "Element name is NULL. Returning element type UNKOWN.";
		return ElementType_Unknown;
	}

	ElementType result = ElementType_Unknown;

	char *str = _strdup(elementName);
	for (unsigned int i=0; i<strlen(elementName); i++)
		str[i] = static_cast<char>(tolower(elementName[i]));

	if(!strcmp(str, "marker"))
		result = ElementType_Marker;
	else if(!strcmp(str, "centerline"))
		result = ElementType_Centerline;
	else if(!strcmp(str, "segmentation"))
		result = ElementType_Segmentation;

	free(str);
	return result;
}