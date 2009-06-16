//----------------------------------------------------------------------------------
//! The Inventor module class SoFramesPerSecond
/*!
// \file    SoFramesPerSecond.cpp
// \author  Christian Tietjen
// \date    2004-12-23
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoFramesPerSecond.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoFramesPerSecond)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoFramesPerSecond::initClass()
{
   SO_NODE_INIT_CLASS(SoFramesPerSecond, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoFramesPerSecond::SoFramesPerSecond()
{
   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoFramesPerSecond);

   SO_NODE_ADD_FIELD(fps, (0));

   _lastSnapShot = 0.0;
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoFramesPerSecond::~SoFramesPerSecond() 
{
}

void SoFramesPerSecond::GLRenderBelowPath(SoGLRenderAction* action) { 
   SoSeparator::GLRenderBelowPath(action);
   
   _currentFrameTime = SbTime::getTimeOfDay();
   _frameCount++;
   
   double time_passed = (_currentFrameTime - _lastSnapShot).getValue();
   
   if (time_passed > 1.0) {
      fps.setValue(((double) _frameCount) / time_passed);
      _frameCount = 0;
      _lastSnapShot = SbTime::getTimeOfDay();
   }
}
