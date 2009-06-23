// Local includes
#include "../METK.h"
#include "mlMETKMsgReceiver.h"
#include "METKMsgManager.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKMsgReceiver, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKMsgReceiver::METKMsgReceiver (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
   ML_TRACE_IN("METKMsgReceiver::METKMsgReceiver()");
   
   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();
   
   messageFld = getFieldContainer()->addString("message");
   dataFld    = getFieldContainer()->addString("data");
   statusFld  = getFieldContainer()->addString("status");
   doneFld    = getFieldContainer()->addNotify("done");	
   
   _processing = false;
   METKMsgManager::add(this);

   statusFld->setStringValue("idle");
   clearAcceptedObjectIDs();
   addAcceptedObjectID(OBJ_COMMUNICATION);
   clearAcceptedInfoLayerNames();
   addAcceptedInfoLayerName(LAY_STATUSEVENTS);

   
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}


METKMsgReceiver::~METKMsgReceiver() {
   ML_TRACE_IN("METKMsgReceiver::~METKMsgReceiver()");
   METKMsgManager::remove(this);
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKMsgReceiver::handleNotification (Field *field)
{
   ML_TRACE_IN("METKMsgReceiver::handleNotification(Field *field)");
   inherited::handleNotification(field);
   
   if (field == doneFld && _processing) {
      _processing = false;
      omObjectContainer* oc = getObjContainer();
      if (oc) {
         omObject &obj = (*oc)[OBJ_COMMUNICATION];
         if (obj.isValid()) {
            obj[LAY_STATUSEVENTS][INF_MESSAGE] = messageFld->getStringValue();
            //obj[LAY_STATUSEVENTS][INF_STATUS] = omMessage(MSG_READY);
			obj[LAY_STATUSEVENTS][INF_STATUS] = MSG_READY; //for linux compatibility - not sure if it still sends an omMessage
			//SoDebugError::postInfo("RECEIVER: READY","");
            sendNotification();
            statusFld->setStringValue(MSG_DONE);
         }
      }
   }
}


void METKMsgReceiver::handleObjMgrNotification()
{
   ML_TRACE_IN("METKMsgReceiver::handleObjMgrNotification()");
   if (!_processing)
   {
      omEventContainer myEventList = getEventContainer();
      
      //Durchiterieren der EventList
      omEventContainer::const_iterator iter;		
      for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
      {
         ObjMgrEvent myEvent = (*iter);
         
         //Status has changed
         if (myEvent.infoID == INF_STATUS) {	
            // retrieve data from Object Communcation / Layer Global
            omObjectContainer* oc = getObjContainer();
            if (oc) {
               omObject &obj = (*oc)[OBJ_COMMUNICATION];
               if (obj.isValid()) {
                  const omMessage& status  = obj[LAY_STATUSEVENTS][INF_STATUS];
                  const std_string& message = obj[LAY_STATUSEVENTS][INF_MESSAGE];
                  
                  // request for new process received, checking responsibility
                  if (status == MSG_INIT && message == messageFld->getStringValue()) {
                     // instance is responsible. Setting INF_STATUS to MSG_PROCESSING
                     statusFld->setStringValue(MSG_PROCESSING);
                     obj[LAY_STATUSEVENTS][INF_STATUS] = omMessage(std::string(MSG_PROCESSING)); //see line 64 for linux test
					 //SoDebugError::postInfo("RECEIVER: PROCESSING","");
                     dataFld->setStringValue(obj[LAY_STATUSEVENTS][INF_DATA]);
                     sendNotification();
                     _processing = true;
                  }
               }
            }
         }
      }
      clearEventContainer();
   }
}


//See 3.1.3. Module Persistence and Overloading activateAttachments()
//in MLGuide
//----
//Diese Funktion wird nicht ausgeführt, wenn das Modul per C++ in ML in ein Netz eingefügt wird, sondern scheinbar nur,
//wenn das Modul in ein "echtes" MeVisLab-Netz eingefügt wird.
void METKMsgReceiver::activateAttachments()
{
   ML_TRACE_IN("METKMsgReceiver::activateAttachments()");
   statusFld->setStringValue("idle");
   
   clearAcceptedObjectIDs();
   addAcceptedObjectID(OBJ_COMMUNICATION);
   clearAcceptedInfoLayerNames();
   addAcceptedInfoLayerName(LAY_STATUSEVENTS);
   
   sendNotification();
   
   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}


const ObjMgr* METKMsgReceiver::getObjMgr() const {
   ML_TRACE_IN("METKMsgReceiver::getObjMgr()");
   return _objMgr;
}


ML_END_NAMESPACE

