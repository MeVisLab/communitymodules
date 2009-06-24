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
//! TileSphere is used in the class \c MinimalDistancePointClouds.
//! It defines a single sub sphere for 
//! a fast nearest pair search algortihm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
//! TileSphere will generate her own sub spheres if necessary
/*!
// \file    TileSphere.cpp
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------

#include "TileSphere.h"
#include "MinimalDistancePointClouds.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

TileSphere::TileSphere() 
{
    ML_TRACE_IN("TileSphere::TileSphere() ");

    _initSphere(NULL, 2, 1103, 0);
}

//////////////////////////////////////////////////////////////////////////

TileSphere::TileSphere(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) 
{
    ML_TRACE_IN("TileSphere::TileSphere(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) ");

    _initSphere(minimalDistance, maxPartition, numElements, error);
}

//////////////////////////////////////////////////////////////////////////

TileSphere::~TileSphere() 
{
    ML_TRACE_IN("TileSphere::~TileSphere()");

    ML_DELETE_ARR(_tileSpheres);
    ML_DELETE_ARR(_subset);
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::setParameter(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) 
{
    ML_TRACE_IN("void TileSphere::setParameter(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) ");

    _minimalDistance = minimalDistance;
    _maxEntries      = numElements;
    _partition       = maxPartition;
    _cubicPartition  = _partition * _partition * _partition;
    _sphereIndex     = minimalDistance->getUniqueIndex();
    _error           = error;

    _subset = NULL;
    ML_CHECK_NEW(_subset, float*[numElements]);
}

//////////////////////////////////////////////////////////////////////////

float TileSphere::computeDistance(TileSphere* tileSphere, float*& point1, float*& point2) 
{
    ML_TRACE_IN("float TileSphere::computeDistance(TileSphere* tileSphere, float*& point1, float*& point2)");

    float distance = -1;
    // Determine current distance
    if ((point1 != NULL) && (point2 != NULL)){
        distance = _distance(point1, point2);
    }

    if (tileSphere != NULL) {

        // Register this call in the hash table
        _minimalDistance->getHashTable()->addPair(this, tileSphere);

        // Distance can not be less..
        if (fabs(distance) < FLT_EPSILON){
            return 0;
        }

        // Descend recursively in both sets until leafs are reached.
        // On the way, split the sphere with the largest radius.

        if ((_radius > tileSphere->_getRadius()) && _hasSubSpheresFlag) {
            distance = tileSphere->computeDistance(this->_getPreciserSphere(tileSphere, distance), point2, point1);
        } else {
            if ((_radius < tileSphere->_getRadius()) && tileSphere->_hasSubSpheres()) {
                distance = this->computeDistance(tileSphere->_getPreciserSphere(this, distance), point1, point2);
            } else {
                // This branch is active if the one sphere was smaller,
                // but the other did not contain any sub spheres.
                if (_hasSubSpheresFlag) {
                    distance = tileSphere->computeDistance(this->_getPreciserSphere(tileSphere, distance), point2, point1);
                }
                if (tileSphere->_hasSubSpheres()) {
                    distance = this->computeDistance(tileSphere->_getPreciserSphere(this, distance), point1, point2);
                }
            }
        }

        if (!_hasSubSpheresFlag && !tileSphere->_hasSubSpheres()) {
            // Compute direct distance between two sets of points.

            float** otherSubset = NULL;
            unsigned int otherSize = 0;            
            float tmpDistance   = 0;
            float tmpDistance2  = 0;
            float *tmpPoint1    = NULL;
            float *tmpPoint2    = NULL;

            int otherSizeParameter=0;
            tileSphere->_getSubset(otherSubset, otherSizeParameter);
            otherSize = static_cast<unsigned int>(otherSizeParameter);

            if (distance == -1) {
                distance = _distance(_subset[0], otherSubset[0]);
                point1   = _subset[0];
                point2   = otherSubset[0];
            }

            tmpDistance2 = _fastDistance(_subset[0], otherSubset[0]);
            tmpPoint1    = _subset[0];
            tmpPoint2    = otherSubset[0];

            unsigned int counter=0, otherCounter=0;

            // Test all points pairwise
            for (otherCounter = 0; otherCounter < otherSize; otherCounter++) {
                for (counter = 0; counter < _numEntries; counter++) {
                    tmpDistance = _fastDistance(_subset[counter], otherSubset[otherCounter]);
                    if (tmpDistance2 > tmpDistance) {
                        tmpDistance2 = tmpDistance;
                        tmpPoint1 = _subset[counter];
                        tmpPoint2 = otherSubset[otherCounter];
                    }
                }
            }

            tmpDistance2 = sqrtf(tmpDistance2);
            if (distance > tmpDistance2) {
                distance = tmpDistance2;
                point1 = tmpPoint1;
                point2 = tmpPoint2;
            }
        }
        if (_hasSubSpheresFlag) {
            // Compare the distance with all distances to other spheres
            // and test if there are other spheres within the same range.
            TileSphere* tmp = this->_getPreciserSphere(tileSphere, distance);
            while (tmp != NULL) {
                distance = tileSphere->computeDistance(tmp, point1, point2);
                tmp = this->_getPreciserSphere(tileSphere, distance);
            }
        }
        if (tileSphere->_hasSubSpheres()) {
            // Compare the distance with all distances to each other spheres
            // and test if there are other spheres within the same range.
            TileSphere* tmp = tileSphere->_getPreciserSphere(this, distance);
            while (tmp != NULL) {
                distance = this->computeDistance(tmp, point1, point2);
                tmp = tileSphere->_getPreciserSphere(this, distance);
            }
        }
    }

    return distance;
}

//////////////////////////////////////////////////////////////////////////

unsigned int TileSphere::getSphereIndex() 
{
    ML_TRACE_IN("unsigned int TileSphere::getSphereIndex() ");

    return _sphereIndex;
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::getDeFactoSize() 
{
    ML_TRACE_IN("void TileSphere::getDeFactoSize()");

    // Computes the central point of the BB and acquire the farthest 
    // sub sphere/point of the sub spheres/pointset for the radius
    float minX=0, maxX=0, minY=0, maxY=0, minZ=0, maxZ=0;
    bool  unInit = true;

    unsigned int counter=0;

    if (_hasSubSpheresFlag) {
        // Computes the BB of the contained spheres
        float buffer=0;
        for (counter = 0; counter < _cubicPartition; counter++) {
            _tileSpheres[counter].getDeFactoSize();
            // not every sphere has
            if (_tileSpheres[counter]._radius != -1) {
                if (unInit) {
                    minX = _tileSpheres[counter]._position[0] - _tileSpheres[counter]._radius;
                    maxX = _tileSpheres[counter]._position[0] + _tileSpheres[counter]._radius;
                    minY = _tileSpheres[counter]._position[1] - _tileSpheres[counter]._radius;
                    maxY = _tileSpheres[counter]._position[1] + _tileSpheres[counter]._radius;
                    minZ = _tileSpheres[counter]._position[2] - _tileSpheres[counter]._radius;
                    maxZ = _tileSpheres[counter]._position[2] + _tileSpheres[counter]._radius;
                    unInit = false;
                } else {
                    buffer = _tileSpheres[counter]._position[0] - _tileSpheres[counter]._radius;
                    if (minX > buffer) { minX = buffer; }
                    buffer = _tileSpheres[counter]._position[0] + _tileSpheres[counter]._radius;
                    if (maxX < buffer) { maxX = buffer; }
                    buffer = _tileSpheres[counter]._position[1] - _tileSpheres[counter]._radius;
                    if (minY > buffer) { minY = buffer; }
                    buffer = _tileSpheres[counter]._position[1] + _tileSpheres[counter]._radius;
                    if (maxY < buffer) { maxY = buffer; }
                    buffer = _tileSpheres[counter]._position[2] - _tileSpheres[counter]._radius;
                    if (minZ > buffer) { minZ = buffer; }
                    buffer = _tileSpheres[counter]._position[2] + _tileSpheres[counter]._radius;
                    if (maxZ < buffer) { maxZ = buffer; }
                }
            }
        }
    } else {
        if (_numEntries != 0) {
            unInit = false;
            float* pointPos = _subset[0];
            minX = maxX = * pointPos;
            minY = maxY = *(pointPos + 1);
            minZ = maxZ = *(pointPos + 2);

            for (counter = 1; counter < _numEntries; counter++) {
                pointPos = _subset[counter];
                if (minX > *pointPos) { minX = *pointPos; }
                if (maxX < *pointPos) { maxX = *pointPos; }
                pointPos++;
                if (minY > *pointPos) { minY = *pointPos; }
                if (maxY < *pointPos) { maxY = *pointPos; }
                pointPos++;
                if (minZ > *pointPos) { minZ = *pointPos; }
                if (maxZ < *pointPos) { maxZ = *pointPos; }
            }
        }
    }
    if (!unInit) {
        _position[0] = (maxX + minX) * 0.5f;
        _position[1] = (maxY + minY) * 0.5f;
        _position[2] = (maxZ + minZ) * 0.5f;
        if (_hasSubSpheresFlag) {
            bool init = true;
            float tmpDist=0;
            for (counter = 0; counter < _cubicPartition; counter++) {
                if (_tileSpheres[counter]._getRadius() != -1) {
                    if (init) {                        
                        _radius = _distance(_position, _tileSpheres[counter]._getPosition()) + _tileSpheres[counter]._getRadius();
                        init = false;
                    } else {
                        tmpDist = _distance(_position, _tileSpheres[counter]._getPosition()) + _tileSpheres[counter]._getRadius();
                        if (tmpDist > _radius){
                            _radius = tmpDist;
                        }
                    }
                }
            }
        } else {
            float tmpDist;
            _radius = _distance(_position, _subset[0]);
            for (counter = 1; counter < _numEntries; counter++) {
                tmpDist = _distance(_position, _subset[counter]);
                if (tmpDist > _radius){
                    _radius = tmpDist;
                }
            }
        }
    } else {
        _radius = -1;
    }
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::getStatistics(int &_treeDepth, int &_usedSpheres, int &_leafSpheres, int &_maxEnt) 
{
    ML_TRACE_IN("void TileSphere::getStatistics(int &_treeDepth, int &_usedSpheres, int &_leafSpheres, int &_maxEnt) ");

    int treeDepth  = 0, treeDepthTmp = 0;
    float** dummy  = NULL;
    int subsetSize = 0;

    unsigned int counter=0;

    // Has sub spheres
    if (_hasSubSpheresFlag) {
        for (counter = 0; counter < _cubicPartition; counter++) {
            _tileSpheres[counter]._getSubset(dummy, subsetSize);
            if (subsetSize != 0 || _tileSpheres[counter]._hasSubSpheres()) {
                _tileSpheres[counter].getStatistics(treeDepthTmp, _usedSpheres, _leafSpheres, _maxEnt);
                if (treeDepthTmp > treeDepth) {
                    treeDepth = treeDepthTmp;
                }
            }
        }
    } else {
        // Has set of points
        if (_numEntries > _maxEnt){ _maxEnt = _numEntries; }
        _leafSpheres++;
    }

    treeDepth++;
    _usedSpheres++;

    _treeDepth = treeDepth;
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::setBB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) 
{
    ML_TRACE_IN("void TileSphere::setBB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) ");

    // Assigns a bounding box to the tileSphere.
    _minX = minX; 
    _maxX = maxX;
    _minY = minY;
    _maxY = maxY;
    _minZ = minZ;
    _maxZ = maxZ;
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::_getBB(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) 
{
    ML_TRACE_IN("void TileSphere::_getBB(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) ");

    // Gets the bounding box of the tileSphere.
    minX = _minX; 
    maxX = _maxX;
    minY = _minY;
    maxY = _maxY;
    minZ = _minZ;
    maxZ = _maxZ;
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::addPoint(float* position) 
{
    ML_TRACE_IN("void TileSphere::addPoint(float* position) ");

    // Adds a point into the tile sphere.
    float minX=0, maxX=0, minY=0, maxY=0, minZ=0, maxZ=0;

    int counter = 0;

    // If the sphere has sub spheres, add point into the according sub sphere.
    if (_hasSubSpheresFlag) {
        for (counter = 0; counter < _cubicPartition; counter++) {
            _tileSpheres[counter]._getBB(minX, maxX, minY, maxY, minZ, maxZ);

            if ((*(position)     >= minX) && 
                (*(position)     <= maxX) &&
                (*(position + 1) >= minY) &&
                (*(position + 1) <= maxY) &&
                (*(position + 2) >= minZ) &&
                (*(position + 2) <= maxZ))
            {
                _tileSpheres[counter].addPoint(position);
            }
        }
    } else {
        // If there is no further sub-division, integrate point into subset.
        _subset[_numEntries] = position;
        _numEntries++;
        // If capacity is reached, establish sub spheres and transfer the containing points.
        if (_numEntries == static_cast<unsigned int>(_maxEntries)) {
            // Establish further sub spheres.
            if (_hasSubSpheresFlag == false) {
                ML_DELETE_ARR(_tileSpheres);
                ML_CHECK_NEW (_tileSpheres, TileSphere[_cubicPartition]);
                _hasSubSpheresFlag = true;
            }

            int xCounter=0, yCounter=0, zCounter=0;

            float partitionDiv = 1.0f / static_cast<float>(_partition);

            float stepX = (_maxX - _minX) * partitionDiv;
            float stepY = (_maxY - _minY) * partitionDiv;
            float stepZ = (_maxZ - _minZ) * partitionDiv;

            float newMaxX = 0.0f;
            float newMaxY = 0.0f;
            float newMaxZ = 0.0f;

            // Constitute BBs for the sub spheres.
            for (xCounter = 0; xCounter < _partition; xCounter++) {
                for (yCounter = 0; yCounter < _partition; yCounter++) {
                    for (zCounter = 0; zCounter < _partition; zCounter++) {
                        _tileSpheres[counter].setParameter(_minimalDistance, _partition, _numEntries, _error);

                        // to avoid rounding errors, take the real maximum value at the upper border
                        newMaxX = (xCounter+1 < _partition) ? (_minX + stepX * xCounter + stepX) : _maxX;
                        newMaxY = (yCounter+1 < _partition) ? (_minY + stepY * yCounter + stepY) : _maxY;
                        newMaxZ = (zCounter+1 < _partition) ? (_minZ + stepZ * zCounter + stepZ) : _maxZ;

                        _tileSpheres[counter].setBB(
                            _minX + stepX * xCounter,
                            newMaxX,
                            _minY + stepY * yCounter,
                            newMaxY,
                            _minZ + stepZ * zCounter,
                            newMaxZ);

                        counter++;
                    }
                }
            }
            // Transfer points
            for (counter = 0; counter < _maxEntries; counter++){
                addPoint(_subset[counter]);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::_initSphere(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) 
{
    ML_TRACE_IN("void TileSphere::_initSphere(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error) ");

    _numEntries        = 0;
    _hasSubSpheresFlag = false;
    _radius            = -1;
    _tileSpheres       = NULL;
    _subset            = NULL;

    if (minimalDistance != NULL){
        setParameter(minimalDistance, maxPartition, numElements, error);
    }
}

//////////////////////////////////////////////////////////////////////////

float TileSphere::_getRadius()
{
    ML_TRACE_IN("float TileSphere::_getRadius()");

    return _radius;
}

//////////////////////////////////////////////////////////////////////////

float* TileSphere::_getPosition()
{
    ML_TRACE_IN("float* TileSphere::_getPosition()");

    return _position;
}

//////////////////////////////////////////////////////////////////////////

void TileSphere::_getSubset(float** &subset, int &size)
{
    ML_TRACE_IN("void TileSphere::_getSubset(float** &subset, int &size)");

    subset = _subset;
    size   = _numEntries;
}

//////////////////////////////////////////////////////////////////////////

bool TileSphere::_hasSubSpheres()
{
    ML_TRACE_IN("bool TileSphere::_hasSubSpheres()");

    return _hasSubSpheresFlag;
}

//////////////////////////////////////////////////////////////////////////

TileSphere* TileSphere::_getPreciserSphere(TileSphere* referenceSphere, float closestDistance) 
{
    ML_TRACE_IN("TileSphere* TileSphere::_getPreciserSphere(TileSphere* referenceSphere, float closestDistance) ");

    bool unInit   = true;
    bool firstRun = false;

    if (closestDistance == -1){
        firstRun = true;
    }

    TileSphere *closestTileSphere = NULL, *tmpSphere = NULL;
    float tmpDistance=0;

    unsigned int counter=0;

    // Traverse each sub sphere.

    for (counter = 0; counter < _cubicPartition; counter++) {

        // Only examine further if the sphere contains more details.        
        if ((_tileSpheres[counter]._getRadius() != -1) &&
            !_minimalDistance->getHashTable()->existPair(&_tileSpheres[counter], referenceSphere))
        {
            if (unInit) {
                // Initialize variables once if they are yet uninitialized.
                tmpDistance = _distance(&_tileSpheres[counter], referenceSphere);
                if (firstRun) {
                    unInit = false;
                    closestTileSphere = &_tileSpheres[counter];
                    closestDistance = tmpDistance;
                } else {                
                    if ((tmpDistance * (1 + _error)) < closestDistance) {
                        unInit = false;
                        closestTileSphere = &_tileSpheres[counter];
                        closestDistance = tmpDistance;
                    }
                }
            } else {
                tmpSphere = &_tileSpheres[counter];
                tmpDistance = _distance(tmpSphere, referenceSphere);
                if (firstRun) {
                    if (tmpDistance < closestDistance) {
                        closestDistance = tmpDistance;
                        closestTileSphere = tmpSphere;
                    }
                } else {
                    if ((tmpDistance * (1 + _error)) < closestDistance) {
                        closestDistance = tmpDistance;
                        closestTileSphere = tmpSphere;
                    }
                }
            }
        }
    }

    return closestTileSphere;
}

//////////////////////////////////////////////////////////////////////////

float TileSphere::_distance(TileSphere* tileSphere1, TileSphere* tileSphere2) 
{   
    ML_TRACE_IN("float TileSphere::_distance(TileSphere* tileSphere1, TileSphere* tileSphere2)");

    float radiusSum = tileSphere1->_getRadius() + tileSphere2->_getRadius();

    float result = _distance(tileSphere1->_getPosition(), tileSphere2->_getPosition()) - radiusSum;

    return (result>=0)?result:0; 
}

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE

