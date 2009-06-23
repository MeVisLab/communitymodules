//----------------------------------------------------------------------------------
//! The Inventor module class SoDraw2DSilhouette derived from SoView2DExtension
/*!
// \file    SoDraw2DSilhouette.h
// \author  Björn Meyer
// \date    2004-12-17
//
// SoDraw2DSilhouette retrieves data/a SoIndexedLineSet that is drawn in a SoView2D render area
*/
//----------------------------------------------------------------------------------

#ifndef __SODRAW2DSILHOUETTE_H__
#define __SODRAW2DSILHOUETTE_H__


// Local includes
#include "UMDSoView2DExtensionsSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <SoView2DExtension.h>
#include <View2DSlice.h>
#include <View2DSliceList.h>
// include used fields
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include <SoView2D.h>





//! 
class UMDSOVIEW2DEXTENSIONS_EXPORT SoDraw2DSilhouette : public SoView2DExtension
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoDraw2DSilhouette);

public:
  //! Constructor
  SoDraw2DSilhouette();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  // Input Connector for vertex-pointer
  SoSFNode inLineSet;

	SoSFVec3f barycentre;
	SoSFInt32 stippleFactor;

  //! Handling of events occurring in the viewer.
  virtual bool evalEvent(SoView2D *view2d, View2DSliceList *list,
                         View2DEvent* event, View2DEventPhase phase);

  //! Virtual method called by the \c SoView2D node when an image is rendered.
  virtual void draw(View2DSliceList *list, View2DSlice *slice, int z);

protected:
  //! Protected destructor
  virtual            ~SoDraw2DSilhouette();

  //! Sensor to detect changes in node's fields.
  SoNodeSensor      *_nodeSensor;

  //! Called by \c _nodeSensor when any field in node changes.
  //! Only redirects the call of this static function to the
  //! class method \c nodeChanged.
  static void       nodeChangedCB(void *data, SoSensor* sens);

  //! Called by the static function \c _nodeChangedCB to notify node changes.
  void              nodeChanged(SoNodeSensor* sensor);


private:

  void SoDraw2DSilhouette::_drawSilhouette(View2DSlice *slice);
  //void SoDraw2DSilhouette::_drawSilhouette();

  // your own member variables...
  SoIndexedLineSet *_lineSet;
  SoVertexProperty *_vertexProp;

  bool _lineSetDetected;
  float _colour[3];
};

#endif // __SODRAW2DSILHOUETTE_H

