#ifndef __MLMETKGLOBALMESSAGES_H
#define __MLMETKGLOBALMESSAGES_H


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


//! 
class METKMESSAGING_EXPORT METKGlobalMessages : public ObjMgrClient
{
public:

	//! Constructor.
	METKGlobalMessages();

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);

	virtual void handleObjMgrNotification();

	virtual void activateAttachments();


private:

	typedef ObjMgrClient inherited;

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------
	
	NotifyField* loadedFld;	
   NotifyField* cleanupFld;
	StringField* lastMessageFld;
   //@}

	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(METKGlobalMessages)
};


ML_END_NAMESPACE

#endif // __MLMETKGLOBALMESSAGES_H


