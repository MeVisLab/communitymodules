//----------------------------------------------------------------------------------
//! The Inventor node SoMousePosition getting the mouse position into a node field.
/*!
// \file    SoMousePosition.h
// \author  Maik Beyer
// \date    2007-02-20
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOMOUSEPOSITION_H
#define __SOMOUSEPOSITION_H


// Local includes
#include "UMDToolsSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSeparator.h>

// include used fields
#include <Inventor/fields/SoSFVec2s.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/events/SoLocation2Event.h>

#include "XVLeaveScope.h"

class SoEventCallback;

//! The Inventor node SoMousePosition getting the mouse position into a node field.
class UMDTOOLS_EXPORT SoMousePosition : public SoGroup
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoMousePosition);

public:
   //! Constructor
   SoMousePosition();

   SoSFVec2s mousePosition;
   SoSFInt32 mouseX;
   SoSFInt32 mouseY;

   //! Initializes this class (called on dll initialization).
   static void  initClass();

protected:
   //! Protected destructor
   virtual            ~SoMousePosition();

   //! Callback for Location2-Events
   static void mouseMovedCB(void* userData, SoEventCallback* eventCB);
   void        mouseMoved(SoEventCallback* eventCB);

   void getMousePosition(SoEventCallback* eventCB);
//private:

};

#endif // __SOMOUSEPOSITION_H

