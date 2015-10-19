
// $Id: SoShapeInfo.h,v 1.10 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/InventorModules/SoMeasurement/SoShapeInfo.h,v $

/*! Inventor node SoShapeInfo derived from SoSeparator returns the number of points, lines and triangles in the subgraph.
// \file    SoShapeInfo.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _SoShapeInfo
#define _SoShapeInfo

#ifdef WIN32
#pragma warning (disable: 4786)
#pragma warning (disable: 4710)
#pragma warning (disable: 4146)
#pragma warning (push,3)
#pragma warning (disable: 4018)
#pragma warning (disable: 4786)
#pragma warning (disable: 4710)
#pragma warning (disable: 4146)
#endif


#include "UMDToolsSystem.h"
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>

class SoFieldSensor;
class SoPrimitiveVertex;

//! Inventor node SoShapeInfo derived from SoSeparator returns the number of points, lines and triangles in the subgraph.
class UMDTOOLS_EXPORT SoShapeInfo : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(SoShapeInfo);


public:

  //! Constructor
  SoShapeInfo();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! the input node
  SoSFNode rootInput;
  SoSFInt32 numPoints, numLines, numTriangles, numNodes;

  //! forces the computation
  void calculate();


protected:

  //! Destructor
  virtual ~SoShapeInfo();


private:

  //! listen whether the input has been changed
  SoFieldSensor *_inputSens;
  //! callback action to collect the points
  SoCallbackAction* _myAction;

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

  int _numPoints, _numLines, _numTriangles ,_numNodes;
};

#endif
