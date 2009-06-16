// Local includes
#include "../METK.h"
#include "mlMETKGlobalMessages.h"
#include "METKMsgManager.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKGlobalMessages, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor / Destructor
//----------------------------------------------------------------------------------
METKGlobalMessages::METKGlobalMessages (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
   ML_TRACE_IN("METKGlobalMessages::METKGlobalMessages()");
   
   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();
   
   loadedFld       = getFieldContainer()->addNotify("loaded");	
   cleanupFld      = getFieldContainer()->addNotify("cleanup");
   lastMessageFld  = getFieldContainer()->addString("lastMessage");
   
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}


void METKGlobalMessages::activateAttachments()
{
   lastMessageFld->setStringValue("None received.");
   
   clearAcceptedObjectIDs();
   addAcceptedObjectID(OBJ_COMMUNICATION);
   clearAcceptedInfoLayerNames();
   addAcceptedInfoLayerName(LAY_GLOBALEVENTS);
   
   sendNotification();
   
   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}



//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKGlobalMessages::handleNotification (Field *field)
{
   inherited::handleNotification(field);
}


void METKGlobalMessages::handleObjMgrNotification()
{
   omEventContainer myEventList = getEventContainer();
   
   //Durchiterieren der EventList
   omEventContainer::const_iterator iter;		
   for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
   {
      ObjMgrEvent myEvent = (*iter);
      
      //Status has changed
      if (myEvent.infoID == INF_CASELOADED) {	
         // retrieve data from Object Communcation / Layer Global
         const omObjectContainer* oc = getObjContainer();
         if (oc) {
            const omObject &obj = (*oc)[OBJ_COMMUNICATION];
            if (obj.isValid()) {
               const omMessage& message = obj[LAY_GLOBALEVENTS][INF_CASELOADED];
               
               if (message == MSG_LOADED) {
                  loadedFld->notify();
                  lastMessageFld->setStringValue(MSG_LOADED);
               }
               else if (message == MSG_CLEANUP) {
                  cleanupFld->notify();
                  lastMessageFld->setStringValue(MSG_CLEANUP);
               }
            }
         }
      }
   }
   clearEventContainer();
}


ML_END_NAMESPACE

