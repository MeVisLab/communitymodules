//----------------------------------------------------------------------------------
//! The Inventor module class SoMinimalEnclosingCircle derived from SoSeparator
/*!
// \file    SoMinimalEnclosingCircle.h
// \author  Christian Tietjen
// \date    2004-12-23
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SOMINIMALENCLOSINGCIRCLE_H__
#define __SOMINIMALENCLOSINGCIRCLE_H__


// Local includes
#include "UMDToolsSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSubNode.h>
// include used fields
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include "XVLeaveScope.h"

class SoState;
class SoNodeSensor;

//! 
class UMDTOOLS_EXPORT SoMinimalEnclosingCircle : public SoSeparator
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoMinimalEnclosingCircle);
   
public:
   //! Constructor
   SoMinimalEnclosingCircle();
   
   SoSFVec3f center;
   SoSFFloat radius;
   
   //! Initializes this class (called on dll initialization).
   static void  initClass();
   
protected:
   //! Protected destructor
   virtual      ~SoMinimalEnclosingCircle();
   virtual void GLRenderBelowPath(SoGLRenderAction* action);
   virtual void GLRender(SoGLRenderAction* action);
   
   void compute(SoGLRenderAction* action);
   
private:

   enum SoMinimalEnclosingCircle_Method {MEC, MidPointRadius};
   //! Sensor to detect changes in node's fields.
   SoNodeSensor      *_nodeSensor;
   
   //! Called by \c _nodeSensor when any field in node changes.
   //! Only redirects the call of this static function to the
   //! class method \c nodeChanged.
   static void       nodeChangedCB(void *data, SoSensor* sens);
   
   //! Called by the static function \c _nodeChangedCB to notify node changes.
   void              nodeChanged(SoNodeSensor* sensor);
   
   //! generates a float array out of an scene graph
   void getPointSet(SbVec3f* &pointSet, long &size);
   
   const SbVec3f getScreenPos(const SbVec3f &vertex);
   
   SbMatrix _objToScreen;
   SbVec2f  _winSize;
   SbBool*  _pixelMatrix;
   
   SbVec3f* _pointSet3D;
   long     _size3D;
   float    _zValue;
   SbVec2f* _pointSet2D;
   
   SbVec2f* _pointSetRaster;
   long     _sizeRaster;

   void rasterizeAndSort();
   
   void screenSpaceBoundingBox();
   
   inline float distance2D(const register float* point1, const register float* point2) {
      register float d1 = * point1      - * point2;
      register float d2 = *(point1 + 1) - *(point2 + 1);
      return sqrtf(d1*d1 + d2*d2);
   }
};

#endif // __SoMinimalEnclosingCircle_H


