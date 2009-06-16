//----------------------------------------------------------------------------------
//! The ML module class UMDInventorToPointer.
/*!
// \file    mlUMDInventorToPointer.cpp
// \author  Konrad
// \date    2006-02-01
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlUMDInventorToPointer_H
#include "mlUMDInventorToPointer.h"
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(UMDInventorToPointer, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
UMDInventorToPointer::UMDInventorToPointer (void) : inherited(0,0,ObjMgrClient::EVENTS_NONE)
{
   ML_TRACE_IN("UMDInventorToPointer::UMDInventorToPointer()");

   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();

   _pointer = getFieldContainer()->addInt("Pointer");
   inInventor = getFieldContainer()->addSoNode("inInventor");
   fieldObjectID = getFieldContainer()->addString("objectID");
   fieldLayerID = getFieldContainer()->addString("layerID");
   fieldInfoID = getFieldContainer()->addString("infoID");	
   fieldRefresh = getFieldContainer()->addNotify("refresh");

   _pointer->setIntValue(0);
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}

UMDInventorToPointer::~UMDInventorToPointer (void) {
   ML_TRACE_IN("UMDInventorToPointer::~UMDInventorToPointer()");
}


//See 3.1.3. Module Persistence and Overloading activateAttachments()
//in MLGuide
void UMDInventorToPointer::activateAttachments()
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

   _pointer->setIntValue(0);

   sendNotification();

   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void UMDInventorToPointer::handleNotification (Field *field)
{
   ML_TRACE_IN("UMDInventorToPointer::handleNotification(Field *field)");
   inherited::handleNotification(field);

   if (field == inInventor)
   {            
      _pointer->setStringValue(inInventor->getStringValue());
      writePointer();
   }

   else if (field == fieldObjectID)
   {		
      writePointer();
   }

   else if (field == fieldLayerID)
   {		
      writePointer();	
   }

   else if (field == fieldInfoID)
   {
      writePointer();	
   }

   else if (field == fieldRefresh)
   {
      writePointer();
   }
}


void UMDInventorToPointer::writePointer()
{	
   ML_TRACE_IN("UMDInventorToPointer::writePointer()");
   if (fieldObjectID->getStringValue() != "" && 
      fieldLayerID->getStringValue()  != "" && 
      fieldInfoID->getStringValue()   != "")
   {
      // Get writable access to object container
      omObjectContainer* oc = getObjContainer();
      if(oc) {			
         omObject& obj = (*oc)[fieldObjectID->getStringValue()];
         if (&obj) {
            if (!obj.hasAttribute(fieldLayerID->getStringValue(), fieldInfoID->getStringValue())) {
               omAttribute& attrNew = (*oc)[fieldObjectID->getStringValue()][fieldLayerID->getStringValue()][fieldInfoID->getStringValue()];
               attrNew.set_MLint32(_pointer->getIntValue());
               attrNew.flags().markPersistent(false);
            }
            else{
               (*oc)[fieldObjectID->getStringValue()][fieldLayerID->getStringValue()][fieldInfoID->getStringValue()].set_MLint32(_pointer->getIntValue());
            }
            sendNotification();
         }
      }
   }
}



ML_END_NAMESPACE