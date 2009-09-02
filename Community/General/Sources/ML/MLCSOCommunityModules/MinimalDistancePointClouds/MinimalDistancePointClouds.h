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
// \file    MinimalDistancePointClouds.h
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------



#ifndef __MinimalDistancePointClouds_H
#define __MinimalDistancePointClouds_H

#include "../MLCSOCommunityModulesDefs.h"

#include "TileSphere.h"
#include "TileSphereHashTable.h"

#include "mlVec3.h"


ML_START_NAMESPACE

//! The MinimalDistancePointClouds implements 
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
class MLCSOCOMMUNITYMODULES_EXPORT MinimalDistancePointClouds 
{
    
public:
    
    //! Constructor.
    MinimalDistancePointClouds();
    //! Destructor.
    virtual ~MinimalDistancePointClouds();
    
    //! This method passes the two point clouds in the form of two standard vectors.
    void setPointSets(const std::vector<ml::vec3>& pointCloud1, const std::vector<ml::vec3>& pointCloud2);
    //! Sets only the first point set anew. The second one (with all its TileSphere tree computations)
    //! is left as it was.
    void setFirstPointSet(const std::vector<ml::vec3>& pointCloud);
    //! Set only the first point set anew, which consists only of a single point.
    //! The second pointSet is left as it was.
    void setFirstSinglePoint(const ml::vec3& point);
    
    
    //! This parameter describes how many points can be put as maximum into one sub sphere,
    //! but the designed value can be lower. The default value is 2000.
    //! The concrete value for \c _entries and \c _partition will be computed automatically.
    void setNumEntries(int entries);
    //! Sets fix values for both objects for \c _entries and \c _partition.
    void setParams(int entries, int partition);
    //! Describes the maintainable error. 0 = 0% and 1 = 100%. The default value is 0.
    void setError(float error);
    
    //! Returns a copy of the both points with the minimal distance.
    //! The method \c setPointSets must have been called before.
    void computeDistance(float* &point1, float* &point2);
    //! Returns two points with minimal distance as vec3.
    //! The method \c setPointSets must have been called before.
    void computeDistance(ml::vec3& point1, ml::vec3& point2);
    
    
    //! Returns an unique index for each tile sphere.
    int getUniqueIndex();
    //! Returns a pointer to the hash table.
    TileSphereHashTable* getHashTable();
    
    //! Dumps the console with additional debugging info.
    void verbose(bool onOff);
    

private:
    
    //! The one point set.
    float* _pointSet1;
    //! The other point set.
    float* _pointSet2;
    //! The sizes of the point sets.
    int _size1, _size2;
    //! The internal sphere index.
    unsigned int _sphereIndex;
    //! The internal hash table.
    TileSphereHashTable* _hashTable;
    
    //! Pointer to the first tileSphere.
    TileSphere* _tileSphere1;
    //! Pointer to the second tileSphere.
    TileSphere* _tileSphere2;
    
    //! The maintainable error.
    float _error;
    
    //! Dumps the console with additional debugging info.
    bool _verbose;
    
    //! Set by \c setNumEntries.
    int _entries, _partition;
    
    //! Generates the tree of spheres for each point set.
    TileSphere* generateTree(float* pointSet, long size);
    
    //! Gets the BB of the main point set.
    void getBB(float* pointSet, long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
};

ML_END_NAMESPACE


#endif // __MinimalDistancePointClouds_H

