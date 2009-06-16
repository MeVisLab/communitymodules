//----------------------------------------------------------------------------------
//! The ML module class UMDBoundingBoxCalculator2.
/*!
// \file    mlUMDBoundingBoxCalculator2.cpp
// \author  Konrad
// \date    2006-05-16
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlUMDBoundingBoxCalculator2_H
#include "mlUMDBoundingBoxCalculator2.h"
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(UMDBoundingBoxCalculator2, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
UMDBoundingBoxCalculator2::UMDBoundingBoxCalculator2 (void) : BaseOp(0, 0)
{
   ML_TRACE_IN("UMDBoundingBoxCalculator2::UMDBoundingBoxCalculator2()");
   
   FieldContainer *fields = getFieldContainer();
   
   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();
   
   (boundingBoxString = fields->addString("boundingBoxString"))->setStringValue("");
   (boundingBoxMin = fields->addVec3f("boundingBoxMin"))->setStringValue("");
   (boundingBoxMax = fields->addVec3f("boundingBoxMax"))->setStringValue("");
   (objectCenter = fields->addVec3f("objectCenter"))->setStringValue("");
   (boxCenter = fields->addVec3f("boxCenter"))->setStringValue("");
   (inputObject = fields->addSoNode("inputObject"));
   
   _moduleComputeBoundingBox = new SoComputeBoundingBox();
   _moduleComputeBoundingBox->ref();
   _moduleCalculator = new SoCalculator();
   _moduleCalculator->ref();
   
   _moduleCalculator->A.connectFrom(&_moduleComputeBoundingBox->min);
   _moduleCalculator->B.connectFrom(&_moduleComputeBoundingBox->max);
   _moduleCalculator->C.connectFrom(&_moduleComputeBoundingBox->boxCenter);
   _moduleCalculator->D.connectFrom(&_moduleComputeBoundingBox->objectCenter);
   
   _minSensor = new SoFieldSensor(UMDBoundingBoxCalculator2::minChangedCB, this);
   _minSensor->attach(&_moduleCalculator->A);
   _minSensor->attach(&_moduleCalculator->B);
   _minSensor->attach(&_moduleCalculator->C);
   _minSensor->attach(&_moduleCalculator->D);
   
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}


UMDBoundingBoxCalculator2::~UMDBoundingBoxCalculator2() {
   _moduleComputeBoundingBox->unref();
   _moduleCalculator->unref();
   if (_minSensor) { delete _minSensor; _minSensor = 0; }
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void UMDBoundingBoxCalculator2::handleNotification (Field *field)
{
   ML_TRACE_IN("UMDBoundingBoxCalculator2::handleNotification()")
      
      if (field==inputObject)
      {
         _moduleComputeBoundingBox->node.setValue(inputObject->getSoNodeValue());
      }
      
}



//-----------------------------------------------------------------------------
// Forwards call to minChanged
//-----------------------------------------------------------------------------
void UMDBoundingBoxCalculator2::minChangedCB(void *data, SoSensor* sens)
{
   ((UMDBoundingBoxCalculator2*) data)->minChanged((SoFieldSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called when field \c min changed
//-----------------------------------------------------------------------------
void UMDBoundingBoxCalculator2::minChanged(SoFieldSensor* sensor)
{	
   SbString temp;
   
   _moduleCalculator->A.get(temp);
   boundingBoxMin->setStringValue(temp.getString());
   
   _moduleCalculator->B.get(temp);	
   boundingBoxMax->setStringValue(temp.getString());
   
   _moduleCalculator->C.get(temp);
   boxCenter->setStringValue(temp.getString());
   
   _moduleCalculator->D.get(temp);	
   objectCenter->setStringValue(temp.getString());
   
   boundingBoxString->setStringValue(kBasics::FloatToString(boundingBoxMin->getVec3fValue()[0]) + " " + kBasics::FloatToString(boundingBoxMin->getVec3fValue()[1]) + " " + kBasics::FloatToString(boundingBoxMin->getVec3fValue()[2]) +
      " 0 0 0 " + 
      kBasics::FloatToString(boundingBoxMax->getVec3fValue()[0]) + " " + kBasics::FloatToString(boundingBoxMax->getVec3fValue()[1]) + " " + kBasics::FloatToString(boundingBoxMax->getVec3fValue()[2]) +
      " 0 0 0" );
}

ML_END_NAMESPACE


