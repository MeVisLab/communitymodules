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
// |  Converts any scenegraph into a pointset            |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoShapeToPointSet.h,v 1.12 2003/09/23 17:24:33 milo Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoShapeToPointSet.h,v $

/*!
// \file    UMDSoShapeToPointSet.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _UMDSoShapeToPointSet
#define _UMDSoShapeToPointSet


#include "UMDMeasurementInit.h"

#include <set>

//! size of the hash table pow(HASH_PARTITION, 3)
#define HASH_PARTITION 16
//! size of the buffer for fast caching of the point set
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10000
#endif


class SoCallbackAction;
class SoFieldSensor;
class SoPointSet;
class SoPrimitiveVertex;
class SoSFMatrix;
class SoVertexProperty;
struct ltstr;

//! Converts any scenegraph into a pointset.
//! Every point occurs just one time in this set.
class SO_MEASUREMENT_CLASS_SPEC UMDSoShapeToPointSet : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoShapeToPointSet);
  
  
public:
  
  //! Constructor
  UMDSoShapeToPointSet();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();
  
  //! the input node
  SoSFNode rootInput;
  //! direct access on the extracted point set
  //! is empty if \c rootInput contains no valid scenegraph
  SoVertexProperty* vertexProperty;
  //! forces the computation
  void calculate();
  

protected:
  
  //! Destructor
  virtual ~UMDSoShapeToPointSet();
  
  
private:
  
  //! internal representation of the point set
  SoPointSet* _pointSet;
  //! number of points
  long _counter, _counterBuffer;
  //! buffer for fast collecting of unique pointsets
  SbVec3f* _buffer;
  //! the transformation matrix
  SoSFMatrix* _worldMatrix;
  //! listen whether the input has been changed
  SoFieldSensor *_inputSens;
  //! callback action to collect the points
  SoCallbackAction* _myAction;
  
  //! inserts the points into the hashtable
  void insertPoint(SbVec3f vector);
  //! reads out the hashtable. every point occurs only one time
  void collectPoints();
  
  //! method called on input changed
  static void inputCB(void *userData, SoSensor*);
  //! called, if a new child in the scenegraph will be traversed
  static SoCallbackAction::Response  preCB(void *userData, SoCallbackAction *action, const SoNode *node);
  //! called if the child is a point set
  static void pointCB   (void* userData, SoCallbackAction* action, 
                         const SoPrimitiveVertex* v);
  //! called if the child is a line set
  static void lineCB    (void* userData, SoCallbackAction* action, 
                         const SoPrimitiveVertex* v1,
                         const SoPrimitiveVertex* v2);
  //! called if the child is a shape
  static void triangleCB(void* userData, SoCallbackAction* action,
                         const SoPrimitiveVertex* v1,
                         const SoPrimitiveVertex* v2,
                         const SoPrimitiveVertex* v3);
  
  //! the hashtable
  std::set<SbVec3f, ltstr>* _hashTable;
};

#endif


//! sort criterion for the hashtable set
struct ltstr {

  //! sorts points lexicographical
  bool operator()(SbVec3f s1, SbVec3f s2) const {
    const float* p1 = s1.getValue();
    const float* p2 = s2.getValue();
    
    if (*p1 < *p2) return true;
    else {
      if (*p1 > *p2) return false;
      else {
        if (*(++p1) < *(++p2)) return true;
        else {
          if (*p1 > *p2) return false;
          else {
            if (*(++p1) < *(++p2)) return true;
            else return false;
          }
        }
      }
    }
  }
};
