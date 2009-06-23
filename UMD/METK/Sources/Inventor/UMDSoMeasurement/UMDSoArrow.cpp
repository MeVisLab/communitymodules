// #############################
// ####
// ####  UMDSoArrow.cpp
// ####
// #############################

#include "UMDSoArrow.h"


SO_NODE_SOURCE(UMDSoArrow);


void UMDSoArrow::initClass() { 
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoArrow, SoSeparator, "SoSeparator");
}

UMDSoArrow::UMDSoArrow() {
  SO_NODE_CONSTRUCTOR(UMDSoArrow);
  initUMDSoArrow();
}


UMDSoArrow::~UMDSoArrow() {
  // ein bisschen aufraeumen
  if (_root) { _root->unref(); _root = NULL; }
  if (_calcStiel) { _calcStiel->unref(); _calcStiel = NULL; }
  if (_rotationCalc) { _rotationCalc->unref(); _rotationCalc = NULL; }
  if (_spitzeSpitz) { _spitzeSpitz->unref(); _spitzeSpitz = NULL; }
  if (_spitzeStumpf) { _spitzeStumpf->unref(); _spitzeStumpf = NULL; }

  if (_displayHeadSens) { delete _displayHeadSens; _displayHeadSens = NULL; }
  if (_scaleSens) { delete _scaleSens; _scaleSens = NULL; }
}


void UMDSoArrow::initUMDSoArrow() { 
  // #### Die Felder initialisieren: #########

  // Ausrichtung und Position des Objektes
  SO_NODE_ADD_FIELD(position, (0, 0, 0));
  SO_NODE_ADD_FIELD(rotationRef, (0, 0, 0));

  // Absolute Länge und Distanz, bei der der Pfeil gekürzt wird
  SO_NODE_ADD_FIELD(length, (10));
  SO_NODE_ADD_FIELD(minDistance, (5));
  SO_NODE_ADD_FIELD(scale, (1));
  _scaleSens = new SoFieldSensor(scaleChangedCB, this);
  _scaleSens->attach(&scale);


  // soll die Pfeilspitze angezeigt werden?
  SO_NODE_ADD_FIELD(displayHead, (TRUE));
  _displayHeadSens = new SoFieldSensor(displayHeadChangedCB, this);
  _displayHeadSens->attach(&displayHead);
  
  // Der Calculator zur Berechnung des Stiels.
  _calcStiel = new SoCalculator; 
  _calcStiel->ref();
  
  _calcStiel->a.connectFrom(&minDistance);
  _calcStiel->b.connectFrom(&length);

  _calcStiel->expression.set1Value(0, "ta =              b                       ");// Länge eines Pfeils
  _calcStiel->expression.set1Value(1, "tb =              a                       ");// minDistance für einen Pfeil
  _calcStiel->expression.set1Value(2, "tc = (ta > 2)   ? (ta / 10)         : 0.2 ");// Skalierung abhängig von der Linienlänge,
  _calcStiel->expression.set1Value(3, "td = (tc < 1)   ? tc                : 1   ");// jedoch nur im Bereich 0.2 < skal < 0.5

  _calcStiel->expression.set1Value(4, "te = (ta > tc)  ? ta / td - 1       : 0   ");// Länge des Zylinders, wenn Linie geschlossen
  _calcStiel->expression.set1Value(5, "tf =              (ta - tb) / td          ");// Länge des Zylinders, wenn Linie offen
  _calcStiel->expression.set1Value(6, "oa = (ta > tb)  ? tf                : te  ");// Ausgabe der Länge des Zylinders

  _calcStiel->expression.set1Value(7, "tg =              1 + oa / 2              ");// y-Position des Zylinders
  _calcStiel->expression.set1Value(8, "oA =              vec3f(0, -tg, 0)        ");// Ausgabe der Zylinderposition

  _calcStiel->expression.set1Value(9, "oB =              vec3f(td, td, td)       ");// Skalierungsfaktor der Linie

  // Der Calculator zur Berechnung der Rotation
  _rotationCalc = new SoCalculator; 
  _rotationCalc->ref();
  _rotationCalc->A.connectFrom(&position);
  _rotationCalc->B.connectFrom(&rotationRef);
  _rotationCalc->expression.set1Value(0, "oA = A - B");
 
  SoComposeRotationFromTo* rotStart = new SoComposeRotationFromTo;
  rotStart->ref();
  rotStart->from.setValue(0, 1, 0);
  rotStart->to.connectFrom(&_rotationCalc->oA);
  
  // An die Basisklasse (SoSeparator) wird ein Separator-Knoten 
  // angehaengt, der das Objekt enthaelt
  _root = new SoSeparator;
  _root->ref();
  addChild(_root);
  
  // verschieben des Pfeils an seine Position
  SoTransform* posTrafo = new SoTransform;
  _root->addChild(posTrafo);
  posTrafo->translation.connectFrom(&position);
  posTrafo->scaleFactor.connectFrom(&_calcStiel->oB);

  // drehen des Pfeils in seine Richtung
  SoRotation* drehPfeil = new SoRotation;
  _root->addChild(drehPfeil);
  drehPfeil->rotation.connectFrom(&rotStart->rotation);

  // die Spitze wird etwas versetzt, damit sie sich nicht mit dem Pfeilende überlappt
  _versatzSpitze = new SoTransform;
  _versatzSpitze->ref();
  _root->addChild(_versatzSpitze);
  _versatzSpitze->translation.setValue(0, -1*scale.getValue(), 0);

  // an dieser Stelle werden ein Cone und ein Cylinder eingefügt
  // und gereft. Es wird immer nur einer von ihnen angezeigt, je
  // nach Zustand von displayHead
  _spitzeSpitz = new SoCone;
  _spitzeSpitz->ref();
  _root->addChild(_spitzeSpitz);
  _spitzeSpitz->height = 2*scale.getValue();
  _spitzeSpitz->bottomRadius = 1*scale.getValue();
  
  _spitzeStumpf = new SoCylinder;
  _spitzeStumpf->ref();
  _spitzeStumpf->height = 2*scale.getValue();
  _spitzeStumpf->radius = 0.2f*scale.getValue();
  
  // da die Position eines Zylinders immer über seinen Mittelpunkt
  // definiert wird, wird er hier entsprechend versetzt
  SoTransform* versatzstiel = new SoTransform;
  _root->addChild(versatzstiel);
  versatzstiel->translation.connectFrom(&_calcStiel->oA);

  SoCylinder* stiel = new SoCylinder;
  _root->addChild(stiel);
  stiel->radius = 0.2f*scale.getValue();
  stiel->height.connectFrom(&_calcStiel->oa);
}


void UMDSoArrow::displayHeadChangedCB(void* userData, SoSensor*) {
  // ein anderer Kopf soll angezeigt werden
  UMDSoArrow* obj = (UMDSoArrow*) userData;
  // abgeleitete Klasse
  if (obj->displayHead.getValue() == TRUE)
    obj->_root->replaceChild(obj->_spitzeStumpf, obj->_spitzeSpitz);
  else
    obj->_root->replaceChild(obj->_spitzeSpitz, obj->_spitzeStumpf);
}

void UMDSoArrow::scaleChangedCB(void* userData, SoSensor*) {  
  UMDSoArrow* obj = (UMDSoArrow*) userData;
  obj->_spitzeSpitz->height = 2*obj->scale.getValue();
  obj->_spitzeSpitz->bottomRadius = 1*obj->scale.getValue();
  obj->_spitzeStumpf->height = 2*obj->scale.getValue();
  obj->_spitzeStumpf->radius = 0.2f*obj->scale.getValue();

  obj->_versatzSpitze->translation.setValue(0, -1*obj->scale.getValue(), 0);

  //Der stiel muss ja auch skaliert werden. Aber ob er das hier wirklich macht ist nicht 100%ig klar
  if (obj->_root->getChild(obj->_root->getNumChildren()-1)->isOfType(SoCylinder::getClassTypeId()))
  {
	((SoCylinder*)(obj->_root->getChild(obj->_root->getNumChildren()-1)))->radius = 0.2f*obj->scale.getValue();
  }

}
