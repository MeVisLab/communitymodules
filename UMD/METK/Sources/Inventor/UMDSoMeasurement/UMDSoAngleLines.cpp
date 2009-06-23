// #############################
// ####
// ####  UMDSoAngleLines.cpp
// ####
// #############################


#include "UMDSoAngleLines.h"
#include "UMDSoArrow.h"
#include "UMDSoCake.h"
#include "SoExtText2.h"


SO_NODE_SOURCE(UMDSoAngleLines);


void UMDSoAngleLines::initClass(){ 
  // muss zur Initialisierung der Klasse einmal
  // explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoAngleLines, UMDSoMeasureTool, "UMDSoMeasureTool");
}


UMDSoAngleLines::UMDSoAngleLines(){
  SO_NODE_CONSTRUCTOR(UMDSoAngleLines);
  initAngleLines();
}


UMDSoAngleLines::~UMDSoAngleLines() {
  // ein bischen aufraeumen
  if (_arrowCalc) { _arrowCalc->unref(); _arrowCalc = NULL; }
  if (_angleCalc) { _angleCalc->unref(); _angleCalc = NULL; }

  if (_computeAngleCRFT) { _computeAngleCRFT->unref(); _computeAngleCRFT = NULL; }
  if (_computeAngle) { _computeAngle->unref(); _computeAngle = NULL; }
  
  if (_kreisDrehenCRFT) { _kreisDrehenCRFT->unref(); _kreisDrehenCRFT = NULL; }
  if (_kreisInEbeneCRFT) { _kreisInEbeneCRFT->unref(); _kreisInEbeneCRFT = NULL; }
  if (_rotTemp) { _rotTemp->unref(); _rotTemp = NULL; }

  if (_areaProps) { _areaProps->unref(); _areaProps = NULL; }
  
  if (_endPosSens) { delete _endPosSens; _endPosSens = NULL; }
  if (_connectPosSens) { delete _connectPosSens; _connectPosSens = NULL; }
}


void UMDSoAngleLines::initAngleLines(){ 
  // #### Die Felder initialisieren: #########
  
  // Start- und endPosition der Linien
  SO_NODE_ADD_FIELD(endPos, (0, 0, 0));
  SO_NODE_ADD_FIELD(connectPos, (0, 0, 0));
  
  // Wert, bei dessen Unterschreiten Linie geschlossen dargestellt
  // wird
  SO_NODE_ADD_FIELD(minDistance, (10.0));
  
  // der Winkel zwischen den beiden Linien (nur zur Ausgabe)
  SO_NODE_ADD_FIELD(angleDegree, (0));
  SO_NODE_ADD_FIELD(angleArc, (0));
  
	SO_NODE_ADD_FIELD(unit,(deg));
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, deg);
  SO_NODE_SET_SF_ENUM_TYPE(unit, MeasureTool_Unit);

  // Die Feld-Sensoren initialisieren
  initAngleLinesFieldSensors();
  
  // ######## Die restlichen Instanz-Variablen: #########
  
  // diverse Berechnungen auf den Linien
  _arrowCalc = new SoCalculator; 
  _arrowCalc->ref();
  _arrowCalc->A.connectFrom(&connectPos);
  _arrowCalc->B.connectFrom(&startPos);
  _arrowCalc->C.connectFrom(&endPos);
  _arrowCalc->a.connectFrom(&minDistance);
  _arrowCalc->expression.set1Value(0, "oa = a / 2              "); // halbierte Lücke für je ein Pfeilende
  _arrowCalc->expression.set1Value(1, "ob = length(B - A) / 2  "); // Berechnung des Teilkreisradius;
  _arrowCalc->expression.set1Value(2, "oc = length(C - A) / 2  "); // -> Hälfte der kürzeren Linie
  _arrowCalc->expression.set1Value(3, "od = ob < oc ? ob : oc  ");
  _arrowCalc->expression.set1Value(4, "oA = cross(A - C, A - B)"); // Normale auf dem Winkelwerkzeug
  _arrowCalc->expression.set1Value(5, "oB = A - B              "); // Vektor AB
  _arrowCalc->expression.set1Value(6, "oC = A - C              "); // Vektor AC
  
  // mit Hilfe von SCRFromTo Winkel berechnen lassen, erspart Ausnahmeregelungen
  // das Ergebnis steht dann in _computeAngle->angle
  _computeAngleCRFT = new SoComposeRotationFromTo;
  _computeAngleCRFT->ref();
  _computeAngleCRFT->from.connectFrom(&_arrowCalc->oB);
  _computeAngleCRFT->to.connectFrom(&_arrowCalc->oC);
  _computeAngle = new SoDecomposeRotation;
  _computeAngle->ref();
  _computeAngle->rotation.connectFrom(&_computeAngleCRFT->rotation);

  // Der Calculator zur Umrechnung des Winkels vom Bogenmaß nach Grad
  _angleCalc = new SoCalculator; 
  _angleCalc->ref();
  _angleCalc->a.connectFrom(&_computeAngle->angle);
  _angleCalc->expression.set1Value(0, "oa = a * 180 / M_PI "); // angleDegree
  
  // Ausgabe des berechneten Winkels
  angleArc.connectFrom(&_computeAngle->angle);
  angleDegree.connectFrom(&_angleCalc->oa);

  _angleText = new SoExtText2;
  _angleText->ref();
  unit.setValue("deg");

  createRotationCalcs();
  createAngleLines();
  pointChanged();
}


void UMDSoAngleLines::initAngleLinesFieldSensors() {  
  // Initialisierung der Feld-Sensoren und Zuweisen der 
  // entsprechenden Callback-Funktionen
  _endPosSens = new SoFieldSensor(endChangedCB, this);
  _endPosSens->attach(&endPos);
  
  _connectPosSens = new SoFieldSensor(connectChangedCB, this);
  _connectPosSens->attach(&connectPos);  
}


void UMDSoAngleLines::createRotationCalcs() {
  // dreht den Teilkreis in die Werkzeugebene
  _kreisInEbeneCRFT = new SoComposeRotationFromTo;
  _kreisInEbeneCRFT->ref();
  _kreisInEbeneCRFT->from.setValue(0, 1, 0);
  _kreisInEbeneCRFT->to.connectFrom(&_arrowCalc->oA);

  // diese Rotation wird in pointChanged() benutzt
  _rotTemp = new SoRotation;
  _rotTemp->ref();
  _rotTemp->rotation.connectFrom(&_kreisInEbeneCRFT->rotation);

  // dreht den Teilkreis zwischen die Flanken des Tools
  _kreisDrehenCRFT = new SoComposeRotationFromTo;
  _kreisDrehenCRFT->ref();
  _kreisDrehenCRFT->from.setValue(-1, 0, 0);
  // nur ein InitialWert, wird in pointChanged() korrekt berechnet
  _kreisDrehenCRFT->to.connectFrom(&_arrowCalc->oA);
}


void UMDSoAngleLines::createAngleLines(){
  // Hier werden die einzelnen Knoten in den 
  // Graphen gehaengt
  
  // hier ein Knoten, der die Objekte, die ein Erscheinen der Dragger
  // ausloesen, enthaelt
  createInteractionSep();
  
  // Die Ausgabe des Abstandes
  createText();
  
  // Flaechen zur besseren Orientierung
  _showAreasSwitch = new SoSwitch;
  _root->addChild(_showAreasSwitch);
  SoSeparator* areasSep = createAreasSep();
  _showAreasSwitch->addChild(areasSep);
}


void UMDSoAngleLines::createInteractionSep(){  
  // initialisiert den Gruppenknoten, der die Pfeilenden und eine
  // Kugel zum Bewegen der beiden Linien enthaelt
  
  // Die Kugel an der startPosition
  _sphereHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_sphereHighlight);
  
  // beim Überfahren des Werkzeuges blinken die einzelnen
  // Teile des Werkzeuges auf
  _sphereHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _sphereHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  
  SoSeparator* sphereSep = new SoSeparator;
  _sphereHighlight->addChild(sphereSep);
  sphereSep->setName("connectSphere");
  
  // der folgende Separator dient nur dazu, die Hierarchie für
  // die Abfrage der Mousebutton-Event gerade zu biegen
  SoSeparator* hierarchieSep = new SoSeparator;
  sphereSep->addChild(hierarchieSep);
  
  SoTransform* sphereTrafo = new SoTransform;
  hierarchieSep->addChild(sphereTrafo);
  sphereTrafo->translation.connectFrom(&connectPos);
  //sphereTrafo->scaleFactor.setValue(0.2f, 0.2f, 0.2f);
  
  SoSphere* aSphere = new SoSphere;
  hierarchieSep->addChild(aSphere);
  
  // Hier zunaechst die beiden Pfeile am Verbindungspunkt
  UMDSoArrow* pfeilConnectStart = new UMDSoArrow;
  _sphereHighlight->addChild(pfeilConnectStart);
  pfeilConnectStart->setName("connectSphere");
  // Position des Pfeils
  pfeilConnectStart->position.connectFrom(&connectPos);
  // von wo er rotiert wird (wie beim Wetterhahn)
  pfeilConnectStart->rotationRef.connectFrom(&startPos);
  // Länge des Pfeils
  pfeilConnectStart->length.connectFrom(&_arrowCalc->ob);
  // Wert, bei dessen Unterschreiten Linie geschlossen dargestellt wird
  pfeilConnectStart->minDistance.connectFrom(&_arrowCalc->oa);
  // die Pfeilspitze wird nicht dargestellt
  pfeilConnectStart->displayHead.setValue(FALSE);
  
  UMDSoArrow* pfeilConnectEnd = new UMDSoArrow;
  _sphereHighlight->addChild(pfeilConnectEnd);
  pfeilConnectEnd->setName("connectSphere");
  pfeilConnectEnd->position.connectFrom(&connectPos);
  pfeilConnectEnd->rotationRef.connectFrom(&endPos);
  pfeilConnectEnd->length.connectFrom(&_arrowCalc->oc);
  pfeilConnectEnd->minDistance.setValue(32000);
  pfeilConnectEnd->displayHead.setValue(FALSE);
  
  // hier die beiden anderen getrennten Linienenden
  _arrowStartHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_arrowStartHighlight);
  _arrowStartHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _arrowStartHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  UMDSoArrow* pfeilStart = new UMDSoArrow;
  _arrowStartHighlight->addChild(pfeilStart);
  pfeilStart->setName("startCone");
  pfeilStart->position.connectFrom(&startPos);
  pfeilStart->rotationRef.connectFrom(&connectPos);
  pfeilStart->length.connectFrom(&_arrowCalc->ob);
  pfeilStart->minDistance.connectFrom(&_arrowCalc->oa);
  
  _arrowEndHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_arrowEndHighlight);
  _arrowEndHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _arrowEndHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  UMDSoArrow* pfeilEnd = new UMDSoArrow;
  _arrowEndHighlight->addChild(pfeilEnd);
  pfeilEnd->setName("endCone");
  pfeilEnd->position.connectFrom(&endPos);
  pfeilEnd->rotationRef.connectFrom(&connectPos);
  pfeilEnd->length.connectFrom(&_arrowCalc->oc);
  pfeilEnd->minDistance.setValue(32000);
  
  // Refresh des Winkels
  getTextString();
  // und Bewegen des Wertes
  getTextTrafo();
}


SoSeparator* UMDSoAngleLines::createAreasSep(){
  // Flaechen zur besseren Orientierung
  SoSeparator* angleAreas = new SoSeparator;
  angleAreas->ref();
  
  // die Flächen sollen zwar sichtbar, aber nicht
  // selektierbar sein
  SoPickStyle* pickstyle = new SoPickStyle;
  angleAreas->addChild(pickstyle);
  pickstyle->style.setValue(SoPickStyle::UNPICKABLE);
  
  // die Flächen werden halbtransparent dargestellt
  SoMaterial* areaMat = new SoMaterial;
  angleAreas->addChild(areaMat);
  areaMat->diffuseColor.connectFrom(&color);
  areaMat->transparency.setValue(0.5);
  
  // das FaceSet wird in getAreas() initialisiert
  _areas = new SoIndexedFaceSet;
  angleAreas->addChild(_areas);
  // einfügen eines Teilkreises, der den Winkel nachzeichnet
  SoTransform* teilkreisTrafo = new SoTransform;
  angleAreas->addChild(teilkreisTrafo);
  // verschieben des Teilkreises in den Werkzeugmittelpunkt
  teilkreisTrafo->translation.connectFrom(&connectPos);
  // drehen des Teilkreises in die Werkzeugebene
  teilkreisTrafo->rotation.connectFrom(&_kreisInEbeneCRFT->rotation);

  SoRotation* teilkreisDrehung = new SoRotation;
  angleAreas->addChild(teilkreisDrehung);
  // dreht den Teilkreis zwischen die Flanken des Tools
  teilkreisDrehung->rotation.connectFrom(&_kreisDrehenCRFT->rotation);
  
  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, würde die Winkelfläche seltsam
  // aussehen, wenn man es nicht mit FILLED unterbinden würde
  SoDrawStyle* style = new SoDrawStyle;
  angleAreas->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);
  
  // da der Halbkreis zwei Außenflächen hat, muß die Transparenz
  // verdoppelt werden
  SoMaterial* teilkreisMat = new SoMaterial;
  angleAreas->addChild(teilkreisMat);
  teilkreisMat->diffuseColor.connectFrom(&color);
  teilkreisMat->transparency.setValue(0.75);
  
  // der Teilkreis an sich
  _teilkreis = new UMDSoCake;
  angleAreas->addChild(_teilkreis);
  _teilkreis->radius.connectFrom(&_arrowCalc->od);
  _teilkreis->height.setValue(0.02f);
  _teilkreis->angleArc.connectFrom(&angleArc);
  
  getAreas();
  angleAreas->unrefNoDelete();
  return angleAreas;
}


void UMDSoAngleLines::getAreas(){
  // Anzeigen von zwei Flaechen;
  // 1. Flaeche -> Viereck, senkrecht auf dem Dreieck
  //               (connectPosition + endPosition als eine Kante)
  // 2. Flaeche -> Viereck, senkrecht auf dem Dreieck
  //               (connectPosition + endPosition als eine Kante)
  
  SoMFVec3f coords, coords1;
  
  SbVec3f normal = (connectPos.getValue() - startPos.getValue()).cross(connectPos.getValue() - endPos.getValue());
  normal.normalize();
  SbVec3f startNormal(connectPos.getValue() - startPos.getValue());
  startNormal.normalize();
  SbVec3f endNormal(connectPos.getValue() - endPos.getValue());
  endNormal.normalize();
  
  // Die Koordinaten
  coords.set1Value(0, connectPos.getValue() - 1 * normal);
  coords.set1Value(1, startPos.getValue() - 1 * normal);
  coords.set1Value(2, endPos.getValue() - 1 * normal);
  coords.set1Value(3, connectPos.getValue() + 1 * normal);
  coords.set1Value(4, startPos.getValue() + 1 * normal);
  coords.set1Value(5, endPos.getValue() + 1 * normal);
  int indices[14] = {0, 2, 5, 3, -1, 3, 4, 1, 0, -1};
  
  _areaProps = new SoVertexProperty;
  _areaProps->ref();
  _areaProps->vertex = coords;
  _areas->vertexProperty = _areaProps;
  _areas->coordIndex.setValues(0, 10, indices);
}


void UMDSoAngleLines::createText(){
  // Rueckgabe eines Separators, der den Text 
  // fuer den Winkel enthaelt
  _textSep->addChild(_textTrafo);
  getTextTrafo();

  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, würde der Texthintergrund
  // nur noch als Rahmen dargestellt werden,
  // wenn man es nicht mit FILLED unterbinden würde
  SoDrawStyle* style = new SoDrawStyle;
  _textSep->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);

  _textSep->addChild(_angleText);
  _angleText->unref();
  _angleText->justification.setValue(SoExtText2::CENTER);
  _angleText->color.setValue(1, 1, 1);
  _angleText->backgroundColor.connectFrom(&color);
  _angleText->backgroundAlpha.setValue(0.3f);
}


void UMDSoAngleLines::getTextString(){
  // Setzt den darzustellenden Text
  char* aString = new char[30];
  char* aUnit = new char[10];
  aString[0] = 0;
  if (toolNameFlag.getValue() == TRUE) {
    strcat(aString, toolName.getValue().getString());
    strcat(aString, ":\n");
  }
  sprintf(aUnit, "%.1f", angleDegree.getValue()); 
  strcat(aString, aUnit);
  if (unitFlag.getValue() == TRUE) {
    SbString tempString;
    unit.get(tempString);
    strcat(aString, " ");
    strcat(aString, tempString.getString());
  }
  _angleText->string.setValue(aString);

  delete[] aUnit;
  delete[] aString;
}


void UMDSoAngleLines::getTextTrafo(){
  SbVec3f offeneLinie(connectPos.getValue() - startPos.getValue());
  if (offeneLinie.length() > minDistance.getValue())
    // Bewegt den Text so, dass er in der Mitte der einen Linie ist
    _textTrafo->translation.setValue(startPos.getValue() + 0.5 * (connectPos.getValue() - startPos.getValue()));
  else {
    // bewegt den Text so, daß er sich gegenüber von den beiden Linien bewegt
    SbVec3f geschlosseneLinie(connectPos.getValue() - endPos.getValue());
    offeneLinie.normalize();
    geschlosseneLinie.normalize();
    SbVec3f punkt(offeneLinie + geschlosseneLinie);
    punkt.normalize();
    
    _textTrafo->translation.setValue(connectPos.getValue() + punkt);
  }
}


SbBool UMDSoAngleLines::getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint){
  // diese Funktion wird bei jedem Betaetigen der linken
  // Maustaste aufgerufen;
  // pickedPoint -> mit Mauszeiger getroffener Punkt
  SbBool handled = FALSE;
  
  // Rueckgabewert ist TRUE, falls ein Kegel oder eine Kugel getroffen 
  // wurde, sonst FALSE; dies dient vor allem dafuer, um das 
  // Mousebutton-Event auf handled zu setzen oder nicht
  if (pickedPoint != NULL){
    SoPath* aPath = pickedPoint->getPath();
    aPath->ref();
    SoNode* tail = aPath->getTail();
    tail->ref();
    // Wird ein Cone getroffen, so wird der Switch-Knoten 
    // fuer die Dragger umgeschaltet
    if (tail->isOfType(SoCone::getClassTypeId()) || tail->isOfType(SoCylinder::getClassTypeId()) || tail->isOfType(SoSphere::getClassTypeId())) { 
      // ist tail wirklich Teil DIESER Instanz?
      if(aPath->getNodeFromTail(3) == _sphereHighlight
        || aPath->getNodeFromTail(3) == _arrowStartHighlight
        || aPath->getNodeFromTail(3) == _arrowEndHighlight) {
        // nur um sicherzugehen: ist getNodeFromTail(5) wirklich der _root-Knoten
        if (aPath->getNodeFromTail(5)->isOfType(SoSeparator::getClassTypeId())) {
          SoSeparator* aGroup = (SoSeparator*) aPath->getNodeFromTail(5);
          // nur um sicherzugehen: haben wir hier wirklich einen Switch-Knoten
          if (aGroup->getChild(3)->isOfType(SoSwitch::getClassTypeId())) {
            SoSwitch* switchNode = (SoSwitch*) aGroup->getChild(3); 
            SoSeparator* highlightGroup = (SoSeparator*) aGroup->getChild(4); 
            short whichDragger = -1;
            // welcher Teil des Werkzeuges wurde getroffen
            if (aPath->getNodeFromTail(2)->getName() == "connectSphere"){
              whichDragger = 0;
              setInteractionMode(highlightGroup, inactive, 1);
              setInteractionMode(highlightGroup, inactive, 2);
            }
            if (aPath->getNodeFromTail(2)->getName() == "startCone"){
              whichDragger = 1;
              setInteractionMode(highlightGroup, inactive, 0);
              setInteractionMode(highlightGroup, inactive, 2);
            }
            if (aPath->getNodeFromTail(2)->getName() == "endCone"){
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
              setInteractionMode(highlightGroup, directSelect, whichDragger);
              switchNode->whichChild.setValue(-1);
              _showAreasSwitch->whichChild.setValue(0);
              getAreas();
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
        // wenn einer der Endpunkte für Pick-Modus aktiviert wurde,
        // dann setze den Pfeil auf die gepickte Position
        if (((UMDSoAngleLines*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoAngleLines*)_currentTool)->getActivePart(((UMDSoAngleLines*)_currentTool)->_highlightSep) == 0) {
          connectPos.setValue(point);
        }
        if (((UMDSoAngleLines*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoAngleLines*)_currentTool)->getActivePart(((UMDSoAngleLines*)_currentTool)->_highlightSep) == 1) {
          startPos.setValue(point);
        }
        if (((UMDSoAngleLines*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoAngleLines*)_currentTool)->getActivePart(((UMDSoAngleLines*)_currentTool)->_highlightSep) == 2) {
          endPos.setValue(point);
        }
      }
    }
    tail->unref();
    aPath->unref();
  }
  // alle Dragger verschwinden lassen
  else {
    resetHighLightNode(_currentTool);
    _currentTool = NULL;
    _showAreasSwitch->whichChild.setValue(-1);
    getAreas();
  }
  return handled;
} //getPickedObjectMouseLeft


void UMDSoAngleLines::pointChanged() {
  // Refresh der Winkelangabe
  getTextString();
  // und Bewegen des Wertes
  getTextTrafo();
  _showAreasSwitch->whichChild.setValue(0);
  getAreas();

  // Drehung der Winkelebene, damit die Kanten mit den beiden Flanken
  // übereinstimmen; kann leider nicht von einer Engine übernommen werden
  // also machen wir's hier
  SbRotation invRot = _rotTemp->rotation.getValue();
  invRot.invert();
  SbVec3f temp = startPos.getValue() - connectPos.getValue();
  invRot.multVec(temp, temp);
  // wenn man tempVec nicht einsetzt, flimmert die Fläche bei Bewegung
  SoSFVec3f tempVec;
  tempVec.setValue(temp);
  _kreisDrehenCRFT->to.connectFrom(&tempVec);
}


SbBool UMDSoAngleLines::setArrowByKeyboard(float x, float y, float z, Taste /*taste*/) {
  // ein Pfeil wurde per Tastatur bewegt
  SbVec3f richtung;
  richtung.setValue(x * keyboardIncrement.getValue(), y * keyboardIncrement.getValue(), z * keyboardIncrement.getValue());
  // den aktiven Knoten suchen und verschieben
  if (_arrowStartHighlight->mode.getValue() == SoLocateHighlight::ON) {
    ((UMDSoAngleLines*) _currentTool)->startPos.setValue(startPos.getValue() + richtung);
    return TRUE;
  }
  if (_arrowEndHighlight->mode.getValue() == SoLocateHighlight::ON) {
    ((UMDSoAngleLines*) _currentTool)->endPos.setValue(endPos.getValue() + richtung);
    return TRUE;
  }
  if (_sphereHighlight->mode.getValue() == SoLocateHighlight::ON) {
    ((UMDSoAngleLines*) _currentTool)->connectPos.setValue(connectPos.getValue() + richtung);
    return TRUE;
  }
  return FALSE;
}


// ############## Die Callback-Funktionen #################

void UMDSoAngleLines::endChangedCB(void* userData, SoSensor*) {
  // Der Endpunkt wurde geaendert
  UMDSoAngleLines* obj = (UMDSoAngleLines*) userData;
  // abgeleitete Klasse
  obj->pointChanged();
}


void UMDSoAngleLines::connectChangedCB(void* userData, SoSensor*){
  // Der Endpunkt der geteilten Linie wurde geaendert
  UMDSoAngleLines* obj = (UMDSoAngleLines*) userData;
  obj->pointChanged();
}
