// #############################
// ####
// ####  UMDSoMinimalDistance.cpp
// ####
// #############################


#include "UMDMinimalDistance.h"
#include "UMDSoDistanceLine.h"
#include "UMDSoShapeToPointSet.h"
#include "UMDSoMinimalDistance.h"

//Poting ILAB4 - new field names
#ifndef ILAB5
#define inObject1Node inputObject
#define inObject2Node inputObject2
#endif


SO_NODE_SOURCE(UMDSoMinimalDistance);

void UMDSoMinimalDistance::initClass() { 
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoMinimalDistance, UMDSoAutoMeasureTool, "UMDSoAutoMeasureTool");
}


UMDSoMinimalDistance::UMDSoMinimalDistance() {
  SO_NODE_CONSTRUCTOR(UMDSoMinimalDistance);
  
  SO_NODE_ADD_FIELD(computeMinimalDistance, ());
  SO_NODE_ADD_FIELD(autoCompute, (FALSE));

  SO_NODE_ADD_FIELD(inObject2Node, (NULL));
  
  SO_NODE_ADD_FIELD(startPos, (0, 0, 0));
  SO_NODE_ADD_FIELD(endPos, (0, 0, 0));
  SO_NODE_ADD_FIELD(distance, (0));

  // Wert, bei dessen Unterschreiten Linie geschlossen dargestellt
  // wird
  SO_NODE_ADD_FIELD(minDistance, (10.0));


    // choice of the distance line unit
  SO_NODE_ADD_FIELD(unit,(mm));
  SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, mm);
  SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, cm);
  SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, m);
  SO_NODE_SET_SF_ENUM_TYPE(unit, AutoMeasureTool_Unit);

  SO_NODE_ADD_FIELD(scaleLine, (1));

  SO_NODE_ADD_FIELD(autoParam, (FALSE));
  SO_NODE_ADD_FIELD(autoEntries, (2000));
  SO_NODE_ADD_FIELD(manEntries,  (200));
  SO_NODE_ADD_FIELD(manPartition, (2));

  SO_NODE_ADD_FIELD(verbose, (FALSE));
  SO_NODE_ADD_FIELD(displaySpheres, (FALSE));
  SO_NODE_ADD_FIELD(sizeOfPointSet1, (0));
  SO_NODE_ADD_FIELD(sizeOfPointSet2, (0));

  SO_NODE_ADD_FIELD(error, (0));

  // DistanceLine einhängen
  _distanceLine = new UMDSoDistanceLine;
  _distanceLine->ref();
  _toolSep->addChild(_distanceLine);
  
  // Felder für DistanceLine initialisieren
  startPos.connectFrom(&_distanceLine->startPos);
  endPos.connectFrom(&_distanceLine->endPos);
  _distanceLine->enableText.connectFrom(&displayToolMeasure);
  _distanceLine->unitFlag.connectFrom(&displayUnit);
  _distanceLine->unit.connectFrom(&unit);
  _distanceLine->minDistance.connectFrom(&minDistance);
  _distanceLine->toolNameFlag.connectFrom(&displayName);
  _distanceLine->toolName.connectFrom(&toolName);
  color.setValue(_distanceLine->color.getValue());
  _distanceLine->color.connectFrom(&color);
  _distanceLine->scaleLine.connectFrom(&scaleLine);
  
  SoMaterial* mat = new SoMaterial;
  addChild(mat);
  mat->transparency = 0.9f;
  _spheres = new SoSeparator;
  addChild(_spheres);

  // die Feldsensoren
  _computeMinimalDistanceSens = new SoFieldSensor(computeMinimalDistanceCB, this);
  _computeMinimalDistanceSens->attach(&computeMinimalDistance);

  _displaySpheresSensor = new SoFieldSensor(computeMinimalDistanceCB, this);
  _displaySpheresSensor->attach(&displaySpheres);

  _inputSens2 = new SoFieldSensor(inputCB, this);
  _inputSens2->attach(&inObject2Node);

  _pointSet1 = _pointSet2 = NULL;
  _size1 = _size2 = 0;

}


UMDSoMinimalDistance::~UMDSoMinimalDistance() {
  // ein bischen aufraeumen
  if (_computeMinimalDistanceSens) { delete _computeMinimalDistanceSens; _computeMinimalDistanceSens = NULL; }
  if (_inputSens2) { delete _inputSens2; _inputSens2 = NULL; }
  if (_pointSet1) { delete[] _pointSet1; _pointSet1 = NULL; }
  if (_pointSet2) { delete[] _pointSet2; _pointSet2 = NULL; }

  if (_distanceLine) { _distanceLine->unref(); _distanceLine = NULL; }
}


void UMDSoMinimalDistance::computeMinimalDistanceCB(void* userData, SoSensor*) {
  UMDSoMinimalDistance* obj = (UMDSoMinimalDistance*) userData;
  obj->_computeMinimalDistance();
}
bool UMDSoMinimalDistance::_computeMinimalDistance() {
  if (_size1 != 0 && _size2 != 0) {
    float *point1;
    float *point2;
    UMDMinimalDistance* minimalDistance = new UMDMinimalDistance;
    minimalDistance->setPointSets(_pointSet1, _pointSet2, _size1, _size2);

    minimalDistance->setError(error.getValue() / 100);
    if (autoParam.getValue() == TRUE)
      minimalDistance->setNumEntries(autoEntries.getValue());
    else
      minimalDistance->setParams(manEntries.getValue(), manPartition.getValue());
    
    minimalDistance->verbose(verbose.getValue() == TRUE);
    minimalDistance->displaySpheres(displaySpheres.getValue() == TRUE);
    
    minimalDistance->computeDistance(point1, point2);

    _distanceLine->startPos.setValue(point1);
    _distanceLine->endPos.setValue(point2);
    startPos.connectFrom(&_distanceLine->startPos);
    endPos.connectFrom(&_distanceLine->endPos);
    distance.connectFrom(&_distanceLine->lineLength);

    _spheres->removeAllChildren();
    if (displaySpheres.getValue() == TRUE) {
      _spheres->addChild(minimalDistance->_tileSphere1);
      _spheres->addChild(minimalDistance->_tileSphere2);
    }
    
    // clean up memory
    delete minimalDistance;
    resultsValid.setValue(TRUE);
    outputChanged();
    return true;
  }
  return false;
}


bool UMDSoMinimalDistance::_calculate() {
  if (_pointSet1) { delete[] _pointSet1; _size1 = 0; _pointSet1 = NULL; }
  if (_pointSet2) { delete[] _pointSet2; _size2 = 0; _pointSet2 = NULL; }
  sizeOfPointSet1.setValue(0);
  sizeOfPointSet2.setValue(0);

  if (inObject1Node.getValue() != NULL) {
    getPointSet(_inputSep1,  _pointSet1, _size1);
    sizeOfPointSet1.setValue(_size1);
  }
  if (inObject2Node.getValue() != NULL) {
    getPointSet(_inputSep2, _pointSet2, _size2);
    sizeOfPointSet2.setValue(_size2);
  }

  if (inObject1Node.getValue() != NULL && 
      inObject2Node.getValue() != NULL && 
      autoCompute.getValue() == TRUE) {
    return _computeMinimalDistance();
  }
  return false;
}


void UMDSoMinimalDistance::display() {
  _toolSep->addChild(_distanceLine);
}


void UMDSoMinimalDistance::inputCB(void* userData, SoSensor*) {
  UMDSoMinimalDistance* obj = (UMDSoMinimalDistance*) userData;
  
  // Eingang hat sich geändert, Ergebnisse nicht ok
  obj->resultsValid.setValue(FALSE);
  obj->minDistance.setValue(0);
  
  obj->_inputSep2->removeAllChildren();
  
  // Inhalte einfügen, wenn vorhanden
  if (obj->inObject2Node.getValue() != NULL) {
    obj->_inputSep2->addChild(obj->inObject2Node.getValue());
  }
  obj->inputChanged();
}
