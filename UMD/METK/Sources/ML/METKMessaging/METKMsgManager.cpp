#include "METKMsgManager.h"

METKMsgManager* METKMsgManager::metkMsgManager = 0;


METKMsgManager::METKMsgManager() {
   ML_TRACE_IN("METKMsgManager::METKMsgManager()");
   // nothing to do
}


METKMsgManager::~METKMsgManager() {
   ML_TRACE_IN("METKMsgManager::~METKMsgManager()");
   // nothing to do
}


const void METKMsgManager::add(const ml::METKMsgReceiver* receiver) {
   ML_TRACE_IN("METKMsgManager::add(const ml::METKMsgReceiver* receiver)");
   // if there is no cache at all, create one
   if (!metkMsgManager)
      metkMsgManager = new METKMsgManager();
   // append receiver
   metkMsgManager->ReceiverVec.push_back(receiver);
}

const void METKMsgManager::add(const ml::METKMsgSender* sender) {
   ML_TRACE_IN("METKMsgManager::add(const ml::METKMsgSender* sender)");
   // if there is no cache at all, create one
   if (!metkMsgManager)
      metkMsgManager = new METKMsgManager();
   // append sender
   metkMsgManager->SenderVec.push_back(sender);
}


const void METKMsgManager::remove(const ml::METKMsgReceiver* receiver) {
   ML_TRACE_IN("METKMsgManager::remove(const ml::METKMsgReceiver* receiver)");
   // if there is a cache
   if (metkMsgManager) {
      T_ReceiverIterator iter;
      for (iter = metkMsgManager->ReceiverVec.begin(); iter != metkMsgManager->ReceiverVec.end(); iter++) {
         if ((*iter) == receiver) {
            metkMsgManager->ReceiverVec.erase(iter);
            //break;
			return;
         }
      }
   }
}


const void METKMsgManager::remove(const ml::METKMsgSender* sender) {
   ML_TRACE_IN("METKMsgManager::remove(const ml::METKMsgSender* sender)");
   // if there is a cache
   if (metkMsgManager) {
      T_SenderIterator iter;
      for (iter = metkMsgManager->SenderVec.begin(); iter != metkMsgManager->SenderVec.end(); iter++) {
         if ((*iter) == sender) {
            metkMsgManager->SenderVec.erase(iter);
            //break;
			return;
         }
      }
   }
}


const int METKMsgManager::numListener(const ml::METKMsgSender* sender) {
   ML_TRACE_IN("METKMsgManager::numListener(const ml::METKMsgSender* sender)");
   T_ConstReceiverIterator iter;
   int listener = 0;
   if (metkMsgManager) {
      const std::string message = sender->messageFld->getStringValue();
      for (iter = metkMsgManager->ReceiverVec.begin(); iter != metkMsgManager->ReceiverVec.end(); iter++) {
		 if (*iter!=NULL) {
			if ((*iter)->isProcessing() == false && 
				(*iter)->messageFld->getStringValue() == message &&
				sender->getObjMgr() == (*iter)->getObjMgr()) {
				listener++;
			}
		 }
		 else { std::cout << "Fatal Error. One of the receiver in metkMsgManager=NULL. That should never happen!" << std::endl; }
      } 
   }
   return listener;
}