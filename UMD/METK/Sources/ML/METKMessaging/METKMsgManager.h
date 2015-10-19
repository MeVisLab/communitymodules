
//! The class METKMsgManager.
//! \file  METKMsgManager.h

#ifndef _METKMSGMANAGER_H
#define _METKMSGMANAGER_H

#include "mlMETKMsgReceiver.h"
#include "mlMETKMsgSender.h"

#include <vector>

//! The class METKMsgManager.
class METKMsgManager {

public:
   typedef std::vector<const ml::METKMsgReceiver*> T_ReceiverVec;
   typedef std::vector<const ml::METKMsgReceiver*>::iterator T_ReceiverIterator;
   typedef std::vector<const ml::METKMsgReceiver*>::const_iterator T_ConstReceiverIterator;

   typedef std::vector<const ml::METKMsgSender*> T_SenderVec;
   typedef std::vector<const ml::METKMsgSender*>::iterator T_SenderIterator;
   typedef std::vector<const ml::METKMsgSender*>::const_iterator T_ConstSenderIterator;

   //! this function must be called in the constructor of the calling inventor node
   static const void             add(const ml::METKMsgReceiver* receiver);
   static const void             add(const ml::METKMsgSender* sender);

   //! this function must be called in the destructor of the calling inventor node
   static const void             remove(const ml::METKMsgReceiver* receiver);
   static const void             remove(const ml::METKMsgSender* sender);

   static const int numListener(const ml::METKMsgSender* sender);

protected:
   //! no one can call the con-/ destructor directly. Use the methods add/remove instead
   METKMsgManager();
   virtual ~METKMsgManager();

private:
   //! pointer to the one and only instance of METKMsgManager
   static METKMsgManager* metkMsgManager;

   T_ReceiverVec ReceiverVec;
   T_SenderVec   SenderVec;

};

#endif // _METKMSGMANAGER_H
