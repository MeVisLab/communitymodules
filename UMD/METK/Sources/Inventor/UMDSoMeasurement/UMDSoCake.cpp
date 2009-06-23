// #############################
// ####
// ####  UMDSoCake.cpp
// ####
// #############################



#include "UMDSoCake.h"


SO_NODE_SOURCE(UMDSoCake);


void UMDSoCake::initClass() { 
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoCake, SoSeparator, "SoSeparator");
}


UMDSoCake::UMDSoCake() {
  SO_NODE_CONSTRUCTOR(UMDSoCake);
  initUMDSoCake();
}


UMDSoCake::~UMDSoCake() {
  // ein bischen aufraeumen
  if (_root) { _root->unref(); _root = NULL; }
  if (_flaeche) { _flaeche->unref(); _flaeche = NULL; }
  if (_vertexProp) { _vertexProp->unref(); _vertexProp = NULL; }
  
  if (_radiusSens) { delete _radiusSens; _radiusSens = NULL; }
  if (_heightSens) { delete _heightSens; _heightSens = NULL; }
  if (_angleArcSens) { delete _angleArcSens; _angleArcSens = NULL; }
  if (_angleDegreeSens) { delete _angleDegreeSens; _angleDegreeSens = NULL; }
}


void UMDSoCake::initUMDSoCake() { 
  // #### Die Felder initialisieren: #########
  SO_NODE_ADD_FIELD(angleArc, (2 * (float)M_PI));
  SO_NODE_ADD_FIELD(angleDegree, (360));
  SO_NODE_ADD_FIELD(radius, (1));
  SO_NODE_ADD_FIELD(height, (1));
  
  // Die Feld-Sensoren initialisieren
  initCakeFieldSensors();
  
  _root = new SoSeparator;
  _root->ref();
  addChild(_root);
  initCakeTree();
}


void UMDSoCake::initCakeFieldSensors(){
  // Initialisierung der Feld-Sensoren und Zuweisen der 
  // entsprechenden Callback-Funktionen
  _radiusSens = new SoFieldSensor(valueChangedCB, this);
  _radiusSens->attach(&radius);
  
  _heightSens = new SoFieldSensor(valueChangedCB, this);
  _heightSens->attach(&height);
  
  _angleArcSens = new SoFieldSensor(valueChangedCB, this);
  _angleArcSens->attach(&angleArc);
  
  _angleDegreeSens = new SoFieldSensor(degreeValueChangedCB, this);
  _angleDegreeSens->attach(&angleDegree);
}


void UMDSoCake::initCakeTree() {
  // die FaceSet wird hier zus�zlich gereft, damit sie evtl.
  // nach wiedereinfgen vom Vollkreismodus nicht wieder neu
  // erstellt werden mu�  
  
  _flaeche = new SoIndexedFaceSet;
  _flaeche->ref();
  _root->addChild(_flaeche);
  
  _vertexProp = new SoVertexProperty;
  _vertexProp->ref();
  
  cakeValueChanged();
}


void UMDSoCake::cakeValueChanged() {
  // wenn Vollkreis, normalen SoCylinder zeichnen
  if (angleArc.getValue() == 2 * M_PI){
    SoCylinder* vollkreis = new SoCylinder;
    _root->replaceChild(0, vollkreis);
    vollkreis->height = height;
    vollkreis->radius = radius;
  }
  else {
    // wenn vorher Vollkreis, jetzt wieder durch Teilkreis ersetzen
    if (_root->getChild(0)->isOfType(SoCylinder::getClassTypeId()))
      _root->replaceChild(0, _flaeche);

    SbVec3f mitte(0, height.getValue() / 2, 0);
    
    SbVec3f angleVec(-radius.getValue(), 0, 0);
    SbRotation drehung;
    SbMatrix drehMatrix;
    
    // alle 20 Grad ein Kreispunkt
    int anzahlPunkte = (int)(angleArc.getValue() / ((float) M_PI / 9));
    
    drehung.setValue(SbVec3f(0, 1, 0), (float) M_PI / 9);
    drehMatrix.setRotate(drehung);
    
    // Startkoordinaten einfgen
    // gerader Index = obere Fl�he; ungerader Index = untere F�he
    int vertex = 0;
    
    // Mittelpunkte einfgen
    _coords.set1Value(vertex++, mitte);
    _coords.set1Value(vertex++, -mitte);
    
    // Punkt bei 0 Grad einfgen
    _coords.set1Value(vertex++, angleVec + mitte);
    _coords.set1Value(vertex++, angleVec - mitte);
    
    // alle Kreiskoordinaten berechnen
    for (int counter = 0; counter < anzahlPunkte; counter++){
      drehMatrix.multMatrixVec(angleVec, angleVec);
      _coords.set1Value(vertex++, angleVec + mitte);
      _coords.set1Value(vertex++, angleVec - mitte);
    }
    
    // angegebenen Abschlusswert einfgen
    drehung.setValue(SbVec3f(0, 1, 0), angleArc.getValue());
    angleVec.setValue(-radius.getValue(), 0, 0);
    drehMatrix.setRotate(drehung);
    drehMatrix.multMatrixVec(angleVec, angleVec);
    
    _coords.set1Value(vertex++, angleVec + mitte);
    _coords.set1Value(vertex++, angleVec - mitte);
    
    // falls vorhanden, berschssige Punkte von vorher l�chen
    if (_coords.getNum() > vertex)
      _coords.deleteValues(vertex);
    
    // jetzt wird das Indices-Feld erstellt
    int indices[1000];
    
    // index speichert die momentane indices-Feldposition
    // gerader Index = obere Fl�he; ungerader Index = untere F�he
    int index = 0;
    
    // zuerst die obere Fl�he
    for (int indexOben = 2; indexOben < vertex - 2; indexOben += 2){
      indices[index++] = 0;
      indices[index++] = indexOben + 2;
      indices[index++] = indexOben;
      indices[index++] = -1;
    }
    
    // dann die untere Fl�he
    for (int indexUnten = 3; indexUnten < vertex - 2; indexUnten += 2){
      indices[index++] = 1;
      indices[index++] = indexUnten;
      indices[index++] = indexUnten + 2;
      indices[index++] = -1;
    }
    
    // und zum Schlu�der Rand
    int indexRand = 0;
    for (indexRand = 0; indexRand < vertex - 2; indexRand += 2){
      indices[index++] = indexRand;
      indices[index++] = indexRand + 2;
      indices[index++] = indexRand + 1;
      indices[index++] = -1;
      indices[index++] = indexRand + 1;
      indices[index++] = indexRand + 2;
      indices[index++] = indexRand + 3;
      indices[index++] = -1;
    }
    indices[index++] = indexRand;
    indices[index++] = 0;
    indices[index++] = indexRand + 1;
    indices[index++] = -1;
    indices[index++] = indexRand + 1;
    indices[index++] = 0;
    indices[index++] = 1;
    indices[index++] = -1;
    
    _vertexProp->vertex = _coords;
    _flaeche->vertexProperty = _vertexProp;
    _flaeche->coordIndex.setValues(0, index, indices);
    if (_flaeche->coordIndex.getNum() > index)
      _flaeche->coordIndex.deleteValues(index);
  }
}


bool UMDSoCake::areEqual(const float& float1, const float& float2) {
  if (fabs(float1 - float2) < 0.0001)
    return TRUE;
  else
    return FALSE;
}


void UMDSoCake::valueChangedCB(void* userData, SoSensor*) {
  // etwas wurde geaendert
  UMDSoCake* obj = (UMDSoCake*) userData;
  // abgeleitete Klasse
  if (!obj->areEqual(obj->angleDegree.getValue(), obj->angleArc.getValue() * 180 / M_PI)) {
    obj->angleDegree.setValue(obj->angleArc.getValue() * 180 / M_PI); // Umwandlung Bogenma�-> Grad
    obj->cakeValueChanged();
  }
}


void UMDSoCake::degreeValueChangedCB(void* userData, SoSensor*) {
  // Das Gradma�wurde geaendert
  UMDSoCake* obj = (UMDSoCake*) userData;
  // abgeleitete Klasse
  if (!obj->areEqual(obj->angleDegree.getValue(), obj->angleArc.getValue() * 180 / M_PI)) {
    obj->angleArc.setValue(obj->angleDegree.getValue() / 180 * M_PI); // Umwandlung Grad -> Bogenma�    obj->cakeValueChanged();
  }
}

