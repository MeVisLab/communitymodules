// #############################
// ####
// ####  UMDSoDistanceLine.cpp
// ####
// #############################


#include "UMDSoArrow.h"
#include "UMDSoDistanceLine.h"
//#include "SoDragger3D.h"
#include "SoExtText2.h"

SO_NODE_SOURCE(UMDSoDistanceLine);


void UMDSoDistanceLine::initClass(){ 
  // muss zur Initialisierung der Klasse einmal
  // explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoDistanceLine, UMDSoMeasureTool, "UMDSoMeasureTool");
}


UMDSoDistanceLine::UMDSoDistanceLine() {
  SO_NODE_CONSTRUCTOR(UMDSoDistanceLine);
  initDistanceLine();
}


UMDSoDistanceLine::~UMDSoDistanceLine() {
  // ein bischen aufraeumen
  if (_endPosSens) { delete _endPosSens; _endPosSens = NULL; }

  if (_enableTextSens) { delete _enableTextSens; _enableTextSens = NULL; }
  
  if (_arrowCalc) { _arrowCalc->unref(); _arrowCalc = NULL; }
}


void UMDSoDistanceLine::initDistanceLine() {
  // #### Die Felder initialisieren: #########
  
  SO_NODE_ADD_FIELD(endPos, (0, 0, 0));

  // Wert, bei dessen Unterschreiten Linie geschlossen dargestellt
  // wird
  SO_NODE_ADD_FIELD(minDistance, (10.0));

  // Länge der Linie (nur zur Ausgabe)
  SO_NODE_ADD_FIELD(lineLength, (0));

  // Beschriftung ein/aus
  SO_NODE_ADD_FIELD(enableText, (true));

  // Skalierung Pfeil
  SO_NODE_ADD_FIELD(scaleLine, (1));

  
  // definieren der Einheiten
  SO_NODE_ADD_FIELD(unit,(mm));
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, mm);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, cm);
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_Unit, m);
  SO_NODE_SET_SF_ENUM_TYPE(unit, MeasureTool_Unit);

  // Die Feld-Sensoren initialisieren
  initDistLineFieldSensors();
  
  // Der Calculator zur Berechnung der Endpunkte der Linie.
  // Der Calculator zur Berechnung der Linienlänge
  _arrowCalc = new SoCalculator; 
  _arrowCalc->ref();
  _arrowCalc->A.connectFrom(&startPos);
  _arrowCalc->B.connectFrom(&endPos);
  _arrowCalc->a.connectFrom(&minDistance);
  _arrowCalc->b.connectFrom(_unitFactor);
  _arrowCalc->expression.set1Value(0, "oa = a / 2            ");
  _arrowCalc->expression.set1Value(1, "ob = length(B - A) / 2");
  _arrowCalc->expression.set1Value(2, "oc = length(A - B) / b");
  
  lineLength.connectFrom(&_arrowCalc->oc);

  // ######## Die restlichen Instanz-Variablen: #########
  
  // der Text
  _distText = new SoExtText2();
  _distText->ref();
  unit.setValue("mm");

  _endDraggerDragged = FALSE;
  createDistLine();
}

void UMDSoDistanceLine::initDistLineFieldSensors() {
  // Initialisierung der Feld-Sensoren und Zuweisen der 
  // entsprechenden Callback-Funktionen
  _endPosSens = new SoFieldSensor(endChangedCB, this);
  _endPosSens->attach(&endPos);

  _enableTextSens = new SoFieldSensor(enableTextChangedCB, this);
  _enableTextSens->attach(&enableText);

  _scaleLineSens = new SoFieldSensor(scaleLineChangedCB, this);
  _scaleLineSens->attach(&scaleLine);

}


void UMDSoDistanceLine::createDistLine() {
  // Hier werden die einzelnen Knoten in den Graphen gehaengt.
  // Zuerst die Pfeilenden der Linie.
  createArrows();
  
  // Die Ausgabe des Abstandes
  createText();
}


void UMDSoDistanceLine::createArrows() {
  // gibt die Pfeilenden zurueck
  
  // Hier zunaechst der Pfeil am Anfangspunkt 
  _arrowStartHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_arrowStartHighlight);
  _arrowStartHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _arrowStartHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  UMDSoArrow* pfeilStart = new UMDSoArrow;
  pfeilStart->scale = scaleLine.getValue();
  _arrowStartHighlight->addChild(pfeilStart);
  pfeilStart->setName("startCone");
  pfeilStart->position.connectFrom(&startPos);
  pfeilStart->rotationRef.connectFrom(&endPos);
  pfeilStart->length.connectFrom(&_arrowCalc->ob);
  pfeilStart->minDistance.connectFrom(&_arrowCalc->oa);
  
  // Hier der Pfeil am Endpunkt
  _arrowEndHighlight = new SoLocateHighlight;
  _highlightSep->addChild(_arrowEndHighlight);
  _arrowEndHighlight->mode.setValue(SoLocateHighlight::AUTO);
  _arrowEndHighlight->color.setValue(0.3f, 0.3f, 0.3f);
  UMDSoArrow* pfeilEnd = new UMDSoArrow;
  pfeilEnd->scale=scale.getValue();
  _arrowEndHighlight->addChild(pfeilEnd);
  pfeilEnd->setName("endCone");
  pfeilEnd->position.connectFrom(&endPos);
  pfeilEnd->rotationRef.connectFrom(&startPos);
  pfeilEnd->length.connectFrom(&_arrowCalc->ob);
  pfeilEnd->minDistance.connectFrom(&_arrowCalc->oa);
  
  getTextTrafo();
  getTextString();
}


void UMDSoDistanceLine::createText() {
  // Rueckgabe eines Separators, der den Text fuer den Abstand
  // sowie die Einheit (falls erwuenscht) enthaelt
  _textSep->addChild(_textTrafo);
  getTextTrafo();

  // da das Werkzeug an sich ab einer bestimmten Entfernung zur Kamera
  // noch im Linienmodus dargestellt wird, würde der Texthintergrund
  // nur noch als Rahmen dargestellt werden,
  // wenn man es nicht mit FILLED unterbinden würde
  SoDrawStyle* style = new SoDrawStyle;
  _textSep->addChild(style);
  style->style.setValue(SoDrawStyle::FILLED);

  _textSep->addChild(_distText);
  _distText->unref();
  _distText->justification.setValue(SoExtText2::CENTER);
  _distText->color.setValue(1, 1, 1);
  _distText->backgroundColor.connectFrom(&color);
  _distText->backgroundAlpha.setValue(0.3f);
}


void UMDSoDistanceLine::getTextString() {
  // Setzt den darzustellenden Text
  _arrowCalc->b.connectFrom(_unitFactor);
  char* aString = new char[30];
  char* aUnit = new char[10];
  aString[0] = 0;
  if (toolNameFlag.getValue() == TRUE) {
    strcat(aString, toolName.getValue().getString());
    strcat(aString, ":\n");
  }
  if (_unitFactor->getValue() == 1)
    sprintf(aUnit, "%.1f", lineLength.getValue());
  else
    sprintf(aUnit, "%.2f", lineLength.getValue());
  strcat(aString, aUnit);
  if (unitFlag.getValue() == TRUE) {
    SbString tempString;
    unit.get(tempString);
    strcat(aString, " ");
    strcat(aString, tempString.getString());
  }
  if (enableText.getValue()) 
  {
	  _distText->string.setValue(aString);
	  _distText->backgroundAlpha.setValue(0.3f);
  }
  else
  {
      _distText->string.setValue("");
	  _distText->backgroundAlpha.setValue(0.0f);
  }
  
  delete[] aUnit;
  delete[] aString;
}


void UMDSoDistanceLine::getTextTrafo() {
  // Bewegt den Text so, dass er sich ab einer bestimmten
  // Laenge in der Mitte der Distance Line (bzw. hier nun
  // etwas naeher zur Anfangsposition) befindet;
  // wird diese bestimmte Laenge unterschritten, wird
  // der Text ausserhalb der Linie dargestellt
  if ((lineLength.getValue() * _unitFactor->getValue()) < minDistance.getValue()) { 
    SbVec3f direction = endPos.getValue() - startPos.getValue();
    direction.normalize(); 
    
    // Welcher der beiden Dragger ? (Anfangs- oder Endpunkt)
    if (_endDraggerDragged) {
      _textTrafo->translation.setValue(SbVec3f(startPos.getValue() - 0.6f * direction));
      _endDraggerDragged = FALSE;
    } 
    else
      _textTrafo->translation.setValue(SbVec3f(endPos.getValue() + 0.6f * direction));
  } 
  else {  
    SbVec3f testV = startPos.getValue() + 0.5f * (endPos.getValue() - startPos.getValue());
    _textTrafo->translation.setValue(testV[0], testV[1] - 0.07, testV[2]);
  }
}


SbBool UMDSoDistanceLine::getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint) {
  // diese Funktion wird bei jedem Betaetigen der linken Maustaste aufgerufen;
  // pickedPoint -> mit Mauszeiger getroffener Punkt

  // Rueckgabewert ist TRUE, falls ein Kegel getroffen wurde, sonst
  // FALSE; dies dient vor allem dafuer, um das Mousebutton-Event
  // auf handled zu setzen oder nicht
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
    if (tail->isOfType(SoCone::getClassTypeId()) || tail->isOfType(SoCylinder::getClassTypeId())) {
      // ist tail wirklich Teil DIESER Instanz?
      if(aPath->getNodeFromTail(3) == _arrowStartHighlight || aPath->getNodeFromTail(3) == _arrowEndHighlight){
        // nur um sicherzugehen: ist getNodeFromTail(5) wirklich der _root-Knoten
        if (aPath->getNodeFromTail(5)->isOfType(SoSeparator::getClassTypeId())) {
          SoSeparator* aGroup = (SoSeparator*) aPath->getNodeFromTail(5);
          // nur um sicherzugehen: haben wir hier wirklich einen Switch-Knoten
          if (aGroup->getChild(3)->isOfType(SoSwitch::getClassTypeId())) {
            SoSwitch* switchNode = (SoSwitch*) aGroup->getChild(3);
            // Hier wird zunaechst geprueft, ob es sich um den 
            // Kegel am Anfangs- oder Endpunkt handelt;
            SoSeparator* highlightGroup = (SoSeparator*) aGroup->getChild(4);
            short whichDragger;
            if (aPath->getNodeFromTail(2)->getName() == "startCone"){
              whichDragger = 0;
              setInteractionMode(highlightGroup, inactive, 1);
            }
            else{
              whichDragger = 1;
              setInteractionMode(highlightGroup, inactive, 0);
            }
            // vorherigen Handler wiederherstellen
            if (_currentTool != this) {
              resetHighLightNode(_currentTool);
              _currentTool = this;
            }
            // Umschaltung zwischen DirectSelect und Draggerbewegung
            switch (interactionMode.getValue()) {
            case directSelect: // umschalten auf Pick-Modus
              setInteractionMode(highlightGroup, directSelect, whichDragger);
              switchNode->whichChild.setValue(-1);
              break;              
            }
          }
        }
        handled = TRUE;
      }
    }
    else {
      // SoPickedPointAction
      if (_currentTool == this){
        SbVec3f point = pickedPoint->getPoint();
        // wenn einer der Endpunkte für Pick-Modus aktiviert wurde,
        // dann setze den Pfeil auf die gepickte Position
        if (((UMDSoDistanceLine*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoDistanceLine*)_currentTool)->getActivePart(((UMDSoDistanceLine*)_currentTool)->_highlightSep) == 0) {
          startPos.setValue(point);
        }
        if (((UMDSoDistanceLine*)_currentTool)->interactionMode.getValue() == directSelect &&
            ((UMDSoDistanceLine*)_currentTool)->getActivePart(((UMDSoDistanceLine*)_currentTool)->_highlightSep) == 1) {
          endPos.setValue(point);
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
  }

  return handled;
}


SbBool UMDSoDistanceLine::setArrowByKeyboard(float x, float y, float z, Taste /*taste*/){
  // ein Pfeil wurde per Tastatur bewegt
  SbVec3f richtung;
  richtung.setValue(x * keyboardIncrement.getValue(), y * keyboardIncrement.getValue(), z * keyboardIncrement.getValue());
  // den aktiven Knoten suchen und verschieben
  if (_arrowStartHighlight->mode.getValue() == SoLocateHighlight::ON){
    ((UMDSoDistanceLine*)_currentTool)->startPos.setValue(startPos.getValue() + richtung);
    return TRUE;
  }
  if (_arrowEndHighlight->mode.getValue() == SoLocateHighlight::ON){
    ((UMDSoDistanceLine*)_currentTool)->endPos.setValue(endPos.getValue() + richtung);
    return TRUE;
  }
  return FALSE;
}


// ############## Die Callback-Funktionen #################

// Bei Bewegung eines Draggers wird entweder der
// Anfangs- oder der Endpunkt aktualisiert;

void UMDSoDistanceLine::pointChanged() {
  // Ein Punkt wurde geaendert
  // Funktionsaufruf so SoMeasureTool
  getTextString();
  getTextTrafo();
}

void UMDSoDistanceLine::endChangedCB(void* userData, SoSensor*) {
  // Der Endpunkt wurde geaendert
  UMDSoDistanceLine* obj = (UMDSoDistanceLine*) userData;
  // abgeleitete Klasse
  obj->_endDraggerDragged = TRUE;
  obj->pointChanged();
}

void UMDSoDistanceLine::enableTextChangedCB(void* userData, SoSensor*) {  
  UMDSoDistanceLine* obj = (UMDSoDistanceLine*) userData;
  obj->getTextString();  
}

void UMDSoDistanceLine::scaleLineChangedCB(void* userData, SoSensor*) {  
  UMDSoDistanceLine* obj = (UMDSoDistanceLine*) userData;
  if (obj->_arrowStartHighlight->getChild(0)->isOfType(UMDSoArrow::getClassTypeId()))
  {
	  ((UMDSoArrow*)(obj->_arrowStartHighlight->getChild(0)))->scale=obj->scaleLine.getValue();
  }
  if (obj->_arrowEndHighlight->getChild(0)->isOfType(UMDSoArrow::getClassTypeId()))
  {
	  ((UMDSoArrow*)(obj->_arrowEndHighlight->getChild(0)))->scale=obj->scaleLine.getValue();
  }

  //scale is from MeasureTools
  obj->scale=obj->scaleLine.getValue();
}
