//----------------------------------------------------------------------------------
//! The Inventor module class SoPickInfo derived from SoSeparator
/*!
// \file    SoPickInfo.h
// \author  Stefan Hiller
// \date    2007-09-11
//
// Get picking information.
*/
//----------------------------------------------------------------------------------

#ifndef __SOPICKINFO_H__
#define __SOPICKINFO_H__


// Local includes
#include "UMDPickInfoSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoType.h>

#include <Inventor/actions/SoHandleEventAction.h>

#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/details/SoCylinderDetail.h>
#include <Inventor/details/SoCubeDetail.h>
#include <Inventor/details/SoConeDetail.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include "XVLeaveScope.h"



//! The Inventor module class SoPickInfo derived from SoSeparator to get picking information.
class UMDPICKINFO_EXPORT SoPickInfo : public SoSeparator
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoPickInfo);

public:
  //! Constructor
  SoPickInfo();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  SoSFVec3f     infoHitCoordinate;
  SoSFVec3f     infoObjHitCoordinate;
  SoSFVec3f     infoHitNormal;
  SoSFVec3f     infoObjHitNormal;
  SoSFString      infoStatus;
  SoSFString      infoTypeName;
  SoSFString      infoPickPath;
  SoSFString      infoDetail;


protected:
  //! Protected destructor
  virtual            ~SoPickInfo();

  //! Method for event handling (keyboard/mouse).
    virtual void    handleEvent(SoHandleEventAction* action);

private:

  SbVec3f       _tempPickPoint;
  bool        _leftMouseButtonDown;

  void        _resetFields();
};

#endif // __SOPICKINFO_H


