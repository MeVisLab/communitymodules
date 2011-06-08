// #############################
// ####
// ####  UMDSoMeasureTool.cpp
// ####
// #############################


#include "UMDSoMeasureTool.h"


SO_NODE_SOURCE(UMDSoMeasureTool);

UMDSoMeasureTool* UMDSoMeasureTool::_currentTool = NULL;
int UMDSoMeasureTool::_colorCounter = 0;

void UMDSoMeasureTool::initClass() {
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoMeasureTool, SoSeparator, "SoSeparator");
}


UMDSoMeasureTool::UMDSoMeasureTool() {
  SO_NODE_CONSTRUCTOR(UMDSoMeasureTool);
  initMeasureTool();
}


UMDSoMeasureTool::~UMDSoMeasureTool() {
  // ein bischen aufraeumen
  if (_root) { _root->unref(); _root = NULL; }

  if (_startPosSens) { delete _startPosSens; _startPosSens = NULL; }

  if (_toolNameFlagSens) { delete _toolNameFlagSens; _toolNameFlagSens = NULL; }
  if (_toolNameSens) { delete _toolNameSens; _toolNameSens = NULL; }

  if (_unitFlagSens) { delete _unitFlagSens; _unitFlagSens = NULL; }
  if (_unitSens) { delete _unitSens; _unitSens = NULL; }

  if (_interactionModeSens) { delete _interactionModeSens; _interactionModeSens = NULL; }

  if (_scaleSens) { delete _scaleSens; _scaleSens = NULL; }
}


void UMDSoMeasureTool::initMeasureTool() {
  // #### Die Felder initialisieren: #########

  // Start- und Endposition der Linie
  SO_NODE_ADD_FIELD(startPos, (0, 0, 0));

  // Farbe des Objektes
  SO_NODE_ADD_FIELD(color, (1, 1, 1));

  // Name des Tools
  SO_NODE_ADD_FIELD(toolNameFlag, (FALSE));
  SO_NODE_ADD_FIELD(toolName, (""));

  // Gr�e der Verschiebung bei Cursorsteuerung
  SO_NODE_ADD_FIELD(keyboardIncrement, (0.01f));

  // ein Flag fuer das Anzeigen einer Einheit
  SO_NODE_ADD_FIELD(unitFlag, (FALSE));

  SO_NODE_ADD_FIELD(scale, (1));

  // Auswahl der verschiedenen Modi zum bewegen des Werkzeugs
  SO_NODE_ADD_FIELD(interactionMode,(INTERACTION_DIRECT_SELECT));
  SO_NODE_DEFINE_ENUM_VALUE(MeasureTool_InteractionMode, INTERACTION_DIRECT_SELECT);
  SO_NODE_SET_SF_ENUM_TYPE(interactionMode, MeasureTool_InteractionMode);


  // Die Feld-Sensoren initialisieren
  initMeasureToolFieldSensors();

  // ######## Die restlichen Instanz-Variablen: #########

  _unitFactor = new SoSFInt32;
  _unitFactor->setValue(1);

  // An die Basisklasse (SoSeparator) wird ein Separator-Knoten
  // angehaengt, der das Objekt enthaelt
  _root = new SoSeparator;
  _root->ref();
  addChild(_root);
  _root->setName("UMDSoMeasureTool_root");

  getObject();
}


void UMDSoMeasureTool::initMeasureToolFieldSensors() {
  // Initialisierung der Feld-Sensoren und Zuweisen der
  // entsprechenden Callback-Funktionen
  _startPosSens = new SoFieldSensor(startChangedCB, this);
  _startPosSens->attach(&startPos);

  _toolNameFlagSens = new SoFieldSensor(unitChangedCB, this);
  _toolNameFlagSens->attach(&toolNameFlag);

  _toolNameSens = new SoFieldSensor(unitChangedCB, this);
  _toolNameSens->attach(&toolName);

  _unitFlagSens = new SoFieldSensor(unitChangedCB, this);
  _unitFlagSens->attach(&unitFlag);

  _unitSens = new SoFieldSensor(unitChangedCB, this);
  _unitSens->attach(&unit);

  _interactionModeSens = new SoFieldSensor(interactionModeChangedCB, this);
  _interactionModeSens->attach(&interactionMode);

  _scaleSens = new SoFieldSensor(scaleChangedCB, this);
  _scaleSens->attach(&scale);
}


void UMDSoMeasureTool::getObject() {
  // EventCallback fuer die Mousebutton-Ereignisse
  SoEventCallback* eventCB = new SoEventCallback;
  _root->addChild(eventCB);
  eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressedCB, this);
  eventCB->addEventCallback(SoKeyboardEvent::getClassTypeId(), cursorPressCB, this);


  // hier wird ein LevelOfDetail-Knoten angehaengt,
  // der fuer die Font-Groesse verantwortlich ist
  SoLevelOfDetail* lod = (SoLevelOfDetail*) getLOD();
  _root->addChild(lod);

  // Das Material und die Linienstaerke
  _objectMaterial = new SoMaterial;
  _root->addChild(_objectMaterial);
  _objectMaterial->diffuseColor.connectFrom(&color);

  // Switch-Knoten, der die Dragger enthaelt
  _switchDragger = new SoSwitch;
  _root->addChild(_switchDragger);

  // der Inhaber aller LocateHighlight-Knoten
  _highlightSep = new SoSeparator;
  _root->addChild(_highlightSep);

  _textSep = new SoAnnotation;
  _root->addChild(_textSep);

  // der Text soll zwar sichtbar, aber nicht
  // selektierbar sein
  SoPickStyle* pickstyle = new SoPickStyle;
  _textSep->addChild(pickstyle);
  pickstyle->style.setValue(SoPickStyle::UNPICKABLE);
} //getObject


SoLevelOfDetail* UMDSoMeasureTool::getLOD() {
  // hier wird ein LevelOfDetail-Knoten zurueckgegeben;
  // dieser beinhaltet drei Kinder mit jeweils verschiedener Font-Groesse;
  // ausserdem wird ein unsichtbarer Wuerfel in den jeweiligen Kinderknoten eingehaengt,
  // der dafuer verantwortlich ist, wann welcher Kinderknoten verwendet wird (zur Bestimmung
  // der screenArea)
  SoLevelOfDetail* lod = new SoLevelOfDetail;
  lod->ref();

  float areas[2] = {4000, 2000};
  lod->screenArea.setValues(0, 2, areas);

  SoSeparator* sphereSep = new SoSeparator;
  sphereSep->ref();

  // der Wrfel wird immer zusammen mit dem Text bewegt
  _textTrafo = new SoTransform;
  sphereSep->addChild(_textTrafo);

  // hier der unsichtbare Wuerfel
  SoDrawStyle* style = new SoDrawStyle;
  sphereSep->addChild(style);
  style->style.setValue(SoDrawStyle::INVISIBLE);

  // der Wrfel darf nicht selektierbar sein
  SoPickStyle* pickstyle = new SoPickStyle;
  sphereSep->addChild(pickstyle);
  pickstyle->style.setValue(SoPickStyle::UNPICKABLE);

  _sphere = new SoSphere;
  _sphere->ref();
  sphereSep->addChild(_sphere);
  _sphere->radius = 10*scale.getValue();
  //cube->height = 10*90;
  //cube->depth = 10*90;

  // es folgen die einzelnen Kinder vom LevelOfDetail-Knoten,
  // die die unterschiedlichen Font-Groessen beinhalten
  SoGroup* group1 = new SoGroup;
  lod->addChild(group1);
  SoFont* font1 = new SoFont;
  group1->addChild(font1);
  font1->size = 18;
  font1->name.setValue("Helvetica"); // Arial
  group1->addChild(sphereSep);

  SoGroup* group2 = new SoGroup;
  lod->addChild(group2);
  SoFont* font2 = new SoFont;
  group2->addChild(font2);
  font2->size = 12;
  font2->name.setValue("Helvetica");
  group2->addChild(sphereSep);

  SoGroup* group3 = new SoGroup;
  lod->addChild(group3);
  SoFont* font3 = new SoFont;
  group3->addChild(font3);
  font3->size = 10;
  font3->name.setValue("Helvetica");
  group3->addChild(sphereSep);

  // ab einer gewissen Entfernung werden die Werkzeuge
  // einfach als Linie dargestellt;
  // so bleiben sie auf jeden Fall sichtbar
  SoDrawStyle* lineStyle = new SoDrawStyle;
  group3->addChild(lineStyle);
  lineStyle->style.setValue(SoDrawStyle::LINES);
  lineStyle->lineWidth.setValue(1);

  sphereSep->unref();
  lod->unrefNoDelete();
  return lod;
}


void UMDSoMeasureTool::resetHighLightNode(UMDSoMeasureTool* tool) {
  // setzt alle Highlightknoten eines Tools wieder zurck
  if (tool != NULL){
    for (int counter = 0; counter < tool->_highlightSep->getNumChildren(); counter++) {
      ((SoLocateHighlight*)tool->_highlightSep->getChild(counter))->mode.setValue(SoLocateHighlight::AUTO);
      ((SoLocateHighlight*)tool->_highlightSep->getChild(counter))->color.setValue(0.3f, 0.3f, 0.3f);
    }
    // Dragger abschalten
    tool->_switchDragger->whichChild.setValue(-1);
  }
}


void UMDSoMeasureTool::setInteractionMode(SoGroup* highSep, MeasureTool_InteractionMode wert, short whichDragger) {
  // setzt den Interaktionsmodus eines Werkzeugs
  SoLocateHighlight* highlightNode;
  if (whichDragger != -1) {
    highlightNode = (SoLocateHighlight*) highSep->getChild(whichDragger);
    if (wert == inactive)
      highlightNode->mode.setValue(SoLocateHighlight::AUTO);
    else
      highlightNode->mode.setValue(SoLocateHighlight::ON);
  }
}


short UMDSoMeasureTool::getActivePart(SoGroup *highSep) {
  for (int counter = 0; counter < highSep->getNumChildren(); counter++) {
    if (((SoLocateHighlight*) highSep->getChild(counter))->mode.getValue() == ON)
      return counter;
  }
  return -1;
}


bool UMDSoMeasureTool::areEqual(const SbVec3f& vec1, const SbVec3f& vec2) {
  // gibt zurck, ob zwei Vektoren (fast) gleich sind
  float difference = fabs(SbVec3f(vec1 - vec2).length());
  if (difference < 0.0001)
    return TRUE;
  else
    return FALSE;
}


// ############ Callback-Methoden ##############################


void UMDSoMeasureTool::startChangedCB(void* userData, SoSensor*) {
  // Der Anfangspunkt wurde geaendert
  UMDSoMeasureTool* obj = (UMDSoMeasureTool*) userData;
  // abgeleitete Klasse
  obj->pointChanged();
}


void UMDSoMeasureTool::mousePressedCB(void* userData, SoEventCallback* eventCB) {
  // Wird die linke Maustaste gedrueckt, so wird der getroffene Punkt ermittelt;
  // getPickedObject wird von der Unterklasse separat implementiert
  const SoEvent* event = eventCB->getEvent();
  UMDSoMeasureTool* obj = (UMDSoMeasureTool*) userData;

  // linke Maustaste
  if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {

      //std::cout << " UMDSoMeasureTool::mousePressedCB::leftButton" << std::endl;

    if (obj->getPickedObjectMouseLeft(eventCB->getPickedPoint()))
      eventCB->setHandled();
  }
  // mittlere Maustaste
  // alle Pfeile werden deselektiert
  if (SO_MOUSE_PRESS_EVENT(event, BUTTON2)) {
    obj->resetHighLightNode(_currentTool);
    obj->_currentTool = NULL;
  }
}


void UMDSoMeasureTool::cursorPressCB(void* /*userData*/, SoEventCallback* eventCB) {
  // check for the arrow keys being pressed
  const SoEvent* event = eventCB->getEvent();
  Taste zusatzTaste = none; //{shift, control, alt};
  if (_currentTool != NULL){
    if (!eventCB->isHandled()){
      if (event->wasShiftDown())
        zusatzTaste = shift;

      if (event->wasCtrlDown())
        zusatzTaste = control;

      /*
      if (SO_KEY_PRESS_EVENT(event, SPACE)) {
        if (_currentTool != NULL) {
          if (_currentTool->interactionMode.getValue() == directSelect)
            _currentTool->interactionMode.setValue(dragMode);
          else
            _currentTool->interactionMode.setValue(directSelect);
          eventCB->setHandled();
        }
      }
      else
      */
        if (SO_KEY_PRESS_EVENT(event, UP_ARROW)) {
          if (_currentTool->cursorUpPressed(zusatzTaste))
            eventCB->setHandled();
        }
        else
          if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW)) {
            if (_currentTool->cursorDownPressed(zusatzTaste))
              eventCB->setHandled();
          }
          else
            if (SO_KEY_PRESS_EVENT(event, LEFT_ARROW)) {
              if (_currentTool->cursorLeftPressed(zusatzTaste))
                eventCB->setHandled();
            }
            else
              if (SO_KEY_PRESS_EVENT(event, RIGHT_ARROW)) {
                if (_currentTool->cursorRightPressed(zusatzTaste))
                  eventCB->setHandled();
              }
              else
                if (SO_KEY_PRESS_EVENT(event, PAGE_UP)) {
                  if (_currentTool->pageUpPressed(zusatzTaste))
                    eventCB->setHandled();
                }
                else
                  if (SO_KEY_PRESS_EVENT(event, PAGE_DOWN)) {
                    if (_currentTool->pageDownPressed(zusatzTaste))
                      eventCB->setHandled();
                  }
    }
  }
}

SbBool UMDSoMeasureTool::cursorUpPressed(Taste taste){
  return setArrowByKeyboard(0, 1, 0, taste);
}


SbBool UMDSoMeasureTool::cursorDownPressed(Taste taste){
  return setArrowByKeyboard(0, -1, 0, taste);
}


SbBool UMDSoMeasureTool::cursorLeftPressed(Taste taste){
  return setArrowByKeyboard(-1, 0, 0, taste);
}


SbBool UMDSoMeasureTool::cursorRightPressed(Taste taste){
  return setArrowByKeyboard(1, 0, 0, taste);
}


SbBool UMDSoMeasureTool::pageUpPressed(Taste taste){
  return setArrowByKeyboard(0, 0, -1, taste);
}


SbBool UMDSoMeasureTool::pageDownPressed(Taste taste){
  return setArrowByKeyboard(0, 0, 1, taste);
}


void UMDSoMeasureTool::unitChangedCB(void* userData, SoSensor*) {
  // es soll eine andere Einheit angezeigt werden
  UMDSoMeasureTool* obj = (UMDSoMeasureTool*) userData;
  SbString tempString;
  obj->unit.get(tempString);
  switch (obj->unit.getValue()) {
  case mm:
  case cmm:
    obj->_unitFactor->setValue(1);
    break;
  case cm:
    obj->_unitFactor->setValue(10);
    break;
  case m:
    obj->_unitFactor->setValue(1000);
    break;
  case ml:
    obj->_unitFactor->setValue(1000);
    break;
  case l:
    obj->_unitFactor->setValue(1000000);
    break;
  case deg: // wird nicht gebraucht
    obj->_unitFactor->setValue(0);
    break;
  }
  obj->getTextString();
}


void UMDSoMeasureTool::interactionModeChangedCB(void* userData, SoSensor*) {
  // es soll eine andere Einheit angezeigt werden
  UMDSoMeasureTool* obj = (UMDSoMeasureTool*) userData;
  switch (obj->interactionMode.getValue()) {
  case directSelect:
    obj->setInteractionMode(obj->_highlightSep, directSelect, obj->getActivePart(obj->_highlightSep));
    //obj->_switchDragger->whichChild.setValue(-1);
    break;
  }
}


void UMDSoMeasureTool::scaleChangedCB(void* userData, SoSensor*) {
  UMDSoMeasureTool* obj = (UMDSoMeasureTool*) userData;
  obj->_sphere->radius = 10*obj->scale.getValue();
}

