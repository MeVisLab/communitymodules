//----------------------------------------------------------------------------------
//! The Inventor module class SoPickObject2
/*!
// \file    SoPickObject2.cpp
// \author  Konrad
// \date    2005-09-28
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoPickObject.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"


#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/elements/SoCacheElement.h>

#include <Inventor/actions/SoHandleEventAction.h>

//#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <SoMouseWheelEvent.h>

#include <Inventor/engines/SoComputeBoundingBox.h>

#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoPickObject)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoPickObject::initClass()
{
   SO_NODE_INIT_CLASS(SoPickObject, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoPickObject::SoPickObject()
{
   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoPickObject);

   _mlPick = 0;
   _scrapLight = 0.2;
   _showBB = false;

   // EventCallback fuer die Mousebutton-Ereignisse
   SoEventCallback* eventCB = new SoEventCallback;
   addChild(eventCB);
   eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressedCB, this);
   eventCB->addEventCallback(SoLocation2Event::getClassTypeId(), mouseMovedCB, this);
   //eventCB->addEventCallback(SoMouseWheelEvent::getClassTypeId(), mouseWheelCB, this);

   _debugOutput = new SoSeparator();
   addChild(_debugOutput);

   _computeBBox = new SoComputeBoundingBox();
   _computeBBox->ref();

}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoPickObject::~SoPickObject()
{
   _computeBBox->unref();
}


void SoPickObject::setMLClass(ml::METKPicking* mlPick) {
   _mlPick = mlPick;
}


void SoPickObject::setScrapLight(const float scrapLight) {
   _scrapLight = scrapLight;
}


void SoPickObject::setBBWeight(const float bbWeight) {
   _bbWeight = bbWeight;
}


void SoPickObject::setImportanceWeight(const float impWeight) {
   _impWeight = impWeight;
}


void SoPickObject::setPickPixelTolerance(const int pipi) {
   _pickPixelTolerance = pipi;
}


void SoPickObject::setshowBB(const bool showBB) {
   _showBB = showBB;
}

void SoPickObject::setEnableMouseOver(const bool bValue) {
	_enableMouseOver = bValue;
}


void SoPickObject::mousePressedCB(void* userData, SoEventCallback* eventCB) {
   SoPickObject* obj = (SoPickObject*) userData;
   obj->mousePressed(eventCB);
}


void SoPickObject::mousePressed(SoEventCallback* eventCB) {
   const SoEvent* event = eventCB->getEvent();

   // linke Maustaste [und rechte gleichermaßen (KONRAD)]
   if (SO_MOUSE_PRESS_EVENT(event, BUTTON1) || SO_MOUSE_PRESS_EVENT(event, BUTTON3)) {
      _pressPosition = eventCB->getEvent()->getPosition();
   }
   else if ((SO_MOUSE_RELEASE_EVENT(event, BUTTON1) || SO_MOUSE_RELEASE_EVENT(event, BUTTON3)) && _mlPick) {
      if (SbVec2f(_pressPosition[0] - event->getPosition()[0], _pressPosition[1] - event->getPosition()[1]).length() < float(_pickPixelTolerance)) {
         std::vector<SoNode*>* allObjInRay = new std::vector<SoNode*>;
         SoNode* tempNode = getShape(eventCB,allObjInRay);

         if (tempNode) {			 
            _mlPick->setPickedShape(tempNode, &_worldPosition, allObjInRay);
         }
         else {
            _mlPick->setPickedShape(0, &SbVec3f(0, 0, 0), 0);
         }
      }
   }
}

void SoPickObject::mouseMovedCB(void* userData, SoEventCallback* eventCB)
{	
	( (SoPickObject*) userData )->mouseMoved(eventCB);
}


void SoPickObject::mouseMoved(SoEventCallback* eventCB)
{
	//SbVec2s temp = eventCB->getEvent()->getPosition();
	//cout << "x:" << temp[0] << "y:" << temp[1] << endl;
	if (_mlPick && _enableMouseOver)
	{
		std::vector<SoNode*>* allObjInRay = new std::vector<SoNode*>;
		SoNode* tempNode = getShape(eventCB,allObjInRay);
		if (tempNode) {			 
			_mlPick->setMouseOverShape(tempNode, allObjInRay);
		}
		else {
			_mlPick->setMouseOverShape(0, 0);
		}
	}
}


/*void SoPickObject::mouseWheelCB(void* userData, SoEventCallback* eventCB) {
SoPickObject* obj = (SoPickObject*) userData;
obj->mouseWheel(eventCB);
}

void SoPickObject::mouseWheel(SoEventCallback* eventCB) {
const SoEvent* event = eventCB->getEvent();
}*/


SoNode* SoPickObject::getShape(SoEventCallback* eventCB, std::vector<SoNode*>* allObjectsInRay) {
   SoNode* pickedShape = 0;
   const SoPickedPoint* pickedPoint = 0;
   if (allObjectsInRay==NULL)
      allObjectsInRay = new std::vector<SoNode*>;

   // initialize DebugOutput
   _debugOutput->removeAllChildren();
   if (_showBB) {
      SoDrawStyle* ds = new SoDrawStyle();
      ds->style = SoDrawStyle::LINES;
      _debugOutput->addChild(ds);

      SoMaterial* mat = new SoMaterial();
      mat->diffuseColor.setValue(0, 0, 0);
      mat->transparency.setValue(0);
      _debugOutput->addChild(mat);

      SoPickStyle* pick = new SoPickStyle();
      pick->style = SoPickStyle::UNPICKABLE;
      _debugOutput->addChild(pick);
   }

   SoHandleEventAction*	hea = eventCB->getAction();
   const SoPickedPointList* pickedPointList = &hea->getPickedPointList();
   SoTransform* neededForVec3f = new SoTransform();
   neededForVec3f->ref();
   float rayAttenuation = 1;
   float bbRating = 0, impRating = 0, overallRating = -1;
   float maxBBox = 0;

   // Get all surrounding bounding box for normalization
   if (pickedPointList->getLength() > 0) {
      SoNode* head = (*pickedPointList)[0]->getPath()->getHead();
      float maxX, maxY, maxZ;
      _computeBBox->node.setValue(head);
      neededForVec3f->translation.connectFrom(&_computeBBox->min);
      neededForVec3f->scaleFactor.connectFrom(&_computeBBox->max);
      (neededForVec3f->scaleFactor.getValue() - neededForVec3f->translation.getValue()).getValue(maxX, maxY, maxZ);
      maxBBox = maxX * maxY * maxZ;
   }
   for (int counter = 0; counter < pickedPointList->getLength(); counter++)
   {
      // wenn der Strahl noch stark genug ist
      //if (rayAttenuation > _scrapLight) {
      pickedPoint = (*pickedPointList)[counter];
      float x, y, z;

      SoNode* tail = 0;

      // from all nodes in this path get the one who is registered in the METK database
      for (int pathPos = 0; pathPos < pickedPoint->getPath()->getLength(); pathPos++) {
         if (_mlPick->containsNode(pickedPoint->getPath()->getNodeFromTail(pathPos))) {
            tail = pickedPoint->getPath()->getNodeFromTail(pathPos);
            break;
         }
      }

      if (tail) {			
         _computeBBox->node.setValue(tail);
         neededForVec3f->translation.connectFrom(&_computeBBox->min);
         neededForVec3f->scaleFactor.connectFrom(&_computeBBox->max);

         (neededForVec3f->scaleFactor.getValue() - neededForVec3f->translation.getValue()).getValue(x, y, z);

         // wenn überhaupt anzupicken
         if (_mlPick->getPickable(tail) && _mlPick->getTransparency(tail) < 1.0) {
            bbRating = (1 - (x * y * z) / maxBBox) * _bbWeight;
            impRating = _mlPick->getImportance(tail) * _impWeight;
            // wenn der Strahl noch stark genug ist
            if (rayAttenuation > _scrapLight) {
               if (_showBB)
                  _debugOutput->addChild(addBB(neededForVec3f->translation.getValue(), neededForVec3f->scaleFactor.getValue()));
               if (overallRating < bbRating + impRating) {
                  overallRating = bbRating + impRating;
                  pickedShape = tail;
                  _worldPosition = pickedPoint->getPoint();
               }
            }

            //in jedem Fall wird's in die Liste aufgenommen
            allObjectsInRay->push_back(tail);
         }

         rayAttenuation = rayAttenuation * _mlPick->getTransparency(tail);
      }
      //}
      //else { break; }
   }
   neededForVec3f->unref();
   //eventCB->setHandled();
   return pickedShape;
}


SoSeparator* SoPickObject::addBB(const SbVec3f& corner1, const SbVec3f& corner2) {
   SoSeparator* temp = new SoSeparator();
   temp->ref();
   SoTranslation* trans = new SoTranslation();
   trans->translation = corner1 + ((corner2 - corner1) / 2.0);
   SoScale* scale = new SoScale();
   scale->scaleFactor = (corner2 - corner1) / 2.0;
   temp->addChild(trans);
   temp->addChild(scale);
   temp->addChild(new SoCube());

   temp->unrefNoDelete();
   return temp;
}