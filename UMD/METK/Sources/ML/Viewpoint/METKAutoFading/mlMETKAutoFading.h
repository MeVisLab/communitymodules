//----------------------------------------------------------------------------------
//! The ML module class METKAutoFading.
/*!
// \file    mlMETKAutoFading.h
// \author  Konrad Mühler
// \date    2007-06-18
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKAutoFading_H
#define __mlMETKAutoFading_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "kBasics.h"
#include "kCamera.h"
#include "METK.h"
#include "HashTable.h"

#include "vpBasic/vpCalcVis.h"
#include "vpBasic/vpField.h"
#include "UMDVisDataViewer/SoVisDataViewer.h"

#include <Inventor/sensors/SoTimerSensor.h>

#include "METKMsgManager.h"
#include "ObjMgrCommunicator.h"


ML_START_NAMESPACE


//!
class VIEWPOINT_EXPORT METKAutoFading : public ObjMgrClient
{
public:

	//! Constructor.
	METKAutoFading (void);

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);
	virtual void handleObjMgrNotification();
	virtual void activateAttachments();

private:

	typedef ObjMgrClient inherited;

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	SoNodeField* _outScene;
	//!
	BoolField* _EnableFading;
	BoolField* _UseMETKValues;

	//!
	StringField* _CurrentObject;

	StringField* _ViewerName;

	NotifyField* _init;
	NotifyField* _calc;

	StringField* _dataPath;

	Vec3fField* _similarPosition;
	Vec3fField* _camPosition;
	Vec4fField* _camOrientation;

	StringField* _messageData;
	StringField* _message;


	//@}

	struct Appearance
	{
		float Transparency;
		SbVec3f Color;
		float SilhouetteWidth;
		SbVec3f SilhouetteColor;
		bool SilhouetteVisible;
	};

	ObjMgrCommunicator* myObjMgr;

	kCamera* myCamera;
	
	METKMsgReceiver* oReceiver;
	SoTimerSensor* timerSensor; //for doneFld touching with a little delay
	static void timerEvent(void* data, SoDataSensor* a);


	CvpCalcVis m_calcVis;

	SoVisDataViewer* m_soViewer;

	HashTable<Appearance> htOldValues; //ObjNamen und OldValues
	set<string>	currentFading; //Kurze Liste zum schnelleren Durchiterieren
	

	void calcNewPosition();	
	void setAcceptedEvents();
	void resetAllCurrentOccluders();
	bool findInSet(set<string> mySet, string toFind);

	//! Implements interface for the runtime type system of the ML.
	ML_BASEOP_CLASS_HEADER(METKAutoFading)

};


ML_END_NAMESPACE

#endif // __mlMETKAutoFading_H


