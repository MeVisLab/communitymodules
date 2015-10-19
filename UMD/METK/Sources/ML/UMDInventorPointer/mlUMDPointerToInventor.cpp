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
#ifndef __mlUMDPointerToInventor_H
#include "mlUMDPointerToInventor.h"
#endif

#include <Inventor/nodes/SoGroup.h>

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(UMDPointerToInventor, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
UMDPointerToInventor::UMDPointerToInventor (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
   ML_TRACE_IN("UMDPointerToInventor::UMDPointerToInventor()");
   
   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();
   
   fieldPointer = getFieldContainer()->addInt("Pointer");
   outInventor = getFieldContainer()->addSoNode("outInventor");
   fieldObjectID = getFieldContainer()->addString("objectID");
   fieldLayerID = getFieldContainer()->addString("layerID");
   fieldInfoID = getFieldContainer()->addString("infoID");	
   
   
   addAcceptedObjectID("*");
   addAcceptedInfoLayerName("*");
   
   _myPointer = 0;
   fieldPointer->setIntValue(0);
   
   outInventor->setSoNodeValue(0);
   
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}

UMDPointerToInventor::~UMDPointerToInventor (void) {
   ML_TRACE_IN("UMDPointerToInventor::~UMDPointerToInventor()");
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void UMDPointerToInventor::handleNotification(Field *field)
{
   ML_TRACE_IN("UMDPointerToInventor::handleNotification(Field *field)");
   inherited::handleNotification(field);
   if (field == fieldObjectID)
   {
      clearAcceptedObjectIDs();
      if (fieldObjectID->getStringValue()!="")
         addAcceptedObjectID(fieldObjectID->getStringValue());
      getPointer();
      
   }
   
   else if (field == fieldLayerID)
   {
      clearAcceptedInfoLayerNames();
      if (fieldLayerID->getStringValue()!="")
         addAcceptedInfoLayerName(fieldLayerID->getStringValue());
      getPointer();
   }
   
   else if (field == fieldInfoID)
   {
      getPointer();	
   }	
}




void UMDPointerToInventor::handleObjMgrNotification()
{
   ML_TRACE_IN("UMDPointerToInventor::handleObjMgrNotification()");
   omEventContainer myEventList = getEventContainer();
   
   //Durchiterieren der EventList
   omEventContainer::const_iterator iter;		
   for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
   {
      ObjMgrEvent myEvent = (*iter);
      
      if (myEvent.infoID==fieldInfoID->getStringValue())
      {	
         getPointer();
      }
   }

   clearEventContainer();
}


//See 3.1.3. Module Persistence and Overloading activateAttachments()
//in MLGuide
void UMDPointerToInventor::activateAttachments()
{
   ML_TRACE_IN("UMDPointerToInventor::activateAttachments()");
   // Implement your update stuff here ...	
   clearAcceptedObjectIDs();
   if (fieldObjectID->getStringValue()!="")
   {
      addAcceptedObjectID(fieldObjectID->getStringValue());
   }
   clearAcceptedInfoLayerNames();
   if (fieldLayerID->getStringValue()!="")
   {
      addAcceptedInfoLayerName(fieldLayerID->getStringValue());		
   }
   
   fieldPointer->setIntValue(0);
   outInventor->setSoNodeValue(0);
   
   sendNotification();
   
   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}



void UMDPointerToInventor::getPointer()
{
   ML_TRACE_IN("UMDPointerToInventor::getPointer()");
   _myPointer = 0;		
   
   if (fieldObjectID->getStringValue()!="" && fieldLayerID->getStringValue()!="" && fieldInfoID->getStringValue()!="")
   {
      //Get read-only access to object container
      const omObjectContainer* oc = getConstObjContainer();
      if(oc) {
         // Get read-only access to object with given ID.
         const omObject &obj= (*oc)[fieldObjectID->getStringValue()];
         if (obj.isValid())
         {
            bool pValid = obj.hasAttribute(fieldLayerID->getStringValue(), fieldInfoID->getStringValue());
            if (pValid){
               _myPointer = obj[fieldLayerID->getStringValue()][fieldInfoID->getStringValue()].get_MLint32();
               #pragma message("error in UMDPointerToInventor::getPointer(): This function will fail for pointers larger than 2^31 and requires porting (see e.g. MLStringToPtr())!")
               #ifdef MEVIS_64BIT
                 ML_PRINT_ERROR("UMDPointerToInventor::getPointer()", 
                                ML_BAD_DATA_TYPE, 
                                "This function will fail for pointers larger than 2^31 and requires porting (see e.g. MLStringToPtr())!");
               #endif
            }
               
         }
      }		
   }
   
   if (fieldPointer->getIntValue() != _myPointer) {
      fieldPointer->setIntValue(_myPointer);
      outInventor->setSoNodeValue((SoNode*)_myPointer);
   }
}


ML_END_NAMESPACE