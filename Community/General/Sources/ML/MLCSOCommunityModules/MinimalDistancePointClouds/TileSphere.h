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
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
//! TileSphere will generate its own sub spheres if necessary.
/*!
// \file    TileSphere.h
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------


#ifndef __TileSphere_H
#define __TileSphere_H

#include "../MLCSOCommunityModulesDefs.h"

#include <mlVec3.h>
#include "TileSphereHashTable.h"


ML_START_NAMESPACE

// Forward declaration
class MinimalDistancePointClouds;


//! TileSphere is used in the class \c MinimalDistancePointClouds.
//! It defines a single sub sphere for 
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
//! TileSphere will generate its own sub spheres if necessary.
class TileSphere 
{
    
public:
    
    //! Default Constructor.
    TileSphere();
    //! Constructor with a pointer on the main class and presets for the distance computation.
    TileSphere(MinimalDistancePointClouds* minimalDistance, int maxPartition = 2, int numElements = 1103, float error = 0);
    //! Destructor.
    virtual ~TileSphere();
    
    //! Must be called, when TileSphere was instantiated with the default constructor.
    void setParameter(MinimalDistancePointClouds* minimalDistance, int maxPartition, int numElements, float error);
    
    //! Defines which points will be put into this sphere.
    void setBB(float  minX, float  maxX, float  minY, float  maxY, float  minZ, float  maxZ);
    //! Put a point into this sphere (or one of its sub spheres).
    void addPoint(float* position);
    //! Computes the optimal bounding sphere.
    void getDeFactoSize();
    //! Computes the distance and returns references to the nearest points of both sets.
    float computeDistance(TileSphere* tileSphere, float*& point1, float*& point2);
    
    //! Computes the distance and returns a references to the entryNumbers of the nearest nodes of both sets.
    float computeDistance(TileSphere* tileSphere, unsigned int& node1, unsigned int& nodet2);
    
    
    //! Returns the index of the TileSphere.
    unsigned int getSphereIndex();
    
    //! Returns statistics for debugging purposes.
    void getStatistics(int &_treeDepth, int &usedSpheres, int &leafSpheres, int &_maxEnt);
    
    
private:
    
    //! Initializes the tile sphere.
    void _initSphere(MinimalDistancePointClouds* minimalDistance, int partition, int numElements, float error);
    //! Pointer to the main class.
    MinimalDistancePointClouds* _minimalDistance;
    //! Pointer to the sub spheres.
    TileSphere* _tileSpheres;
    
    //! Stores the sphere index.
    unsigned int _sphereIndex;
    
    //! The maintainable error.
    float _error;
    
    //! Stores whether this sphere has sub spheres.
    bool _hasSubSpheresFlag;
    //! Stores the number of sub spheres, according to _partition to the power of 3.
    unsigned int _partition;
    //! The cubic of the partition.
    unsigned int _cubicPartition;


    //! Pointer to the contained points.
    float** _subset;
    //! Number of contained points.
    unsigned int _numEntries;
    //! Maximum number of contained points.
    int _maxEntries;
    
    //! Extend of the point set.
    float _minX, _maxX, _minY, _maxY, _minZ, _maxZ;
    //! Position vector of the tile sphere.
    float _position[3];
    //! Radius of the tile sphere.
    float _radius;
    
    //! Returns the BB of the tile sphere.
    void _getBB(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
    //! Returns the position vector of the tile sphere.
    float* _getPosition();
    //! Returns the radius of the tile sphere.
    float _getRadius();
    //! Returns the contained subset and its size.
    void _getSubset(float** &subset, int &size);
    //! True if this sphere has sub spheres.
    bool _hasSubSpheres();
    
    //! Returns a preciser sub sphere, which is closest to this sphere.
    //! The method will return NULL, if there is no closer sphere then \c distance
    //! or if the closest sphere was already visited.
    TileSphere* _getPreciserSphere(TileSphere* referenceSphere, float distance);
    
    //! Returns the euclidean distance between two spheres. It returns not the distance between the
    //! both position vectors, but the distance between the sphere hulls.
    float _distance(TileSphere* tileSphere1, TileSphere* tileSphere2);
    
    //! Returns the euclidean distance between two points.
    inline float _distance(register float* point1, register float* point2) {
        register float d1 = * point1      - * point2;
        register float d2 = *(point1 + 1) - *(point2 + 1);
        register float d3 = *(point1 + 2) - *(point2 + 2);
        return sqrtf(d1*d1 + d2*d2 + d3*d3);
    }
    
    //! Returns the distance between two points without calculating the square root.
    inline float _fastDistance(register float* point1, register float* point2) {
        register float d1 = * point1      - * point2;
        register float d2 = *(point1 + 1) - *(point2 + 1);
        register float d3 = *(point1 + 2) - *(point2 + 2);
        return (d1*d1 + d2*d2 + d3*d3);
    }
};


ML_END_NAMESPACE

#endif // __TileSphere_H

