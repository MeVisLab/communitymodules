// #############################
// ####
// ####  UMDSoAngleToObjects.cpp
// ####
// #############################

#include "UMDSoAngleToObjects.h"
#include "UMDMainAxis.h"
#include "UMDSoAngleLines.h"
#include "UMDSoDistanceLine.h"

SO_NODE_SOURCE(UMDSoAngleToObjects);

void UMDSoAngleToObjects::initClass() {
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoAngleToObjects, UMDSoAutoMeasureTool, "UMDSoAutoMeasureTool");
}


UMDSoAngleToObjects::UMDSoAngleToObjects() {
  SO_NODE_CONSTRUCTOR(UMDSoAngleToObjects);

  SO_NODE_ADD_FIELD(angleDegree, (FALSE));

  SO_NODE_ADD_FIELD(inObject2Node, (NULL));

  SO_NODE_ADD_FIELD(startPos, (0, 0, 0));
  SO_NODE_ADD_FIELD(endPos, (0, 0, 0));
  SO_NODE_ADD_FIELD(connectPos, (0, 0, 0));

  // AngleLines einhängen
  _angleLines = new UMDSoAngleLines;
  _angleLines->ref();
  _toolSep->addChild(_angleLines);

  // Felder für AngleLines initialisieren
  startPos.connectFrom(&_angleLines->startPos);
  endPos.connectFrom(&_angleLines->endPos);
  connectPos.connectFrom(&_angleLines->connectPos);
  angleDegree.connectFrom(&_angleLines->angleDegree);
  _angleLines->unitFlag.connectFrom(&displayUnit);
  _angleLines->toolNameFlag.connectFrom(&displayName);
  _angleLines->toolName.connectFrom(&toolName);
  color.setValue(_angleLines->color.getValue());
  _angleLines->color.connectFrom(&color);

  _inputSens2 = new SoFieldSensor(inputCB, this);
  _inputSens2->attach(&inObject2Node);
}


UMDSoAngleToObjects::~UMDSoAngleToObjects() {
  // ein bischen aufraeumen
  if (_inputSens2) { delete _inputSens2;   _inputSens2 = NULL; }
  if (_angleLines) { _angleLines->unref(); }
}


bool UMDSoAngleToObjects::getLargestMainAxis(SoGroup *node, SbVec3f& vec1, SbVec3f& vec2) {
  // Eingabe (node): Knoten mit Objekt; Ausgabe (vec1, vec2): längste Hauptachse
  float* pointSet;
  long size;
  getPointSet(node, pointSet, size);

  vec1 = SbVec3f(0, 0, 0);
  vec2 = SbVec3f(0, 0, 0);

  if (size > 0) {
    UMDMainAxis *mainAxis = new UMDMainAxis;
    // SoMainAxis ausführen
    mainAxis->computeMainAxis(pointSet, size);
    SbVec3f axis = SbVec3f(mainAxis->getLargestMainAxis());
    float x,y,z;
    mainAxis->getMidPoint(&x, &y, &z);
    SbVec3f midPoint = SbVec3f(x, y, z);
    float halfLength = mainAxis->getLargestExtension() / 2;

    axis.normalize();
    vec1 = midPoint + axis * halfLength;
    vec2 = midPoint - axis * halfLength;
    delete mainAxis;
    delete[] pointSet;
    return true;
  }
  else return false;
}


bool UMDSoAngleToObjects::_calculate() {
  // nur rechnen, wenn ein Graph angehängt ist
  if (inObject1Node.getValue() != NULL && inObject2Node.getValue() != NULL) {
    SbVec3f *vec = new SbVec3f[4];

    // Hauptachse für beide Eingänge berechnen
    bool valid1 = getLargestMainAxis(_inputSep1,  vec[0], vec[1]);
    bool valid2 = getLargestMainAxis(_inputSep2, vec[2], vec[3]);
    if (valid1 && valid2) {

      // am nächsten zusammenliegende Punkte suchen
      float *length = new float[4];
      length[0] = (vec[0] - vec[2]).length();
      length[1] = (vec[0] - vec[3]).length();
      length[2] = (vec[1] - vec[2]).length();
      length[3] = (vec[1] - vec[3]).length();

      int closestVec1 = 0, closestVec2 = 0;
      if (length[0] < length[1]) {
        if (length[2] < length[3]) {
          if (length[0] < length[2]) {
            closestVec1 = 0;
            closestVec2 = 2;
          }
          else {
            closestVec1 = 1;
            closestVec2 = 2;
          }
        }
        else  {//(length[2] < length[3])
          if (length[0] < length[3]) {
            closestVec1 = 0;
            closestVec2 = 2;
          }
          else {
            closestVec1 = 1;
            closestVec2 = 3;
          }
        }
      }
      else {//length[0] < length[1]
        if (length[2] < length[3]) {
          if (length[1] < length[2]) {
            closestVec1 = 0;
            closestVec2 = 3;
          }
          else {
            closestVec1 = 1;
            closestVec2 = 2;
          }
        }
        else  {//(length[2] < length[3])
          if (length[1] < length[3]) {
            closestVec1 = 0;
            closestVec2 = 3;
          }
          else {
            closestVec1 = 1;
            closestVec2 = 3;
          }
        }
      }

      // Punkte in eine Ebene legen
      // bzw. Punkte von 4 auf 3 reduzieren
      SbVec3f versatz = (vec[closestVec1] - vec[closestVec2]) / 2;
      vec[0] -= versatz;
      vec[1] -= versatz;
      vec[2] += versatz;
      vec[3] += versatz;

      // _angleLines auf die Punkte setzen
      _angleLines->connectPos.setValue(vec[closestVec1]);
      if (closestVec1 == 0)
        _angleLines->startPos.setValue(vec[1]);
      else
        _angleLines->startPos.setValue(vec[0]);
      if (closestVec2 == 2)
        _angleLines->endPos.setValue(vec[3]);
      else
        _angleLines->endPos.setValue(vec[2]);

      delete[] length;
      delete[] vec;
      // Ergebnisse wieder ok
      return true;
    }
    delete[] vec;
  }
  return false;
}


void UMDSoAngleToObjects::inputCB(void* userData, SoSensor*) {
  UMDSoAngleToObjects* obj = (UMDSoAngleToObjects*) userData;

  // Eingang hat sich geändert, Ergebnisse nicht ok
  obj->resultsValid.setValue(FALSE);

  obj->_inputSep2->removeAllChildren();

  // Inhalte einfügen, wenn vorhanden
  if (obj->inObject2Node.getValue() != NULL)
    obj->_inputSep2->addChild(obj->inObject2Node.getValue());
  obj->inputChanged();
}


void UMDSoAngleToObjects::display() {
  _toolSep->addChild(_angleLines);
}
