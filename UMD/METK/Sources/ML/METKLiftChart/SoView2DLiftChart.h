//----------------------------------------------------------------------------------
//! Inventor node SoView2DLiftChart derived from SoView2DExtension.
/*!
// \file    SoView2DLiftChart.h
// \author  Christian Tietjen
// \date    2006-03-30
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOVIEW2DLIFTCHART_H__
#define __SOVIEW2DLIFTCHART_H__


// Local includes
#include "METKLiftChartSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

#include <View2DFont.h>
#include <View2DSlice.h>
#include <View2DSliceList.h>
#include <SoView2DExtension.h>

#include <Inventor/fields/SoSFInt32.h>

#include "XVLeaveScope.h"

#include "LiftChartData.h"

//! Inventor node SoView2DLiftChart derived from SoView2DExtension.
class METKLIFTCHART_EXPORT SoView2DLiftChart : public SoView2DExtension
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoView2DLiftChart);

public:
   //! Constructor
   SoView2DLiftChart();

   //! Initializes this class (called on dll initialization).
   static void  initClass();

   //@{! Fields
   SoSFInt32 verticalOffset;
   //@}

   //! Handling of events occurring in the viewer.
   virtual bool evalEvent(SoView2D *view2d, View2DSliceList *list,
      View2DEvent* event, View2DEventPhase phase);

   //! Virtual method called by the \c SoView2D node when an image is rendered.
   virtual void draw(View2DSliceList *list, View2DSlice *slice, int z);

   void setLiftChartData(LiftChartData* lcd);

protected:
   //! Protected destructor
   virtual            ~SoView2DLiftChart();

   //! Sensor to detect changes in node's fields.
   SoNodeSensor      *_nodeSensor;

   //! Called by \c _nodeSensor when any field in node changes.
   //! Only redirects the call of this static function to the
   //! class method \c nodeChanged.
   static void       nodeChangedCB(void *data, SoSensor* sens);

   //! Called by the static function \c _nodeChangedCB to notify node changes.
   void              nodeChanged(SoNodeSensor* sensor);

private:
   int _currentSlice;
   int _width, _height;
   LiftChartData* _liftChartData;
   int _topOffset, _bottomOffset, _leftOffset;

   void drawBookmarks();
   int  drawOneEntryPerColumn(int x1, int y1);
   int  drawOneStructurePerColumn(int x1, int y1);
   int  drawOneStructureGroupPerColumn(int x1, int y1);
   int  drawAllStructuresInOneColumn(int x1, int y1);
   void drawBorder(int x1, int y1, int x2, int y2);
   void drawSliceIndicator(int x1, int x2, int y, int slice);
   void drawBar(Structure* structure, int x1, int y1, int x2, int y2);

   //! maps the world coordinate to local voxel coordinates
   View2DSliceList* _dsl;
   float mapWorldToChart(const float* world);
   void  mapWorldToVoxel(const float* world, float &voxelX, float &voxelY, float &voxelZ);
   float getRatio();

   const bool structureIsVisible(T_ConstStructureIterator iter);

   struct ltstr
   {
      bool operator()(const std::string s1, const std::string s2) const
      {
         return s1 < s2;
      }
   };
};

#endif // __SoView2DLiftChart_H


