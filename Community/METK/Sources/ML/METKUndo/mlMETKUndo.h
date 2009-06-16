//----------------------------------------------------------------------------------
//! The ML module class METKUndo.
/*!
// \file    mlMETKUndo.h
// \author  kmuehler
// \date    2008-06-19
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKUndo_H
#define __mlMETKUndo_H


// Local includes
#include "METKUndoSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <list>
#include "kBasics.h"
#include "ObjMgrCommunicator.h"

ML_START_NAMESPACE


//!
class METKUNDO_EXPORT METKUndo : public ObjMgrClient
{
public:

	//! Constructor.
	METKUndo ();
	~METKUndo();

	//! Handle field changes of the field field.
	virtual void handleNotification (Field *field);

	virtual void handleObjMgrNotification();

	virtual void activateAttachments ();

private:

	typedef ObjMgrClient inherited;

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//!
	NotifyField *_UndoFld;

	//!
	IntField *_undoCountFld;

	//@}

	struct SINGLE_EVENT {
		string objID;
		string layer;
		string info;
		string value;
		//SINGLE_EVENT() : objID(""), layer(""), info(""), value("") {} // default constructor
		//SINGLE_EVENT(string objID_, string layer_, string info_, string value_) : objID(objID_), layer(layer_), info(info_), value(value_) {} // constructor
	};

	struct UNDO_STATE {
		std::vector<SINGLE_EVENT> events;
	};

	std::vector<UNDO_STATE> _undoStates;

	SoTimerSensor* timerSensor;
	static void timerEvent(void* data, SoDataSensor* a);

	ObjMgrCommunicator* myObjMgr;

	bool _enableUndo;
	bool _collectingEvents;
	bool _denyOwnEvents;
	bool _currentlyCreated;

	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(METKUndo)
};


ML_END_NAMESPACE

#endif // __mlMETKUndo_H