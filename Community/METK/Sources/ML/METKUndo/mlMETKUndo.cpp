//----------------------------------------------------------------------------------
//! The ML module class METKUndo.
/*!
// \file    mlMETKUndo.cpp
// \author  kmuehler
// \date    2008-06-19
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlMETKUndo.h"
#include "../METK.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKUndo, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKUndo::METKUndo (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKUndo::METKUndo ()");

	// Get reference to the container for parameters/fields.
	FieldContainer &fields = *getFieldContainer();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	// Add fields to the module and set their values.
	_UndoFld = fields.addNotify("Undo");
	(_undoCountFld = fields.addInt("undoCount"))->setIntValue(0);


	timerSensor = new SoTimerSensor((SoSensorCB*)METKUndo::timerEvent, this);
	timerSensor->setInterval(SbTime(1.5));
	timerSensor->unschedule();

	myObjMgr = new ObjMgrCommunicator();

	_enableUndo = false;
	_collectingEvents = false;
	_denyOwnEvents = false;
	_currentlyCreated = false;

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}

METKUndo::~METKUndo()
{
	if (myObjMgr) delete myObjMgr;
	myObjMgr = NULL;
	if (timerSensor) delete timerSensor;
	timerSensor = NULL;
}

void METKUndo::activateAttachments()
{
	ML_TRACE_IN("METKUndo::activateAttachments()");

	clearAcceptedObjectIDs();
	addAcceptedObjectID("*");
	clearAcceptedInfoLayerNames();
	addAcceptedInfoLayerName(LAY_APPEARANCE);
	addAcceptedInfoLayerName(LAY_GLOBALEVENTS);
	addAcceptedInfoLayerName(LAY_VIEWER_CAMERA);
	
	sendNotification();

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void METKUndo::handleNotification (Field *field)
{
	ML_TRACE_IN("METKUndo::handleNotification ()");

	inherited::handleNotification(field); //sehr wichtig, da sonst keine ObjMgr-Ereignisse empfangen werden können

	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);			
		}	
	}	

	else if (field == _UndoFld)
	{
		std::cout << "UNDO ... list size =" << _undoStates.size() << std::endl;
		if (_undoStates.size()>0)
		{
			UNDO_STATE tempState = _undoStates[_undoStates.size()-1];
			for (unsigned int i=0; i<tempState.events.size(); i++)
			{
				/*cout << " obj:" << tempState.events[i].objID;
				cout << " lay:" << tempState.events[i].layer;
				cout << " inf:" << tempState.events[i].info;
				cout << " val:" << tempState.events[i].value << endl;*/
				if (tempState.events[i].value!="")
				{
					myObjMgr->setObjAttribute(tempState.events[i].objID, tempState.events[i].layer, tempState.events[i].info, new string(tempState.events[i].value),omINFOTYPE_STRING,false,true,true);
				}
			}
			_undoStates.resize(_undoStates.size()-1); // delete last undo state
			_undoCountFld->setIntValue(_undoStates.size());
			_denyOwnEvents=true;
			myObjMgr->sendNotification();
		}
	}
}


void METKUndo::handleObjMgrNotification()
{
	ML_TRACE_IN("METKUndo::handleObjMgrNotification()");

	omEventContainer myEventList = getEventContainer();

	if (_denyOwnEvents)
	{
		cout << "_denyOwnEvents" << endl;
		_denyOwnEvents=false;
		clearEventContainer();
		return;
	}

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;		
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);
						
		if (_enableUndo &&
			(myEvent.layerID == LAY_APPEARANCE || myEvent.layerID == LAY_VIEWER_CAMERA))
		{
			if (_collectingEvents)
			{				
				timerSensor->unschedule();
				timerSensor->schedule(); //restart timer
			}
			else
			{
				std::cout << "start _collectingEvents" << endl;
				_collectingEvents = true;

				UNDO_STATE tempState;
				_undoStates.push_back(tempState);
				timerSensor->schedule(); //start
			}

			//Add Event as new event in current undo state
			if (myEvent.prevValue.isValid())
			{
				if (!_currentlyCreated)
				{
					SINGLE_EVENT tempEvent;
					tempEvent.objID = myEvent.objectID;
					tempEvent.layer = myEvent.layerID;
					tempEvent.info = myEvent.infoID;
					tempEvent.value = myEvent.prevValue.getStringValue();

					if (_undoStates.size()>0)
					{
						_undoStates[_undoStates.size()-1].events.push_back(tempEvent);
						_undoCountFld->setIntValue(_undoStates.size());
					}
					else
					{
						cout << "ERROR:_undoStates.size()==0 " << endl;
					}
				}
				else
					_currentlyCreated=false;
			}
			else
			{
				//cout << "prevValue invalid    obj=" << myEvent.objectID << " inf=" << myEvent.infoID << endl;
				if (myEvent.type==ObjMgrEvent::ATTRIBUTE_CREATED)
				{
					_currentlyCreated=true;
				}
			}


		}
		else if (myEvent.infoID == INF_CASELOADED)
		{
			_enableUndo = true;
			std::cout << "_enableUndo = true" << std::endl;
		}


	}
	clearEventContainer();
}


void METKUndo::timerEvent(void* data, SoDataSensor* /*a*/)
{	
	METKUndo* caller = (METKUndo*)data;
	if (!caller) return;
	if (!caller->timerSensor) return;
	caller->timerSensor->unschedule();
	caller->_collectingEvents=false;
}



ML_END_NAMESPACE

