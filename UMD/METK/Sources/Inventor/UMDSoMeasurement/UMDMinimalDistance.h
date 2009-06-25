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
// |  Efficient Distance Computation between Point Sets  |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDMinimalDistance.h,v 1.13 2003/09/10 09:31:14 ritter Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDMinimalDistance.h,v $

/*! Computes the shortest euclidean distance between two pointsets.
// \file    UMDMinimalDistance.h
// \author  Christian Tietjen
// \date    09/2002
*/
#ifndef _UMDMinimalDistance
#define _UMDMinimalDistance


class TileSphere;
class HashTable;
class SoSeparator;

#include "UMDMeasurementInit.h"


//! Computes the shortest euclidean distance between two pointsets
//! Based on: "Efficient Distance Computation between Non-Convex Objects" by Sean Quinlan
//! see: graphics.stanford.edu/courses/cs468-01-winter/papers/q-edcbnco-94.ps
class SO_MEASUREMENT_CLASS_SPEC UMDMinimalDistance {

public:

  //! Constructor
  UMDMinimalDistance();
  //! Destructor
  virtual ~UMDMinimalDistance();

  //! This method passes the two pointsets in the form of two float-arrays (x,y,z, x,y,z, ...)
  //! and the number of the vectors
  void setPointSets(float* pointSet1, float* pointSet2, long size1, long size2);
  //! This parameter describes, how many points max. can be put into one subsphere,
  //! but the designed value can be lower. The default value is 2000.
  //! The concrete value for \c _entries and \c _partition will be computed automatically.
  void setNumEntries(int entries);
  //! sets fix values for both objects for \c _entries and \c _partition.
  void setParams(int entries, int partition);
  //! describes the maintainable error. 0 = 0% and 1 = 100%. The default value is 0.
  void setError(float error);
  //! The method \c setPointSets must be called befor.
  //! Returns a copy of the both points, whose distance is minimal.
  void computeDistance(float* &point1, float* &point2);


  //! returns an unique index for each tilesphere
  int getUniqueIndex();
  //! returns a pointer on the hashtable
  HashTable* getHashTable();

  //! returns the spheres for debugging purposes
  void displaySpheres(bool onOff);
  //! contains transparent spheres describing the tile spheres
  SoSeparator *_tileSphere1, *_tileSphere2;
  //! dumps the console with additional debbuging info
  void verbose(bool onOff);

private:

  //! the one point set
  float* _pointSet1;
  //! the other point set
  float* _pointSet2;
  //! the sizes of the point sets
  int _size1, _size2;
  //! the internal sphere index
  int32_t _sphereIndex;
  //! the internal hashtable
  HashTable* _hashTable;

  //! the maintainable error
  float _error;

  //! dumps the console with additional debbuging info
  bool _verbose;
  //! decides whether the transparent spheres should be displayed
  bool _displaySpheres;
  //! contains transparent spheres describing the tile spheres
  SoSeparator* displaySpheres(TileSphere* tileSphere);

  //! set by \c setNumEntries
  int _entries, _partition;

  //! generates the tree of spheres for each point set
  TileSphere* generateTree(float* pointSet, long size);
  //! gets the BB of the main point set
  void getBB(float* pointSet, long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
};


//! TileSphere is used in the class \c UMDMinimalDistance
//! It defines a single sub sphere for the algorithm.
//! TileSphere will generate her own sub spheres if necessary
class TileSphere {

public:

  //! default Constructor
  TileSphere();
  //! Constructor with a pointer on the main class and presets for the distance computation
  TileSphere(UMDMinimalDistance* minimalDistance, int maxPartition = 2, int numElements = 2000, float error = 0);
  //! Destructor
  virtual ~TileSphere();

  //! must be called, when TileSphere was instantiated with the default constructor.
  void setParameter(UMDMinimalDistance* minimalDistance, int maxPartition, int numElements, float error);

  //! defines, which points will be put into this sphere
  void setBB(float  minX, float  maxX, float  minY, float  maxY, float  minZ, float  maxZ);
  //! put a point into this sphere (or one of her subspheres)
  void addPoint(float* position);
  //! computes the optimal bounding sphere
  void getDeFactoSize();
  //! computes the distance and returns a reference to the nearest points of both sets
  float computeDistance(TileSphere* tileSphere, float*& point1, float*& point2);


  //! returns the index of the TileSphere
  int32_t getSphereIndex();

  //! returns the spheres for debugging purposes
  SoSeparator* displaySpheres();
  //! returns statistice for debugging purposes
  void getStatistics(int &_treeDepth, int &usedSpheres, int &leafSpheres, int &_maxEnt);


private:

  //! initializes the tile sphere
  void initSphere(UMDMinimalDistance* minimalDistance, int partition, int numElements, float error);
  //! pointer on the main class
  UMDMinimalDistance* _minimalDistance;
  //! pointer on the sub spheres
  TileSphere* _tileSpheres;

  //! stores the sphere index
  int32_t _sphereIndex;

  //! the maintainable error
  float _error;

  //! stores whether this sphere has sub spheres
  bool _hasSubSpheres;
  //! stores the number of sub spheres, according to _partition to the power of 3
  int _partition;

  //! pointer on the containing points
  float** _subset;
  //! number of containing points
  int _numEntries;
  //! maximum number of containing points
  int _maxEntries;

  //! dimension of the point set
  float _minX, _maxX, _minY, _maxY, _minZ, _maxZ;
  //! position vector of the tile sphere
  float _position[3];
  //! radius of the tile sphere
  float _radius;

  //! returns the BB of the tile sphere
  void getBB(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ);
  //! returns the postion vector
  float* getPosition();
  //! returns the radius
  float getRadius();
  //! returns the containing subset and their size
  void getSubset(float** &subset, int &size);
  //! true if has sub spheres
  bool hasSubSpheres();

  //! Returns a preciser sub sphere, which is closest to this sphere.
  //! The method will return NULL, if there is no closer sphere then \c distance
  //! or if the closest sphere was already visited.
  TileSphere* getPreciserSphere(TileSphere* referenceSphere, float distance);
  //! Returns the euclidean distance between two spheres. It returnes not the distance between the
  //! both position vectors, but the distance between the sphere hulls
  float distance(TileSphere* tileSphere1, TileSphere* tileSphere2);
  //! Returns the euclidean distance between two points
  inline float distance(register float* point1, register float* point2) {
    register float d1 = * point1      - * point2;
    register float d2 = *(point1 + 1) - *(point2 + 1);
    register float d3 = *(point1 + 2) - *(point2 + 2);
    return sqrtf(d1*d1 + d2*d2 + d3*d3);
  }

  //! Returns the distance between two points without calculating the square root
  inline float fastDistance(register float* point1, register float* point2) {
    register float d1 = * point1      - * point2;
    register float d2 = *(point1 + 1) - *(point2 + 1);
    register float d3 = *(point1 + 2) - *(point2 + 2);
    return (d1*d1 + d2*d2 + d3*d3);
  }
};


//! HashTable is used in the classes \c UMDMinimalDistance and \c TileSphere
//! It defines a single sub sphere for the algorithm.
//! TileSphere will generate her own sub spheres if necessary
class HashTable {

public:

  //! Constructor
  HashTable();
  //! Destructor
  virtual ~HashTable();

  //! adds a pair of spheres into the hashtable
  void addPair(TileSphere* tileSphere1, TileSphere* tileSphere2);
  //! checks, whether the pair exists or not
  bool existPair(TileSphere* tileSphere1, TileSphere* tileSphere2);

  //! hashtable statistics
  void getStatistics(int &addedPairs, int &hits, int &misses);

private:

  //! size of the hashtable (1000)
  int _hashSize;
  //! the hashtable, consisting of an array of \c vector 's
  std::vector<int64_t>* _hashTable;

  //! hashtable statistics
  int _addedPairs, _hits, _misses;

};


#endif
