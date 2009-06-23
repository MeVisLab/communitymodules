//----------------------------------------------------------------------------------
//! The ML module class METKIntraOPViewpoint.
/*!
// \file    mlMETKIntraOPViewpoint.h
// \author  Konrad Mühler
// \date    2007-07-30
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKIntraOPViewpoint_H
#define __mlMETKIntraOPViewpoint_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include "kBasics.h"
#include "METK.h"
#include "METKMsgManager.h"
#include "ObjMgrCommunicator.h"
#include "UMDAnimation2/AnimationCache.h"
#include "ObjConstrainedIterator/mlObjConstrainedIterator.h"



ML_START_NAMESPACE


//!
class VIEWPOINT_EXPORT METKIntraOPViewpoint : public ObjMgrClient
{
public:

	//! Constructor.
	METKIntraOPViewpoint (void);
	~METKIntraOPViewpoint (void);

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);
	virtual void handleObjMgrNotification();
	virtual void activateAttachments();

private:

	typedef ObjMgrClient inherited;
  
	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//!
	NotifyField *_GenerateVPForNewTumor;

	//!
	Vec3fField *_NewTumorCenter;

	//!
	DoubleField *_NewTumorRadius;

	//!
	NotifyField *_GenerateVPForAllTumors;

	NotifyField* _init;

	BoolField* _initDone;

	//@}


	ObjMgrCommunicator* myObjMgr;
	AnimationCache* myCache;
	ObjConstrainedIterator* myObjIterator;

	bool waitingFor_getAllMeasures;
	bool waitingFor_getCamPos;
	bool gotCamPos;	
	
	//! \name Facilities to get and wait for all measures
	//@{
		METKMsgSender* _modMsgSenderGetCamPos;
		BoolField* _msgGetCamPos_done;		 //!< Listener to receive "done"-Field of _modMsgSenderGetCamPos		
		FieldSensor* _fs_cache_getAllMeasures; //out->in(SoFile)
		static void fieldChanged_cache_getAllMeasures(void *usrData, void *sensor);		
	//@}

		std::list<string> listDummys;


	struct intersectDummy 
	{
		std::string objID;
		double volume;
	};

	std::list<intersectDummy> intersectingDummys;


	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(METKIntraOPViewpoint)

};


ML_END_NAMESPACE

#endif // __mlMETKIntraOPViewpoint_H


