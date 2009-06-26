//----------------------------------------------------------------------------------
//! The Inventor module class SoVisDataViewer derived from SoSeparator.
/*!
// \file    SoVisDataViewer.h
// \author
// \date    2007-04-26
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOVISDATAVIEWER_H__
#define __SOVISDATAVIEWER_H__


// Local includes
#include "UMDVisDataViewerSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
//#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/nodes/SoShape.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/fields/SoSFTrigger.h>
#include "XVLeaveScope.h"

class CvpDivSphere;
class CvpField;



//! The Inventor module class SoVisDataViewer derived from SoSeparator.
class UMDVISDATAVIEWER_EXPORT SoVisDataViewer : public SoShape //SoSeparator
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoVisDataViewer);

public:
  //! Constructor
  SoVisDataViewer();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  SoSFTrigger m_fldGenerateSphere;

  void createSphere (float fSx, float fSy, float fSz, float fRadius, int iDiv);
  void setDataField (CvpField *oField);
  void setSphereMode (unsigned int iMode);

  void GLRender/*BelowPath*/(SoGLRenderAction *action);
  void computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center);
  void generatePrimitives(SoAction *action);
  void inspectPoint(int iIndex);


  void logResults (void);



protected:
  //! Protected destructor
  virtual            ~SoVisDataViewer();

  //! Sensor to detect changes in node's fields.
  SoNodeSensor      *_nodeSensor;

  //! Called by \c _nodeSensor when any field in node changes.
  //! Only redirects the call of this static function to the
  //! class method \c nodeChanged.
  static void       nodeChangedCB(void *data, SoSensor* sens);

  //! Called by the static function \c _nodeChangedCB to notify node changes.
  void              nodeChanged(SoNodeSensor* sensor);


private:
  CvpDivSphere *m_oSphere;
  CvpField *m_oField;
  bool m_flHasSphere;
  bool m_flHasData;
  int m_iSphereMode;
  int iInspect;

  // your own member variables...

};

#endif // __SOVISDATAVIEWER_H


