//----------------------------------------------------------------------------------
//! Class to ...
/*!
// \file    AnimationCache.h
// \author  Konrad Mühler
// \date    2006-11-03
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __AnimationCache_H
#define __AnimationCache_H

// Local includes
#include "AnimationSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>

#include "METK.h"
#include "kBasics.h"
#include "HashTable.h"
#include "mlMETKMsgSender.h"
#include "METKObjXMLWriter/mlMETKObjXMLWriter.h"
#include "kDebug.h"
#include "ObjMgrCommunicator.h"
#include "AnimationParser/AnimationParser.h"


ML_START_NAMESPACE

class ANIMATION_EXPORT AnimationCache : public ObjMgrClient
{
public:

	AnimationCache (void);
	~AnimationCache (void);

	virtual void handleNotification (Field *field);
	virtual void handleObjMgrNotification();

		
	//! \name Get object IDs from ObjMgr
	//@{
		HashTable<string>* htObjectIDs;			//!< Cash for ObjectIDs
		const string findSingleObjectID(string objName);
	//@}


	HashTable<string>* htBBRequested;			//!< List of sended BB Requests, so that a failed request isn't requested again for every script/command
	HashTable<string>* htMainAxisRequested;		//!< List of sended MainAxis Requests, so that a failed request isn't requested again for every script/command

	typedef std::pair<int, int> int_pair; 
	std::vector<int_pair> myList_ObjectCommands;	//!< List of possible object commands\n first int stands for frame number and second for entry number in this frame



	//! \name ObjectCommands	
	//@{
		static const string OC_NONE;					//!< 
		static const string OC_OBJECTSBYSIZE;			//!<
		static const string OC_PARAM_NONE;				//!< 
		static const string OC_PARAM_DIAMETER;			//!< 
		static const string OC_PARAM_LOWER;				//!< 
		static const string OC_PARAM_GREATER;			//!< 
		static const string OC_PARAM_BETWEEN;			//!< 
		string parseSingleObjectCommand(const string objectLine, std::vector<string>* returnStringList);
	//@}


	
	//! Struct to collect all measurement for an object like BoundingBox and diameter
	struct Measures
	{
		SbVec3f BB_max;
		SbVec3f BB_min;
		//SbVec3f BB_center;
		SbVec3f BB_objectCenter;
		SbVec3f BB_boxCenter;

		SbVec3f baryCenter;
		SbVec3f midPoint;		
		SbVec3f xAxis;
		SbVec3f yAxis;
		SbVec3f zAxis;
		double xDiameter;
		double yDiameter;
		double zDiameter;
		double maxDiameter;
	};
	
	typedef list<Measures> MEASURESLIST;	//!< List of Measurements


	//! \name Measurements
	//! Variable to get measurements of objects from ObjMgr
	//@{
		HashTable<Measures>* htMeasures;	//!< Cash for Measures
		vector<string> listObjects;			//!< List of objects, for which measures have to get from ObjMgr	
		int globalMeasuresListCounter;		//!< Counter for listObjects
		METKMsgSender* _modMsgSenderGetMeasures; //!< METKMsgSender to send request for measurements
		METKMsgSender* _modMsgSenderGetMainAxis; //!< METKMsgSender to send request for main axis measurements
		BoolField* _msgGetMeasures_done;		 //!< Listener to receive "done"-Field of _modMsgSenderGetMeasures
		BoolField* _msgGetMainAxis_done;		 //!< Listener to receive "done"-Field of _modMsgSenderGetMainAxis
		bool waitForMeasures;				//!< Process wait for measuremnts for one object from ObjMgr
		bool waitForMainAxis;				//!< Process wait for main axis measures for one object from ObjMgr
		void getAllMeasures(bool reset);		
		bool getMeasures(const string objID, Measures& tempMeasures);
		bool getMainAxis(const string objID, Measures& tempMeasures);
		SbVec3f getBoundingCenter(Measures myMeasures);
		Measures getMasterBoundingBox(MEASURESLIST* myMeasuresList);	
		Measures getMeasuresFromHT(const string objName);
	//@}

	METKObjXMLWriter* _modObjXMLWriter;


	void reset();

	
	void setObjMgr(ObjMgrCommunicator* value);
	void setAnimationParser(AnimationParser* value);

	string getObjXML(bool force=false);

	
private:
	
	typedef ObjMgrClient inherited;
			
	ML_CLASS_HEADER(AnimationCache)


	NotifyField* fldReady;

	ObjMgrCommunicator* myObjMgr;
	AnimationParser* myAnimationParser;


	SoTimerSensor* timerSendRequest;								//!< Timer um das Request-Event ein klein wenig später zu senden
	static void timerSendRequestEvent(void* data, SoDataSensor* a);	//!< Sends the Request-Event
	SoTimerSensor* timerFldReady;									//!< Timer um das FldReady leichtv erzögert zu touchen - damit wenn keine BBs zu holen sind er nicht gleich durchrauscht und dann stehen bleibt
	static void timerFldReadyEvent(void* data, SoDataSensor* a);	//!< Sends the FldReady-Event
		
};


ML_END_NAMESPACE


#endif // __AnimationCache_H