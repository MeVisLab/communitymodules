/*
 * Copyright 2001, MeVis gGmbH
 * ALL RIGHTS RESERVED
 *
 * THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
 * INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
 * DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
 * PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
 *
 */



// +-----------------------------------------------------+
// |                                                     |
// |        Main Axis Computation of a Point Set         |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDMainAxis.h,v 1.7 2003/02/19 16:36:42 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/InventorModules/SoMeasurement/UMDMainAxis.h,v $

/*! Class UMDMainAxis.
// \file    UMDMainAxis.h
// \author  Christian Tietjen
// \date    09/2002
*/
#ifndef _UMDMainAxis
#define _UMDMainAxis

#include "UMDMeasurementInit.h"

//! Some functions to compute three vectors from a set of points
//! from the R3. The vectors represent the global orientation of the
//! point set.
class SO_MEASUREMENT_CLASS_SPEC UMDMainAxis {

public:

  //! Constructor
  UMDMainAxis();

  //! Desructor
  virtual ~UMDMainAxis();

  //! This method passes the pointset in the form of a float-array (x,y,z, x,y,z, ...)
  //! and the number of the vectors
  void computeMainAxis(const float* pointSet, const long size);

  //! returns a copy of the three main axis
  void getMainAxis(float*& xAxis, float*& yAxis, float*& zAxis);
  //! returns the three extensions of the object-orientated bounding box
  void getExtension(float& xExt, float& yExt, float& zExt);

  //! returns a copy of the largest main axis
  float* getLargestMainAxis();
  //! returns the largest extension of the object-orientated bounding box
  float getLargestExtension();

  //! returns a copy of the barycenter
  void getBaryCenter(float* x, float* y, float* z);
  //! returns a copy of the midpoint
  void getMidPoint(float* x, float* y, float* z);


private:

  //! the three main axis
  float *_xAxis, *_yAxis, *_zAxis;
  //! what could that be?
  float *_baryCenter, *_midPoint;
  //! the three extensions of the object-orientated bounding box
  float _xDiameter, _yDiameter, _zDiameter;

  //! calculation of the covarianz-matrix out of a pointset
  void getCovarianceMatrix(const float* vertices, const long size, float **covaMatrix, const float* meanVec);

  //! calculation of the inverse matrix (3x3 matrices only)
  //! \c invMatrix is the inverted matrix of \c aMatrix
  void getInverseMatrix(float** aMatrix, float** invMatrix);

  //! returns a copy of the calculated barycenter.
  float* calcBaryCenter(const float* vertices, int size);

  //! computes the dot (inner / scalar) product of two vectors
  float dotProduct(const float* vec1, const float* vec2);
  //! retruns the BB of the point set
  void getBoundingBox(const float* pointSet, const long size,
    float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);

  //! returns a copy of a vector with the direction of \c vector and the length \c length
  float* stretchVector(const float* vector, const float length);

};

#endif
