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
//! The MinimalDistancePointClouds implements 
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
/*!
// \file    MinimalDistancePointClouds.cpp
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------

#include "MinimalDistancePointClouds.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

MinimalDistancePointClouds::MinimalDistancePointClouds() 
{   
    ML_TRACE_IN("MinimalDistancePointClouds::MinimalDistancePointClouds() ");

    _pointSet1   = NULL;
    _pointSet2   = NULL;

    _hashTable   = NULL;

    _size1       = 0;
    _size2       = 0;

    _sphereIndex = 0;
    _entries     = -1;
    _partition   = -1;

    _tileSphere1 = NULL;
    _tileSphere2 = NULL;
}

//////////////////////////////////////////////////////////////////////////

MinimalDistancePointClouds::~MinimalDistancePointClouds()
{
    ML_TRACE_IN("MinimalDistancePointClouds::~MinimalDistancePointClouds()");

    ML_DELETE_ARR(_pointSet1);
    ML_DELETE_ARR(_pointSet2);

    ML_DELETE(_tileSphere1);
    ML_DELETE(_tileSphere2);
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setPointSets(const std::vector<ml::vec3>& pointCloud1, const std::vector<ml::vec3>& pointCloud2)
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setPointSets()");

    unsigned int i     = 0;
    unsigned int entry = 0;
    
    ML_DELETE_ARR(_pointSet1);
    ML_DELETE_ARR(_pointSet2);

    _size1 = pointCloud1.size();
    _size2 = pointCloud2.size();

    ML_CHECK_NEW(_pointSet1, float[3 * _size1]);
    ML_CHECK_NEW(_pointSet2, float[3 * _size2]);
    
    // copy positions of the nodes to float array

    for (i = 0; i < _size1; i++){

        const ml::vec3& currPos = pointCloud1[i];

        _pointSet1[entry++] = static_cast<float>(currPos[0]);
        _pointSet1[entry++] = static_cast<float>(currPos[1]);
        _pointSet1[entry++] = static_cast<float>(currPos[2]);
    }

    entry = 0;

    for (i = 0; i < _size2; i++){
            
        const ml::vec3& currPos = pointCloud2[i];

        _pointSet2[entry++] = static_cast<float>(currPos[0]);
        _pointSet2[entry++] = static_cast<float>(currPos[1]);
        _pointSet2[entry++] = currPos[2];
    }

    if ((_pointSet1 != NULL) && (_pointSet2 != NULL)){

        ML_DELETE(_tileSphere1);
        ML_DELETE(_tileSphere2);

        _tileSphere1 = generateTree(_pointSet1, _size1);
        _tileSphere2 = generateTree(_pointSet2, _size2);

        // Determine size of both spheres
        _tileSphere1->getDeFactoSize();
        _tileSphere2->getDeFactoSize();


    } else {
        std::cout << "MinimalDistancePointClouds::setPointSets: One of the arguments was NULL" << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setFirstSinglePoint(const ml::vec3& point)
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setFirstSinglePoint()");


    ML_DELETE_ARR(_pointSet1);
    ML_CHECK_NEW (_pointSet1, float[3]);

    // copy node position to point set.

    _pointSet1[0] = static_cast<float>(point[0]);
    _pointSet1[1] = static_cast<float>(point[1]);
    _pointSet1[2] = static_cast<float>(point[2]);
    _size1        = 1;

    // The second tileSphere is already set
    
    ML_DELETE(_tileSphere1);

    _tileSphere1 = generateTree(_pointSet1, _size1);
    _tileSphere1->getDeFactoSize();
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setFirstPointSet(const std::vector<ml::vec3>& pointCloud)
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setFirstPointSet()");

    unsigned int i     = 0;
    unsigned int entry = 0;

    // copy node positions to point set.

    _size1 = pointCloud.size();
    
    ML_DELETE_ARR(_pointSet1);
    ML_CHECK_NEW (_pointSet1, float[3 * _size1]);
    

    for (i = 0; i < _size1; i++){

        const ml::vec3& currPos = pointCloud[i];

        _pointSet1[entry++] = static_cast<float>(currPos[0]);
        _pointSet1[entry++] = static_cast<float>(currPos[1]);
        _pointSet1[entry++] = static_cast<float>(currPos[2]);
    }

    // The second tileSphere is already set
    
    if (_pointSet1 != NULL){

        ML_DELETE(_tileSphere1);

        _tileSphere1 = generateTree(_pointSet1, _size1);
        _tileSphere1->getDeFactoSize();

    } else {
        std::cout << "MinimalDistancePointClouds::setFirstPointSets: The argument was NULL" << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::computeDistance(ml::vec3& point1, ml::vec3& point2)
{
    ML_TRACE_IN("void MinimalDistancePointClouds::computeDistance()");

    // Both tileSpheres exist
    if (_tileSphere1 && _tileSphere2){
        
        // Initialize index
        _sphereIndex = 0;
                
        // Generate hash table        
        ML_CHECK_NEW(_hashTable, TileSphereHashTable);
                
        float distance;
        float* p1 = NULL; // Return coordinates
        float* p2 = NULL;

        // Compute the minimal distance of both point sets
        distance = _tileSphere1->computeDistance(_tileSphere2, p1, p2);

        if ((p1 != NULL) && (p2 != NULL)){
         
            point1[0] = p1[0];
            point1[1] = p1[1];
            point1[2] = p1[2];

            point2[0] = p2[0];
            point2[1] = p2[1];
            point2[2] = p2[2];
        } else {
            std::cout << "MinimalDistancePointClouds::computeDistance: resulting points were NULL" << std::endl;
        }
        
        ML_DELETE(_hashTable);
    }
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::computeDistance(float*& point1, float*& point2) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::computeDistance(float*& point1, float*& point2) ");

    // Both tileSpheres exist
    if (_tileSphere1 && _tileSphere2){
        
        // Initialize index
        _sphereIndex = 0;
                
        // Generate hash table        
        ML_CHECK_NEW(_hashTable, TileSphereHashTable);
                        
        // Compute the minimal distance of both point sets
        (void)_tileSphere1->computeDistance(_tileSphere2, point1, point2);
                
        ML_DELETE(_hashTable);
    }
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setError(float error) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setError(float error) ");

    if (error > 1) { error = 1; }
    if (error < 0) { error = 0; }

    _error = error;
}

//////////////////////////////////////////////////////////////////////////

TileSphere* MinimalDistancePointClouds::generateTree(float* pointSet, long size) 
{
    ML_TRACE_IN("TileSphere* MinimalDistancePointClouds::generateTree(float* pointSet, long size) ");

    // Do we have a valid point set?
    if ((pointSet != NULL) && (size != 0)) {

        // Calculate parameters for the TileSphere:
        // A single sphere should not contain more than 2000 entries.
        // The size parameter should not become smaller than the number of sub spheres times 2000.
        // The less sub spheres, the faster the algorithm will be.
        // If the total amount of points is less than (2^3)*2000, the number of points
        // in one sphere may be reduced.
        // This guarantees (except for worst cases of spatial point distribution) optimal
        // runtime behavior.
        int partition = 0;
        int entries   = 0;

        if (_partition == -1) {
            
            partition = 1 + (int)pow(((float)size / (float)_entries), (1.0f / 3.0f));
            
            if (partition < 2){ partition = 2; }

            entries = size / (int)pow(partition, 3);
            
            if (entries < 3){ entries = 3; }
            
        } else {
            partition = _partition;
            entries   = _entries;
        }

        TileSphere* tileSphere = NULL;
        ML_CHECK_NEW(tileSphere, TileSphere(this, partition, entries, _error));

        float* pointPos = pointSet;
        float minX=0, maxX=0, minY=0, maxY=0, minZ=0, maxZ=0;
        
        // compute Bounding Box
        getBB(pointSet, size, minX, maxX, minY, maxY, minZ, maxZ);
        
        // and assign the Bounding Box to the main sphere
        tileSphere->setBB(minX, maxX, minY, maxY, minZ, maxZ);
        
        // insert all points
        //long tmpSphereCounter = _sphereIndex;
        long counter=0;

        for (counter = 0; counter < size; counter++) {
            tileSphere->addPoint(pointPos);
            pointPos += 3;
        }
                    
        return tileSphere;
    }

    // If the point set was not valid, return a new (and empty) TileSphere.
    TileSphere* dummy = NULL;
    ML_CHECK_NEW(dummy, TileSphere);
    return dummy;
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::getBB(float* pointSet, long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::getBB(float* pointSet, long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ)");

    // Return the bounding box of a point set
    if ((pointSet != NULL) && (size > 0)) {
        float* pointPos = pointSet;
        minX = maxX = *(pointPos++);
        minY = maxY = *(pointPos++);
        minZ = maxZ = *(pointPos++);

        long counter=0;

        for (counter = 1; counter < size; counter++) {
            if (minX > *pointPos) { minX = *pointPos; }
            if (maxX < *pointPos) { maxX = *pointPos; }
            pointPos++;
            if (minY > *pointPos) { minY = *pointPos; }
            if (maxY < *pointPos) { maxY = *pointPos; }
            pointPos++;
            if (minZ > *pointPos) { minZ = *pointPos; }
            if (maxZ < *pointPos) { maxZ = *pointPos; }
            pointPos++;
        } // for
    } // if
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::verbose(bool onOff) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::verbose(bool onOff) ");

    _verbose = onOff;
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setNumEntries(int entries) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setNumEntries(int entries) ");

    _entries   = entries;
    _partition = -1;
}

//////////////////////////////////////////////////////////////////////////

void MinimalDistancePointClouds::setParams(int entries, int partition) 
{
    ML_TRACE_IN("void MinimalDistancePointClouds::setParams(int entries, int partition) ");

    _entries   = entries;
    _partition = partition;
}

//////////////////////////////////////////////////////////////////////////

int MinimalDistancePointClouds::getUniqueIndex() 
{
    ML_TRACE_IN("int MinimalDistancePointClouds::getUniqueIndex()");

    return _sphereIndex++;
}

//////////////////////////////////////////////////////////////////////////

TileSphereHashTable* MinimalDistancePointClouds::getHashTable() 
{
    ML_TRACE_IN("TileSphereHashTable* MinimalDistancePointClouds::getHashTable() ");

    return _hashTable;
}

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE


