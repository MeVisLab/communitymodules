//----------------------------------------------------------------------------------
//! The ML module class METKAutoFading.
/*!
// \file    mlMETKAutoFading.cpp
// \author  Konrad Mühler
// \date    2007-06-18
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMETKAutoFading.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKAutoFading, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKAutoFading::METKAutoFading (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKAutoFading::METKAutoFading()")

	myObjMgr = new ObjMgrCommunicator();
	myCamera = new kCamera();

	FieldContainer *fields = getFieldContainer();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_outScene = getFieldContainer()->addSoNode("outScene");
	(_EnableFading = fields->addBool("EnableFading"))->setBoolValue(FALSE);	
	(_UseMETKValues = fields->addBool("UseMETKValues"))->setBoolValue(FALSE);
	(_CurrentObject = fields->addString("CurrentObject"))->setStringValue("");
	(_ViewerName = fields->addString("ViewerName"))->setStringValue("METKViewer3D");
	_init = fields->addNotify("init");
	_calc = fields->addNotify("calc");
	(_dataPath = fields->addString("dataPath"))->setStringValue("");
	_similarPosition = fields->addVec3f("similarPosition");
	_camPosition = fields->addVec3f("camPosition");
	_camOrientation = fields->addVec4f("camOrientation");	

	setAcceptedEvents();

	m_soViewer = new SoVisDataViewer();
	_outScene->setSoNodeValue(m_soViewer);


	oReceiver = new METKMsgReceiver();
    oReceiver->messageFld->setStringValue("fading");
	/*_message = (StringField*) getFieldContainer()->addField(oReceiver->getFieldContainer()->getField("message"));
	_messageData = (StringField*) getFieldContainer()->addField(oReceiver->getFieldContainer()->getField("data"));*/
	_message = getFieldContainer()->addString("message");
	oReceiver->messageFld->attachField(_message,0);
	_messageData = getFieldContainer()->addString("data");
	oReceiver->getFieldContainer()->getField("data")->attachField(_messageData,0);
    timerSensor = new SoTimerSensor((SoSensorCB*)METKAutoFading::timerEvent, this);
    timerSensor->setInterval(SbTime(1.0/1000.0));
    timerSensor->unschedule();


	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKAutoFading::handleNotification (Field *field)
{
	ML_TRACE_IN("METKAutoFading::handleNotification()")
	inherited::handleNotification(field);

	// ... field notification handling code
	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)			
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(oReceiver->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}
	
	else if (field == _init)
	{
		string sPath;
		if (!myObjMgr->getObjAttributeString(O_CASEOBJECT, LAY_CASEOBJECT_CASE, INF_CASEOBJECT_CASE_DIRECTORY, sPath))
		{
			std::cout << "FEHLER: Keine Falldaten vorhanden!" << std::endl;
		}
		else
		{
			_dataPath->setStringValue(sPath);
		}		
	}

	else if (field == _dataPath)
	{
		if (_dataPath->getStringValue() != "")
		{
			float fX,fY,fZ,fR;
			int iDiv;
			m_calcVis.setData(_dataPath->getStringValue());
			//m_calcVis.logResults();
			if (m_calcVis.hasData())
			{
				m_calcVis.getSphereValues(fX,fY,fZ,fR,iDiv);
				m_soViewer->createSphere(fX,fY,fZ,fR,iDiv);
				m_soViewer->setSphereMode(2);
				m_calcVis.setStackSize(10);
			}
		}
	}

	
	else if (field == _ViewerName)
	{
//		setAcceptedEvents();
	}
	
	else if (field == _EnableFading)
	{
//		setAcceptedEvents();

		if (_EnableFading->getBoolValue())
		{			
			myObjMgr->setObjAttribute(_ViewerName->getStringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS, new bool(true), omINFOTYPE_BOOL, true, false);
		}
		else
		{
			myObjMgr->setObjAttribute(_ViewerName->getStringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS, new bool(false), omINFOTYPE_BOOL, true, false);
		}		
		sendNotification();
	}

	else if (field == _calc)
	{
		calcNewPosition();
		m_soViewer->setDataField(m_calcVis.getStackField(1));
		m_soViewer->touch();

		timerSensor->schedule();
	}

	else if (field == _camOrientation)
	{
		if (_EnableFading->getBoolValue())
		{
			_calc->notify();
		}
	}

	else if (field == _messageData)
	{
		std::cout << "fading field == _messageData=" << _messageData->getStringValue() << std::endl;

		vector<string> VecSplit;
		kBasics::split(_messageData->getStringValue(),' ',2,&VecSplit);
		if (VecSplit.size() < 2) 
		{
			cout<<"FEHLER: MessageString Format für Fading falsch!"<<endl;
			timerSensor->schedule();
		}
		else
		{
			_EnableFading->setBoolValue(kBasics::toUp(VecSplit[1])=="TRUE");
			_UseMETKValues->setBoolValue(kBasics::toUp(VecSplit[1])=="TRUE");

			if (VecSplit[0]!="NONE")
			{
				_CurrentObject->setStringValue(VecSplit[0]);
				_calc->notify();
			}
			else
			{
				_CurrentObject->setStringValue("");
//				std::cout << "6" << std::endl;
				resetAllCurrentOccluders();
//				std::cout << "7" << std::endl;
				timerSensor->schedule();
			}
		}
	}
}

//React on events from METKManager
void METKAutoFading::handleObjMgrNotification()
{	
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;		
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);

		if (myEvent.layerID == LAY_VIEWER_CAMERA && myEvent.infoID == INF_VIEWER_CAMERA_POSITION && _EnableFading->getBoolValue() && _UseMETKValues->getBoolValue() )
		{	
			_calc->notify();
		}

		else if (myEvent.layerID == LAY_APPEARANCE)
		{
			if (myEvent.infoID == INF_VISIBLE)
			{
				if (myEvent.newValue.getStringValue() == "TRUE")
				{					
					//std::cout << "ENABLE" << myEvent.objectID << std::endl;
					m_calcVis.setStatus(myEvent.objectID,true);
				}
				else
				{
					//std::cout << "DISABLE" << myEvent.objectID << std::endl;
					m_calcVis.setStatus(myEvent.objectID,false);
				}
			}		
		}
		
		else if (myEvent.objectID == OBJ_COMMUNICATION && myEvent.layerID == LAY_GLOBALEVENTS && string(myEvent.newValue) == MSG_LOADED)
		{
			std::cout << "METKAutoFading   loaded event ... call _init" << std::endl;
			_init->notify();
		}
	}
	clearEventContainer();
}



void METKAutoFading::activateAttachments()
{
	getFieldContainer()->getField("inObjectContainer")->attachField(oReceiver->getFieldContainer()->getField("inObjectContainer"),true);
	getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),true);

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again.
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}



void METKAutoFading::calcNewPosition()
{
	SbVec3f tempPos;
	SbVec4f tempOrientVec4;
	
	bool result = false;
	if (_UseMETKValues->getBoolValue())
	{
		result = myObjMgr->getObjAttributeVec3f(_ViewerName->getStringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION, tempPos);
		result = result && myObjMgr->getObjAttributeVec4f(_ViewerName->getStringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION, tempOrientVec4);
	}
	else
	{
		result = true;
		tempPos[0] = _camPosition->getVec3fValue()[0]; tempPos[1] = _camPosition->getVec3fValue()[1]; tempPos[2] = _camPosition->getVec3fValue()[2];
		tempOrientVec4[0] = _camOrientation->getVec4fValue()[0]; tempOrientVec4[1] = _camOrientation->getVec4fValue()[1]; tempOrientVec4[2] = _camOrientation->getVec4fValue()[2]; tempOrientVec4[3] = _camOrientation->getVec4fValue()[3];
	}

	if (result)
	{
		myCamera->setPosition(tempPos);
		SbRotation tempOrient;
		tempOrient.setValue(SbVec3f(tempOrientVec4[0],tempOrientVec4[1],tempOrientVec4[2]),tempOrientVec4[3]);
		myCamera->setOrientation(tempOrient);

		SbVec3f lookDir = myCamera->getLookDir();
		SbVec3f inverseLookDir = -1.0 * lookDir;		
		
		inverseLookDir.normalize();
		//SphereCenter
		float sphere_x, sphere_y, sphere_z, sphere_radius; int sphere_div;
		m_calcVis.getSphereValues(sphere_x, sphere_y, sphere_z, sphere_radius, sphere_div);
		SbVec3f spherePoint;
		spherePoint[0] = sphere_x + inverseLookDir[0] * sphere_radius;
		spherePoint[1] = sphere_y + inverseLookDir[1] * sphere_radius;
		spherePoint[2] = sphere_z + inverseLookDir[2] * sphere_radius;

		m_calcVis.clearStack(1);
		m_calcVis.addPointRegionToStackField(1, spherePoint[0], spherePoint[1], spherePoint[2], 0.1);
		SbVec3f similarPosition;
		int posID = m_calcVis.getStackMaxPos(1, similarPosition[0], similarPosition[1], similarPosition[2]);		
		
		_similarPosition->setVec3fValue(vec3(similarPosition[0],similarPosition[1],similarPosition[2]));

		//Alle Occluder an dieser Position für ObOfIn
		std::vector<float> visValues;
		m_calcVis.getMatrixValues(posID, _CurrentObject->getStringValue(), &visValues);
		int objID = m_calcVis.findObjectId(_CurrentObject->getStringValue());		
		string occluName;
	
		set<string> newFading;
		for (int i=0; i<visValues.size()-3; i++)
		{			
			if (visValues[i]!=0 && i!=objID)
			{
				occluName = m_calcVis.getObjectName(i);
				newFading.insert(occluName);
				//htNewFading.insert(occluName, occluName);
			}
		}

		//std::cout << "newFading size = " << newFading.size() << std::endl;
		//std::cout << "currentFading size = " << currentFading.size() << std::endl;

		//Strukturen die neu hinzukommen finden
		string objName = "";
		Appearance oldValues;
		set<string>::iterator iter = newFading.begin();
		for (iter=newFading.begin(); iter!=newFading.end(); iter++)
		{	
			objName = *iter;
			//if (!htOldValues.find(objName))
			if (!findInSet(currentFading, objName) && objName!="")
			{				
				std::cout << "new:" << objName << std::endl;
				//Get Old Values
				myObjMgr->getObjAttributeFloat(objName,LAY_APPEARANCE, INF_TRANSPARENCY, oldValues.Transparency);
				myObjMgr->getObjAttributeVec3f(objName,LAY_APPEARANCE, INF_COLOR, oldValues.Color);				
				myObjMgr->getObjAttributeVec3f(objName,LAY_APPEARANCE, INF_SILHOUETTECOLOR, oldValues.SilhouetteColor);
				myObjMgr->getObjAttributeBool(objName,LAY_APPEARANCE, INF_SILHOUETTE, oldValues.SilhouetteVisible);
				myObjMgr->getObjAttributeFloat(objName,LAY_APPEARANCE, INF_SILHOUETTEWIDTH, oldValues.SilhouetteWidth);

				htOldValues.insert(objName, oldValues);
				currentFading.insert(objName);

				//Ausblenden
				myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_TRANSPARENCY, new double(0.9), omINFOTYPE_DOUBLE, true, false);
				myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTEWIDTH, new double(2), omINFOTYPE_DOUBLE, true, false);
				myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTECOLOR, new vec3(0,0,0), omINFOTYPE_VEC3, true, false);
				myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTE, new bool(true), omINFOTYPE_BOOL, true, false);
			}
		}

		//Alte Element aus currentFading wieder einblenden mit oldValues, die nicht in newFading drin sind		
		for (iter=currentFading.begin(); iter!=currentFading.end(); iter++)
		{
			objName = *iter;
			if (!findInSet(newFading,objName) && objName!="")
			{				
				std::cout << "Wiedereinblenden: " << objName << std::endl;

				if (htOldValues.find(objName))
				{
					oldValues = *(htOldValues.find(objName));
					myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_TRANSPARENCY, new double(oldValues.Transparency), omINFOTYPE_DOUBLE, true, false);
					myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTEWIDTH, new double(oldValues.SilhouetteWidth), omINFOTYPE_DOUBLE, true, false);
					vec3* tempVec3 = new vec3(oldValues.SilhouetteColor[0],oldValues.SilhouetteColor[1],oldValues.SilhouetteColor[2]);
					myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTECOLOR, tempVec3, omINFOTYPE_DOUBLE, true, false);
					delete tempVec3;
					myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTE, new bool(oldValues.SilhouetteVisible), omINFOTYPE_BOOL, true, false);
				}
				else
					std::cout << "No oldValues found in Hashtable for " << objName << std::endl;

				htOldValues.remove(objName);
			}
		}

		currentFading.clear();
		
		//Umkopieren von newFading in currentFading
		for (iter=newFading.begin(); iter!=newFading.end(); iter++)
		{
			currentFading.insert(*iter);
		}

		//std::cout << "ende currentFading size = " << currentFading.size() << std::endl;

		sendNotification();
	}
	else
	{
		std::cout << "Cam Position or Orientation in METK not available" << std::endl;
	}
}



void METKAutoFading::setAcceptedEvents()
{
	clearAcceptedInfoLayerNames();
	addAcceptedInfoLayerName("*");	

	clearAcceptedObjectIDs();
	//addAcceptedObjectID(_ViewerName->getStringValue());
	//addAcceptedObjectID(OBJ_COMMUNICATION);
	addAcceptedObjectID("*"); //Muss aus alle Appearance hören zwecks visible
}


void METKAutoFading::timerEvent(void* data, SoDataSensor* a)
{	
    METKAutoFading* caller = (METKAutoFading*)data;//->render(a);    
    caller->timerSensor->unschedule();    
	std::cout << "caller->oReceiver->doneFld->notify();" << std::endl;
    caller->oReceiver->doneFld->notify();
}


void METKAutoFading::resetAllCurrentOccluders()
{	
	Appearance oldValues;
	string objName = "";
	set<string>::iterator iter;
	for (iter = currentFading.begin(); iter != currentFading.end(); iter++)
	{
		objName = *iter;
		std::cout << "Wiedereinblenden: " << objName << std::endl;

		if (htOldValues.find(objName))
		{
			oldValues = *(htOldValues.find(objName));
			myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_TRANSPARENCY, new double(oldValues.Transparency), omINFOTYPE_DOUBLE, true, false);
			myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTEWIDTH, new double(oldValues.SilhouetteWidth), omINFOTYPE_DOUBLE, true, false);
			vec3* tempVec3 = new vec3(oldValues.SilhouetteColor[0],oldValues.SilhouetteColor[1],oldValues.SilhouetteColor[2]);
			myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTECOLOR, tempVec3, omINFOTYPE_DOUBLE, true, false);
			delete tempVec3;
			myObjMgr->setObjAttribute(objName,LAY_APPEARANCE, INF_SILHOUETTE, new bool(oldValues.SilhouetteVisible), omINFOTYPE_BOOL, true, false);
		}
		else
			std::cout << "No oldValues found in Hashtable for " << objName << std::endl;

		htOldValues.remove(objName);
	}

	currentFading.clear();
	htOldValues.getTable()->clear();

	sendNotification();
}


bool METKAutoFading::findInSet(set<string> mySet, string toFind)
{
	set<string>::iterator iter;
	for (iter = mySet.begin(); iter!= mySet.end(); iter++)
	{
		if (*iter==toFind) return true;
	}
	return false;
}

ML_END_NAMESPACE

