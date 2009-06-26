//----------------------------------------------------------------------------------
//! The Inventor module class SoPickObject derived from SoGroup.
/*!
// \file    SoPickObject.h
// \author  Konrad
// \date    2005-09-28
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOPICKOBJECT_H__
#define __SOPICKOBJECT_H__


// Local includes
#include "METKPickingSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSubNode.h>
// include used fields
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/sensors/SoNodeSensor.h>

#include <Inventor/events/SoLocation2Event.h>

#include "XVLeaveScope.h"

#include <vector>

#include "mlMETKPicking.h"

class SoEventCallback;
class SoComputeBoundingBox;

//! The Inventor module class SoPickObject derived from SoGroup.
class SoPickObject : public SoSeparator
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoPickObject);

public:
   //! Constructor
   SoPickObject();

   //! Initializes this class (called on dll initialization).
   static void  initClass();

   void setMLClass(ml::METKPicking* mlPick);
   void setScrapLight(const float scrapLight);
   void setBBWeight(const float bbWeight);
   void setImportanceWeight(const float impWeight);
   void setPickPixelTolerance(const int pipi);
   void setshowBB(const bool showBB);
   void setEnableMouseOver(const bool bValue);

protected:
   //! Protected destructor
   virtual            ~SoPickObject();

   //! Callback for Mousebutton-Events
   static void mousePressedCB(void* userData, SoEventCallback* eventCB);
   void        mousePressed(SoEventCallback* eventCB);

   //! Callback for Mouse Move Events
   static void mouseMovedCB(void* userData, SoEventCallback* eventCB);
   void      mouseMoved(SoEventCallback* eventCB);

   //! Callback for MouseWheel-Events
   //static void mouseWheelCB(void* userData, SoEventCallback* eventCB);
   //void        mouseWheel(SoEventCallback* eventCB);

private:
   SbVec2s _pressPosition;
   SbVec3f _worldPosition;

   float _scrapLight;
   float _bbWeight;
   float _impWeight;
   int   _pickPixelTolerance;
   bool  _showBB;
   bool _enableMouseOver;

   SoComputeBoundingBox* _computeBBox;
   SoNode* getShape(SoEventCallback* eventCB, std::vector<SoNode*>* allObjectsInRay);
   SoSeparator* _debugOutput;

   SoSeparator* addBB(const SbVec3f& corner1, const SbVec3f& corner2);

   ml::METKPicking* _mlPick;
};

#endif // __SOPICKOBJECT_H__


