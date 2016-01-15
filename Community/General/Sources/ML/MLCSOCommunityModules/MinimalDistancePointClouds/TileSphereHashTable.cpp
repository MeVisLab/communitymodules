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
//! It defines a single sub sphere for an efficient nearest pair search algorithm.
//! TileSphere will generate its own sub spheres if necessary
/*!
// \file    TileSphereHashTable.cpp
// \author  Christian Tietjen, Olaf Konrad, last changed by $Author: okonrad $
// \date    11/2005
*/
//----------------------------------------------------------------------------------

#include "TileSphereHashTable.h"
#include "TileSphere.h"


ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

TileSphereHashTable::TileSphereHashTable()
{
    ML_TRACE_IN("TileSphereHashTable::TileSphereHashTable()");

    _hashSize   = 1103; // prime number
    _hashTable  = NULL;
    ML_CHECK_NEW(_hashTable, std::vector<MLuint32>[_hashSize]);

    _addedPairs =
    _hits       =
    _misses     = 0;
}

//////////////////////////////////////////////////////////////////////////

TileSphereHashTable::~TileSphereHashTable()
{
    ML_TRACE_IN("TileSphereHashTable::~TileSphereHashTable() ");

    ML_DELETE_ARR(_hashTable);
}

//////////////////////////////////////////////////////////////////////////

void TileSphereHashTable::addPair(TileSphere* tileSphere1, TileSphere* tileSphere2)
{
    ML_TRACE_IN("void TileSphereHashTable::addPair(TileSphere* tileSphere1, TileSphere* tileSphere2) ");

    MLuint32 tmpIndex1 = tileSphere1->getSphereIndex();
    MLuint32 tmpIndex2 = tileSphere2->getSphereIndex();

    // if one of the indices is larger than 16bit, the entry value is no longer unique
    if ( (tmpIndex1 <= ML_UINT16_MAX) && (tmpIndex2 <= ML_UINT16_MAX) ){
        MLuint16 index1 = (MLuint16)tileSphere1->getSphereIndex();
        MLuint16 index2 = (MLuint16)tileSphere2->getSphereIndex();

        int      hashKey = 0;
        MLuint32 entry   = 0;

        // Always smaller index first.
        // Concatenate both indices to one entry value.
        if (index1 < index2){
            entry = (index1 << 16) | index2;
        } else{
            entry = (index2 << 16) | index1;
        }

        hashKey = (int)entry % _hashSize;

        _hashTable[hashKey].push_back(entry);

        _addedPairs++;
    }
    else {
        std::cout << "TileSphereHashTable::addPair: one of the indices was greater than ML_UINT16_MAX" << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////

bool TileSphereHashTable::existPair(TileSphere* tileSphere1, TileSphere* tileSphere2)
{
    ML_TRACE_IN("bool TileSphereHashTable::existPair(TileSphere* tileSphere1, TileSphere* tileSphere2) ");

    MLuint32 tmpIndex1 = tileSphere1->getSphereIndex();
    MLuint32 tmpIndex2 = tileSphere2->getSphereIndex();

    // if one of the indices is larger than 16bit, the entry value is no longer unique
    if ( (tmpIndex1 <= ML_UINT16_MAX) && (tmpIndex2 <= ML_UINT16_MAX) ) {
        MLuint16 index1 = (MLuint16)tileSphere1->getSphereIndex();
        MLuint16 index2 = (MLuint16)tileSphere2->getSphereIndex();

        int      hashKey = 0;
        MLuint32 entry   = 0;

        // Always smaller index first.
        // Concatenate both indices to one entry value.
        if (index1 < index2){
            entry = (index1 << 16) | index2;
        } else{
            entry = (index2 << 16) | index1;
        }

        hashKey = (int)entry % _hashSize;

        std::vector<MLuint32>::iterator i;

        for(i = _hashTable[hashKey].begin(); i != _hashTable[hashKey].end(); i++) {
            if (*i == entry) {
                _hits++;
                return true;
            }
        }
        _misses++;
    }
    else {
        std::cout << "TileSphereHashTable::existPair: one of the indices was greater than ML_UINT16_MAX" << std::endl;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

void TileSphereHashTable::getStatistics(int &addedPairs, int &hits, int &misses)
{
    ML_TRACE_IN("void TileSphereHashTable::getStatistics(int &addedPairs, int &hits, int &misses) ");

    addedPairs = _addedPairs;
    hits       = _hits;
    misses     = _misses;
}

//////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE

