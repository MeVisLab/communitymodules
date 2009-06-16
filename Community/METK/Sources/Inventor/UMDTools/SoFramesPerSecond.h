//----------------------------------------------------------------------------------
//! The Inventor module class SoFramesPerSecond derived from SoSeparator
/*!
// \file    SoFramesPerSecond.h
// \author  Christian Tietjen
// \date    2004-12-23
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SoFramesPerSecond_H__
#define __SoFramesPerSecond_H__


// Local includes
#include "UMDToolsSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
// include used fields
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/SbTime.h>
#include "XVLeaveScope.h"

//! 
class UMDTOOLS_EXPORT SoFramesPerSecond : public SoSeparator
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoFramesPerSecond);
   
public:
   //! Constructor
   SoFramesPerSecond();

   SoSFFloat fps;
   
   //! Initializes this class (called on dll initialization).
   static void  initClass();
   
protected:
   //! Protected destructor
   virtual            ~SoFramesPerSecond();

   virtual void GLRenderBelowPath(SoGLRenderAction* action);

private:
   int _frameCount;
   SbTime _lastSnapShot;
   SbTime _currentFrameTime;
};

#endif // __SoFramesPerSecond_H


