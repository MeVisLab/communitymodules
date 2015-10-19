// #############################
// ####
// ####  UMDSoVolEstimation.cpp
// ####
// #############################



#include "UMDSoVolEstimation.h"
#include "SoExtText2.h"



SO_NODE_SOURCE(UMDSoVolEstimation);


void UMDSoVolEstimation::initClass(){ 
  // muss zur Initialisierung der Klasse einmal
  // explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoVolEstimation, UMDSoMeasureTool, "UMDSoMeasureTool");
}


UMDSoVolEstimation::UMDSoVolEstimation() {
  SO_NODE_CONSTRUCTOR(UMDSoVolEstimation);
  initVolEstimation();
}


UMDSoVolEstimation::~UMDSoVolEstimation() {
  // ein bischen aufraeumen
  if (_volExternFlagSens) { delete _volExternFlagSens; _volExternFlagSens = NULL; }
  if (_externVolumeSens) { delete _externVolumeSens; _externVolumeSens = NULL; }
  
  if (_externRotSens) { delete _externRotSens; _externRotSens = NULL; }
  if (_externScaleSens) { delete _externScaleSens; _externScaleSens = NULL; }
  
  if (_volumeCalc) { _volumeCalc->unref(); _volumeCalc = NULL; }
}


void UMDSoVolEstimation::initVolEstimation() {
  // #### Die Felder initialisieren: #########
  
  SO_NODE_ADD_FIELD(volExternFlag, (FALSE));
  
  SO_NODE_ADD_FIELD(rotation, (1, 0, 0, 1.57f));
  SO_NODE_ADD_FIELD(scaleFactor, (5, 5, 5));
  // das Volumen des ellipsoiden laut Volumenformel
  SO_NODE_ADD_FIELD(absVolume, (0));
  // ein extern berechnetes Volumen (z.B. unter Berücksichtigung
  // eines Thresholds)
  SO_NODE_ADD_FIELD(externVolume, (0));
  
  SO_NODE_ADD_FIELD(unit, (ml));
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, cmm);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, ml);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, l);
  SO_NODE_SET_SF_ENUM_TYPE(unit, MeasureTool_Unit);
  
  SO_NODE_ADD_FIELD(rasterFunction, (Sphere));
  SO_NODE_DEFINE_ENUM_VALUE(SoVolEst_function, Sphere);
  SO_NODE_DEFINE_ENUM_VALUE(SoVolEst_function, Box);
  SO_NODE_DEFINE_ENUM_VALUE(SoVolEst_function, Cylinder);
  SO_NODE_SET_SF_ENUM_TYPE(rasterFunction, SoVolEst_function);

  // Die Feld-Sensoren initialisieren
  initVolEstimationFieldSensors();
  
  // ######## Die restlichen Instanz-Variablen: #########
  
  // falls der Dragger nur angeklickt wurde, soll er abgeschaltet werden.
  // dafür muß die Mausposition zwischengespeichert werden
  _pickedPoint = new SoSFVec3f;
  
  _volText = new SoExtText2;
  _volText->ref();
  unit.setValue("ml");
  
  _internChanged = false;

  createVolEstimation();
}

void UMDSoVolEstimation::initVolEstimationFieldSensors() {
  // Initialisierung der Feld-Sensoren und Zuweisen der 
  // entsprechenden Callback-Funktionen
  _volExternFlagSens = new SoFieldSensor(unitChangedCB, this);
  _volExternFlagSens->attach(&volExternFlag);
  _externVolumeSens = new SoFieldSensor(unitChangedCB, this);
  _externVolumeSens->attach(&externVolume);
  
  _rasterFunctionSens = new SoFieldSensor(rasterFunctionChangedCB, this);
  _rasterFunctionSens->attach(&rasterFunction);
  
  _externRotSens = new SoFieldSensor(rotationExternChangedCB, this);
  _externRotSens->attach(&rotation);
  _externScaleSens = new SoFieldSensor(scaleExternChangedCB, this);
  _externScaleSens->attach(&scaleFactor);
}


void UMDSoVolEstimation::createVolEstimation() {
  // Hier werden die einzelnen Knoten in den Graphen gehaengt.
  // Zuerst die Pfeilenden der Linie.
  createDragger();
  createEllipsoid();
  
  // Der Calculator zur Berechnung des Volumens
  // muß an dieser Stelle geschehen, damit _transformDragger initialisiert ist
  // V = 4/3 * pi * abc  --  a, b und c sind die Radien
  _volumeCalc = new SoCalculator; 
  _volumeCalc->ref();
  _volumeCalc->A.connectFrom(&_transformDragger->scaleFactor);
  _volumeCalc->a.connectFrom(&externVolume);
  _volumeCalc->b.connectFrom(_unitFactor);
  
  _volumeCalc->expression.set1Value(0, "oa = (4 / 3 * M_PI * A[0] * A[1] * A[2]) / b");
  
  absVolume.connectFrom(&_volumeCalc->oa);
  
  // Die Ausgabe des Abstandes
  createText();
}


void UMDSoVolEstimation::createDragger() {
  // der Dragger zum bearbeiten der Sphere
  _transformDragger = new SoTransformerDragger;
  _switchDragger->addChild(_transformDragger);
  
  _transformDragger->scaleFactor.setValue(scaleFactor.getValue());
  _transformDragger->translation.setValue(startPos.getValue());
  _transformDragger->addValueChangedCallback(valueChangedCB, this);
}


void UMDSoVolEstimation::createEllipsoid() {
  // die zu transformierende Sphere
  _sphereHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_sphereHighlight);
  _sphereHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _sphereHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  
  // Transform-Knoten zum anwenden der Draggermanipulationen
  _transform = new SoTransform;
  _sphereHighlight->addChild(_transform);
  
  _transform->translation.connectFrom(&_transformDragger->translation);
  _transform->scaleFactor.connectFrom(&_transformDragger->scaleFactor);
  _transform->rotation.connectFrom(&_transformDragger->rotation);
  
  // Detail und Drawstyle runterfahren, damit nicht soviel vom
  // Originalbild verdeckt wird
  SoComplexity* detail = new SoComplexity;
  _sphereHighlight->addChild(detail);
  detail->value.setValue(0.3f);
  
  SoDrawStyle* wireframe = new SoDrawStyle;
  _sphereHighlight->addChild(wireframe);
  wireframe->style.setValue(SoDrawStyle::LINES);
  
  _funcSwitch = new SoSwitch;
  //_sphereHighlight->addChild(new SoSphere);
  _sphereHighlight->addChild(_funcSwitch);
  _funcSwitch->whichChild.setValue(0);

  SoSeparator *platzhalter1 = new SoSeparator;
  SoSeparator *platzhalter2 = new SoSeparator;
  SoSeparator *cylSep = new SoSeparator;
  _funcSwitch->addChild(platzhalter1);
  _funcSwitch->addChild(platzhalter2);
  _funcSwitch->addChild(cylSep);

  platzhalter1->addChild(new SoSphere);
  platzhalter2->addChild(new SoCube);
  cylSep->addChild(new SoCylinder);
}



void UMDSoVolEstimation::createText() {
  // Rueckgabe eines Separators, der den Text fuer den Abstand
  // sowie die Einhalt (falls erwuenscht) enthaelt
  _textSep->addChild(_transform);
  
  // verschieben von Text-Cube in Ellipsoid
  _textTrafo->translation.connectFrom(&startPos);

  // verschiebt den Text bei Bewegung an die Ecke
  _textTrafo2 = new SoTransform;
  _textSep->addChild(_textTrafo2);
  _textTrafo2->translation.setValue(0, 0, 0);
  
  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, würde der Texthintergrund
  // nur noch als Rahmen dargestellt werden,
  // wenn man es nicht mit FILLED unterbinden würde
  SoDrawStyle* style = new SoDrawStyle;
  _textSep->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);
  
  _textSep->addChild(_volText);
  _volText->unref();
  _volText->justification.setValue(SoExtText2::CENTER);
  _volText->color.setValue(1, 1, 1);
  _volText->backgroundColor.connectFrom(&color);
  _volText->backgroundAlpha.setValue(0.3f);
  
  getTextString();
}


void UMDSoVolEstimation::getTextString() {
  // Setzt den darzustellenden Text
  _volumeCalc->b.connectFrom(_unitFactor);
  char* aString = new char[30];
  char* aUnit = new char[10];
  aString[0] = 0;
  if (toolNameFlag.getValue() == TRUE) {
    strcat(aString, toolName.getValue().getString());
    strcat(aString, ":\n");
  }
  float wert;
  if (volExternFlag.getValue() == TRUE)
    wert = externVolume.getValue() / _unitFactor->getValue();
  else
    wert = absVolume.getValue();
  
  sprintf(aUnit,"%.1f", wert);
  strcat(aString, aUnit);
  if (unitFlag.getValue() == TRUE) {
    SbString tempString;
    unit.get(tempString);
    strcat(aString, " ");
    strcat(aString, tempString.getString());
  }
  _volText->string.setValue(aString);
  
  delete []aUnit;
  delete []aString;
}


SbBool UMDSoVolEstimation::getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint) {
  // diese Funktion wird bei jedem Betaetigen der linken Maustaste aufgerufen;
  // pickedPoint -> mit Mauszeiger getroffener Punkt
  
  // Rueckgabewert ist TRUE, falls ein Kegel getroffen wurde, sonst
  // FALSE; dies dient vor allem dafuer, um das Mousebutton-Event
  // auf handled zu setzen oder nicht
  SbBool handled = FALSE;
  
  // beim ersten selektieren der Kugel kommt man in den Pick-Modus (rote Einfärbung)
  // und beim Zweiten in den Manipulations-Modus (blaue Einfärbung).
  // Die inkrementelle Tastatur-Bewegung ist jederzeit möglich, solange die Kugel aktiv ist
  if (pickedPoint != NULL) {
    SoPath* aPath = pickedPoint->getPath();
    aPath->ref();
    SoNode* tail = aPath->getTail();
    tail->ref();
    // Wird die Sphere getroffen, so wird der Switch-Knoten 
    // fuer die Dragger umgeschaltet
    if (tail->isOfType(SoSphere::getClassTypeId()) ||
      tail->isOfType(SoCube::getClassTypeId()) ||
      tail->isOfType(SoCylinder::getClassTypeId())) {
      // ist tail wirklich Teil DIESER Instanz?
      if(aPath->getNodeFromTail(2) == _funcSwitch){
        if (aPath->getNodeFromTail(5)->isOfType(SoSeparator::getClassTypeId())) {
          SoSeparator* wurzel = (SoSeparator*) aPath->getNodeFromTail(5);
          if (wurzel->getChild(3)->isOfType(SoSwitch::getClassTypeId())) {
            //SoSwitch* switchNode = (SoSwitch*) wurzel->getChild(3);
            SoSeparator* highlightGroup = (SoSeparator*) wurzel->getChild(4);
            // vorherigen Handler wiederherstellen
            if (_currentTool != this){
              resetHighLightNode(_currentTool);
              _currentTool = this;
            }
            switch (interactionMode.getValue()) {
            case directSelect: // umschalten auf Pick-Modus
              setInteractionMode(highlightGroup, directSelect, 0);
              _textTrafo2->translation.setValue(
                pow(_transform->scaleFactor.getValue()[0], 1 / 3) + 0.5f,
                pow(_transform->scaleFactor.getValue()[1], 1 / 3),
                pow(_transform->scaleFactor.getValue()[2], 1 / 3));
              break;              
            /*
            case dragMode:     // umschalten auf Drag-Modus
              setInteractionMode(highlightGroup, dragMode, 0);
              // Der Zustand des Switch-Knotens wird umgeschaltet
              switchNode->whichChild.setValue(0);
              // beim Umsetzen der Pfeile sind die Dragger-Positionen nicht mehr
              // aktuell, außerdem können von ILab noch Werte gespeichert sein,
              // die an dieser Stelle einmalig aktualisiert werden müssen
              _transformDragger->translation.setValue(startPos.getValue());
              break;
            */
            }
          }
        }
        handled = TRUE;
      }
    }
    else {
      // SoPickedPointAction
      if (_currentTool == this) {
        SbVec3f point = pickedPoint->getPoint();
        // wenn die Kartoffel für den Pick-Modus aktiviert wurde,
        // dann setze sie auf die gepickte Position
        if (((UMDSoVolEstimation*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoVolEstimation*)_currentTool)->getActivePart(((UMDSoVolEstimation*)_currentTool)->_highlightSep) == 0) {
          startPos.setValue(point);
          _transform->translation.setValue(point);
          handled = TRUE;
        }
      }
    }
    tail->unref();
    aPath->unref();
  }
  else {
    // alle Dragger und Modi verschwinden lassen
    resetHighLightNode(_currentTool);
    _currentTool = NULL;
    _textTrafo2->translation.setValue(0, 0, 0);
  }
  return handled;
}


SbBool UMDSoVolEstimation::setArrowByKeyboard(float x, float y, float z, Taste taste){
  // die Kartoffel wird per Tastatur bewegt.
  // ohne weitere Angabe von "Taste" wird sie verschoben
  // mit Zunahme von Shift wird sie rotiert
  // mit Zunahme von Ctrl wird sie skaliert
  SbVec3f richtungCtrl(
    z * keyboardIncrement.getValue(),
    y * keyboardIncrement.getValue(),
    x * keyboardIncrement.getValue());
  SbVec3f richtungNone(
    x * keyboardIncrement.getValue(),
    y * keyboardIncrement.getValue(),
    z * keyboardIncrement.getValue());
  
  SbRotation richtungShift(z, x, y, keyboardIncrement.getValue() / 10);
  SbRotation richtungVorher(((UMDSoVolEstimation*) _currentTool)->_transformDragger->rotation.getValue());
  
  // Dragger abschalten
  ((UMDSoVolEstimation*) _currentTool)->_switchDragger->whichChild.setValue(-1);
  if (_sphereHighlight->mode.getValue() == SoLocateHighlight::ON){
    switch(taste){
    case shift:
      ((UMDSoVolEstimation*) _currentTool)->_transformDragger->rotation.setValue(richtungShift * richtungVorher);
      break;
    case control:
      ((UMDSoVolEstimation*) _currentTool)->_transformDragger->scaleFactor.setValue(scaleFactor.getValue() + richtungCtrl);
      break;
    default:
      ((UMDSoVolEstimation*) _currentTool)->startPos.setValue(startPos.getValue() + richtungNone);
      ((UMDSoVolEstimation*) _currentTool)->_transformDragger->translation.setValue(startPos.getValue());
      break;
    }
    return TRUE;
  }
  return FALSE;
}


void UMDSoVolEstimation::pointChanged() {
  // Ein Punkt wurde geaendert
  // Funktionsaufruf so SoMeasureTool
  _internChanged = true;
  _transformDragger->translation.setValue(startPos.getValue());
}


// ############## Die Callback-Funktionen #################


void UMDSoVolEstimation::valueChangedCB(void* userData, SoDragger* dragger) {
  SoTransformerDragger* drag = (SoTransformerDragger*) dragger;
  UMDSoVolEstimation* volEst = (UMDSoVolEstimation*) userData;
  
  if (!volEst->areEqual(volEst->startPos.getValue(), drag->translation.getValue()))
    volEst->startPos.setValue(drag->translation.getValue());
  
  SbVec3f volEstAxisTemp;
  float volEstAngleTemp;
  volEst->rotation.getValue().getValue(volEstAxisTemp, volEstAngleTemp);
  SbVec3f draggerAxisTemp;
  float draggerAngleTemp;
  drag->rotation.getValue().getValue(draggerAxisTemp, draggerAngleTemp);
  
  if (!volEst->areEqual(volEst->scaleFactor.getValue(), drag->scaleFactor.getValue()) && volEst->_internChanged == false)
    volEst->scaleFactor.setValue(drag->scaleFactor.getValue());
  volEst->_internChanged = false;

  //if (volEst->scaleFactor.getValue().length() != 0)
  //cout << volEstAxisTemp.getValue()[0] << volEstAxisTemp.getValue()[1] << volEstAxisTemp.getValue()[2] << endl << flush;
  volEst->rotation.setValue(drag->rotation.getValue());
  //cout << volEstAxisTemp.getValue()[0] << volEstAxisTemp.getValue()[1] << volEstAxisTemp.getValue()[2] << endl << flush;

  if (volEst->unitFlag.getValue() == TRUE) {
    volEst->_textTrafo2->translation.setValue(
      pow(volEst->_transform->scaleFactor.getValue()[0], 1 / 3) + 0.5f,
      pow(volEst->_transform->scaleFactor.getValue()[1], 1 / 3),
      pow(volEst->_transform->scaleFactor.getValue()[2], 1 / 3));
  }
  volEst->getTextString();
}


void UMDSoVolEstimation::rotationExternChangedCB(void* userData, SoSensor*) {
  // es soll eine andere Einheit angezeigt werden
  UMDSoVolEstimation* volEst = (UMDSoVolEstimation*) userData;
  
  SbVec3f volEstAxisTemp;
  float volEstAngleTemp;
  volEst->rotation.getValue().getValue(volEstAxisTemp, volEstAngleTemp);
  SbVec3f draggerAxisTemp;
  float draggerAngleTemp;
  volEst->_transformDragger->rotation.getValue().getValue(draggerAxisTemp, draggerAngleTemp);
  //if (!volEst->areEqual(volEstAxisTemp, draggerAxisTemp) || (abs(volEstAngleTemp - draggerAngleTemp)) > 0.0001f)
  //cout << draggerAxisTemp.getValue()[0] << draggerAxisTemp.getValue()[1] << draggerAxisTemp.getValue()[2] << endl << flush;
  volEst->_transformDragger->rotation.setValue(volEst->rotation.getValue());
  //cout << draggerAxisTemp.getValue()[0] << draggerAxisTemp.getValue()[1] << draggerAxisTemp.getValue()[2] << endl << flush;
}


void UMDSoVolEstimation::scaleExternChangedCB(void* userData, SoSensor*) {
  // es soll eine andere Einheit angezeigt werden
  UMDSoVolEstimation* volEst = (UMDSoVolEstimation*) userData;

  if (!volEst->areEqual(volEst->_transformDragger->scaleFactor.getValue(), volEst->scaleFactor.getValue())) {
    volEst->_internChanged = true;
    volEst->_transformDragger->scaleFactor.setValue(volEst->scaleFactor.getValue());
  }

  if (volEst->_textTrafo2->translation.getValue().length() != 0) {
    volEst->_textTrafo2->translation.setValue(
      pow(volEst->_transform->scaleFactor.getValue()[0], 1 / 3) + 0.5f,
      pow(volEst->_transform->scaleFactor.getValue()[1], 1 / 3),
      pow(volEst->_transform->scaleFactor.getValue()[2], 1 / 3));
  }
}


void UMDSoVolEstimation::rasterFunctionChangedCB(void* userData, SoSensor*) {
  // es soll eine andere Einheit angezeigt werden
  UMDSoVolEstimation* volEst = (UMDSoVolEstimation*) userData;
  switch (volEst->rasterFunction.getValue()) {
  case Sphere:
    volEst->_funcSwitch->whichChild.setValue(0);
    break;
  case Box:
    volEst->_funcSwitch->whichChild.setValue(1);
    break;
  case Cylinder:
    volEst->_funcSwitch->whichChild.setValue(2);
  }
}
