// #######################
// ####
// ####  UMDSoRuler.cpp
// ####
// #######################




#include "UMDSoRuler.h"
#include "SoExtText2.h"


SO_NODE_SOURCE(UMDSoRuler);


void UMDSoRuler::initClass(){ 
  // muss zur Initialisierung der Klasse einmal
  // explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoRuler, UMDSoMeasureTool, "UMDSoMeasureTool");
}


UMDSoRuler::UMDSoRuler(){
  SO_NODE_CONSTRUCTOR(UMDSoRuler);
  initRuler();
}


UMDSoRuler::~UMDSoRuler(){
  // ein bischen aufraeumen
  if (_lineCalc) { _lineCalc->unref(); _lineCalc = NULL; }
  
  if (_rotLine) { _rotLine->unref(); _rotLine = NULL; }
  if (_oneStrokeSep) { _oneStrokeSep->unref(); _oneStrokeSep = NULL; }
  if (_oneSmallStrokeSep) { _oneSmallStrokeSep->unref(); _oneSmallStrokeSep = NULL; }
  if (_reglerAktiv) { _reglerAktiv->unref(); _reglerAktiv = NULL; }
  if (_reglerInaktiv) { _reglerInaktiv->unref(); _reglerInaktiv = NULL; }
  if (_rotDragger) { _rotDragger->unref(); _rotDragger = NULL; }
  
  if (_endPosSens) { delete _endPosSens; _endPosSens = NULL; }
  
  if (_minDistanceSens) { delete _minDistanceSens; _minDistanceSens = NULL; }
}


void UMDSoRuler::initRuler(){
  // #### Die Felder initialisieren: #########
  
  SO_NODE_ADD_FIELD(endPos, (0, 0, 0));
  
  // Abstand zwischen den Strokes
  SO_NODE_ADD_FIELD(minDistance, (25.0));
  
  // L�ge des Lineals (nur zur Ausgabe)
  SO_NODE_ADD_FIELD(lineLength, (0));
  
  SO_NODE_ADD_FIELD(unit,(mm));
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, mm);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, cm);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, m);
  SO_NODE_SET_SF_ENUM_TYPE(unit, MeasureTool_Unit);
  
  // Die Feld-Sensoren initialisieren
  initRulerFieldSensors();
  
  _lineCalc = new SoCalculator; 
  _lineCalc->ref();
  _lineCalc->A.connectFrom(&startPos);
  _lineCalc->B.connectFrom(&endPos);
  _lineCalc->a.connectFrom(_unitFactor);
  _lineCalc->expression.set1Value(0, "oA = A - B");
  _lineCalc->expression.set1Value(1, "oa = length(oA)");
  _lineCalc->expression.set1Value(2, "oB = vec3f(0, -oa / 2, 0)");
  _lineCalc->expression.set1Value(3, "ob = length(A - B) / a");
  
  lineLength.connectFrom(&_lineCalc->ob);
  
  // rotieren der Verbindungslinie zwischen die beiden Endpunkte
  _rotLine = new SoComposeRotationFromTo;
  _rotLine->ref();
  _rotLine->from.setValue(0, 1, 0);
  _rotLine->to.connectFrom(&_lineCalc->oA);
  
  // ######## Die restlichen Instanz-Variablen: #########
  
  unit.setValue("mm");
  
  // Position des Linealnullpunktes
  _strokeCenterPos = 0;
  
  createRuler();

  _changedByDragger = FALSE;
}


void UMDSoRuler::initRulerFieldSensors() {  
  // Initialisierung der Feld-Sensoren und Zuweisen der 
  // entsprechenden Callback-Funktionen
  _endPosSens = new SoFieldSensor(endChangedCB, this);
  _endPosSens->attach(&endPos);
  
  _minDistanceSens = new SoFieldSensor(minDistanceChangedCB, this);
  _minDistanceSens->attach(&minDistance);
}


SoSeparator* UMDSoRuler::createDragMoveScaleSep(){
  // der Separator (dragCompleteSep) enthaelt neben dem 
  // Dragger einen Transformations-Knoten, der den Dragger 
  // immer in der Mitte des Lineals positioniert und den 
  // Dragger skaliert
  SoSeparator* dragMoveScaleSep = new SoSeparator;
  dragMoveScaleSep->ref();
  
  SoTransform* dragMoveScaleTrafo = new SoTransform;
  dragMoveScaleSep->addChild(dragMoveScaleTrafo);
  dragMoveScaleTrafo->translation.connectFrom(&startPos);
  
  _rotDragger = new SoComposeRotationFromTo;
  _rotDragger->ref();
  _rotDragger->from.setValue(1, 0, 0);
  _rotDragger->to.connectFrom(&_lineCalc->oA);
  dragMoveScaleTrafo->rotation.connectFrom(&_rotDragger->rotation);
  
  _reglerAktiv = new SoSeparator;
  _reglerAktiv->ref();
  _reglerInaktiv = new SoSeparator;
  _reglerInaktiv->ref();
  
  // ohne diese Drehung wrde der Dragger querstehen
  SoRotation* drehung = new SoRotation;
  _reglerAktiv->addChild(drehung);
  _reglerInaktiv->addChild(drehung);
  drehung->set("rotation 0 0 1  1.57");
  
  // Materials for the dragger in regular and active states
  SoMaterial* myMtl = new SoMaterial;
  _reglerInaktiv->addChild(myMtl);
  SoMaterial* myActiveMtl = new SoMaterial;
  _reglerAktiv->addChild(myActiveMtl);
  
  myMtl->diffuseColor.setValue(1, 1, 1);
  myActiveMtl->diffuseColor.setValue(1, 1, 0);
  SoCylinder* regler = new SoCylinder;
  _reglerAktiv->addChild(regler);
  _reglerInaktiv->addChild(regler);
  regler->radius = 1.5f;
  regler->height = 0.4f;
  
  // Now, customize the draggers with the pieces we created.
  _dragMoveScale = new SoTranslate1Dragger;
  dragMoveScaleSep->addChild(_dragMoveScale);
  _dragMoveScale->setPart("translator", _reglerInaktiv);
  _dragMoveScale->setPart("translatorActive", _reglerAktiv);
  
  // Callback-Funktionsaufruf fuer den Dragger
  _dragMoveScale->addValueChangedCallback(draggerMoveScaleCB, this);
  
  dragMoveScaleSep->unrefNoDelete();
  return dragMoveScaleSep;
}


void UMDSoRuler::createRuler() {      
  // Hier werden die einzelnen Knoten in den 
  // Graphen gehaengt
  
  SoSeparator* dragComp = createDragMoveScaleSep();
  _switchDragger->addChild(dragComp);
  
  // Highlightknoten fuer die Kugeln, bei deren Betaetigung
  // die Dragger zur Manipulation des Lineals erscheinen
  _sphereStartHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_sphereStartHighlight);
  _sphereStartHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _sphereStartHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  _sphereTrafoStart = new SoTransform;
  _sphereStartHighlight->addChild(_sphereTrafoStart);
  _sphereTrafoStart->translation.connectFrom(&startPos);
  SoSphere* startSphere = new SoSphere;
  _sphereStartHighlight->addChild(startSphere);
  startSphere->setName("startSphere");
  
  _sphereEndHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_sphereEndHighlight);
  _sphereEndHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _sphereEndHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  _sphereTrafoEnd = new SoTransform;
  _sphereEndHighlight->addChild(_sphereTrafoEnd);
  _sphereTrafoEnd->translation.connectFrom(&endPos);
  SoSphere* endSphere = new SoSphere;
  _sphereEndHighlight->addChild(endSphere);
  endSphere->setName("endSphere");
  
  _lineHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_lineHighlight);
  _lineHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _lineHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  _lineHighlight->addChild(_sphereTrafoStart);
  
  // Berechnung der Rotation des Zylinders
  SoRotation* drehLinie = new SoRotation;
  _lineHighlight->addChild(drehLinie);
  drehLinie->rotation.connectFrom(&_rotLine->rotation);
  
  SoTransform* versatz = new SoTransform;
  _lineHighlight->addChild(versatz);
  versatz->translation.connectFrom(&_lineCalc->oB);
  SoCylinder* linie = new SoCylinder;
  _lineHighlight->addChild(linie);
  linie->setName("linie");
  linie->height.connectFrom(&_lineCalc->oa);
  linie->radius = 0.2f;
  
  //den Separator fr die Skalenstriche einfgen
  SoTransform* trafoMarkierung = new SoTransform;
  _root->addChild(trafoMarkierung);
  trafoMarkierung->translation.connectFrom(&startPos);
  
  SoRotation* drehMarkierung = new SoRotation;
  _root->addChild(drehMarkierung);
  drehMarkierung->rotation.connectFrom(&_rotLine->rotation);
  _strokesSep = new SoSeparator;
  _root->addChild(_strokesSep);
  
  _oneStrokeSep = new SoSeparator;
  _oneStrokeSep->ref();
  SoCylinder* stroke = new SoCylinder;
  _oneStrokeSep->addChild(stroke);
  stroke->height = 0.2f;
  stroke->radius = 2;
  
  _oneSmallStrokeSep = new SoSeparator;
  _oneSmallStrokeSep->ref();
  SoCylinder* smallStroke = new SoCylinder;
  _oneSmallStrokeSep->addChild(smallStroke);
  smallStroke->height = 0.1f;
  smallStroke->radius = 1;
  
  createLevelOfStrokes();
  getStrokes();
  _endDraggerDragged = FALSE;
  createUnitGroup();
}


void UMDSoRuler::createUnitGroup() {
  // Separator zurueckgegeben,
  // der einen Transformations-Knoten sowie
  // einen Text-Knoten (die Mass-Einheit) enthaelt
  _unitSwitch = new SoSwitch;
  _textSep->addChild(_unitSwitch);
  
  SoSeparator* aSep = new SoSeparator;
  _unitSwitch->addChild(aSep);
  
  aSep->addChild(_textTrafo);
  getUnitPos();  
  
  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, wrde der Texthintergrund
  // nur noch als Rahmen dargestellt werden,
  // wenn man es nicht mit FILLED unterbinden wrde
  SoDrawStyle* style = new SoDrawStyle;
  aSep->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);
  
  _unitText = new SoExtText2;
  aSep->addChild(_unitText);
  _unitText->justification.setValue(SoExtText2::RIGHT);
  _unitText->color.setValue(1, 1, 1);
  _unitText->backgroundColor.connectFrom(&color);
  _unitText->backgroundAlpha.setValue(0.3f);
  _unitText->string = unit.getValue();
}


void UMDSoRuler::getUnitPos() {
  // Bestimmen der Position des Textes fuer die Mass-Einheit
  
  // neuer normierter Richtungsvektor des Lineals
  SbVec3f direction = startPos.getValue() - endPos.getValue();
  direction.normalize();
  if (_endDraggerDragged) {
    _textTrafo->translation.setValue(SbVec3f(startPos.getValue() + 2.5f * direction));
    _endDraggerDragged = FALSE;
  } 
  else
    _textTrafo->translation.setValue(SbVec3f(endPos.getValue() - 2.5f * direction));
}


void UMDSoRuler::createLevelOfStrokes() {
  // Position des ersten Strokes
  _firstStroke = new SoTranslation;
  _strokesSep->addChild(_firstStroke);
  
  // wenn das Lineal weit von Kamera entfernt ist, berlappen sich
  // die einzelnen Bemassungen, dem soll hier abgeholfen werden
  SoLevelOfDetail* levelOfStrokes = new SoLevelOfDetail;
  _strokesSep->addChild(levelOfStrokes);
  
  float areas[5] = {500000, 250000, 100000, 50000, 1000};
  levelOfStrokes->screenArea.setValues(0, 5, areas);
  
  _strokeGroup0 = new SoSeparator; // maximal 32 Strokes anzeigen
  levelOfStrokes->addChild(_strokeGroup0);
  _strokeGroup1 = new SoSeparator; // 16 Strokes anzeigen
  levelOfStrokes->addChild(_strokeGroup1);
  _strokeGroup2 = new SoSeparator; // 8 Strokes anzeigen
  levelOfStrokes->addChild(_strokeGroup2);
  _strokeGroup3 = new SoSeparator; // 4 Strokes anzeigen
  levelOfStrokes->addChild(_strokeGroup3);
  _strokeGroup4 = new SoSeparator; // 2 Strokes anzeigen
  levelOfStrokes->addChild(_strokeGroup4);
  _strokeGroup5 = new SoSeparator; // 1 Stroke anzeigen
  levelOfStrokes->addChild(_strokeGroup5);
}


void UMDSoRuler::getStrokes() {
  // hier werden die Anzahl der Striche sowie 
  // die Positionen der einzelnen Striche den _strokes zugewiesen
  if (minDistance.getValue() != 0) {
    _strokeGroup0->removeAllChildren();
    _strokeGroup1->removeAllChildren();
    _strokeGroup2->removeAllChildren();
    _strokeGroup3->removeAllChildren();
    _strokeGroup4->removeAllChildren();
    _strokeGroup5->removeAllChildren();
    // die Distanz zwischen den Strokes
    SbVec3f direction = startPos.getValue() - endPos.getValue();
    direction.normalize();
    float distance = SbVec3f(direction * minDistance.getValue()).length();
    
    // Anzahl der Strokes kleiner Null
    int numStrokesDown = (int)(-_strokeCenterPos / distance);
    
    // Anzahl der Strokes gr�er Null
    int numStrokesUp = (int)(
      (SbVec3f(startPos.getValue() - endPos.getValue()).length()
      + _strokeCenterPos) / distance);
    
    // Strokes insgesamt
    int numStrokes = numStrokesDown + numStrokesUp + 1;
    
    _firstStroke->translation.setValue(0, _strokeCenterPos + numStrokesDown * distance, 0);
    
    SoTranslation* distanceTrafo = new SoTranslation;
    distanceTrafo->ref();
    distanceTrafo->translation.setValue(0, -distance, 0);
    
    int temp;
    for (int counter = -numStrokesDown; counter <= numStrokesUp; counter++){
      // maximal 32 Strokes in Gruppe 0 einfgen 
      temp = numStrokes / 32 + 1;
      if (temp == 0 || (counter % temp) == 0) {
        _strokeGroup0->addChild(_oneStrokeSep);
        _strokeGroup0->addChild(getDigit(counter * minDistance.getValue()));
      }
      
      // einfgen von 16 Strokes in Gruppe 1
      temp = (int)(numStrokes / 16) + 1;
      if (temp == 0 || (counter % temp) == 0) {
        _strokeGroup1->addChild(_oneStrokeSep);
        _strokeGroup1->addChild(getDigit(counter * minDistance.getValue()));
      }
      
      // einfgen von 8 Strokes in Gruppe 2
      temp = (int)(numStrokes / 8) + 1;
      if (temp == 0 || (counter % temp) == 0) {
        _strokeGroup2->addChild(_oneStrokeSep);
        _strokeGroup2->addChild(getDigit(counter * minDistance.getValue()));
      }
      
      // einfgen von 4 Strokes in Gruppe 3
      temp = (int)(numStrokes / 4) + 1;
      if (temp == 0 || (counter % temp) == 0) {
        _strokeGroup3->addChild(_oneStrokeSep);
        _strokeGroup3->addChild(getDigit(counter * minDistance.getValue()));
      }
      
      // einfgen von 2 Strokes in Gruppe 4
      if (counter == -numStrokesDown || counter == numStrokesUp) {
        _strokeGroup4->addChild(_oneStrokeSep);
        _strokeGroup4->addChild(getDigit(counter * minDistance.getValue()));
      }
      
      // einfgen von 1 Strokes in Gruppe 5
      if (counter == 0) {
        _strokeGroup5->addChild(_oneStrokeSep);
        _strokeGroup5->addChild(getDigit(counter * minDistance.getValue()));
      }
      // in allen Gruppen den Versatz und kleinen Stroke einfgen
      _strokeGroup0->addChild(_oneSmallStrokeSep);
      _strokeGroup0->addChild(distanceTrafo);
      _strokeGroup1->addChild(_oneSmallStrokeSep);
      _strokeGroup1->addChild(distanceTrafo);
      _strokeGroup2->addChild(_oneSmallStrokeSep);
      _strokeGroup2->addChild(distanceTrafo);
      _strokeGroup3->addChild(_oneSmallStrokeSep);
      _strokeGroup3->addChild(distanceTrafo);
      _strokeGroup4->addChild(_oneSmallStrokeSep);
      _strokeGroup4->addChild(distanceTrafo);
      _strokeGroup5->addChild(_oneSmallStrokeSep);
      _strokeGroup5->addChild(distanceTrafo);
    }
    distanceTrafo->unref();
  }
}


SoAnnotation* UMDSoRuler::getDigit(float digit){
  // liefert einen Knoten zurueck, der die entsprechende
  // Ziffer des Lineals beinhaltet
  SoAnnotation* aSep = new SoAnnotation;
  aSep->ref();
  
  SoTransform* versatz = new SoTransform;
  aSep->addChild(versatz);
  versatz->translation.setValue(3, 0, 0);
  
  char* aString = new char[10];
  sprintf(aString, "%.1f", digit / _unitFactor->getValue());
  
  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, wrde der Texthintergrund
  // nur noch als Rahmen dargestellt werden,
  // wenn man es nicht mit FILLED unterbinden wrde
  SoDrawStyle* style = new SoDrawStyle;
  aSep->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);
  
  SoExtText2* aText2 = new SoExtText2;
  aSep->addChild(aText2);
  aText2->justification.setValue(SoExtText2::CENTER);
  aText2->color.setValue(1, 1, 1);
  aText2->backgroundColor.connectFrom(&color);
  aText2->backgroundAlpha.setValue(0.3f);
  aText2->string.setValue(aString);
  
  delete[] aString;
  
  aSep->unrefNoDelete();
  return aSep; 
}


void UMDSoRuler::getTextString() {
  _lineCalc->a.connectFrom(_unitFactor);
  if (unitFlag.getValue() || toolNameFlag.getValue())
    _unitSwitch->whichChild.setValue(0);
  else 
    _unitSwitch->whichChild.setValue(-1);
  
  char* aString = new char[30];
  aString[0] = 0;
  if (toolNameFlag.getValue() == TRUE) {
    strcat(aString, toolName.getValue().getString());
    strcat(aString, ": ");
  }
  if (unitFlag.getValue() == TRUE) {
    SbString tempString;
    unit.get(tempString);
    strcat(aString, tempString.getString());
  }
  _unitText->string.setValue(aString);
}


SbBool UMDSoRuler::getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint){
  // diese Funktion wird bei jedem Betaetigen der linken
  // Maustaste aufgerufen;
  // pickedPoint -> mit Mauszeiger getroffener Punkt
  SbBool handled = FALSE;
  
  // Rueckgabewert ist TRUE, falls ein Kegel, ein Zylinder oder eine Kugel getroffen 
  // wurde, sonst FALSE; dies dient vor allem dafuer, um das 
  // Mousebutton-Event auf handled zu setzen oder nicht
  if (pickedPoint != NULL){
    SoPath* aPath = pickedPoint->getPath();
    aPath->ref();
    SoNode* tail = aPath->getTail();
    tail->ref();
    // Wird eine Sphere getroffen, so wird der Switch-Knoten 
    // fuer die Dragger umgeschaltet
    if (tail->isOfType(SoSphere::getClassTypeId()) || tail->isOfType(SoCylinder::getClassTypeId())) { 
      // ist tail wirklich Teil DIESER Instanz?
      if(aPath->getNodeFromTail(1) == _lineHighlight
        || aPath->getNodeFromTail(1) == _sphereStartHighlight
        || aPath->getNodeFromTail(1) == _sphereEndHighlight){
        // nur um sicherzugehen: ist getNodeFromTail(5) wirklich der _root-Knoten
        if (aPath->getNodeFromTail(3)->isOfType(SoSeparator::getClassTypeId())) {
          SoSeparator* aGroup = (SoSeparator*) aPath->getNodeFromTail(3);
          // nur um sicherzugehen: haben wir hier wirklich einen Switch-Knoten
          if (aGroup->getChild(3)->isOfType(SoSwitch::getClassTypeId())) {
            SoSwitch* switchNode = (SoSwitch*) aGroup->getChild(3);
            SoSeparator* highlightGroup = (SoSeparator*) aGroup->getChild(4);
            short whichDragger = -1;
            if (tail->getName() == "startSphere"){
              whichDragger = 0;
              setInteractionMode(highlightGroup, inactive, 1);
              setInteractionMode(highlightGroup, inactive, 2);
            }
            if (tail->getName() == "endSphere"){
              whichDragger = 1;
              setInteractionMode(highlightGroup, inactive, 0);
              setInteractionMode(highlightGroup, inactive, 2);
            }
            if (tail->getName() == "linie"){
              whichDragger = 2;
              setInteractionMode(highlightGroup, inactive, 0);
              setInteractionMode(highlightGroup, inactive, 1);
            }
            // vorherigen Handler wiederherstellen
            if (_currentTool != this){
              resetHighLightNode(_currentTool);
              _currentTool = this;
            }
            // Umschaltung zwischen DirectSelect und Draggerbewegung
            switch (interactionMode.getValue()){
            case directSelect: // umschalten auf Pick-Modus
              if (whichDragger != 2) {
                setInteractionMode(highlightGroup, directSelect, whichDragger);
                switchNode->whichChild.setValue(-1);
              }
              // Sonderbehandlung fr den _moveScaleDragger
              // DirectSelect macht hier einfach keinen Sinn
              else {
                switchNode->whichChild.setValue(2);
                
                _dragMoveScale->translation.setValue(_strokeCenterPos, 0, 0);
              }
              break;              
            }
            handled = TRUE; 
          }
        }
      }
    }
    else {
      // SoPickedPointAction
      if (_currentTool == this){
        SbVec3f point = pickedPoint->getPoint();
        // wenn einer der Endpunkte fr Pick-Modus aktiviert wurde,
        // dann setze den Pfeil auf die gepickte Position
        // _moveScaleDragger wird nicht umgesetzt!
        if (((UMDSoRuler*)_currentTool)->interactionMode.getValue() == directSelect &&
          ((UMDSoRuler*)_currentTool)->getActivePart(((UMDSoRuler*)_currentTool)->_highlightSep) == 0) {
          startPos.setValue(point);
        }
        if (((UMDSoRuler*)_currentTool)->interactionMode.getValue() == directSelect &&
          ((UMDSoRuler*)_currentTool)->getActivePart(((UMDSoRuler*)_currentTool)->_highlightSep) == 1) {
          endPos.setValue(point);
        }
      }
      tail->unref();
      aPath->unref();
    }
  }
  else {
    // alle Dragger und Modi verschwinden lassen
    resetHighLightNode(_currentTool);
    _currentTool = NULL;
  }
  
  return handled;
}


SbBool UMDSoRuler::setArrowByKeyboard(float x, float y, float z, Taste /*taste*/){
  // ein Pfeil wurde per Tastatur bewegt
  SbVec3f richtung;
  richtung.setValue(x * keyboardIncrement.getValue(), y * keyboardIncrement.getValue(), z * keyboardIncrement.getValue());
  // Dragger abschalten
  ((UMDSoRuler*)_currentTool)->_switchDragger->whichChild.setValue(-1);
  // den aktiven Knoten suchen und verschieben
  if (_sphereStartHighlight->mode.getValue() == SoLocateHighlight::ON){
    ((UMDSoRuler*)_currentTool)->startPos.setValue(startPos.getValue() + richtung);
    return TRUE;
  }
  if (_sphereEndHighlight->mode.getValue() == SoLocateHighlight::ON){
    ((UMDSoRuler*)_currentTool)->endPos.setValue(endPos.getValue() + richtung);
    return TRUE;
  }
  if (_lineHighlight->mode.getValue() == SoLocateHighlight::ON){
    ((UMDSoRuler*)_currentTool)->_strokeCenterPos += x;
    ((UMDSoRuler*)_currentTool)->_dragMoveScale->translation.setValue(((UMDSoRuler*)_currentTool)->_strokeCenterPos, 0, 0);
    return TRUE;
  }
  return FALSE;
}


void UMDSoRuler::pointChanged(){
  // Ein Punkt wurde geaendert
  // Funktionsaufruf so SoMeasureTool
  
  // Aktualisierungen veranlassen
  float laenge = SbVec3f(startPos.getValue() - endPos.getValue()).length();
  if (_strokeCenterPos < -laenge)
    _strokeCenterPos = -laenge;
  // neue Positionen
  getStrokes();
  // die Position des Unit-Textes refreshen   
  getUnitPos();
  // wenn ein Punkt nicht ber seinen Dragger ge�dert wurde (sondern z.B.
  // ber den ILAB-Panel), dann mu�der Dragger hinterhergezogen werden
  if (!_changedByDragger) {
    _dragMoveScale->translation.setValue(_strokeCenterPos, 0, 0);
  }
  _changedByDragger = FALSE;
}


// ########### Callback-Methoden #######################

void UMDSoRuler::draggerMoveScaleCB(void* userData, SoDragger* dragger){ 
  // wenn der Dragger, der fuer die Bewegung der Strokes
  // verantwortlich ist, bewegt wird, wirkt sich das aus
  SoTranslate1Dragger* drag = (SoTranslate1Dragger*) dragger;
  UMDSoRuler* obj = (UMDSoRuler*) userData;
  // darauf achten, das der Dragger nicht nach au�rhalb
  // des Lineals verschoben wird
  if (drag->translation.getValue()[0] > 0)
    drag->translation.setValue(0, 0, 0);
  float laenge = SbVec3f(obj->startPos.getValue() - obj->endPos.getValue()).length();
  if (drag->translation.getValue()[0] < -laenge)
    drag->translation.setValue(-laenge, 0, 0);
  obj->_strokeCenterPos = drag->translation.getValue()[0];
  obj->_changedByDragger = TRUE;
  obj->pointChanged();
}


void UMDSoRuler::endChangedCB(void* userData, SoSensor*) {
  // Der Endpunkt wurde geaendert
  UMDSoRuler* obj = (UMDSoRuler*) userData;
  // abgeleitete Klasse
  obj->_endDraggerDragged = TRUE;
  obj->pointChanged();
}


void UMDSoRuler::minDistanceChangedCB(void* userData, SoSensor*){
  // Der Abstand zwischen den einzelnen Skalenstrichen wurde 
  // veraendert
  UMDSoRuler* obj = (UMDSoRuler*) userData;  
  obj->pointChanged();
}

