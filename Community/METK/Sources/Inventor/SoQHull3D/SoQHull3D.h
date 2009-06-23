//----------------------------------------------------------------------------------
//! The Inventor module class SoQHull3D derived from SoSeparator
/*!
// \file    SoQHull3D.h
// \author  Jens Haase
// \date    2005-02-01
//
// convex hull of invetor scene using qhull
*/
//----------------------------------------------------------------------------------

#ifndef __SOQHULL3D_H__
#define __SOQHULL3D_H__


// Local includes
#include "UMDQHull3DSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFColor.h>

#include <Inventor/actions/SoCallbackAction.h>

#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>

#include <Inventor/sensors/SoNodeSensor.h>

#include <Inventor/SoPrimitiveVertex.h>

#include "XVLeaveScope.h"

#include <vector>

// QHull: include
extern "C"
{
  #include "qhull/qhull_a.h"
}

//QHull: global vector for input points
typedef vector <realT> realTVector;
static realTVector inputPointList;



//global vectors for point values
typedef vector <SbVec3f> PointVector;
static PointVector convexHullPoints;
static PointVector convexHullFaceNormals;
static PointVector convexHullVertexNormals;

//... and for index values
typedef vector <int> IndexVector;
static IndexVector convexHullIndexLUT;
static IndexVector convexHullFaceIndexList;


//! convex hull of invetor scene using qhull
class UMDQHULL3D_EXPORT SoQHull3D : public SoSeparator
{
  //! implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoQHull3D);

public:
  //! constructor
  SoQHull3D();

  //! initializes this class (called on dll initialization).
  static void  initClass();

  //@{! fields
  SoSFInt32 numInput;
  SoSFInt32 numOutput;
  SoSFInt32 numTriangles;

  SoSFInt32 ch_surface;
  SoSFInt32 ch_volume;

  SoSFBool showPoints;
  SoSFBool showFaces;

  SoSFColor pointColor;
  SoSFColor faceColor;

  SoSFBool useColors;

  SoSFString state;

  //! 'hidden' fields in automatic panel
  SoSFBool showNormals;
  SoSFBool useFaceNormals;

  //! output
  SoSFNode out_qhull;

  //@}


protected:
  //! protected destructor
  virtual ~SoQHull3D();

  //! sensor to detect changes in node's fields.
  SoNodeSensor *nodeSensor;

  //! called by nodeSensor when any field in node changes.
  //! only redirects the call of this static function to the
  //! class method nodeChanged.
  static void nodeChangedCB(void *data, SoSensor* sens);

  //! called by the static function nodeChangedCB to notify node changes.
  void nodeChanged(SoNodeSensor* sensor);

  //! callback action for collectiong primitives of an open inventor scene
  SoCallbackAction *callbackAction;

  //! called when entering a node during callback
  static SoCallbackAction::Response preCB(void *userData, SoCallbackAction *action, const SoNode *node);

  //! called if the child is a shape (consisting of points)
  static void pointCB(void *userData, SoCallbackAction *action,
    const SoPrimitiveVertex *v);

  //! called if the child is a shape (consisting of line segments)
  static void lineSegmentCB(void *userData, SoCallbackAction *action,
    const SoPrimitiveVertex *v1,
    const SoPrimitiveVertex *v2);

  //! called if the child is a shape (consisting of triangles)
  static void triangleCB(void *userData, SoCallbackAction *action,
    const SoPrimitiveVertex *v1,
    const SoPrimitiveVertex *v2,
    const SoPrimitiveVertex *v3);


  //! collects points, lines and triangles of the inventor scene
  void collectPrimitives();

  //! execute QHull
  void callQHull();

  //! drawing routines
  void drawPoints();
  void drawFaces();
  void drawNormals();

private:
  //! group for output
  SoGroup *output;

  //! separators for points, faces and normals
  SoSeparator *chPoints, *chFaces, *chNormals;

  //! state variable
  bool convexHullFound;

  //! transformation matrix
  SbMatrix transMatrix;

};

#endif // __SOQHULL3D_H

