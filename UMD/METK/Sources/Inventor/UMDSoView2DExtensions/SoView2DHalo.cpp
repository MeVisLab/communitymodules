//----------------------------------------------------------------------------------
//! The Inventor module class SoView2DHalo
/*!
// \file    SoView2DHalo.cpp
// \author  Björn Meyer
// \date    2005-06-27
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoView2DHalo.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#include "XVLeaveScope.h"


SO_NODE_SOURCE(SoView2DHalo)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoView2DHalo::initClass() {
   SO_NODE_INIT_CLASS(SoView2DHalo, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoView2DHalo::SoView2DHalo() {
   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoView2DHalo);
   SO_NODE_ADD_FIELD(upColor,       (1,0,0));
   SO_NODE_ADD_FIELD(downColor,     (0,1,0));
   SO_NODE_ADD_FIELD(worldPosition, (0,0,0));
   SO_NODE_ADD_FIELD(useLineStyle,  (TRUE));
   SO_NODE_ADD_FIELD(useColor,      (FALSE));
   
   
   //! Set inherited fields to a default state using the default string setting
   //! mechanism available in OpenInventor. E.g. in this case the extension
   //! shall be activated by button 1 and it must be over a valid voxel position
   //! to be activated.
   set("button1 PRESSED button2 IGNORED button3 IGNORED needsValidVoxel TRUE");
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoView2DHalo::~SoView2DHalo() {
   // Remove the node sensor.
}


// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoView2DHalo::evalEvent(SoView2D* /*view2d*/, View2DSliceList *slicelist, View2DEvent* ec, View2DEventPhase /*phase*/) {
   SbVec3f voxel, worldCoord;

   // get the position of the mouse, map it to world coordinates and save it in worldPosition
   slicelist->mapEventToVoxel(ec, voxel);
   slicelist->mapVoxelToWorld(voxel, worldCoord);
   worldPosition.setValue(worldCoord);
   return true;
}


void SoView2DHalo::draw(View2DSliceList *dsl, View2DSlice* dslice, int slice)
{
   // Drawing field enabled, i.e. are we supposed to draw?
   if (drawingOn.getValue()) {
      //*** map the world coordinates to device (window) coordinates ***
      float xDevice, yDevice;
      SbVec3f voxelCoord;
      dslice->mapWorldToDevice(worldPosition.getValue(), xDevice, yDevice);
      dsl->mapWorldToVoxel(worldPosition.getValue(), voxelCoord);

      glEnable(GL_LINE_STIPPLE);
      if ((slice - voxelCoord[2]) < 0) {
         if (useLineStyle.getValue())
            glLineStipple(1, 0xAAAA);
         if (useColor.getValue())
            glColor3f(downColor.getValue()[0], downColor.getValue()[1], downColor.getValue()[2]);
      }
      else {
         if (useLineStyle.getValue())
            glLineStipple(1, 0xFF00);
         if (useColor.getValue())
            glColor3f(upColor.getValue()[0], upColor.getValue()[1], upColor.getValue()[2]);
      }
      drawMiniCircle(xDevice, yDevice, abs(slice - voxelCoord[2]));
      glDisable(GL_LINE_STIPPLE);
   }
}
