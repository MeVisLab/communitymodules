//----------------------------------------------------------------------------------
//
// Copyright 2001, MeVis gGmbH
// ALL RIGHTS RESERVED
//
// THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
// INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
// DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
// PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
//
//----------------------------------------------------------------------------------
//! This class computes the main axis for a point cloud.
/*!
// \file    MainAxisPCA.h
// \author  Olaf Konrad, Christian Tietjen last changed by $Author: okonrad $
// \date    05/2006
*/
//----------------------------------------------------------------------------------


#ifndef __MainAxisPCA_H
#define __MainAxisPCA_H

#include <mlVec3.h>

#include "../MLCSOCommunityModulesSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

ML_START_NAMESPACE

//! This class computes the main axis for a point cloud by a principal component analysis.
class MLCSOCOMMUNITYMODULES_EXPORT MainAxisPCA
{
    
public:
    
    //! Constructor
    MainAxisPCA();
    
    //! Destructor
    virtual ~MainAxisPCA();
    
    //! Computes the main axes on behalf of the given point cloud.
    void computeMainAxis(const std::vector<ml::vec3>& points);    
    
    //! Returns the main axes.
    void getMainAxis(ml::vec3& xAxis, ml::vec3& yAxis, ml::vec3& zAxis);
    //! Returns the three extends of the object aligned bounding box
    void getExtension(float& xExt, float& yExt, float& zExt);
    
    //! Returns a copy of the largest main axis
    float* getLargestMainAxis();
    //! Returns the largest extend of the object aligned bounding box
    float getLargestExtension();
    
    //! Returns the barycenter of the object aligned bounding box.
    ml::vec3 getBaryCenter();
    //! Returns the middle point of the object aligned bounding box.
    ml::vec3 getMidPoint();
    
    
private:
    
    //! The three main axis.
    float *_xAxis, *_yAxis, *_zAxis;
    
    float *_baryCenter, *_midPoint;
    //! The three extensions of the object aligned bounding box.
    float _xDiameter, _yDiameter, _zDiameter;
    
    //! Calculation of the covarianz-matrix out of a point set.
    void getCovarianceMatrix(const float* vertices, const long size, float **covaMatrix, const float* meanVec);
    
    //! Calculation of the inverse matrix (3x3 matrices only)
    //! \c invMatrix is the inverted matrix of \c aMatrix.
    void getInverseMatrix(float** aMatrix, float** invMatrix);
    
    //! Returns a copy of the calculated barycenter.
    float* calcBaryCenter(const float* vertices, int size);
    
    //! Computes the dot (inner / scalar) product of two vectors.
    float dotProduct(const float* vec1, const float* vec2);
    
    //! Returns the BB of the point set.
    void getBoundingBox(const float* pointSet, const long size,     
        float& minX, float& maxX, 
        float& minY, float& maxY, 
        float& minZ, float& maxZ);
    
    //! Returns a copy of a vector with the direction of \c vector and the length \c length.
    float* stretchVector(const float* vector, const float length);
    
};


ML_END_NAMESPACE


#endif // __MainAxisPCA_H
