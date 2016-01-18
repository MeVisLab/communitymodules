//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    coordinates3D.h
// \author  Christian Hoffmann
// \date    2013-11
//
// A class representing 3D-coordinates of e.g. a vertex or a mesh.
*/
//----------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <boost/current_function.hpp>
#include <boost/optional.hpp>

#include <mlTypeDefs.h> //for macro ML_NAMESPACE
#include <mlVector3.h>


class Coordinates3D {

public:  
  
  typedef enum  {
    ElementType_Unknown,     ///< unknown
    ElementType_Marker,      ///< vertex
    ElementType_Centerline,  ///< line
    ElementType_Segmentation ///< mesh
  } ElementType;


  Coordinates3D();
  void setSurfaceNumber(const boost::optional<unsigned long>);
  boost::optional<unsigned long> getSurfaceNumber() const;
  void setSurfaceComment(char*);
  Coordinates3D::ElementType getElementType() const;
  void setIsVolumeFinite(const bool);
  bool isVolumeFinite() const;
  void addCoordinate(const float, const float, const float);
  std::vector<ML_NAMESPACE::Vector3> getCoordinates() const;
  void addNormal(const float, const float, const float);
  std::vector<ML_NAMESPACE::Vector3> getNormals() const;
  void addCoordinateIndex(const unsigned long);
  std::vector<unsigned long> getCoordinateIndices() const;

private:

  boost::optional<unsigned long> _surfaceNumber; ///< The number of the surface.
  char* _surfaceComment; ///< The comment pertaining to the coordinates.
  ElementType _elementType; ///< The element type the coordinates relate to.

  /**
   * \brief Indicates wether the surface represents a solid ("waterproof") object with
   *        an inside and an outside.
   *
   *        Per default, this value is set to false.
   */
  bool _isVolumeFinite;

  /**
  * \brief  The coordinates.
  * 
  * If the coordinate numbers are to be reordered, the order information is stored
  * in the coordinateIndices-vector. Otherwise, this vector is empty.
  */
  std::vector<ML_NAMESPACE::Vector3> _coordinates;

  /**
   * Index-order information for the coordinate points and normals.
   *
   * If the points described by the coordinates-array and the normals-array
   * are to be reordered, this vector holds the vector indices in the correct order. 
   * If the aformenetioned vectors do already contain the coordinate points in correct
   * order, the coordinateIndices-vector is empty.
   */
  std::vector<unsigned long> _coordinateIndices;

  /**
   * \brief  The normals.
   *
   * If the normals are to be reordered, the order information is stored in the 
   * coordinateIndices-vector. Otherwise, this vector is empty.
   */
  std::vector<ML_NAMESPACE::Vector3> _normals;

  ElementType parseElementType(char* elementName);
};