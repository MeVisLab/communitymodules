// #############################
// ####
// ####  UMDSoAutoMeasureTool.cpp
// ####
// #############################


#include "UMDSoShapeToPointSet.h"
#include "UMDSoAutoMeasureTool.h"

#include <iostream>

//Poting ILAB4 - new field names
#ifndef ILAB5
#define inObject1Node inputObject
#endif

SO_NODE_SOURCE(UMDSoAutoMeasureTool);

void UMDSoAutoMeasureTool::initClass() { 
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoAutoMeasureTool, SoSeparator, "SoSeparator");
}


UMDSoAutoMeasureTool::UMDSoAutoMeasureTool() {
  SO_NODE_CONSTRUCTOR(UMDSoAutoMeasureTool);
  
  SO_NODE_ADD_FIELD(resultsValid, (FALSE));
  
  SO_NODE_ADD_FIELD(color, (0, 1, 0));
  
  SO_NODE_ADD_FIELD(inObject1Node, (NULL));
  
  SO_NODE_ADD_FIELD(displayToolMeasure, (TRUE));
  SO_NODE_ADD_FIELD(displayUnit, (TRUE));
  SO_NODE_ADD_FIELD(displayName, (FALSE));
  SO_NODE_ADD_FIELD(toolName, (""));

  SO_NODE_ADD_FIELD(displayToolOnlyOnValid, (TRUE));
  SO_NODE_ADD_FIELD(displayInput, (TRUE));

  // Eingaenge des Moduls
  _displayInputSwitch = new SoSwitch;
  addChild(_displayInputSwitch);
  _displayInputSwitch->whichChild.setValue(0);
  SoGroup* group = new SoGroup();
  _displayInputSwitch->addChild(group);
  
  _inputSep1 = new SoSeparator;
  group->addChild(_inputSep1);
  _inputSep2 = new SoSeparator;
  group->addChild(_inputSep2);
  
  // hier kommen Hauptachsen und/oder BoundingBox rein
  _toolSep = new SoSeparator;
  addChild(_toolSep);
  
  _displayToolOnlyOnValidSensor = new SoFieldSensor(outputChangedCB, this);
  _displayToolOnlyOnValidSensor->attach(&displayToolOnlyOnValid);
  _displayInputSensor = new SoFieldSensor(outputChangedCB, this);
  _displayInputSensor->attach(&displayInput);
  
  _inputObjectSensor = new SoFieldSensor(inputChangedCB, this);
  _inputObjectSensor->attach(&inObject1Node);
}


UMDSoAutoMeasureTool::~UMDSoAutoMeasureTool() {
  // ein bischen aufraeumen
  if (_displayToolOnlyOnValidSensor) { delete _displayToolOnlyOnValidSensor; _displayToolOnlyOnValidSensor = NULL; }
  if (_displayInputSensor)           { delete _displayInputSensor;           _displayInputSensor = NULL; }
  if (_inputObjectSensor)            { delete _inputObjectSensor;            _inputObjectSensor = NULL; }
}


void UMDSoAutoMeasureTool::inputChangedCB(void* userData, SoSensor*) {
  // Call the member function.
  ((UMDSoAutoMeasureTool*) userData)->inputChanged();
}
void UMDSoAutoMeasureTool::inputChanged() {
  
  resultsValid.setValue(FALSE);

  _inputSep1->removeAllChildren();

  if (inObject1Node.getValue() != NULL) {
    // Inhalt einhängen, wenn vorhanden
    _inputSep1->addChild(inObject1Node.getValue());   
  }
  if(calculate()) {
    resultsValid.setValue(TRUE);
  }
  outputChanged();
}


void UMDSoAutoMeasureTool::outputChangedCB(void* userData, SoSensor*) {
  // Call the member function.
  ((UMDSoAutoMeasureTool*) userData)->outputChanged();
}
void UMDSoAutoMeasureTool::outputChanged() {

  _toolSep->removeAllChildren();

  
  if (displayInput.getValue() == TRUE)
    _displayInputSwitch->whichChild.setValue(0);
  else
    _displayInputSwitch->whichChild.setValue(-1);

  if (displayToolOnlyOnValid.getValue() == TRUE) {
    if (resultsValid.getValue() == TRUE)
      display();
  }
  else
    display();
}


void UMDSoAutoMeasureTool::getPointSet(SoGroup* inputGroup, float* &pointSet, long &size) {
  
  // Punktmenge für beide Eingänge einlesen
  if (inputGroup->getNumChildren() != 0) {
    UMDSoShapeToPointSet* shapeToPointSet = new UMDSoShapeToPointSet;
    shapeToPointSet->ref();
    //shapeToPointSet->isUnique.setValue(isUnique.getValue());
    shapeToPointSet->rootInput.setValue(inputGroup);
    shapeToPointSet->calculate();
    size = shapeToPointSet->vertexProperty->vertex.getNum();
    if (size != 0) {
      pointSet = new float[size * 3];
      int counter2 = 0;
      const SbVec3f* pSbVec3f = shapeToPointSet->vertexProperty->vertex.getValues(0);
      for (int counter = 0; counter < size; counter++) {
        pointSet[counter2++] = pSbVec3f->getValue()[0];
        pointSet[counter2++] = pSbVec3f->getValue()[1];
        pointSet[counter2++] = pSbVec3f->getValue()[2];
        pSbVec3f++;
      }
    }
    shapeToPointSet->unref();
  }
}


bool UMDSoAutoMeasureTool::calculate() {
    bool success = _calculate();
    resultsValid.setValue(success);
    return success;
}
