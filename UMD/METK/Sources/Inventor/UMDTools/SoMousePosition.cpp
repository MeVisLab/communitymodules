//----------------------------------------------------------------------------------
//! The Inventor module class SoMousePosition getting the mouse position into a node field.
/*!
// \file    SoMousePosition.cpp
// \author  Maik Beye
// \date    2007-02-20
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoMousePosition.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoEventCallback.h>
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoMousePosition)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoMousePosition::initClass()
{
   SO_NODE_INIT_CLASS(SoMousePosition, SoGroup, "Group");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoMousePosition::SoMousePosition()
{

   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoMousePosition);

   SO_NODE_ADD_FIELD(mousePosition, (0.0,0.0));
   SO_NODE_ADD_FIELD(mouseX, (0));
   SO_NODE_ADD_FIELD(mouseY, (0));

   // EventCallback fuer die Mousebutton-Ereignisse
   SoEventCallback* eventCB = new SoEventCallback;
   addChild(eventCB);
   //eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressedCB, this);
   eventCB->addEventCallback(SoLocation2Event::getClassTypeId(), mouseMovedCB, this);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoMousePosition::~SoMousePosition()
{
}


void SoMousePosition::mouseMovedCB(void* userData, SoEventCallback* eventCB) {
   SoMousePosition* obj = (SoMousePosition*) userData;
   obj->mouseMoved(eventCB);
}


void SoMousePosition::mouseMoved(SoEventCallback* eventCB) {
   getMousePosition(eventCB);
}

void SoMousePosition::getMousePosition(SoEventCallback* eventCB) {

   const SoEvent* event = eventCB->getEvent();
   mousePosition.setValue( SbVec2s( event->getPosition()) );


}
