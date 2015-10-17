#include "SoFileToSceneGraph.h"
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/actions/SoSearchAction.h>

#include <Inventor/nodes/SoFile.h>

SO_NODE_SOURCE(SoFileToSceneGraph);

SoFileToSceneGraph::SoFileToSceneGraph()
{
   SO_NODE_CONSTRUCTOR(SoFileToSceneGraph);

   SO_NODE_ADD_FIELD(inputObject, (0));
   SO_NODE_ADD_FIELD(outConvertedScene, (0));

   _inputObjectSensor = new SoFieldSensor(inputChangedCB, this);
   _inputObjectSensor->attach(&inputObject);
   _inputSep = new SoSeparator;

   _searchAction = new SoSearchAction();
   _searchAction->setInterest(SoSearchAction::ALL);
   _searchAction->setType(SoFile::getClassTypeId());

   addChild(_inputSep);
}


void SoFileToSceneGraph::inputChangedCB(void* userData, SoSensor*) {
   // Call the member function.
   ((SoFileToSceneGraph*) userData)->inputChanged();
}
void SoFileToSceneGraph::inputChanged() {
   if (_inputSep->getNumChildren() > 0) {
      _inputSep->removeAllChildren();
      outConvertedScene.setValue(0);
   }

   if (inputObject.getValue() != 0) {
      // Inhalt einhängen, wenn vorhanden
      if (inputObject.getValue()->isOfType(SoFile::getClassTypeId())) {
         _inputSep->addChild(((SoFile*)inputObject.getValue())->copyChildren());
      }
      else {
         // alle SoFiles suchen
         _searchAction->apply(inputObject.getValue());
         SoPathList pList = _searchAction->getPaths();

         if (pList.getLength() > 0) {
            // alle SoFiles ersetzen mit Kopie ihres momentanen Inhaltes
            for (int i = 0; i < pList.getLength(); ++i) {
               SoFile* file = (SoFile*)pList[i]->getTail();
               SoGroup* copy = file->copyChildren();
               ((SoGroup*)pList[i]->getNodeFromTail(1))->replaceChild(file, copy);
               //_inputSep->addChild(((SoFile*)inputObject.getValue())->copyChildren());
            }
         }
         _inputSep->addChild(inputObject.getValue());
      }
      outConvertedScene.setValue(inputObject.getValue());
      }
   }

   SoFileToSceneGraph::~SoFileToSceneGraph() {
      if (_inputObjectSensor) { delete _inputObjectSensor; _inputObjectSensor = 0; }
      if (_searchAction) { delete _searchAction; _searchAction = 0; }
   }

   void
      SoFileToSceneGraph::initClass() {
         SO_NODE_INIT_CLASS(SoFileToSceneGraph, SoSeparator, "SoSeparator");
   }
