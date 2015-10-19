// #############################
// ####
// ####  UMDSoShapeToPointSet.cpp
// ####
// #############################


#include "UMDSoShapeToPointSet.h"



SO_NODE_SOURCE(UMDSoShapeToPointSet);

void UMDSoShapeToPointSet::initClass() { 
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoShapeToPointSet, SoSeparator, "SoSeparator");
}


UMDSoShapeToPointSet::UMDSoShapeToPointSet() {
  SO_NODE_CONSTRUCTOR(UMDSoShapeToPointSet);
  
  SO_NODE_ADD_FIELD(rootInput, (NULL));
  //SO_NODE_ADD_FIELD(isUnique, (FALSE));
  
  // Matrix zum speichern der aktuellen Transformation
  _worldMatrix = new SoSFMatrix;
  
  // zum Punkte einsammeln
  _pointSet = new SoPointSet;
  addChild(_pointSet);
  
  vertexProperty = new SoVertexProperty;
  vertexProperty->ref();
  _pointSet->vertexProperty = vertexProperty;
  
  // Callback zum einsammeln der Koordinaten
  _myAction = new SoCallbackAction;
  _myAction->addPreCallback        (SoShape::getClassTypeId(), preCB, this);
  _myAction->addPointCallback      (SoShape::getClassTypeId(), pointCB,    this);
  _myAction->addLineSegmentCallback(SoShape::getClassTypeId(), lineCB,     this);
  _myAction->addTriangleCallback   (SoShape::getClassTypeId(), triangleCB, this);
  
  _inputSens = new SoFieldSensor(inputCB, this);
  _inputSens->attach(&rootInput);
  
  _buffer = new SbVec3f[BUFFER_SIZE];
  int64_t size= (int64_t)pow((double)HASH_PARTITION, 3);
  _hashTable = new std::set<SbVec3f, ltstr>[size];
}


UMDSoShapeToPointSet::~UMDSoShapeToPointSet() {
  // ein bischen aufraeumen
  if (_worldMatrix)   { delete   _worldMatrix;   _worldMatrix   = NULL; }
  if (vertexProperty) { vertexProperty->unref(); vertexProperty = NULL; }
  if (_inputSens)     { delete   _inputSens;     _inputSens     = NULL; }
  if (_myAction)      { delete   _myAction;      _myAction      = NULL; }
  if (_hashTable)     { delete[] _hashTable;     _hashTable     = NULL; }
  if (_buffer)        { delete[] _buffer;        _buffer        = NULL; }
}


void UMDSoShapeToPointSet::inputCB(void* userData, SoSensor*) {
  UMDSoShapeToPointSet* obj = (UMDSoShapeToPointSet*) userData;
  obj->calculate();
}


void UMDSoShapeToPointSet::calculate() {
  vertexProperty->vertex.deleteValues(0,-1);
  
  // Eingang hat sich ge�dert
  if (rootInput.getValue() != NULL) {
    // Dreiecke aufsammeln
    _counter = 0;
    _counterBuffer = 0;

    _myAction->apply(rootInput.getValue());
    collectPoints();
  }
}

SoCallbackAction::Response UMDSoShapeToPointSet::preCB(void* userData, SoCallbackAction* /*action*/, const SoNode* /*node*/) {
  UMDSoShapeToPointSet* obj = (UMDSoShapeToPointSet*) userData;
  
  // aktuelle Transformationsmatrix besorgen
  obj->_worldMatrix->setValue(obj->_myAction->getModelMatrix());
  
  return SoCallbackAction::CONTINUE;
}


void UMDSoShapeToPointSet::pointCB(void* userData, 
                                SoCallbackAction* /*action*/,
                                const SoPrimitiveVertex* v) {
  UMDSoShapeToPointSet* obj = (UMDSoShapeToPointSet*) userData;
  
  SbVec3f tmp;
  // Punkt abspeichern
  obj->_worldMatrix->getValue().multVecMatrix(v->getPoint(), tmp); 
  obj->insertPoint(tmp);
  
  obj->_counter++;
}


void UMDSoShapeToPointSet::lineCB(void* userData, 
                               SoCallbackAction* /*action*/,
                               const SoPrimitiveVertex* v1,
                               const SoPrimitiveVertex* v2) {
  UMDSoShapeToPointSet* obj = (UMDSoShapeToPointSet*) userData;
  
  SbVec3f tmp;
  // Linie abspeichern
  obj->_worldMatrix->getValue().multVecMatrix(v1->getPoint(), tmp);
  obj->insertPoint(tmp);
  obj->_counter++;
  
  obj->_worldMatrix->getValue().multVecMatrix(v2->getPoint(), tmp);
  obj->insertPoint(tmp);
  obj->_counter++;
}


void UMDSoShapeToPointSet::triangleCB(void* userData, 
                                   SoCallbackAction* /*action*/,
                                   const SoPrimitiveVertex* v1,
                                   const SoPrimitiveVertex* v2,
                                   const SoPrimitiveVertex* v3) {
  UMDSoShapeToPointSet* obj = (UMDSoShapeToPointSet*) userData;
  
  SbVec3f tmp;
  // Dreieck abspeichern
  obj->_worldMatrix->getValue().multVecMatrix(v1->getPoint(), tmp);
  obj->insertPoint(tmp);
  obj->_counter++;
  
  obj->_worldMatrix->getValue().multVecMatrix(v2->getPoint(), tmp);
  obj->insertPoint(tmp);
  obj->_counter++;
  
  obj->_worldMatrix->getValue().multVecMatrix(v3->getPoint(), tmp);
  obj->insertPoint(tmp);
  obj->_counter++;
}


void UMDSoShapeToPointSet::insertPoint(SbVec3f vector) {
    float x, y, z;
    int hashKey;
    vector.getValue(x, y, z);
    hashKey =
      ((int)fabs(x) % HASH_PARTITION) +
      (((int)fabs(y) % HASH_PARTITION) * HASH_PARTITION) +
      (((int)fabs(z) % HASH_PARTITION) * HASH_PARTITION * HASH_PARTITION);
    
    _hashTable[hashKey].insert(vector);
}


void UMDSoShapeToPointSet::collectPoints() {
    vertexProperty->vertex.setNum(_counter / 6);
    long counter = 0;
    if (_counter != 0) {
      for (int index = 0; index < pow((double)HASH_PARTITION, 3); index++) {
        for (std::set<SbVec3f, ltstr>::iterator pos = _hashTable[index].begin(); pos != _hashTable[index].end(); ++pos) {
          vertexProperty->vertex.set1Value(counter, *pos);
          counter++;
        }
        _hashTable[index].clear();
      }
    }
    vertexProperty->vertex.deleteValues(counter, -1);
}
