//----------------------------------------------------------------------------------
//! The Inventor module class SoView2DExtensionExample
/*!
// \file    SoView2DExtensionExample.cpp
// \author  Konrad Mühler
// \date    2008-01-07
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoView2DExtensionExample.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
#include <Inventor/actions/SoGLRenderAction.h>
// TODO: include more inventor things here
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoView2DExtensionExample)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoView2DExtensionExample::initClass()
{
  SO_NODE_INIT_CLASS(SoView2DExtensionExample, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoView2DExtensionExample::SoView2DExtensionExample()
{
  SO_TRACE_IN("SoView2DExtensionExample::SoView2DExtensionExample")

  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoView2DExtensionExample);

  //! Set inherited fields to a default state using the default string setting
  //! mechanism available in OpenInventor. E.g. in this case the extension
  //! shall be activated by button 1 and it must be over a valid voxel position
  //! to be activated.
  set("button1 PRESSED button2 IGNORED button3 IGNORED needsValidVoxel TRUE");



  // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
  // sensor to be sure that changes are not delayed or collected.
  _nodeSensor = new SoNodeSensor(SoView2DExtensionExample::nodeChangedCB, this);
  _nodeSensor->setPriority(0);
  _nodeSensor->attach(this);


}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoView2DExtensionExample::~SoView2DExtensionExample()
{
  SO_TRACE_IN("SoView2DExtensionExample::~SoView2DExtensionExample")


  // Remove the node sensor.
  if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoView2DExtensionExample::nodeChangedCB(void *data, SoSensor* sens)
{
  SO_TRACE_IN("SoView2DExtensionExample::nodeChangedCB")

  ((SoView2DExtensionExample*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoView2DExtensionExample::nodeChanged(SoNodeSensor* sensor)
{
  SO_TRACE_IN("SoView2DExtensionExample::nodeChanged")

  // Get the field which caused the notification.
  SoField* field = sensor->getTriggerField();

  // Handle changed fields here
  if (field == &button1) {/* ... */}
}



// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoView2DExtensionExample::evalEvent(SoView2D * /*view2d*/, View2DSliceList *slicelist,
                                View2DEvent* ec, View2DEventPhase phase)
{
  SO_TRACE_IN("SoView2DExtensionExample::evalEvent")

  // Invalidate last selected voxel position.
  _lastSelectedPos.setValue(-1, -1, -1);

  // Editing enabled?
  if (isEditingOn()){
    // Convert mouse event position to voxel coordinates.
    // \c flag is true if voxel coordinates are valid, false if coordinates are out
    // of image range.
    float vx, vy, vz, wx, wy, wz;
    bool flag = slicelist->mapEventToVoxel(ec, vx, vy, vz);

    // Position is inside slice?
    if (flag)
    {
      // Convert voxel to world coordinates and print coordinates to console.
      slicelist->mapVoxelToWorld(vx, vy, vz, wx, wy, wz);
      std::cout << "Voxel position: " << vx << " " << vy << " " << vz << std::endl;
      std::cout << "World position: " << wx << " " << wy << " " << wz << std::endl;

      // Evaluate event phases. On activation and motion draw cross
      // and clean it on release.
      switch (phase) {
        case EVENT_START:
        case EVENT_MOTION:
          // Store current selected voxel position in _lastSelectedPosition.
          _lastSelectedPos.setValue(vx, vy, vz);
        break;

        default:
        break;
      }
    }

    // Marks this node as changed to force redraw.
    touch();
  }
  return FALSE;
}

void SoView2DExtensionExample::draw(View2DSliceList *dsl, View2DSlice *dslice, int slice)
{
  SO_TRACE_IN("SoView2DExtensionExample::draw")

  // Drawing field enabled, i.e. are we supposed to draw?
  if (drawingOn.getValue()){
    // Yes, get size of connected image from the slice list. Note that this is the
    // list of slices connected to the SoView2D. Note that there is usually no need
    // for an additional image connected to this SoView2DExtension since that one of
    // the SoView2D accessable by dsl->getInputImage() is the relevant one.
    int x, y, z, c, t;
    dsl->getInputImage()->getSize(x, y, z, c, t);

    //! Get the 2d devices (pixel) coordinates of the origin and the opposite corner of the
    //! 2D region in which the slice is drawn. Note that these coordinates have nothing to do
    //! with voxel coordinates. These coordinates are directly drawable device coordinates.
    float dx1, dy1;
    float dx2, dy2;
    dslice->getVisibleDeviceRect(dx1, dy1, dx2, dy2);


    //----------------------------------------------------------------------
    //! Drawing part 1: Draw a diagonal line from the origin to the opposite
    //! corner of the region in which the slice is shown.
    //----------------------------------------------------------------------
    // Set OpenGL drawing color. Note that this color does not have a transparency component.
    glColor3fv(_rgbColor);
    glBegin(GL_LINES);
      glVertex2f(dx1, dy2);
      glVertex2f(dx2, dy1);
    glEnd();


    //----------------------------------------------------------------------
    //! Drawing part 2: Draw a diagonal line from the origin voxel and the
    //! opposite voxel in the voxel image.
    //----------------------------------------------------------------------
    //! Set red as drawing color.
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
      //! Tranform coordinates of origin voxel of this slice to device coordinates and draw line start.
      dslice->mapVoxelToDevice(0, 0, slice, dx1, dy1);
      glVertex2f(dx1, dy1);

      //! Transform coordinates of opposite voxel of this slice to device coordinates and draw line.
      dslice->mapVoxelToDevice(x, y, slice, dx1, dy1);
      glVertex2f(dx1, dy1);
    glEnd();


    //----------------------------------------------------------------------
    //! Drawing part 3: Draw a small cross at the last selected position if
    //! it's valid and if it's near enough to currently drawn slice. Note
    //! that voxel centers are considered at (0.5, 0.5, 0.5).
    //----------------------------------------------------------------------
    float vx, vy, vz, cx, cy;
    _lastSelectedPos.getValue(vx, vy, vz);

    if ((vx>=0) && (vy>=0) && (vz>=slice+0.45) && (vz<=slice+0.55)){
      glBegin(GL_LINES);
        //! Transform coordinates of origin voxel of this slice to device coordinates and draw line start.
        //! This transformation corrects drawing positions to correct voxel centers.
        dslice->mapVoxelToDevice(vx,vy,vz, cx, cy);
        glVertex2f(cx-3, cy);
        glVertex2f(cx+3, cy);

        glVertex2f(cx, cy-3);
        glVertex2f(cx, cy+3);
      glEnd();
    }
  }
}


