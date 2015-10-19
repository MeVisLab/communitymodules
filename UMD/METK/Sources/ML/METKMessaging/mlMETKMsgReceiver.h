
//! ML ObjMgrClient module METKMsgReceiver.
//! \file  mlMETKMsgReceiver.h

#ifndef __MLMETKMSGRECEIVER_H
#define __MLMETKMSGRECEIVER_H


// Local includes
#ifndef __UMDInventorPointerSystem_H
#include "METKMessagingSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>


ML_START_NAMESPACE

//! ML ObjMgrClient module METKMsgReceiver.
class METKMESSAGING_EXPORT METKMsgReceiver : public ObjMgrClient
{

public:

   //! Constructor.
   METKMsgReceiver();
   ~METKMsgReceiver();

   StringField* messageFld;
   NotifyField* doneFld;

   //! Handle field changes of the field \c field.
   virtual void handleNotification (Field *field);

   virtual void handleObjMgrNotification();

   virtual void activateAttachments();

   const bool isProcessing() const { return _processing; }

   const ObjMgr* getObjMgr() const;

private:

   typedef ObjMgrClient inherited;

   // ----------------------------------------------------------
   //@{ \name Module field declarations
   // ----------------------------------------------------------

   StringField* dataFld;
   StringField* statusFld;
   //@}


   //! Implements interface for the runtime type system of the ML.
   ML_CLASS_HEADER(METKMsgReceiver);

   bool _processing;

};


ML_END_NAMESPACE

#endif // __mlMETKMsgReceiver_H


