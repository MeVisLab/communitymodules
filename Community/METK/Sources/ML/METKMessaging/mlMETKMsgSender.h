#ifndef __MLMETKMSGSENDER_H
#define __MLMETKMSGSENDER_H


// Local includes
#ifndef __METKMessagingSystem_H
#include "METKMessagingSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>


ML_START_NAMESPACE


//! 
class METKMESSAGING_EXPORT METKMsgSender : public ObjMgrClient
{

public:

	//! Constructor.
	METKMsgSender();
	~METKMsgSender();

 	StringField* messageFld;
	StringField* dataFld;
	StringField* statusFld;
	NotifyField* sendFld;	

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);

	virtual void handleObjMgrNotification();

	virtual void activateAttachments();

   const bool isSending() const { return _sending; }

   const ObjMgr* getObjMgr() const;

private:

	typedef ObjMgrClient inherited;

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------
	
   BoolField*   doneFld;
   BoolField*   successFld;
   //@}


	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(METKMsgSender)

	bool _sending;

};


ML_END_NAMESPACE

#endif // __mlMETKMsgSender_H


