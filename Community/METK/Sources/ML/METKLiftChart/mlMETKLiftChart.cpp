//----------------------------------------------------------------------------------
//! The ML module class UMDPointerToInventor.
/*!
// \file    mlUMDPointerToInventor.cpp
// \author  Konrad
// \date    2006-02-02
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMETKLiftChart.h"
#include "SoView2DLiftChart.h"
#include "LiftChartData.h"
#include "../METK.h"

#include <ObjMgr/mlObjMgrClient.h>
#include "mlAPI.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTranslation.h>
#include "XVLeaveScope.h"

ML_START_NAMESPACE


//----------------------------------------------------------------------------------
//! 1. Select structures to display
//! 2. Select a column to display the structures
//! 3. Select a rendering order
//----------------------------------------------------------------------------------

//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKLiftChart, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKLiftChart::METKLiftChart (void) : inherited(0,0,ObjMgrClient::EVENTS_ALL)
{
   ML_TRACE_IN("METKLiftChart::METKLiftChart()");

   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();

   static const char* aggregationModes[4] = {"All","Structure","StructureGroup","None"};

   _active          = getFieldContainer()->addBool("active");
   _verticalOffset = getFieldContainer()->addInt("verticalOffset");

   _aggregation    = getFieldContainer()->addEnum("aggregation", aggregationModes, 4);

   _minImportance = getFieldContainer()->addFloat("minImportance");
   _maxExtent     = getFieldContainer()->addFloat("maxExtent");

   _divideBySide   = getFieldContainer()->addBool("divideBySide");
   _hideSideUnkown = getFieldContainer()->addBool("hideSideUnkown");

   _outInventor = getFieldContainer()->addSoNode("outInventor");


   _active->setBoolValue(true);
   _verticalOffset->setIntValue(100);
   _aggregation->setStringValue("Structure");

   _minImportance->setFloatValue(0.5);
   _maxExtent->setFloatValue(0.75);

   _divideBySide->setBoolValue(false);
   _hideSideUnkown->setBoolValue(false);

   _liftChartData = new LiftChartData;

   _view2DLiftChart = new SoView2DLiftChart;
   _view2DLiftChart->ref();
   _view2DLiftChart->setLiftChartData(_liftChartData);
   _outInventor->setSoNodeValue(_view2DLiftChart);

   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}

METKLiftChart::~METKLiftChart() {
   ML_TRACE_IN("METKLiftChart::~METKLiftChart()");
   _view2DLiftChart->unref();
   if (_liftChartData) { delete _liftChartData; _liftChartData = 0; }
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKLiftChart::handleNotification(Field *field) {
   ML_TRACE_IN("METKLiftChart::handleNotification(Field *field)");
   inherited::handleNotification(field);

   if (field == _verticalOffset) {
      _view2DLiftChart->verticalOffset.setValue(_verticalOffset->getIntValue());
   }

   if (field == _minImportance) {
      _liftChartData->setMinImportance(_minImportance->getFloatValue());
   }

   if (field == _maxExtent) {
      _liftChartData->setMaxExtent(_maxExtent->getFloatValue());
   }

   if (field == _divideBySide) {
      _liftChartData->setDivideBySide(_divideBySide->getBoolValue());
   }

   if (field == _hideSideUnkown) {
      _liftChartData->setHideSideUnkown(_hideSideUnkown->getBoolValue());
   }

   if (field == _aggregation) {
      if      (_aggregation->getStringValue() == "All")
         _liftChartData->setAggregation(LiftChartData::AGGREGATE_ALL);
      else if (_aggregation->getStringValue() == "Structure")
         _liftChartData->setAggregation(LiftChartData::AGGREGATE_STRUCTURE);
      else if (_aggregation->getStringValue() == "StructureGroup")
         _liftChartData->setAggregation(LiftChartData::AGGREGATE_STRUCTUREGROUP);
      else if (_aggregation->getStringValue() == "None")
         _liftChartData->setAggregation(LiftChartData::AGGREGATE_NONE);
   }

   _view2DLiftChart->drawingOn.setValue(_active->getBoolValue());

   _liftChartData->sortStructures();
   _view2DLiftChart->touch();
}




void METKLiftChart::handleObjMgrNotification() {
   ML_TRACE_IN("METKLiftChart::handleObjMgrNotification()");
   omEventContainer myEventList = getEventContainer();
   omEventContainer::const_iterator iter;		

   //Durchiterieren der EventList
   for ( iter = myEventList.begin();iter!=myEventList.end(); iter++) {
      ObjMgrEvent myEvent = (*iter);

      // TODO: Structure added?

      //Status has changed
      if (myEvent.infoID == INF_NAME || myEvent.infoID == INF_SIDE || myEvent.infoID == INF_VISIBLE || 
         myEvent.infoID == INF_COLOR || myEvent.infoID == INF_BBMIN || myEvent.infoID == INF_BBMAX  || 
         myEvent.infoID == INF_TRANSPARENCY || myEvent.infoID == INF_IMPORTANCE) {
            const omObjectContainer* oc = getObjContainer();
            if (oc) {
               const omObject &obj = (*oc)[myEvent.objectID];
               if (obj.isValid()) {
                  _liftChartData->cleanup();
                  buildLiftChart();
               }
            }
         }
      else if (myEvent.infoID == INF_CASELOADED) {
         const omObjectContainer* oc = getObjContainer();
         if (oc) {
            const omObject &obj = (*oc)[myEvent.objectID];
            if (obj.isValid()) {
               const std::string status = obj[LAY_GLOBALEVENTS][INF_CASELOADED];
               if (status == MSG_LOADED) {
                  _liftChartData->cleanup();
                  buildLiftChart();
               }
               else if (status == MSG_CLEANUP) {
                  _liftChartData->cleanup();
               }
            }
         }
      }
   }
   clearEventContainer();
}



//See 3.1.3. Module Persistence and Overloading activateAttachments()
//in MLGuide
void METKLiftChart::activateAttachments() {
   ML_TRACE_IN("METKLiftChart::activateAttachments()");
   // Implement your update stuff here ...	
   clearAcceptedObjectIDs();
   clearAcceptedInfoLayerNames();
   sendNotification();

   handleNotificationOff();

   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();

   _liftChartData->setMinImportance(_minImportance->getFloatValue());
   _liftChartData->setMaxExtent(_maxExtent->getFloatValue());
   _liftChartData->setDivideBySide(_divideBySide->getBoolValue());
   _liftChartData->setHideSideUnkown(_hideSideUnkown->getBoolValue());
   if      (_aggregation->getStringValue() == "All")
      _liftChartData->setAggregation(LiftChartData::AGGREGATE_ALL);
   else if (_aggregation->getStringValue() == "Structure")
      _liftChartData->setAggregation(LiftChartData::AGGREGATE_STRUCTURE);
   else if (_aggregation->getStringValue() == "StructureGroup")
      _liftChartData->setAggregation(LiftChartData::AGGREGATE_STRUCTUREGROUP);
   else if (_aggregation->getStringValue() == "None")
      _liftChartData->setAggregation(LiftChartData::AGGREGATE_NONE);
   _aggregation->notifyAttachments();

   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}


void METKLiftChart::buildLiftChart() {
   ML_TRACE_IN("METKLiftChart::buildLiftChart()");
   const omObject* obj = getNextStructure();
   while (obj != 0){
      Structure* structure = _liftChartData->addStructure();
      structure->setName((*obj)[LAY_DESCRIPTION][INF_NAME]);
      structure->setStructure((*obj)[LAY_DESCRIPTION][INF_STRUCTURE]);
      structure->setStructureGroup((*obj)[LAY_DESCRIPTION][INF_STRUCTUREGROUP]);
      structure->setVisible((*obj)[LAY_APPEARANCE][INF_VISIBLE].get_bool());
      if ((*obj).hasAttribute(LAY_DESCRIPTION, INF_SIDE)) {
         if ((*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("Left") || (*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("left")) {
            structure->setSide(Structure::LEFT);
         } else if ((*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("Right") || (*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("right")) {
            structure->setSide(Structure::RIGHT);
         } else if ((*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("Ambilateral") || (*obj)[LAY_DESCRIPTION][INF_SIDE] == std::string("ambilateral")) {
            structure->setSide(Structure::AMBILATERAL);
         } else
            structure->setSide(Structure::UNKNOWN);
      } else
         structure->setSide(Structure::UNKNOWN);

      float fmin[3], fmax[3], frgba[4];
      const vec3 min = (*obj)[LAY_MEASURES][INF_BBMIN].get_vec3();
      const vec3 max = (*obj)[LAY_MEASURES][INF_BBMAX].get_vec3();
      fmin[0] = min[0];
      fmin[1] = min[1];
      fmin[2] = min[2];
      fmax[0] = max[0];
      fmax[1] = max[1];
      fmax[2] = max[2];
      structure->setExtension(fmin, fmax);

      if ((*obj).hasAttribute(LAY_APPEARANCE,INF_COLOR)) {
         const vec3 col = (*obj)[LAY_APPEARANCE][INF_COLOR].get_vec3();
         frgba[0] = col[0];
         frgba[1] = col[1];
         frgba[2] = col[2];
      } else {
         frgba[0] = 0.8f;
         frgba[1] = 0.8f;
         frgba[2] = 0.8f;
      }
      if ((*obj).hasAttribute(LAY_APPEARANCE,INF_TRANSPARENCY)) {
         const float tra = (*obj)[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
         frgba[3] = tra;
      } else {
         frgba[3] = 1.0f;
      }
      structure->setRGBA(frgba);

      if ((*obj).hasAttribute(LAY_DESCRIPTION, INF_IMPORTANCE)) {
         structure->setImportance((*obj)[LAY_DESCRIPTION][INF_IMPORTANCE].get_double());
      } else if ((*obj).hasAttribute(LAY_APPEARANCE,INF_TRANSPARENCY)) {
         structure->setImportance(1.0 - (*obj)[LAY_APPEARANCE][INF_TRANSPARENCY].get_double());
      } else {
         structure->setImportance(1.0f);
      }

      obj = getNextStructure(obj);
   }
   _liftChartData->sortStructures();
   _view2DLiftChart->touch();
}


// next available structure which provides all necessary data
const omObject* METKLiftChart::getNextStructure(const omObject* current) {
   ML_TRACE_IN("METKLiftChart::getNextStructure(const omObject* current)");
   const omObjectContainer* oc = getObjContainer();
   omObjectContainer::const_iterator iter = oc->begin();
   if (oc) {
      if (current != 0) {
         // search for the next structure
         while (iter != oc->end() && &iter->second != current) {
            iter++;
         }
         iter++;
      }
      while (iter != oc->end()) {
         if (iter->second.isValid()) {
            if (iter->second.exists(LAY_APPEARANCE)) {
               if (iter->second.exists(LAY_MEASURES)) {
                  bool bbMinValid = iter->second.hasAttribute(LAY_MEASURES, INF_BBMIN);
                  bool bbMaxValid = iter->second.hasAttribute(LAY_MEASURES, INF_BBMAX);
                  if (bbMinValid && bbMaxValid) {
                     // next structure found
                     return &iter->second;
                  }
               }
            }
         }
         iter++;
      }
   }
   return 0;
}


ML_END_NAMESPACE

