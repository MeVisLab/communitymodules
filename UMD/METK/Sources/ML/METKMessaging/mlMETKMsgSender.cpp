// Local includes
#include "../METK.h"
#include "mlMETKMsgSender.h"
#include "METKMsgManager.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKMsgSender, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKMsgSender::METKMsgSender (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
   ML_TRACE_IN("METKMsgSender::METKMsgSender()");
   
   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();
   
   messageFld = getFieldContainer()->addString("message");
   dataFld    = getFieldContainer()->addString("data");
   statusFld  = getFieldContainer()->addString("status");
   sendFld    = getFieldContainer()->addNotify("send");	
   doneFld    = getFieldContainer()->addBool("done");	
   successFld = getFieldContainer()->addBool("successful");	
   
   _sending = false;
   activateAttachments(); //dass muss hier von Hand aufgerufen werden, das es sonst bei Sender-Modulen, die per ML/C++ in Netzwerke eingefügt werden nicht ausgeführt wird
   METKMsgManager::add(this);
   
   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}


void METKMsgSender::activateAttachments()
{
   ML_TRACE_IN("METKMsgSender::activateAttachments()");
   statusFld->setStringValue("idle");   
   successFld->setBoolValue(false);
   doneFld->setBoolValue(false); //done immer erst nach success als letztes setzen
   
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


METKMsgSender::~METKMsgSender() {
   ML_TRACE_IN("METKMsgSender::~METKMsgSender()");
   METKMsgManager::remove(this);
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKMsgSender::handleNotification (Field *field)
{
   ML_TRACE_IN("METKMsgSender::handleNotification");
   inherited::handleNotification(field);

   if (field == sendFld && !_sending) {

      omObjectContainer* oc = getObjContainer();
      if (oc) {
         omObject &obj = (*oc)[OBJ_COMMUNICATION];
         if (obj.isValid()) {
            if (METKMsgManager::numListener(this) > 0) {
               _sending = true;
			   //std::cout<<"SENDER: _sending=true "<<std::endl;
               obj[LAY_STATUSEVENTS][INF_MESSAGE] = messageFld->getStringValue();
               obj[LAY_STATUSEVENTS][INF_DATA] = dataFld->getStringValue();
               obj[LAY_STATUSEVENTS][INF_STATUS] = omMessage(ML_NAMESPACE::std_string(MSG_INIT));
			   //std::cout<<"SENDER: INIT"<<std::endl;
               sendNotification();
               statusFld->setStringValue(MSG_PROCESSING);               
               successFld->setBoolValue(false);
			   doneFld->setBoolValue(false); //done immer erst nach success als letztes setzen
            } else {               
               statusFld->setStringValue("Nobody's listening.");
               obj[LAY_STATUSEVENTS][INF_STATUS] = omMessage(ML_NAMESPACE::std_string(MSG_DONE));
			   //std::cout<<"SENDER: DONE   NOBODYs LISTENING"<<std::endl;
               sendNotification();
               successFld->setBoolValue(false);
			   doneFld->setBoolValue(true); //done immer erst nach success als letztes setzen
            }
         }
      }
   }
}


void METKMsgSender::handleObjMgrNotification()
{
   ML_TRACE_IN("METKMsgSender::handleObjMgrNotification()");
   /*SoDebugError::postInfo("METKMsgSender::handleObjMgrNotification",messageFld->getStringValue().c_str());
   if (_sending)
	   SoDebugError::postInfo("_sending","");
	else
		SoDebugError::postInfo("not _sending","");
		*/
   if (_sending) {
      omEventContainer myEventList = getEventContainer();
      	  
      //Durchiterieren der EventList
      omEventContainer::const_iterator iter;		
      for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
      {
         ObjMgrEvent myEvent = (*iter);
         
		 //SoDebugError::postInfo("METKMsgSender::handleObjMgrNotification in _sending     ",myEvent.infoID.c_str());

         //Status has changed
         if (myEvent.infoID == INF_STATUS) {	
            // retrieve data from Object Communcation / Layer Global
            omObjectContainer* oc = getObjContainer();
            if (oc) {
               omObject &obj = (*oc)[OBJ_COMMUNICATION];
               if (obj.isValid()) {
                  const omMessage& status  = obj[LAY_STATUSEVENTS][INF_STATUS];
                  const std_string& message = obj[LAY_STATUSEVENTS][INF_MESSAGE];
                  
				  //SoDebugError::postInfo("METKMsgSender::handleObjMgrNotification in _sending STATUS   ",(status + " " + message).c_str());

                  // request for new process received, checking responsibility
                  if (status == MSG_READY && message == messageFld->getStringValue()) {
                     // instance is responsible. Setting INF_STATUS to MSG_DONE
                     obj[LAY_STATUSEVENTS][INF_STATUS] = omMessage(ML_NAMESPACE::std_string(MSG_DONE));
					 //SoDebugError::postInfo("SENDER: DONE","");
                     sendNotification();
                     _sending = false;
					 //SoDebugError::postInfo("SENDER: _sending=false ","");
                     
					 statusFld->setStringValue(MSG_DONE);
                     successFld->setBoolValue(true);
					 doneFld->setBoolValue(true); //done immer erst nach success als letztes setzen
                  }
               }
            }
         }
      }
      clearEventContainer();
   }
}


const ObjMgr* METKMsgSender::getObjMgr() const {
   ML_TRACE_IN("METKMsgSender::getObjMgr()");
   return _objMgr;
}

ML_END_NAMESPACE

