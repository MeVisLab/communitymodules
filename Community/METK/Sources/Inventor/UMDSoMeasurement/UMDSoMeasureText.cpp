// #############################
// ####
// ####  UMDSoMeasureText.cpp
// ####
// #############################


#include "UMDSoMeasureText.h"
#include <Inventor/nodes/SoAnnotation.h>
#include "SoExtText2.h"


SO_NODE_SOURCE(UMDSoMeasureText);


void UMDSoMeasureText::initClass(){ 
  // muss zur Initialisierung der Klasse einmal
  // explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoMeasureText, SoSeparator, "SoSeparator");
}


UMDSoMeasureText::UMDSoMeasureText() {
  SO_NODE_CONSTRUCTOR(UMDSoMeasureText);
  
  SO_NODE_ADD_FIELD(isCoupled, (TRUE));
  SO_NODE_ADD_FIELD(string, (""));
  SO_NODE_ADD_FIELD(color, (1, 1, 1));
  
  _lastClick = 0;
  
  _isCoupledSensor = new SoFieldSensor(isCoupledCB, this);
  _isCoupledSensor->attach(&isCoupled);

  // checks for double clicks
  SoEventCallback* eventCB = new SoEventCallback;
  addChild(eventCB);
  eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressedCB, this);
  
  // switches between 2D and 3D translation mode
  _switch2D3D = new SoSwitch;
  addChild(_switch2D3D);
  _switch2D3D->whichChild.setValue(0);
  
  // 3D
  SoSeparator *sep3D = new SoSeparator;
  _switch2D3D->addChild(sep3D);
  
  // 2D
  SoAnnotation *sep2D = new SoAnnotation;
  _switch2D3D->addChild(sep2D);
  
  SoTranslate2Dragger *dragger = new SoTranslate2Dragger;
  sep2D->addChild(dragger);
  sep3D->addChild(dragger);
  // the Text
  SoExtText2 *extText = new SoExtText2();
  sep2D->addChild(extText);
  sep3D->addChild(extText);
  
  extText->justification.setValue(SoExtText2::CENTER);
  extText->color.setValue(1, 1, 1);
  extText->backgroundColor.connectFrom(&color);
  extText->backgroundAlpha.setValue(0.3f);
  extText->string.connectFrom(&string);

  SoDrawStyle *drawStyle = new SoDrawStyle;
  sep2D->addChild(drawStyle);
  sep3D->addChild(drawStyle);
  drawStyle->style.setValue(SoDrawStyle::POINTS);

  SoSphere *pickSphere = new SoSphere;
  sep2D->addChild(pickSphere);
  sep3D->addChild(pickSphere);
  pickSphere->setName("MeasureTextSpezial");
}


UMDSoMeasureText::~UMDSoMeasureText() {
  if(_isCoupledSensor) { delete _isCoupledSensor; _isCoupledSensor = 0; }
}


void UMDSoMeasureText::mousePressedCB(void* /*userData*/, SoEventCallback* eventCB) {
  const SoEvent* event = eventCB->getEvent();
  
  // left mouse button
  if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {
    const SoPickedPoint* pickedPoint = eventCB->getPickedPoint();
    if (pickedPoint != NULL){
      SoPath* aPath = pickedPoint->getPath();
      aPath->ref();
      SoNode* tail = aPath->getTail();
      tail->ref();
      
      // is an instance of UMDSoMeasureText
      if (tail->isOfType(SoSphere::getClassTypeId()) && (tail->getName() == "MeasureTextSpezial")) {
        UMDSoMeasureText* obj = (UMDSoMeasureText*) aPath->getNodeFromTail(3);
        
        // checks for a double click (two clicks in between 200 ms)
        if ((event->getTime().getMsecValue() - obj->_lastClick) < 300) {
          obj->isCoupled.setValue(!obj->isCoupled.getValue());
          obj->_lastClick = 0;
          eventCB->setHandled();
        }
        else {
          obj->_lastClick = event->getTime().getMsecValue();
        }
      }
      tail->unref();
      aPath->unref();
    }
  }
}
  

void UMDSoMeasureText::isCoupledCB(void* userData, SoSensor*) {
  UMDSoMeasureText* obj = (UMDSoMeasureText*) userData;
  // abgeleitete Klasse
  obj->toggleTranslationMode();
}


void UMDSoMeasureText::toggleTranslationMode() {
  if (isCoupled.getValue() == TRUE) {
    // 3d
    _switch2D3D->whichChild.setValue(0);
  }
  else {
    // 2D
    _switch2D3D->whichChild.setValue(1);
  }
}

