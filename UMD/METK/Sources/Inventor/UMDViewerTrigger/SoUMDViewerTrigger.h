// --------------------------------------------------------------------------
/*! Inventor node SoUMDViewerTrigger derived from SoSeparator to control the rendering status of an ExaminerViewer.
// \file    SoUMDViewerTrigger.h
// \author  Konrad Mühler
// \date    2005-04-23
//
//
*/
// --------------------------------------------------------------------------

#ifndef __SOUMDVIEWERTRIGGER_H__
#define __SOUMDVIEWERTRIGGER_H__


// Local includes
#include "UMDViewerTriggerSystem.h"
#include "UMDViewerTriggerInit.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/elements/SoCacheElement.h>
#include "XVLeaveScope.h"


//! Inventor node SoUMDViewerTrigger derived from SoSeparator to control the rendering status of an ExaminerViewer.
class UMDVIEWERTRIGGER_EXPORT SoUMDViewerTrigger : public SoSeparator
{
  SO_NODE_HEADER(SoUMDViewerTrigger); //!< Implements the runtime type system interface of this new node.

public:
  SoUMDViewerTrigger();

  static void  initClass();

  virtual void GLRenderBelowPath(SoGLRenderAction* action);

  //SoSFString ViewerNameFld; //!< Relay Name of Viewer to ViewerWriter
  SoSFString StatusFld;   //!< Rendering/Ready
  //SoSFString PongFld;
  //SoSFVec3f PositionFld;    //!< Postion of the camera
  //SoSFRotation OrientationFld;//!< Orientation of the camera

  //SoSFTrigger ForceRedrawFld;

protected:
  virtual ~SoUMDViewerTrigger();

private:
  bool frameRendered; //!< Sets FALSE before RenderPath and TRUE after coming back
  //SoGLRenderAction* globalAction;
  bool currentlyLeft; //!< Denies a rerendering after a direct call of rendering
  SoTimerSensor* timerSensor; //!< Delays the "Ready" of status a little bit to let a change proceed

  static void timerEvent(void* data, SoDataSensor* a);
  void render(SoDataSensor* sensor);


    //! node sensor: to react on field changes
    //SoNodeSensor* _nodeSensor;
    //! static callback function called on node change
    //static void _nodeChangedCB(void* data, SoDataSensor* sensor);
    //! private member function gets called by node change callback
    //void        _nodeChanged(SoDataSensor* sensor);
};

#endif // __SOUMDVIEWERTRIGGER_H


