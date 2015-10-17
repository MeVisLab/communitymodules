// #############################
// ####
// ####  SoShapeInfo.cpp
// ####
// #############################


#include "SoShapeInfo.h"

#include <Inventor/nodes/SoShape.h>
#include <Inventor/sensors/SoFieldSensor.h>



SO_NODE_SOURCE(SoShapeInfo);

void SoShapeInfo::initClass() { 
   // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
   SO_NODE_INIT_CLASS(SoShapeInfo, SoSeparator, "Separator");
}


SoShapeInfo::SoShapeInfo() {
   SO_NODE_CONSTRUCTOR(SoShapeInfo);
   
   SO_NODE_ADD_FIELD(rootInput, (0));
   SO_NODE_ADD_FIELD(numPoints, (0));
   SO_NODE_ADD_FIELD(numLines, (0));
   SO_NODE_ADD_FIELD(numTriangles, (0));
   SO_NODE_ADD_FIELD(numNodes, (0));

   _numPoints = _numLines = _numTriangles = _numNodes = 0;
   
   // Callback zum einsammeln der Koordinaten
   _myAction = new SoCallbackAction;
   _myAction->addPreCallback        (SoShape::getClassTypeId(), preCB,      this);
   _myAction->addPointCallback      (SoShape::getClassTypeId(), pointCB,    this);
   _myAction->addLineSegmentCallback(SoShape::getClassTypeId(), lineCB,     this);
   _myAction->addTriangleCallback   (SoShape::getClassTypeId(), triangleCB, this);
   
   _inputSens = new SoFieldSensor(inputCB, this);
   _inputSens->attach(&rootInput);
   
}


SoShapeInfo::~SoShapeInfo() {
   // ein bischen aufraeumen
   if (_inputSens)     { delete   _inputSens;     _inputSens     = NULL; }
   if (_myAction)      { delete   _myAction;      _myAction      = NULL; }
}


void SoShapeInfo::inputCB(void* userData, SoSensor*) {
   SoShapeInfo* obj = (SoShapeInfo*) userData;
   obj->calculate();
}


void SoShapeInfo::calculate() {
   // Eingang hat sich geändert
   if (rootInput.getValue() != NULL) {
      // Dreiecke aufsammeln
      _numPoints = _numLines = _numTriangles = 0;
      _myAction->apply(rootInput.getValue());
      numPoints.setValue(_numPoints);
      numLines.setValue(_numLines);
      numTriangles.setValue(_numTriangles);
      numNodes.setValue(_numNodes);
   }
}


SoCallbackAction::Response SoShapeInfo::preCB(void* userData, 
                                              SoCallbackAction* action, 
                                              const SoNode* node) {
   SoShapeInfo* obj = (SoShapeInfo*) userData;
   obj->_numNodes++;
   return SoCallbackAction::CONTINUE;
}


void SoShapeInfo::pointCB(void* userData, 
                          SoCallbackAction*,
                          const SoPrimitiveVertex* ) {
   SoShapeInfo* obj = (SoShapeInfo*) userData;
   obj->_numPoints++;
}


void SoShapeInfo::lineCB(void* userData, 
                         SoCallbackAction*,
                         const SoPrimitiveVertex*,
                         const SoPrimitiveVertex*) {
   SoShapeInfo* obj = (SoShapeInfo*) userData;
   obj->_numLines++;
}


void SoShapeInfo::triangleCB(void* userData, 
                             SoCallbackAction*,
                             const SoPrimitiveVertex*,
                             const SoPrimitiveVertex*,
                             const SoPrimitiveVertex*) {
   SoShapeInfo* obj = (SoShapeInfo*) userData;
   obj->_numTriangles++;
}
