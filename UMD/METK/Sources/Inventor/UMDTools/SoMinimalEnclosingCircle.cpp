//----------------------------------------------------------------------------------
//! The Inventor module class SoMinimalEnclosingCircle
/*!
// \file    SoMinimalEnclosingCircle.cpp
// \author  Christian Tietjen
// \date    2004-12-23
//
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoMinimalEnclosingCircle.h"
#include <UMDSoShapeToPointSet.h>

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/misc/SoState.h>

#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoCacheElement.h>

#include <Inventor/sensors/SoNodeSensor.h>

#include "XVLeaveScope.h"

#include <mlOpenGL.h>

SO_NODE_SOURCE(SoMinimalEnclosingCircle)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoMinimalEnclosingCircle::initClass()
{
   SO_NODE_INIT_CLASS(SoMinimalEnclosingCircle, SoSeparator, "Separator");
   SO_ENABLE(SoGLRenderAction, SoViewingMatrixElement);
   //SO_ENABLE(SoGLRenderAction, SoLazyElement);
   //SO_ENABLE(SoGLRenderAction, SoModelMatrixElement);
   //SO_ENABLE(SoGLRenderAction, SoViewVolumeElement);

}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoMinimalEnclosingCircle::SoMinimalEnclosingCircle()
{

   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoMinimalEnclosingCircle);

   SO_NODE_ADD_FIELD(center,     (0, 0, 0));
   SO_NODE_ADD_FIELD(radius,     (0));

   _pointSet3D = 0;
   _pointSet2D = 0;
   _size3D = 0;
   _zValue = 0;
   _pixelMatrix = 0;
   _winSize[0] = _winSize[1] = -1;

   _pointSetRaster = 0;
   _sizeRaster = 0;

   _nodeSensor = new SoNodeSensor(&nodeChangedCB, this);
   _nodeSensor->attach(this);
   _nodeSensor->setPriority(0);

}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoMinimalEnclosingCircle::~SoMinimalEnclosingCircle()
{
   // Remove the node sensor.
   if (_nodeSensor) { delete _nodeSensor; _nodeSensor = 0; }

   if (_pointSet3D) { delete[] _pointSet3D; _pointSet3D = 0; }
   if (_pointSet2D) { delete[] _pointSet2D; _pointSet2D = 0; }

   if (_pixelMatrix) { delete[] _pixelMatrix; _pixelMatrix = 0; }
   if (_pointSetRaster) { delete[] _pointSetRaster; _pointSetRaster = 0; }
}


//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoMinimalEnclosingCircle::nodeChangedCB(void *data, SoSensor* sens)
{
   ((SoMinimalEnclosingCircle*) data)->nodeChanged((SoNodeSensor*)sens);
}


//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoMinimalEnclosingCircle::nodeChanged(SoNodeSensor* sensor)
{
   // Get the field which caused the notification.
   SoField* field = sensor->getTriggerField();

   // Handle changed fields here
   if (field == 0) {
      // at least one of the children was changed
      getPointSet(_pointSet3D, _size3D);
      _pointSet2D = new SbVec2f[_size3D];
   }
}


const SbVec3f SoMinimalEnclosingCircle::getScreenPos(const SbVec3f &vertex) {
   // the resulting values for visible vertices ranges from -1 to +1 for x and y dimension
   SbVec3f  screenPoint;

   // Transform the vertex into screen space
   _objToScreen.multVecMatrix(vertex, screenPoint);

   return screenPoint;
}


void SoMinimalEnclosingCircle::GLRenderBelowPath(SoGLRenderAction* action)
{
   SoGroup::GLRenderBelowPath(action);
}


void SoMinimalEnclosingCircle::GLRender(SoGLRenderAction* action)
{
   if (_size3D > 2) {
      compute(action);
   }
   else {
      center.setValue(-1, -1, -1);
      radius.setValue(-1);
   }
}




void SoMinimalEnclosingCircle::compute(SoGLRenderAction* action) {
   SoState* state = action->getState();
   int counter = 0;

   // Get the matrices from the state to convert from object to screen space
   _objToScreen = (SoViewingMatrixElement::get(state) * SoProjectionMatrixElement::get(state));

   // Get the size of the window from the state
   // Resize the pixelmatrix only if the window size has changed
   if (SbVec2s(_winSize[0], _winSize[1]) != SoViewportRegionElement::get(state).getWindowSize()) {
      _winSize[0] = SoViewportRegionElement::get(state).getWindowSize()[0];
      _winSize[1] = SoViewportRegionElement::get(state).getWindowSize()[1];
      if (_pixelMatrix) delete[] _pixelMatrix;
      _pixelMatrix = new SbBool[static_cast<int>((_winSize[0] + 1) * (_winSize[1] + 1))];
   }

   // iniatilize the raster
   for (counter = 0; counter < _winSize[0] * _winSize[1]; counter++)
      _pixelMatrix[counter] = false;

   SbVec3f temp;
   _zValue = 0;
   // project the vertices onto the screen
   for (counter = 0; counter < _size3D; counter++) {
      temp = getScreenPos(_pointSet3D[counter]);
      _pointSet2D[counter].setValue(temp[0], temp[1]);
      _zValue += temp[2];
   }
   _zValue /= _size3D;

   rasterizeAndSort();
   if (_sizeRaster > 2) {
      screenSpaceBoundingBox();
   } else {
      center.setValue(-1, -1, -1);
      radius.setValue(-1);
   }
}


void SoMinimalEnclosingCircle::getPointSet(SbVec3f* &pointSet, long &size) {

   // Punktmenge einlesen
   if (getNumChildren() != 0) {
      UMDSoShapeToPointSet* shapeToPointSet = new UMDSoShapeToPointSet;
      shapeToPointSet->ref();
      shapeToPointSet->rootInput.setValue(this);
      shapeToPointSet->calculate();
      size = shapeToPointSet->vertexProperty->vertex.getNum();
      if (size != 0) {
         if (_pointSet3D) delete[] _pointSet3D;
         pointSet = new SbVec3f[size];
         const SbVec3f* pSbVec3f = shapeToPointSet->vertexProperty->vertex.getValues(0);
         for (int counter = 0; counter < size; counter++) {
            pointSet[counter] = pSbVec3f[counter];
         }
      }
      shapeToPointSet->unref();
   }
}


void SoMinimalEnclosingCircle::rasterizeAndSort() {

   float pointSetX, pointSetY;
   int counter;
   _sizeRaster = 0;

   // rasterize all points to the viewport-pixels, points outside the viewport will be discarded
   // all points will be sorted lexicographical
   for (counter = 0; counter < _size3D; counter++) {
      pointSetX = _pointSet2D[counter][0];
      pointSetY = _pointSet2D[counter][1];

      // point is visible?
      if (fabs(pointSetX) < 1.0 && fabs(pointSetY) < 1.0) {
         // convert 2D coords in 1D coords
         int coord =
            (int)((pointSetX + 1) * 0.5f * _winSize[0]) +
            (int)((pointSetY + 1) * 0.5f * _winSize[1]) * _winSize[0];
         // if the pixel is not already set, set it and don't forget, that there is one point more
         if (!_pixelMatrix[coord]) {
            _sizeRaster++;
            _pixelMatrix[coord] = true;
         }
      }
   }

   if (_pointSetRaster) delete[] _pointSetRaster;
   _pointSetRaster = new SbVec2f[_sizeRaster];

   int pRaster = 0;
   // walk through the array until all points are found
   for (counter = 0; counter < _sizeRaster; counter++) {
      // walk to the next point
      while (_pixelMatrix[pRaster] == false)
         ++pRaster;
      // convert 1D coords in 2D coords
      // transform them to the interval -1..0..1
      _pointSetRaster[counter][0] = (((float)(pRaster % (int)_winSize[0]) / _winSize[0]) - 0.5f) * 2;
      _pointSetRaster[counter][1] = (((float)(pRaster / (int)_winSize[0]) / _winSize[1]) - 0.5f) * 2;
      ++pRaster;
   }
}



void SoMinimalEnclosingCircle::screenSpaceBoundingBox() {
   SbVec2f minBBox2, maxBBox2;
   minBBox2 = maxBBox2 = _pointSetRaster[0];

   for (int counter = 0; counter < _sizeRaster; counter++) {
      if (minBBox2[0] > _pointSetRaster[counter][0]) minBBox2[0] = _pointSetRaster[counter][0];
      if (minBBox2[1] > _pointSetRaster[counter][1]) minBBox2[1] = _pointSetRaster[counter][1];
      if (maxBBox2[0] < _pointSetRaster[counter][0]) maxBBox2[0] = _pointSetRaster[counter][0];
      if (maxBBox2[1] < _pointSetRaster[counter][1]) maxBBox2[1] = _pointSetRaster[counter][1];
   }
   center.setValue(minBBox2[0] + ((maxBBox2[0] - minBBox2[0]) / 2.0f), minBBox2[1] + ((maxBBox2[1] - minBBox2[1]) / 2.0f), _zValue);
   radius.setValue(distance2D(minBBox2.getValue(), maxBBox2.getValue()));
}
