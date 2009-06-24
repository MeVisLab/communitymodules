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
//! TileSphereHashTable is used in the classes \c MinimalDistancePointClouds and \c TileSphere.
//! The TileSphereHashTable is the underlying data structure for
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
/*!
// \file    TileSphereHashTable.h
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------


#ifndef __TileSphereHashTable_H
#define __TileSphereHashTable_H

#include "../MLCSOCommunityModulesDefs.h"


ML_START_NAMESPACE

// Forward declaration
class TileSphere;


//! TileSphereHashTable is used in the classes \c MinimalDistancePointClouds and \c TileSphere.
//! The TileSphereHashTable is the underlying data structure for
//! a fast nearest pair search algorithm described by Sean Quinlan 
//! in 'Efficient Distance Computation between Non-Convex Objects'.
class TileSphereHashTable
{
    
public:
    
    //! Constructor.
    TileSphereHashTable();
    //! Destructor.
    virtual ~TileSphereHashTable();
    
    //! Adds a pair of spheres into the hash table.
    void addPair(TileSphere* tileSphere1, TileSphere* tileSphere2);
    //! Checks whether the given pair exists or not.
    bool existPair(TileSphere* tileSphere1, TileSphere* tileSphere2);
    
    //! Hash table statistics.
    void getStatistics(int &addedPairs, int &hits, int &misses);

    
private:
    
    //! Size of the hash table.
    int _hashSize;
    //! The hash table, consisting of an array of \c vector s.
    std::vector<MLuint32>* _hashTable;
    
    //! Hash table statistics.
    int _addedPairs, _hits, _misses;
    
};


ML_END_NAMESPACE

#endif // __TileSphereHashTable_H
