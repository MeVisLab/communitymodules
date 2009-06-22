//----------------------------------------------------------------------------------
//! The Inventor module class SoView2DLiftChart
/*!
// \file    SoView2DLiftChart.cpp
// \author  Christian Tietjen
// \date    2006-03-30
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes

#include "LiftChartData.h"

#include "SoView2DLiftChart.h"


#include "XVEnterScope.h"
#include <SoView2D.h>          // SoView2D
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include <map>

SO_NODE_SOURCE(SoView2DLiftChart)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoView2DLiftChart::initClass()
{
   SO_NODE_INIT_CLASS(SoView2DLiftChart, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoView2DLiftChart::SoView2DLiftChart() {
   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoView2DLiftChart);
   
   SO_NODE_ADD_FIELD(verticalOffset, (100));
   

   //! Set inherited fields to a default state using the default string setting
   //! mechanism available in OpenInventor. E.g. in this case the extension
   //! shall be activated by button 1 and it must be over a valid voxel position
   //! to be activated.
   set("button1 PRESSED button2 IGNORED button3 IGNORED needsValidVoxel FALSE");

   _topOffset = _bottomOffset = 100;
   _leftOffset = 10;
   _currentSlice = 0;
   _width = 0;
   _height = 0;
   _dsl = 0;
   _liftChartData = 0;

   // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
   // sensor to be sure that changes are not delayed or collected.
   _nodeSensor = new SoNodeSensor(SoView2DLiftChart::nodeChangedCB, this);
   _nodeSensor->setPriority(0);
   _nodeSensor->attach(this);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoView2DLiftChart::~SoView2DLiftChart()
{
   if (_nodeSensor){ delete _nodeSensor; _nodeSensor = 0; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoView2DLiftChart::nodeChangedCB(void *data, SoSensor* sens)
{
   ((SoView2DLiftChart*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoView2DLiftChart::nodeChanged(SoNodeSensor* sensor)
{
   // Get the field which caused the notification.
   SoField* field = sensor->getTriggerField();

   // Handle changed fields here
   if (field == &verticalOffset)
      _topOffset = _bottomOffset = verticalOffset.getValue();
}


// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoView2DLiftChart::evalEvent(SoView2D *view2d, View2DSliceList *slicelist,
                                  View2DEvent* ec, View2DEventPhase phase)
{
   _dsl = slicelist;
   // Editing enabled?
   if (isEditingOn()){
      // Testen, ob Mausaktion innerhalb das LiftChart-Widgets ist
      int posX = ec->devicePosX;
      int posY = ec->devicePosY;

      if (posY > _topOffset && posY < _topOffset + _height &&
         posX > _leftOffset && posX < _leftOffset + _width + 20) {

            int x, y, z, c, t;
            slicelist->getInputImage()->getSize(x, y, z, c, t);

            switch (phase) {
         case EVENT_START:
            break;
         case EVENT_MOTION:
         case EVENT_RELEASE:
            // zur Schicht springen
            int slice;
            slice = (int)(z - ((float)(posY - _topOffset) / (float)_height) * (float)z);
            view2d->startSlice.setValue(slice);
            break;

         default:
            break;
            }

            // Marks this node as changed to force redraw.
            touch();
         }
   }
   _dsl = 0;
   return FALSE;
}


void SoView2DLiftChart::setLiftChartData(LiftChartData* lcd) {
   _liftChartData = lcd;
}


//-----------------------------------------------------------------------------
//! Drawing methods
//-----------------------------------------------------------------------------

void SoView2DLiftChart::draw(View2DSliceList *dsl, View2DSlice *dslice, int slice)
{
   _dsl = dsl;
   // Drawing field enabled, i.e. are we supposed to draw?
   if (_liftChartData && drawingOn.getValue()){

      // Yes, get size of connected image from the slice list. Note that this is the
      // list of slices connected to the SoView2D. Note that there is usually no need
      // for an additional image connected to this SoView2DExtension since that one of
      // the SoView2D accessable by dsl->getInputImage() is the relevant one.
      int x, y, z, c, t;
      dsl->getInputImage()->getSize(x, y, z, c, t);

      // Set image dimension as extent of the LiftChart
      float start[3], end[3];
      dsl->mapVoxelToWorld(0, 0, 0, start[0], start[1], start[2]);
      dsl->mapVoxelToWorld(x, y, z, end[0], end[1], end[2]);
      _liftChartData->setExtension(start, end);

      // Get the 2d devices (pixel) coordinates of the origin and the opposite corner of the
      // 2D region in which the slice is drawn. Note that these coordinates have nothing to do
      // with voxel coordinates. These coordinates are directly drawable device coordinates.
      float dx1, dy1;
      float dx2, dy2;
      dslice->getVisibleDeviceRect(dx1, dy1, dx2, dy2);
      _height = dy2 - _bottomOffset - (dy1 + _topOffset);
      
      // Balken fuer Strukturen einzeichnen
      switch (_liftChartData->getAggregation()) {
      case LiftChartData::AGGREGATE_ALL:
         _width = drawAllStructuresInOneColumn(dx1 + _leftOffset, dy1 + _topOffset);
         break;
      case LiftChartData::AGGREGATE_STRUCTURE:
         _width = drawOneStructurePerColumn(dx1 + _leftOffset, dy1 + _topOffset);
         break;
      case LiftChartData::AGGREGATE_STRUCTUREGROUP:
         _width = drawOneStructureGroupPerColumn(dx1 + _leftOffset, dy1 + _topOffset);
         break;
      case LiftChartData::AGGREGATE_NONE:
         _width = drawOneEntryPerColumn(dx1 + _leftOffset, dy1 + _topOffset);
         break;
      }

      // Rahmen zeichnen
      drawBorder(dx1 + _leftOffset, dy1 + _topOffset, dx1 + _leftOffset + _width, dy2 - _bottomOffset);
      
      drawBookmarks();

      // Slider für aktuelle Schicht
      int pos = 1 + dy1 + _topOffset + _height - (int) ((float)_height * ((float)slice / (float)(z - 1)));
      drawSliceIndicator(dx1 + _leftOffset, dx1 + _leftOffset + _width, pos, slice);
   }
   _dsl = 0;
}


void SoView2DLiftChart::drawBookmarks() {
}


int SoView2DLiftChart::drawOneEntryPerColumn(int x1, int y1) {
   int width = 0;
   int min, max;
   for (T_ConstStructureIterator iter = _liftChartData->structureSortedBegin(); iter != _liftChartData->structureSortedEnd(); iter++) {
      if (structureIsVisible(iter)) {
         min = mapWorldToChart((*iter)->getMin());
         max = mapWorldToChart((*iter)->getMax());
         drawBar(*iter, x1 + width, min + y1, x1 + width + 6, max + y1);
         width += 6;
      }
   }
   return width;
}


int SoView2DLiftChart::drawOneStructurePerColumn(int x1, int y1) {
   int width = 0;
   int min, max;
   T_ConstStructureIterator prev = _liftChartData->structureSortedBegin();
   
   for (T_ConstStructureIterator iter = _liftChartData->structureSortedBegin(); iter != _liftChartData->structureSortedEnd(); iter++) {
      if (structureIsVisible(iter)) {
         if ((*prev)->getStructure() != (*iter)->getStructure()) {
            width += 6;
            prev = iter;
         }
         min = mapWorldToChart((*iter)->getMin());
         max = mapWorldToChart((*iter)->getMax());
         drawBar(*iter, x1 + width, min + y1, x1 + width + 6, max + y1);
      }
   }
   return width + 6;
}


int SoView2DLiftChart::drawOneStructureGroupPerColumn(int x1, int y1) {
   int width = 0;
   int min, max;
   T_ConstStructureIterator prev = _liftChartData->structureSortedBegin();
   
   for (T_ConstStructureIterator iter = _liftChartData->structureSortedBegin(); iter != _liftChartData->structureSortedEnd(); iter++) {
      if (structureIsVisible(iter)) {
         if ((*prev)->getStructureGroup() != (*iter)->getStructureGroup()) {
            width += 6;
            prev = iter;
         }
         min = mapWorldToChart((*iter)->getMin());
         max = mapWorldToChart((*iter)->getMax());
         drawBar(*iter, x1 + width, min + y1, x1 + width + 6, max + y1);
      }
   }
   return width + 6;
}


int SoView2DLiftChart::drawAllStructuresInOneColumn(int x1, int y1) {
   int min, max;
   for (T_ConstStructureIterator iter = _liftChartData->structureSortedBegin(); iter != _liftChartData->structureSortedEnd(); iter++) {
      if (structureIsVisible(iter)) {
         min = mapWorldToChart((*iter)->getMin());
         max = mapWorldToChart((*iter)->getMax());
         drawBar(*iter, x1, min + y1, x1 + 6, max + y1);
      }
   }
   return 6;
}


void SoView2DLiftChart::drawBorder(int x1, int y1, int x2, int y2) {
   glColor3f(1, 1, 1);
   glBegin(GL_LINE_LOOP);
   glVertex2f(x1 - 1, y1 - 1);
   glVertex2f(x2 + 1, y1 - 1);
   glVertex2f(x2 + 1, y2 + 1);
   glVertex2f(x1 - 1, y2 + 1);
   glEnd();
}


void SoView2DLiftChart::drawSliceIndicator(int x1, int x2, int y, int slice) {
   glColor3f(1, 1, 1);
   glBegin(GL_LINES);
   glVertex2f(x1,     y - 1);
   glVertex2f(x2 + 6, y - 1);
   glEnd();

   // convert float distance to string
   SbString sliceString = float(slice);

   View2DFont* font = SoView2D::globalFont(_dsl->getCurrentCacheContext());
   float fcolor[4] = {color.getValue()[0],color.getValue()[1],color.getValue()[2],1};
   font->drawString(x2 + 10, y - 6, 12, fcolor, sliceString.getString(), 0, true);
}


void SoView2DLiftChart::drawBar(Structure* structure, int x1, int y1, int x2, int y2) {
   glColor4fv(structure->getRGBA());
   glBegin(GL_POLYGON);
   glVertex2f(x1, y1);
   glVertex2f(x2, y1);
   glVertex2f(x2, y2);
   glVertex2f(x1, y2);
   glEnd();
}



//-----------------------------------------------------------------------------
// Transformations
//-----------------------------------------------------------------------------


float SoView2DLiftChart::mapWorldToChart(const float* world) {
   float tempX, tempY, tempZ;

   mapWorldToVoxel(world, tempX, tempY, tempZ);
   
   int value = _height - (tempZ * getRatio()) - 1;
   if (value < 0 ) value = 0;
   if (value > _height) value = _height;
   return value;
}


float SoView2DLiftChart::getRatio() {
   //! Get the current viewing direction (axial, coronal or sagittal)
   //! Get the scaling ratio to map the structure bars onto the screen
   int x, y, z, c, t;
   _dsl->getInputImage()->getSize(x, y, z, c, t);
   return float(_height) / float(z);
}


void SoView2DLiftChart::mapWorldToVoxel(const float* world, float &voxelX, float &voxelY, float &voxelZ) {
   if (_dsl) {
      _dsl->mapWorldToVoxel(world[0], world[1], world[2], voxelX, voxelY, voxelZ);
   }
   else {
      voxelX = 0;
      voxelY = 0; 
      voxelZ = 0;
   }
}


const bool SoView2DLiftChart::structureIsVisible(T_ConstStructureIterator iter) {
   float chartMin, chartMax, strMin, strMax;
   chartMin = mapWorldToChart(_liftChartData->getMin());
   chartMax = mapWorldToChart(_liftChartData->getMax());

   strMin = mapWorldToChart((*iter)->getMin());
   strMax = mapWorldToChart((*iter)->getMax());

   float strExt = abs(strMax - strMin);
   float chartExt = abs(chartMax - chartMin);
   float ratio = strExt / chartExt;
   if ((ratio <= _liftChartData->getMaxExtent() || _liftChartData->getMaxExtent() == 1.0f) && _liftChartData->getStructureVisible(iter))
      return true;
   return false;
}
