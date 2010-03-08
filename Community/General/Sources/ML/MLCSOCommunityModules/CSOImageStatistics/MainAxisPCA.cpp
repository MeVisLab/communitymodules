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
// \file    MainAxisPCA.cpp
// \author  Olaf Konrad, Christian Tietjen last changed by $Author: okonrad $
// \date    05/2006
*/
//----------------------------------------------------------------------------------

#include "MainAxisPCA.h"
#include "MainAxisPCAMatrixRoutines.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

MainAxisPCA::MainAxisPCA()
{
    ML_TRACE_IN("MainAxisPCA::MainAxisPCA()");

    _baryCenter = NULL;
    _xDiameter  = _yDiameter = _zDiameter = 0;
    
    _midPoint = NULL;
    _xAxis    = NULL;
    _yAxis    = NULL;
    _zAxis    = NULL;

    ML_CHECK_NEW(_midPoint, float[3]);
    ML_CHECK_NEW(_xAxis,    float[3]);
    ML_CHECK_NEW(_yAxis,    float[3]);
    ML_CHECK_NEW(_zAxis,    float[3]);
}

//////////////////////////////////////////////////////////////////////////

MainAxisPCA::~MainAxisPCA()
{   
    ML_TRACE_IN("MainAxisPCA::~MainAxisPCA()");

    ML_DELETE_ARR(_xAxis);
    ML_DELETE_ARR(_yAxis);
    ML_DELETE_ARR(_zAxis);
    ML_DELETE_ARR(_baryCenter);
    ML_DELETE_ARR(_midPoint);
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::getInverseMatrix(float **aMatrix, float **invMatrix) 
{
    ML_TRACE_IN("void MainAxisPCA::getInverseMatrix(float **aMatrix, float **invMatrix) ");

    // Compute inverse matrix (only for 3x3 matrices)
    // calculate determinant D and multiply the result with 1/D
    
    float det = aMatrix[1][1] * aMatrix[2][2] * aMatrix[3][3]
        + aMatrix[1][2] * aMatrix[2][3] * aMatrix[3][1]
        + aMatrix[1][3] * aMatrix[2][1] * aMatrix[3][2]
        - aMatrix[1][3] * aMatrix[2][2] * aMatrix[3][1]
        - aMatrix[1][1] * aMatrix[2][3] * aMatrix[3][2]
        - aMatrix[1][2] * aMatrix[2][1] * aMatrix[3][3];
    
    if (det != 0){ det = 1.0/det; }
    
    // Compute adjacent matrix, transpose it and multiply with D
    
    invMatrix[1][1] =  det * (aMatrix[2][2] * aMatrix[3][3] - aMatrix[2][3] * aMatrix[3][2]);
    invMatrix[2][1] = -det * (aMatrix[2][1] * aMatrix[3][3] - aMatrix[2][3] * aMatrix[3][1]);
    invMatrix[3][1] =  det * (aMatrix[2][1] * aMatrix[3][2] - aMatrix[2][2] * aMatrix[3][1]);
    invMatrix[1][2] = -det * (aMatrix[1][2] * aMatrix[3][3] - aMatrix[1][3] * aMatrix[3][2]);
    invMatrix[2][2] =  det * (aMatrix[1][1] * aMatrix[3][3] - aMatrix[1][3] * aMatrix[3][1]);
    invMatrix[3][2] = -det * (aMatrix[1][1] * aMatrix[3][2] - aMatrix[1][2] * aMatrix[3][1]);
    invMatrix[1][3] =  det * (aMatrix[1][2] * aMatrix[2][3] - aMatrix[1][3] * aMatrix[2][2]);
    invMatrix[2][3] = -det * (aMatrix[1][1] * aMatrix[2][3] - aMatrix[1][3] * aMatrix[2][1]);
    invMatrix[3][3] =  det * (aMatrix[1][1] * aMatrix[2][2] - aMatrix[1][2] * aMatrix[2][1]);
}

//////////////////////////////////////////////////////////////////////////

float* MainAxisPCA::calcBaryCenter(const float* vertices, int size) 
{
    ML_TRACE_IN("float* MainAxisPCA::calcBaryCenter(const float* vertices, int size) ");   

    // Computes the center of mass of a set of points and returns the center point
    if (size != 0) {
        float* meanVec = NULL;
        ML_CHECK_NEW(meanVec, float[3]);

        meanVec[0] = 0;
        meanVec[1] = 0;
        meanVec[2] = 0;
        
        for (int counter = 0; counter < size; counter++) {
            meanVec[0] += *vertices++;
            meanVec[1] += *vertices++;
            meanVec[2] += *vertices++;
        }
        // Division by the number of points
        meanVec[0] /= size;
        meanVec[1] /= size;
        meanVec[2] /= size;
        
        return meanVec;
    } 
    else{
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////

float MainAxisPCA::dotProduct(const float* vec1, const float* vec2) 
{
    ML_TRACE_IN("float MainAxisPCA::dotProduct(const float* vec1, const float* vec2) ");
    
    return (* vec1     ) * (* vec2     ) + 
           (*(vec1 + 1)) * (*(vec2 + 1)) + 
           (*(vec1 + 2)) * (*(vec2 + 2));
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::getBoundingBox(const float* pointSet, const long size, 
                                 float& minX, float& maxX, 
                                 float& minY, float& maxY, 
                                 float& minZ, float& maxZ) 
{
    ML_TRACE_IN("void MainAxisPCA::getBoundingBox(...)");

    // Return the bounding box of a point set
    if (pointSet != NULL && size > 0) {
        const float* pointer = pointSet;
        minX = maxX = *(pointer++);
        minY = maxY = *(pointer++);
        minZ = maxZ = *(pointer++);
        for (long counter = 1; counter < size; counter++) {
            if (minX > *pointer) minX = *pointer;
            if (maxX < *pointer) maxX = *pointer;
            pointer++;
            if (minY > *pointer) minY = *pointer;
            if (maxY < *pointer) maxY = *pointer;
            pointer++;
            if (minZ > *pointer) minZ = *pointer;
            if (maxZ < *pointer) maxZ = *pointer;
            pointer++;
        } // for
    } // if
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::getCovarianceMatrix(const float* vertices, const long size, float **covaMatrix, const float* meanVec) 
{
    ML_TRACE_IN("void MainAxisPCA::getCovarianceMatrix(const float* vertices, const long size, float **covaMatrix, const float* meanVec) ");

    // Compute the covariant matrix from a set of points:
    // the set of points and a matrix is given, that forms
    // the end of the covariant matrix.
    
    // Subtract the center of mass from each point's position
    float* difPoints = NULL;
    ML_CHECK_NEW(difPoints, float[size * 3]);
    
    int i=0,j=0;
    int counter=0, counter2 = 0;

    for (counter = 0; counter < size; counter++) {
        difPoints[counter2] = vertices[counter2] - meanVec[0];
        counter2++;
        difPoints[counter2] = vertices[counter2] - meanVec[1];
        counter2++;
        difPoints[counter2] = vertices[counter2] - meanVec[2];
        counter2++;
    }
    
    // Compute the (symmetric) covariance matrix
    
    // reset covariance matrix.
    covaMatrix[1][1] = 0; covaMatrix[1][2] = 0; covaMatrix[1][3] = 0; 
    covaMatrix[2][1] = 0; covaMatrix[2][2] = 0; covaMatrix[2][3] = 0; 
    covaMatrix[3][1] = 0; covaMatrix[3][2] = 0; covaMatrix[3][3] = 0; 
    
    for (counter = 0 ; counter < size; counter++) {
        covaMatrix[1][1] += difPoints[counter * 3    ] * difPoints[counter * 3    ];
        covaMatrix[1][2] += difPoints[counter * 3    ] * difPoints[counter * 3 + 1];
        covaMatrix[1][3] += difPoints[counter * 3    ] * difPoints[counter * 3 + 2];
        
        covaMatrix[2][2] += difPoints[counter * 3 + 1] * difPoints[counter * 3 + 1];
        covaMatrix[2][3] += difPoints[counter * 3 + 1] * difPoints[counter * 3 + 2];
        
        covaMatrix[3][3] += difPoints[counter * 3 + 2] * difPoints[counter * 3 + 2];
    }
    
    ML_DELETE_ARR(difPoints);
    
    // copy double entries (symmetric matrix)
    covaMatrix[2][1] = covaMatrix[1][2];
    covaMatrix[3][1] = covaMatrix[1][3];
    covaMatrix[3][2] = covaMatrix[2][3];
    
    // divide all matrix entries by the number of points.
    for (j = 1; j < 4; j++){
        for (i = 1; i < 4; i++){
            covaMatrix[j][i] /= size;
        }
    }        
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::computeMainAxis(const std::vector<ml::vec3>& pointCloud)
{
    ML_TRACE_IN("void MainAxisPCA::computeMainAxis()");

    float** covaMatrix = matrix(1,3,1,3);
    float** jacobiMat  = matrix(1,3,1,3);
    float** invMatrix  = matrix(1,3,1,3);
    float*  eigenValues = vL_vector(1,3);
    
    const unsigned int size = pointCloud.size();
    
    // copy positions to vertices
    float* vertices = NULL;
    ML_CHECK_NEW(vertices, float[size * 3]);

    unsigned int i=0;
    unsigned int entry = 0;

    for (i = 0; i < size; i++){

        const ml::vec3& currPos = pointCloud[i];

        vertices[entry++] = static_cast<float>(currPos[0]);
        vertices[entry++] = static_cast<float>(currPos[1]);
        vertices[entry++] = static_cast<float>(currPos[2]);
    }
    
    // Calculate center of mass
    ML_DELETE_ARR(_baryCenter);
    _baryCenter = calcBaryCenter(vertices, size);
    
    // Compute covariant matrix, so the Jacobian matrix can be computed
    getCovarianceMatrix(vertices, size, covaMatrix, _baryCenter);
    
    // Compute the Jacobian matrix
    int nrot=0; // dummy variable
    jacobi(covaMatrix, 3, eigenValues, jacobiMat, &nrot);
    
    // Calculate main axes
    unsigned int counter=0;
    for (counter = 0; counter < 3; counter++) {
        _xAxis[counter] = jacobiMat[counter + 1][1];
        _yAxis[counter] = jacobiMat[counter + 1][2];
        _zAxis[counter] = jacobiMat[counter + 1][3];
    }
    
    // Multiply each point by the eigenvectors of the Jacobian matrix
    const float* points = vertices;
    float* newVertices  =  NULL;
    ML_CHECK_NEW(newVertices, float[size * 3]);

    unsigned int counter2 = 0;
    for (counter = 0; counter < size; counter++) {
        
        newVertices[counter2++] = dotProduct(points, _xAxis); //tempPoint.dot(_xAxis);
        newVertices[counter2++] = dotProduct(points, _yAxis); //tempPoint.dot(_yAxis);
        newVertices[counter2++] = dotProduct(points, _zAxis); //tempPoint.dot(_zAxis);
        points += 3;
    }
    
    // Get extends of the bounding box
    float minX=0, maxX=0, minY=0, maxY=0, minZ=0, maxZ=0;
    getBoundingBox(newVertices, size, minX, maxX, minY, maxY, minZ, maxZ);
    
    // Extends of the object aligned bounding box
    _xDiameter = maxX - minX;
    _yDiameter = maxY - minY;
    _zDiameter = maxZ - minZ;
    
    // Half the extend...
    float half_x = _xDiameter / 2.0;
    float half_y = _yDiameter / 2.0;
    float half_z = _zDiameter / 2.0;
    
    // Rotate all points back by multiplying them with the inverse Jacobian matrix.
    getInverseMatrix(jacobiMat, invMatrix);
    
    float* tmp = stretchVector(_xAxis, half_x);  
    for (counter = 0; counter < 3; counter++) {  
        _midPoint[counter] =  
            minX           * invMatrix[1][counter + 1] + 
           (minY + half_y) * invMatrix[2][counter + 1] + 
           (minZ + half_z) * invMatrix[3][counter + 1] + tmp[counter];
    }
    
    ML_DELETE_ARR(tmp);
    ML_DELETE_ARR(newVertices);
    ML_DELETE_ARR(vertices);
    
    free_matrix(covaMatrix, 1, 3, 1, 3);
    free_matrix(jacobiMat,  1, 3, 1, 3);
    free_matrix(invMatrix,  1, 3, 1, 3);
    free(eigenValues);
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::getMainAxis(ml::vec3& xAxis, ml::vec3& yAxis, ml::vec3& zAxis)
{
    ML_TRACE_IN("void MainAxisPCA::getMainAxis()");

    for (int counter = 0; counter < 3; counter++) {
        xAxis[counter] = _xAxis[counter];
        yAxis[counter] = _yAxis[counter];
        zAxis[counter] = _zAxis[counter];
    }
}

//////////////////////////////////////////////////////////////////////////

ml::vec3 MainAxisPCA::getBaryCenter()
{
    ML_TRACE_IN("vec3 MainAxisPCA::getBaryCenter()");

    ml::vec3 result;
    
    result[0] = _baryCenter[0];
    result[1] = _baryCenter[1];
    result[2] = _baryCenter[2];
    
    return result;
}

//////////////////////////////////////////////////////////////////////////

ml::vec3 MainAxisPCA::getMidPoint()
{
    ML_TRACE_IN("vec3 MainAxisPCA::getMidPoint()");

    ml::vec3 result;
    
    result[0] = _midPoint[0];
    result[1] = _midPoint[1];
    result[2] = _midPoint[2];
    
    return result;
}

//////////////////////////////////////////////////////////////////////////

void MainAxisPCA::getExtension(float& xExt, float& yExt, float& zExt) 
{
    ML_TRACE_IN("void MainAxisPCA::getExtension(float& xExt, float& yExt, float& zExt) ");

    xExt = _xDiameter;
    yExt = _yDiameter;
    zExt = _zDiameter;
}

//////////////////////////////////////////////////////////////////////////

float* MainAxisPCA::getLargestMainAxis()
{
    ML_TRACE_IN("float* MainAxisPCA::getLargestMainAxis()");

    float* lA = NULL;
    ML_CHECK_NEW(lA, float[3]);

    if (_xDiameter > _yDiameter && _xDiameter > _zDiameter) {
        lA[0] = _xAxis[0];
        lA[1] = _xAxis[1];
        lA[2] = _xAxis[2];
        
        return lA;
    }
    if (_yDiameter > _xDiameter && _yDiameter > _zDiameter) {
        lA[0] = _yAxis[0];
        lA[1] = _yAxis[1];
        lA[2] = _yAxis[2];
    
        return lA;
    }
    lA[0] = _zAxis[0];
    lA[1] = _zAxis[1];
    lA[2] = _zAxis[2];

    return lA;
}

//////////////////////////////////////////////////////////////////////////

float MainAxisPCA::getLargestExtension()
{
    ML_TRACE_IN("float MainAxisPCA::getLargestExtension()");

    if ((_xDiameter > _yDiameter) && (_xDiameter > _zDiameter))
    {
        return _xDiameter;
    }
    if ((_yDiameter > _xDiameter) && (_yDiameter > _zDiameter)){
        return _yDiameter;
    }
    
    return _zDiameter;
}

//////////////////////////////////////////////////////////////////////////

float* MainAxisPCA::stretchVector(const float* vector, const float length) 
{
    ML_TRACE_IN("float* MainAxisPCA::stretchVector(const float* vector, const float length)");

    float* strVec = NULL;
    ML_CHECK_NEW(strVec, float[3]);
    
    // Sum of squares of the direction vector
    float sumVec = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
    
    // return if this sum equals 0
    if (sumVec == 0) {
        strVec[0] = strVec[1] = strVec[2] = 0;
        return strVec;
    }
    
    // |aVector| = sqrt(aVector[0]^2 + aVector[1]^2 + aVector[2]^2):
    float factor = sqrtf((length * length) / sumVec);
    
    strVec[0] = vector[0] * factor;
    strVec[1] = vector[1] * factor;
    strVec[2] = vector[2] * factor;
    
    return strVec;
}

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE
