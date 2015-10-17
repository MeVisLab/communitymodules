// Local includes
#include "METK.h"
#include "METKKeystates.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
//#include "UMDVisDataViewer/SoVisDataViewer.h"

#include <iostream>
#include <stdio.h>

#include "Constants.h"

using namespace std;


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKKeystates, ObjMgrClient);


const char* METKKeystates::prefRegionTypeStrings[2] = { "Vector region", "Point region" };
const char* METKKeystates::sphereModeStrings[2] = { "Grid Sphere", "Full Sphere" };
const char* METKKeystates::referenceGroupStrings[3] = { "Structure", "Structure Group", "Real Name" };


//----------------------------------------------------------------------------------
//! Constructor / Destructor
//----------------------------------------------------------------------------------
METKKeystates::METKKeystates() : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKKeystates::METKKeystates()");

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_outScene = getFieldContainer()->addSoNode("outScene");
	_calc = getFieldContainer()->addNotify("calc");
	_init = getFieldContainer()->addNotify("init");
	_currentStructure = getFieldContainer()->addString("currentStructure");
	_currentStructure->setStringValue("Structure_Tumor");
	_debug = getFieldContainer()->addString("debug");
	(_viewerName = getFieldContainer()->addString("viewerName"))->setStringValue("METKViewer3D");
	_dataPath = getFieldContainer()->addString("dataPath");
	_showField = getFieldContainer()->addInt("showField");
	_sphereMode = getFieldContainer()->addEnum("sphereMode",sphereModeStrings, 2);
	_sphereMode->setEnumValue(SM_FULL);
	_currentCam = getFieldContainer()->addVec3f("currentCam");
	_camRange = getFieldContainer()->addDouble("camRange");
	_visSta = getFieldContainer()->addDouble("visSta");
	_impSta = getFieldContainer()->addDouble("impSta");
	_inspect = getFieldContainer()->addInt("inspect");
	_wVis = getFieldContainer()->addDouble("wVis");
	_wImp = getFieldContainer()->addDouble("wImp");
	_wNum = getFieldContainer()->addDouble("wNum");
	_wEnt = getFieldContainer()->addDouble("wEnt");
	_wDis = getFieldContainer()->addDouble("wDis");
	_wCam = getFieldContainer()->addDouble("wCam");
	_wReg = getFieldContainer()->addDouble("wReg");
	_wVisSta = getFieldContainer()->addDouble("wVisSta");
	_wImpSta = getFieldContainer()->addDouble("wImpSta");
	_wSilhouette = getFieldContainer()->addDouble("wSilhouette");
	_wImageSpaceCenter = getFieldContainer()->addDouble("wImageSpaceCenter");
	_wKeystateVPDirection = getFieldContainer()->addDouble("wKeystateVPDirection");

	_prefRegionVector = getFieldContainer()->addVec3f("prefRegionVector");
	_prefRegionVector->setVec3fValue(vec3(0,0,1));
	_prefRegionRange = getFieldContainer()->addDouble("prefRegionRange");
	_prefRegionRange->setDoubleValue(1);
	_prefRegionType = getFieldContainer()->addEnum("prefRegionType", prefRegionTypeStrings, 2);
	_prefRegionType->setEnumValue(PR_VECTOR);
	_restrictToRegion = getFieldContainer()->addBool("restrictToRegion");
	_restrictToRegion->setBoolValue(false);

	_result = getFieldContainer()->addVec3f("result");
	_orient = getFieldContainer()->addVec4f("orient");
	_setViewerCamAtTheEnd = getFieldContainer()->addBool("setViewerCamAtTheEnd");
	_writeCamToObjMgr = getFieldContainer()->addNotify("writeCamToObjMgr");

	//static const char* debugStates[3] = {"None","High","Low"};
	//_debugState = getFieldContainer()->addEnum("debugState", debugStates, 3);
	//_debugState->setStringValue("None");
	//kDebug::setDebugLevel(_debugState->getStringValue());

	_datInfo = getFieldContainer()->addString("datInfo");
	_datInfo->setStringValue("");

	_similarityVector = getFieldContainer()->addString("similarityVector");
	_similarityVector->setStringValue("");

	_test = getFieldContainer()->addNotify("test");
	(_keystateFilename = getFieldContainer()->addString("keystateFilename"))->setStringValue("d:/temp/keystates1.xml");
	_loadFile = getFieldContainer()->addNotify("loadFile");
	_saveFile = getFieldContainer()->addNotify("saveFile");

	_createNewState = getFieldContainer()->addNotify("createNewKeystate");
	_currentKeystate = getFieldContainer()->addString("currentKeystate");
	_applyCurrentKeystate = getFieldContainer()->addNotify("applyCurrentKeystate");
	_animationScript = getFieldContainer()->addString("animationScript");
	_clearKeystates = getFieldContainer()->addNotify("clearKeystates");

	_referenceGroup = getFieldContainer()->addEnum("referenceGroup",referenceGroupStrings, 3);	
	_referenceGroup->setEnumValue(RG_STRUCTURE);

	_animationStepLength = getFieldContainer()->addInt("animationStepLength");
	_animationStepLength->setIntValue(3);
	_animationStepPause = getFieldContainer()->addInt("animationStepPause");
	_animationStepPause->setIntValue(1);
	_createAnimationFromKeystates = getFieldContainer()->addNotify("createAnimationFromKeystates");
	_animationAsVideo = getFieldContainer()->addBool("animationAsVideo");
	_animationAsVideo->setBoolValue(false);

	oReceiver.messageFld->setStringValue("similarVP");
	_message = getFieldContainer()->addString("message");
	oReceiver.messageFld->attachField(_message,0);
	_messageData = getFieldContainer()->addString("messageData");
	oReceiver.getFieldContainer()->getField("data")->attachField(_messageData,0);
	timerSensor = new SoTimerSensor((SoSensorCB*)METKKeystates::timerEvent, this);
	timerSensor->setInterval(SbTime(1.0/1000.0));
	timerSensor->unschedule();

	//Create and connect internal ObjMgr
	internalObjMgrOutFld = getFieldContainer()->addBase("outInternalObjMgr");
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(_myKeystateCommunicator.getFieldContainer()->getField("inObjectContainer"),1);
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(internalObjMgrOutFld,1);
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(_objDumpKeystate.getFieldContainer()->getField("inObjectContainer"),1);
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(_objLoaderKeystate.getFieldContainer()->getField("inObjectContainer"),1);
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(_objInfoKeystates.getFieldContainer()->getField("inObjectContainer"),1);
	_objMgrKeystate.getFieldContainer()->getField("outObjectContainer")->attachField(_objIterKeystates.getFieldContainer()->getField("inObjectContainer"),1);

	m_soViewer = new SoVisDataViewer();
	_outScene->setSoNodeValue(m_soViewer);

	clearAcceptedObjectIDs();
	clearAcceptedInfoLayerNames();

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}


METKKeystates::~METKKeystates()
{
	//For every new in constructor or for aother global objects perform a delete
	/*delete timerSensor;*/
	//no destructor or what?
	//delete m_soViewer;
}


void METKKeystates::activateAttachments()
{
	clearAcceptedObjectIDs();
	clearAcceptedInfoLayerNames();
	addAcceptedObjectID("*");
	addAcceptedInfoLayerName(LAY_APPEARANCE);
	addAcceptedInfoLayerName(LAY_GLOBALEVENTS);

	getFieldContainer()->getField("inObjectContainer")->attachField(oReceiver.getFieldContainer()->getField("inObjectContainer"),1);
	getFieldContainer()->getField("inObjectContainer")->attachField(_myMETK.getFieldContainer()->getField("inObjectContainer"),1);
	getFieldContainer()->getField("inObjectContainer")->attachField(_objIterMETK.getFieldContainer()->getField("inObjectContainer"),1);

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again.
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKKeystates::handleNotification (Field *field)
{
	if (field == _init)
	{
		init();
	}

	else if (field == _calc)
	{
		kDebug::Debug("Calc viewpoint for single strucute ("+_currentStructure->getStringValue()+")",kDebug::DL_HIGH);
		m_calcVis.setFocusObject(_currentStructure->getStringValue());
		m_calcVis.calc();
		m_soViewer->touch();
		setCamPosition(SUM_FIELD,false);
		updateObjectMgr();
		timerSensor->schedule();
	}

	else if (field == _showField || field == _sphereMode)
	{
		m_soViewer->setSphereMode(_sphereMode->getEnumValue()+1); //Die ENUMs gehen 0..1 der sphereMode aber 1..2
		if (_showField->getIntValue()==0) m_soViewer->setDataField(m_calcVis.getField(VIS_FIELD));
		if (_showField->getIntValue()==1) m_soViewer->setDataField(m_calcVis.getField(STA_VIS_FIELD));
		if (_showField->getIntValue()==2) m_soViewer->setDataField(m_calcVis.getField(IMP_FIELD));
		if (_showField->getIntValue()==3) m_soViewer->setDataField(m_calcVis.getField(STA_IMP_FIELD));
		if (_showField->getIntValue()==4) m_soViewer->setDataField(m_calcVis.getField(NUM_FIELD));
		if (_showField->getIntValue()==5) m_soViewer->setDataField(m_calcVis.getField(ENT_FIELD));
		if (_showField->getIntValue()==6) m_soViewer->setDataField(m_calcVis.getField(DIS_FIELD));
		if (_showField->getIntValue()==7) m_soViewer->setDataField(m_calcVis.getField(CAM_FIELD));
		if (_showField->getIntValue()==8) m_soViewer->setDataField(m_calcVis.getField(REG_FIELD));
		if (_showField->getIntValue()==9) m_soViewer->setDataField(m_calcVis.getField(SIL_FIELD));
		if (_showField->getIntValue()==10) m_soViewer->setDataField(m_calcVis.getField(CENTER_FIELD));
		if (_showField->getIntValue()==11) m_soViewer->setDataField(m_calcVis.getField(SUM_FIELD));

		if (_showField->getIntValue()==12) m_soViewer->setDataField(m_calcVis.getStackField(1));
		if (_showField->getIntValue()==13) m_soViewer->setDataField(m_calcVis.getStackField(2));
		if (_showField->getIntValue()==14) m_soViewer->setDataField(m_calcVis.getStackField(3));
		if (_showField->getIntValue()==15) m_soViewer->setDataField(m_calcVis.getStackField(4));
		if (_showField->getIntValue()==16) m_soViewer->setDataField(m_calcVis.getStackField(5));
		m_soViewer->touch();
	}

	else if (field == _dataPath)
	{
		if (_dataPath->getStringValue() != "")
		{
			float fX,fY,fZ,fR;
			int iDiv;
			m_calcVis.setData(_dataPath->getStringValue());
			//m_calcVis.logResults();
			_datInfo->setStringValue("Version:"+m_calcVis.getDataVersion()+
				" Viewport:"+kBasics::IntToString(m_calcVis.getDataViewportX())+"x"+kBasics::IntToString(m_calcVis.getDataViewportY())+
				" CamCount:"+kBasics::IntToString(m_calcVis.getDataCamCount())+
				" Objects:"+kBasics::IntToString(m_calcVis.getDataNrOfObjects())+
				" Div:"+kBasics::IntToString(m_calcVis.getDataDiv()));
			if (m_calcVis.hasData())
			{
				m_calcVis.getSphereValues(fX,fY,fZ,fR,iDiv);
				m_soViewer->createSphere(fX,fY,fZ,fR,iDiv);
				m_soViewer->setSphereMode(2);
				m_vStructures = m_calcVis.getStructureNames ();
				for (int i=0; i<m_vStructures.size(); i++)
				{
					m_calcVis.setImportance((*m_vStructures[i]),getImportance(m_vStructures[i]));
				}
				m_calcVis.setStackSize(10);
				m_calcVis.addVectorRegionToStackField(0,0,0,1,1); //eigentlich überflüssig, da es jetzt über externe Felder eingestellt werden kann
				m_calcVis.setStackFieldAsRegionField(0);
			}
			else { std::cout << "!!!!! m_calcVis has NO data" << std::endl; }
		}
	}

	else if (field == _currentStructure)
	{
		m_calcVis.setFocusObject(_currentStructure->getStringValue());
	}

	else if ( field == _currentCam )
	{
		m_calcVis.setCamPos ( _currentCam->getVec3fValue()[0], _currentCam->getVec3fValue()[1], _currentCam->getVec3fValue()[2] );
	}

	else if ( field == _camRange ) { m_calcVis.setCamRange ( _camRange->getDoubleValue() ); }
	else if ( field == _visSta ) { m_calcVis.setVisStability(_visSta->getDoubleValue() ); }
	else if ( field == _impSta ) { m_calcVis.setImpStability(_impSta->getDoubleValue() ); }
	else if ( field == _inspect ) { m_soViewer->inspectPoint(_inspect->getIntValue()); }
	else if ( field == _wVis ) { m_calcVis.setWeight (VIS_FIELD, _wVis->getDoubleValue()); }
	else if ( field == _wImp ) { m_calcVis.setWeight (IMP_FIELD, _wImp->getDoubleValue()); }
	else if ( field == _wNum ) { m_calcVis.setWeight (NUM_FIELD, _wNum->getDoubleValue()); }
	else if ( field == _wEnt ) { m_calcVis.setWeight (ENT_FIELD, _wEnt->getDoubleValue()); }
	else if ( field == _wDis ) { m_calcVis.setWeight (DIS_FIELD, _wDis->getDoubleValue()); }
	else if ( field == _wCam ) { m_calcVis.setWeight (CAM_FIELD, _wCam->getDoubleValue()); }
	else if ( field == _wReg ) { m_calcVis.setWeight (REG_FIELD, _wReg->getDoubleValue()); }
	else if ( field == _wVisSta ) { m_calcVis.setWeight (STA_VIS_FIELD, _wVisSta->getDoubleValue()); }
	else if ( field == _wImpSta ) { m_calcVis.setWeight (STA_IMP_FIELD, _wImpSta->getDoubleValue()); }
	else if ( field == _wSilhouette ) { m_calcVis.setWeight (SIL_FIELD, _wSilhouette->getDoubleValue()); }
	else if ( field == _wImageSpaceCenter ) { m_calcVis.setWeight (CENTER_FIELD, _wImageSpaceCenter->getDoubleValue()); }

	else if ( field == _prefRegionType )
	{
		if (_prefRegionType->getEnumValue()==PR_VECTOR)
			m_calcVis.setPrefRegionType(m_calcVis.PR_VECTOR);
		else
			m_calcVis.setPrefRegionType(m_calcVis.PR_POINT);
	}

	else if ( field == _prefRegionVector )
	{
		m_calcVis.setPrefRegionVector(_prefRegionVector->getVec3fValue()[0], _prefRegionVector->getVec3fValue()[1], _prefRegionVector->getVec3fValue()[2]);
	}

	else if ( field == _prefRegionRange )
	{
		m_calcVis.setPrefRegionRange(_prefRegionRange->getDoubleValue());
	}

	else if (field == _restrictToRegion )
	{
		m_calcVis.setRestrictToRegion(_restrictToRegion->getBoolValue());
	}

	else if (field == _messageData)
	{
/*		std::cout << "_messageData=" << _messageData->getStringValue() << std::endl;
		vector<string> VecSplit;
		kBasics::split(_messageData->getStringValue(),' ',14,&VecSplit);
		if (!_myMETK.getObjAttributeString(O_CASEOBJECT, LAY_CASEOBJECT_CASE, INF_CASEOBJECT_CASE_DIRECTORY, path))
		{
			cout<<"FEHLER: Keine Falldaten vorhanden!"<<endl;
			_debug->setStringValue("error2");
			_myMETK.setObjAttribute("CameraSolver","Calculation","Success",new bool(0),omINFOTYPE_BOOL ,true,false);
			sendNotification();
			timerSensor->schedule();
		}
		else
		{
			if (VecSplit.size() < 14)
			{
				cout<<"FEHLER: MessageString Format falsch!"<<endl;
				_debug->setStringValue("error1");
				_myMETK.setObjAttribute("CameraSolver","Calculation","Success",new bool(0),omINFOTYPE_BOOL ,true,false);
				sendNotification();
				timerSensor->schedule();
			}
			else
			{
				SbVec3f tempPos;
				SbVec4f tempOrientVec4;
				bool result;
				result = _myMETK.getObjAttributeVec3f(_viewerName->getStringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION, tempPos);

				float x,y,z;
				tempPos.getValue(x,y,z);
				m_calcVis.setCamPos( x, y, z );

				if (!result)
				{
					std::cout << "No Camera position found in METK for " << _viewerName->getStringValue() << std::endl;
					timerSensor->schedule();
				}
				else
				{
					_currentStructure->setStringValue(VecSplit[0]);
					_wVis->setDoubleValue(kBasics::StringToDouble(VecSplit[1]));
					_wReg->setDoubleValue(kBasics::StringToDouble(VecSplit[2]));
					_wCam->setDoubleValue(kBasics::StringToDouble(VecSplit[3]));
					_wEnt->setDoubleValue(kBasics::StringToDouble(VecSplit[4]));
					_wNum->setDoubleValue(kBasics::StringToDouble(VecSplit[5]));
					_wImp->setDoubleValue(kBasics::StringToDouble(VecSplit[6]));
					_wVisSta->setDoubleValue(kBasics::StringToDouble(VecSplit[7]));
					_visSta->setDoubleValue(kBasics::StringToDouble(VecSplit[8]));
					_wImpSta->setDoubleValue(kBasics::StringToDouble(VecSplit[9]));
					_impSta->setDoubleValue(kBasics::StringToDouble(VecSplit[10]));
					_wDis->setDoubleValue(kBasics::StringToDouble(VecSplit[11]));
					_wSilhouette->setDoubleValue(kBasics::StringToDouble(VecSplit[12]));
					_wImageSpaceCenter->setDoubleValue(kBasics::StringToDouble(VecSplit[13]));
					_calc->notify();
				}

				//New
				m_soViewer->touch();
				setCamPosition(SUM_FIELD,false);
			}
		}*/
	}

	else if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)
			getFieldContainer()->getField("inObjectContainer")->attachField(_myMETK.getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(oReceiver.getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_objIterMETK.getFieldContainer()->getField("inObjectContainer"),1);
		}
	}

	else if (field == _debugState)
	{
		std::cout << "setDebugLevel=" << _debugState->getStringValue() << std::endl;
		kDebug::setDebugLevel(_debugState->getStringValue());
	}

	else if (field == _writeCamToObjMgr)
	{
		writeCamToObjMgr();
		sendNotification();
	}

	else if (field == _createNewState)
	{
		//First get/refresh the viewer values in the ObjMgr
		_myMETK.setObjAttribute(_viewerName->getStringValue(),L_GLOBAL,LAY_GLOBAL_REFRESH,new string("1"),omINFOTYPE_MESSAGE,true,false);
		_myMETK.sendNotification();

		//..
		//Das Refresh des Viewers wird wohl sofort ausgeführt, so dass wir hier nahtlos weitermachen können
		//..
		string newKeystateID;
		newKeystateID = generateKeystate();
	}

	else if (field == _test)
	{
		std::cout << "Currently do nothing" << std::endl;
	}

	else if (field == _loadFile)
	{
		_objLoaderKeystate.fileNameFld->setStringValue(_keystateFilename->getStringValue());
		_objLoaderKeystate.loadFld->notify();
		setKeystatePath();
	}

	else if (field == _saveFile)
	{
		_objDumpKeystate.fileNameFld->setStringValue(_keystateFilename->getStringValue());
		_objDumpKeystate.saveFld->notify();
	}

	else if (field == _applyCurrentKeystate)
	{
		std::cout << "apply keystate: " << _currentKeystate->getStringValue() << std::endl;
		//Get current Keystate
		string keystateID = _currentKeystate->getStringValue();
		if (keystateID!="")
		{
			applyKeystate(keystateID,"[0] ",true);			
		}
		else
		{
			std::cout << "No Keystate selected" << std::endl;
		}
			
	}

	else if (field == _clearKeystates)
	{
		_objMgrKeystate.clearObjContainerFld->notify();
	}

	else if (field ==_createAnimationFromKeystates)
	{
		generateAnimation();
	}


	inherited::handleNotification(field);
}



//React on visible-Events from METKManager
void METKKeystates::handleObjMgrNotification()
{
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);
		if (myEvent.layerID == LAY_APPEARANCE)
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
					std::cout << "DISABLE" << myEvent.objectID << std::endl;
					m_calcVis.setStatus(myEvent.objectID,false);
				}
			}
		}

		else if (myEvent.objectID == OBJ_COMMUNICATION && myEvent.layerID == LAY_GLOBALEVENTS && string(myEvent.newValue) == MSG_LOADED)
		{
			std::cout << "INIT " << std::endl;
			init();
		}
	}
	clearEventContainer();
}



void METKKeystates::setCamPosition(const int stackOrFieldNr, bool isStackNr)
{
	float fX,fY,fZ;
	if (!isStackNr)
		m_calcVis.getFieldMaxPos(stackOrFieldNr,fX,fY,fZ);
	else
		m_calcVis.getStackMaxPos(stackOrFieldNr,fX,fY,fZ);

	std::cout << "_result = " << fX << "," << fY << "," << fZ << std::endl;
	_result->setVec3fValue(vec3(fX,fY,fZ));

	Cam.setNormPlump(SbVec3f(0.0,0.0,1.0));
	Cam.setUpVecAngle(0.0);
	float fMX,fMY,fMZ,fbla;
	int ibla;
	m_calcVis.getSphereValues(fMX,fMY,fMZ,fbla,ibla);
	Cam.setCamPosition(SbVec3f(fX,fY,fZ),SbVec3f(fMX,fMY,fMZ));
	SbRotation rot=Cam.getOrientation();
	SbVec3f axis;
	float angle,o1,o2,o3;
	rot.getValue(axis,angle);
	axis.getValue(o1,o2,o3);
	_orient->setVec4fValue(vec4(o1,o2,o3,angle));
}


void METKKeystates::timerEvent(void* data, SoDataSensor* a)
{
	METKKeystates* caller = (METKKeystates*)data;//->render(a);
	caller->timerSensor->unschedule();
	caller->oReceiver.doneFld->notify();
}


void METKKeystates::updateObjectMgr()
{
	kDebug::Debug("Update Camera data in CameraSolver object",kDebug::DL_HIGH);
	_myMETK.setObjAttribute("CameraSolver","Camera","Position",new vec3(_result->getVec3fValue()),omINFOTYPE_VEC3,true,false);
	_myMETK.setObjAttribute("CameraSolver","Camera","Orientation",new vec4(_orient->getVec4fValue()),omINFOTYPE_VEC4,true,false);
	_myMETK.setObjAttribute("CameraSolver","Camera","Height",new double(1.23),omINFOTYPE_DOUBLE ,true,false);
	_myMETK.setObjAttribute("CameraSolver","Calculation","Success",new bool(1),omINFOTYPE_BOOL ,true,false);
	if (_setViewerCamAtTheEnd->getBoolValue())
	{
		writeCamToObjMgr();
	}
	sendNotification();
}



void METKKeystates::writeCamToObjMgr()
{
	kDebug::Debug("Write camera data to "+_viewerName->getStringValue(),kDebug::DL_HIGH);
	_myMETK.setObjAttribute(_viewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_POSITION,new vec3(_result->getVec3fValue()),omINFOTYPE_VEC3,true,false);
	_myMETK.setObjAttribute(_viewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_ORIENTATION,new vec4(_orient->getVec4fValue()),omINFOTYPE_VEC4,true,false);
	_myMETK.setObjAttribute(_viewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_NEWDATA,new omMessage("ping"),omINFOTYPE_MESSAGE,true,false);
}


void METKKeystates::enableVisibleObjects()
{
	omObjectContainer* oc = getObjContainer();
	if (oc)
	{
		omObjectContainer::iterator iterObj;

		for (iterObj = oc->begin(); iterObj != oc->end(); iterObj++)
		{
			omObject &obj = iterObj->second;
			if (obj.isValid() && obj.hasAttribute(LAY_APPEARANCE, INF_VISIBLE))
			{
				bool visible;
				_myMETK.getObjAttributeBool(obj.getID(), LAY_APPEARANCE, INF_VISIBLE, visible);
				if (visible)
				{
					//std::cout << "ENABLE" << obj.getID() << std::endl;
					m_calcVis.setStatus(obj.getID(),true);
				}
			}
		}
	}
}


const float METKKeystates::getImportance(const std::string* name)
{
	omObjectContainer* oc = getObjContainer();
	if (oc) {
		if ((*oc).exists(*name)) {
			if ((*oc)[*name].hasAttribute(LAY_DESCRIPTION, INF_IMPORTANCE)) {
				return (*oc)[*name][LAY_DESCRIPTION][INF_IMPORTANCE].get_double();
			} else {
				if ((*oc)[*name].hasAttribute(LAY_APPEARANCE, INF_TRANSPARENCY)) {
					return 1.0 - (*oc)[*name][LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
				}
			}
		}
	}
	return 0;
}




void METKKeystates::init()
{
	kDebug::Debug("init METKKeystates",kDebug::DL_HIGH);
	string sPath;
	if (!_myMETK.getObjAttributeString(O_CASEOBJECT, LAY_CASEOBJECT_CASE, INF_CASEOBJECT_CASE_DIRECTORY, sPath))
	{
		cout<<"FEHLER: Keine Falldaten vorhanden!"<<endl;
	}
	else
	{
		_dataPath->setStringValue(sPath);
		enableVisibleObjects();

		//SET Default ENT-Values:
		_wVis->setDoubleValue(0);//Wenn Strukturen ausgeblendet werden, hat das ganz schlimme Folgen, weil visField immer noch von einer 100% opaken Sichtbarkeit ausgeht//_wVis->setDoubleValue(30);
		_wReg->setDoubleValue(50);
		_wCam->setDoubleValue(0);
		_wEnt->setDoubleValue(50);
		_wNum->setDoubleValue(5);
		_wImp->setDoubleValue(50);
		_wVisSta->setDoubleValue(30);
		_visSta->setDoubleValue(0.7);
		_wImpSta->setDoubleValue(50);
		_impSta->setDoubleValue(0.7);
		_wDis->setDoubleValue(50);
		_wSilhouette->setDoubleValue(50);
		_wImageSpaceCenter->setDoubleValue(50);
		_wKeystateVPDirection->setDoubleValue(100);
	}
}



string METKKeystates::generateKeystate()
{
	ML_TRACE_IN("METKKeystates::generateKeystate()");

	//-01-//Find new keystate ID and create keyste
	int globalID;
	if (!_myKeystateCommunicator.getObjAttributeInt("Global","Keystates","currentID",globalID))
		globalID=0;
	globalID++;
	_myKeystateCommunicator.setObjAttribute(OBJ_GLOBAL,LAY_KEYSTATES,INF_CURRENTID,&globalID,omINFOTYPE_INT32,true,true);
	string newID = "Keystate_"+kBasics::leadingZero(globalID,4);

	//-02-//Create viewpoint data
	generateViewpointData(newID);

	//-03-//Mark Object as Keystate
	_myKeystateCommunicator.setObjAttribute(newID,LAY_GLOBAL,INF_ISKEYSTATE,new bool(true),omINFOTYPE_BOOL,true,true);

	//-04-//Set keystate path (for screenshots)
	setKeystatePath();

	//-05-//Create screenshot
	generateScreenshot(newID);

	//-06-//Generate information about color, transparency, silhouette and visibility
	generateAppearanceData(newID);

	//-07-//Set currentKeystate = newKeystate (bisher noch ohne Rückkopplung zum keystateVis)
	_currentKeystate->setStringValue(newID);

	_myKeystateCommunicator.sendNotification();

	//Update sphere debug output (?)
	m_soViewer->touch();

	return newID;
}


void METKKeystates::generateAppearanceData(string keystateID)
{
	ML_TRACE_IN("METKKeystates::generateAppearanceData(string keystateID)");
	std::cout << "generateAppearanceData" << std::endl;

	HashTable<SbVec3f> structruesColor;
	HashTable<bool> structruesVisibility;
	HashTable<double> structruesTransparency;
	HashTable<bool> structruesSilhouette;
	HashTable<SbVec3f> structruesSilhouetteColor;
	HashTable<int> structruesSilhouetteWidth;


	//-01-//Init the ObjIterator to look for structures in the METK data
	//_objIterMETK.layerIDsFld->setStringValue(LAY_DESCRIPTION);
	_objIterMETK.layerIDsFld->setStringValue(LAY_IMAGE);
	//_objIterMETK.infoIDsFld->setStringValue(INF_STRUCTURE);
	//_objIterMETK.infoIDsFld->setStringValue(INF_NAME);
	_objIterMETK.infoIDsFld->setStringValue(INF_IMAGETYPE);
	_objIterMETK.infoValueFld->setStringValue("Segmentation");
	_objIterMETK.firstObjFld->notify();

	string structure = "";
	string allOccuringStruc = "";
	bool visibility, silhouette;
	float transparency;
	int silhouetteWidth;
	SbVec3f color, silhouetteColor;
	string objID;
	string tempStr;
	//-02-//Get Color etc. data for every structrue(type)
	while (_objIterMETK.opSuccessfulFld->getBoolValue())
	{		
		objID = _objIterMETK.objectIDFld->getStringValue();
		_myMETK.getObjAttributeString(objID,LAY_IMAGE,INF_IMAGETYPE,tempStr);
		if (kBasics::toUp(tempStr)=="SEGMENTATION")
		{			
			//If LAY_IMAGE->INF_IMAGETYPE=SEGMENTATION exists, get appearance data
			switch (_referenceGroup->getEnumValue())
			{
			case RG_STRUCTURE:
				_myMETK.getObjAttributeString(objID,LAY_DESCRIPTION,INF_STRUCTURE,structure);
				break;
			case RG_STRUCTUREGROUP:
				_myMETK.getObjAttributeString(objID,LAY_DESCRIPTION,INF_STRUCTUREGROUP,structure);
				break;
			case RG_REALNAME:
			default:
				_myMETK.getObjAttributeString(objID,LAY_DESCRIPTION,INF_NAME,structure);
				break;
			}
			//std::cout << structure << ";";

			if (structure!="")
			{
				allOccuringStruc = allOccuringStruc+structure+",";
				if (_myMETK.getObjAttributeBool(objID,LAY_APPEARANCE,INF_VISIBLE,visibility))
					structruesVisibility.insert(structure,visibility);
				if (_myMETK.getObjAttributeVec3f(objID,LAY_APPEARANCE,INF_COLOR,color))
					structruesColor.insert(structure,color);
				if (_myMETK.getObjAttributeFloat(objID,LAY_APPEARANCE,INF_TRANSPARENCY,transparency))
					structruesTransparency.insert(structure,transparency);
				if (_myMETK.getObjAttributeBool(objID,LAY_APPEARANCE,INF_SILHOUETTE,silhouette))
					structruesSilhouette.insert(structure,silhouette);
				if (_myMETK.getObjAttributeInt(objID,LAY_APPEARANCE,INF_SILHOUETTEWIDTH,silhouetteWidth))
					structruesSilhouetteWidth.insert(structure,silhouetteWidth);
				if (_myMETK.getObjAttributeVec3f(objID,LAY_APPEARANCE,INF_SILHOUETTECOLOR,silhouetteColor))
					structruesSilhouetteColor.insert(structure,silhouetteColor);
			}
		}

		//std::cout << std::endl;
		_objIterMETK.nextObjFld->notify();
	}

	//-03-//Write appearance data for every structure(type) into the ObjMgr for the keystates
	std::vector<string> splittedStruc;
	std::cout << "allOccuringStruc: " << allOccuringStruc << std::endl;
	kBasics::split(allOccuringStruc,',',1024,&splittedStruc);
	SbVec3f* tempSbVec3f;
	vec3* tempVec3;
	int* tempInt; double* tempDouble; bool* tempBool;
	for (unsigned int i=0;i<splittedStruc.size();i++)
	{
		if (splittedStruc[i]!="")
		{
			//Set Visibility
			tempBool = structruesVisibility.find(splittedStruc[i]);
			if (tempBool)
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VISIBILITY,splittedStruc[i],tempBool,omINFOTYPE_BOOL,true,true);
			else
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VISIBILITY,splittedStruc[i],new bool(false),omINFOTYPE_BOOL,true,true);
			//Set Color
			tempSbVec3f = structruesColor.find(splittedStruc[i]);
			if (tempSbVec3f)
			{
				tempVec3 = new vec3((*tempSbVec3f)[0],(*tempSbVec3f)[1],(*tempSbVec3f)[2]);
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_COLOR,splittedStruc[i],tempVec3,omINFOTYPE_VEC3,true,true);
				delete tempVec3;
			}

			//Set Transparency
			tempDouble = structruesTransparency.find(splittedStruc[i]);
			if (tempDouble)
			{
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_TRANSPARENCY,splittedStruc[i],tempDouble,omINFOTYPE_DOUBLE,true,true);
			}
			//Set Silhouette-Vis
			tempBool = structruesSilhouette.find(splittedStruc[i]);
			if (tempBool)
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_SILHOUETTE,splittedStruc[i],tempBool,omINFOTYPE_BOOL,true,true);
			else
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_SILHOUETTE,splittedStruc[i],new bool(false),omINFOTYPE_BOOL,true,true);
			//Set SilhouetteWidth
			tempInt = structruesSilhouetteWidth.find(splittedStruc[i]);
			if (tempInt)
			{
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_SILHOUETTEWIDTH,splittedStruc[i],tempInt,omINFOTYPE_INT32,true,true);
			}

			//Set SilhouetteColor
			tempSbVec3f = structruesSilhouetteColor.find(splittedStruc[i]);
			if (tempSbVec3f)
			{
				tempVec3 = new vec3((*tempSbVec3f)[0],(*tempSbVec3f)[1],(*tempSbVec3f)[2]);
				_myKeystateCommunicator.setObjAttribute(keystateID,LAY_SILHOUETTECOLOR,splittedStruc[i],tempVec3,omINFOTYPE_VEC3,true,true);
				delete tempVec3;
			}
		}
	}
}



void METKKeystates::generateViewpointData(string keystateID)
{
	string structureOfInterest;

	if (_currentStructure->getStringValue()=="") _currentStructure->setStringValue("Structure_Tumor");
	structureOfInterest = _currentStructure->getStringValue();

	//-01-//Find nearest viewpoint on sphere
	SbVec3f tempPos;
	SbVec4f tempOrientVec4;
	bool result;
	result = _myMETK.getObjAttributeVec3f(_viewerName->getStringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION, tempPos);

	float xReal,yReal,zReal, xOnSphere,yOnSphere,zOnSphere;
	tempPos.getValue(xReal,yReal,zReal);

	std::cout << "----> current position of cam = " << xReal << "," << yReal << "," << zReal << std::endl;

	int posIndex = m_calcVis.getFieldPos(xReal,yReal,zReal);
	std::cout << "posIndex = " << posIndex << std::endl;

	m_calcVis.getSpherePos(posIndex,xOnSphere,yOnSphere,zOnSphere);

	std::cout << "position at pos index = " << xOnSphere << "," << yOnSphere << "," << zOnSphere << std::endl;

	if (!result)
	{
		std::cout << "No Camera position found in METK for " << _viewerName->getStringValue() << std::endl;
		return;
	}

	m_calcVis.setFocusObject(structureOfInterest);
	m_calcVis.calc();

	//-02-//Get all parameter values for this viewpoint
	double valVis			= m_calcVis.getField(VIS_FIELD)->getValueAt(posIndex);
	double valPrefReg		= m_calcVis.getField(REG_FIELD)->getValueAt(posIndex);
	double valEntropy		= m_calcVis.getField(ENT_FIELD)->getValueAt(posIndex);
	double valNumbOcc		= m_calcVis.getField(NUM_FIELD)->getValueAt(posIndex);
	double valImportance	= m_calcVis.getField(IMP_FIELD)->getValueAt(posIndex);
	double valVisSta		= m_calcVis.getField(STA_VIS_FIELD)->getValueAt(posIndex);
	double valImpSta		= m_calcVis.getField(STA_IMP_FIELD)->getValueAt(posIndex);
	double valDistPlane		= m_calcVis.getField(DIS_FIELD)->getValueAt(posIndex);
	double valSilh			= m_calcVis.getField(SIL_FIELD)->getValueAt(posIndex);
	double valImgCenter		= m_calcVis.getField(CENTER_FIELD)->getValueAt(posIndex);

	string simVec;

	simVec = "Structure="+structureOfInterest;
	simVec = simVec + " vis="+kBasics::FloatToString(valVis);
	simVec = simVec + " prefReg="+kBasics::FloatToString(valPrefReg);
	simVec = simVec + " entropy="+kBasics::FloatToString(valEntropy);
	simVec = simVec + " numbOcc="+kBasics::FloatToString(valNumbOcc);
	simVec = simVec + " importance="+kBasics::FloatToString(valImportance);
	simVec = simVec + " visSta="+kBasics::FloatToString(valVisSta);
	simVec = simVec + " impSta="+kBasics::FloatToString(valImpSta);
	simVec = simVec + " distPlane="+kBasics::FloatToString(valDistPlane);
	simVec = simVec + " silh="+kBasics::FloatToString(valSilh);
	simVec = simVec + " imgCenter="+kBasics::FloatToString(valImgCenter);

	_similarityVector->setStringValue(simVec);

	////-03-//Write all values for viewpoint into an object in the ObjMgr
	//The pure values from the parameter maps
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_SOF,new string(structureOfInterest),omINFOTYPE_STRING,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_VIS,&valVis,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_PREFREG,&valPrefReg,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_ENT,&valEntropy,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_NUMOCC,&valNumbOcc,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_IMPVIS,&valImportance,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_STABVIS,&valVisSta,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_STABIMP,&valImpSta,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_DISTPLANE,&valDistPlane,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_SILHLENGTH,&valSilh,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_IMGCENTER,&valImgCenter,omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_NEXTCAM,new vec3(xOnSphere,yOnSphere,zOnSphere),omINFOTYPE_VEC3,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_REALCAM,new vec3(xReal,yReal,zReal),omINFOTYPE_VEC3,true,true);

	float tempHeight,tempRadius, tempx,tempy,tempz; int tempDiv;
	_myMETK.getObjAttributeFloat(_viewerName->getStringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT, tempHeight);
	m_calcVis.getSphereValues(tempx,tempy,tempz,tempRadius,tempDiv);
	if (tempHeight!=0 && tempRadius!=0)
	{
		_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_ZOOM,new double(tempHeight/(tempRadius*2)),omINFOTYPE_DOUBLE,true,true);
	}
	else
	{
		_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_ZOOM,new double(1),omINFOTYPE_DOUBLE,true,true);
	}

	SbVec3f directionOnSphere;
	directionOnSphere[0] = xOnSphere-tempx;
	directionOnSphere[1] = yOnSphere-tempy;
	directionOnSphere[2] = zOnSphere-tempz;
	directionOnSphere.normalize();
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VP,INF_DIRECTIONONSPHERE,new vec3(directionOnSphere[0],directionOnSphere[1],directionOnSphere[2]),omINFOTYPE_VEC3,true,true);



	//The weights
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_SOF,new string(structureOfInterest),omINFOTYPE_STRING,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_VIS,new double(_wVis->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_PREFREG,new double(_wReg->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_ENT,new double(_wEnt->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_NUMOCC,new double(_wNum->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_IMPVIS,new double(_wImp->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_STABVIS,new double(_wVisSta->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_STABIMP,new double(_wImpSta->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_DISTPLANE,new double(_wDis->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_SILHLENGTH,new double(_wSilhouette->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_IMGCENTER,new double(_wImageSpaceCenter->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_VPWEIGHTS,INF_KEYSTATEVPDIR,new double(_wKeystateVPDirection->getDoubleValue()),omINFOTYPE_DOUBLE,true,true);
}




bool METKKeystates::generateViewpointFromKeystate(string keystateID,vec3& position)
{
	string structure;
	float valVis, valPrefReg, valEntropy, valNumbOcc, valImportance, valVisSta, valImpSta, valDistPlane, valSilh, valImgCenter;

	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_VIS,valVis);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_PREFREG,valPrefReg);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_ENT,valEntropy);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_NUMOCC,valNumbOcc);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_IMPVIS,valImportance);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_STABVIS,valVisSta);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_STABIMP,valImpSta);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_DISTPLANE,valDistPlane);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_SILHLENGTH,valSilh);
	_myKeystateCommunicator.getObjAttributeFloat(keystateID,LAY_VP,INF_IMGCENTER,valImgCenter);

	//Calc distances and get position with minimal distance to similarity vector

	_myKeystateCommunicator.getObjAttributeString(keystateID,LAY_VP,INF_SOF,structure);
	m_calcVis.setFocusObject(structure);
	m_calcVis.calc();

	CvpField* fieldVis = m_calcVis.getField(VIS_FIELD);
	CvpField* fieldPrefReg = m_calcVis.getField(REG_FIELD);
	CvpField* fieldEntropy = m_calcVis.getField(ENT_FIELD);
	CvpField* fieldNumbOcc = m_calcVis.getField(NUM_FIELD);
	CvpField* fieldImportance = m_calcVis.getField(IMP_FIELD);
	CvpField* fieldVisSta = m_calcVis.getField(STA_VIS_FIELD);
	CvpField* fieldImpSta = m_calcVis.getField(STA_IMP_FIELD);
	CvpField* fieldDistPlane = m_calcVis.getField(DIS_FIELD);
	CvpField* fieldSilh = m_calcVis.getField(SIL_FIELD);
	CvpField* fieldImgCenter = m_calcVis.getField(CENTER_FIELD);

	//PreCalc the field for Distance to original direction of cam stored in the keystate
	CvpField* fieldDirectionSphere = m_calcVis.getStackField(1);
	fieldDirectionSphere->fillWith(0.0);
	float tempRadius, tempx,tempy,tempz; int tempDiv; SbVec3f dirOnSphere;
	_myKeystateCommunicator.getObjAttributeVec3f(keystateID,LAY_VP,INF_DIRECTIONONSPHERE,dirOnSphere);
	m_calcVis.getSphereValues(tempx,tempy,tempz,tempRadius,tempDiv);
	m_calcVis.addPointRegionToStackField(1,tempx+dirOnSphere[0],tempy+dirOnSphere[1],tempz+dirOnSphere[2],0.2);


	std::cout << "size of fields = " << fieldVis->getSize() << std::endl;
	float euklDist;
	float minDist = FLT_MAX;
	int posIndexForMinDist = -1;
	for (int i=0; i<fieldVis->getSize(); i++)
	{
		euklDist = 0.0;
		euklDist = euklDist + pow((valVis-fieldVis->getValueAt(i))*_wVis->getDoubleValue(),2);
		euklDist = euklDist + pow((valPrefReg-fieldPrefReg->getValueAt(i))*_wReg->getDoubleValue(),2);
		euklDist = euklDist + pow((valEntropy-fieldEntropy->getValueAt(i))*_wEnt->getDoubleValue(),2);
		euklDist = euklDist + pow((valNumbOcc-fieldNumbOcc->getValueAt(i))*_wNum->getDoubleValue(),2);
		euklDist = euklDist + pow((valImportance-fieldImportance->getValueAt(i))*_wImp->getDoubleValue(),2);
		euklDist = euklDist + pow((valVisSta-fieldVisSta->getValueAt(i))*_wVisSta->getDoubleValue(),2);
		euklDist = euklDist + pow((valImpSta-fieldImpSta->getValueAt(i))*_wImpSta->getDoubleValue(),2);
		euklDist = euklDist + pow((valDistPlane-fieldDistPlane->getValueAt(i))*_wDis->getDoubleValue(),2);
		euklDist = euklDist + pow((valSilh-fieldSilh->getValueAt(i))*_wSilhouette->getDoubleValue(),2);
		euklDist = euklDist + pow((valImgCenter-fieldImgCenter->getValueAt(i))*_wImageSpaceCenter->getDoubleValue(),2);
		euklDist = euklDist + pow((1-fieldDirectionSphere->getValueAt(i))*_wKeystateVPDirection->getDoubleValue(),2);

		euklDist = sqrt(euklDist);
		if (euklDist<minDist)
		{
			minDist = euklDist;
			posIndexForMinDist = i;
		}
	}

	std::cout << "minDist = " << minDist << std::endl;
	std::cout << "posIndexForMinDist = " << posIndexForMinDist << std::endl;

	std::cout << "for best position:" << std::endl;

	std::cout << "target valVis:" << valVis << "  ";
	std::cout << "field:" << fieldVis->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wVis->getDoubleValue() << std::endl;
	std::cout << "target valPrefReg:" << valPrefReg << "  ";
	std::cout << "field:" << fieldPrefReg->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wReg->getDoubleValue() << std::endl;
	std::cout << "target valEntropy:" << valEntropy << "  ";
	std::cout << "field:" << fieldEntropy->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wEnt->getDoubleValue() << std::endl;
	std::cout << "target valNumbOcc:" << valNumbOcc << "  ";
	std::cout << "field:" << fieldNumbOcc->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wNum->getDoubleValue() << std::endl;
	std::cout << "target valImportance:" << valImportance << "  ";
	std::cout << "field:" << fieldImportance->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wImp->getDoubleValue() << std::endl;
	std::cout << "target valVisSta:" << valVisSta << "  ";
	std::cout << "field:" << fieldVisSta->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wVisSta->getDoubleValue() << std::endl;
	std::cout << "target valImpSta:" << valImpSta << "  ";
	std::cout << "field:" << fieldImpSta->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wImpSta->getDoubleValue() << std::endl;
	std::cout << "target valDistPlane:" << valDistPlane << "  ";
	std::cout << "field:" << fieldDistPlane->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wDis->getDoubleValue() << std::endl;
	std::cout << "target valSilh:" << valSilh << "  ";
	std::cout << "field:" << fieldSilh->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wSilhouette->getDoubleValue() << std::endl;
	std::cout << "target valImgCenter:" << valImgCenter << "  ";
	std::cout << "field:" << fieldImgCenter->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wImageSpaceCenter->getDoubleValue() << std::endl;
	std::cout << "target keystateVPDirection: 1  ";
	std::cout << "field:" << fieldDirectionSphere->getValueAt(posIndexForMinDist) << "  ";
	std::cout << "weight = " << _wKeystateVPDirection->getDoubleValue() << std::endl;


	if (posIndexForMinDist!=-1)
	{
		float x,y,z;
		m_calcVis.getSpherePos(posIndexForMinDist,x,y,z);
		std::cout << "new perfect position = " << x << "," << y << "," << z << std::endl;
		position[0] = x;
		position[1] = y;
		position[2] = z;
	}
	else
	{
		return false;
	}

	m_soViewer->touch();
	return true;
}






void METKKeystates::generateScreenshot(string keystateID)
{
	std::cout << "generateScreenshot for keystate: " << keystateID << "   (from the current viewer!)"<< std::endl;
	string filename = _keystateFilename->getStringValue()+"_"+keystateID;//+".tif";
	_myMETK.setObjAttribute(_viewerName->getStringValue(), LAY_VIEWER_SCREENSHOT, INF_VIEWER_SCREENSHOT_FILE, &filename, omINFOTYPE_STRING, true, true);
	_myMETK.setObjAttribute(_viewerName->getStringValue(), LAY_VIEWER_SCREENSHOT, INF_VIEWER_SCREENSHOT_FLAG, new bool(true), omINFOTYPE_BOOL, true, true);
	_myMETK.setObjAttribute(_viewerName->getStringValue(), LAY_VIEWER_SCREENSHOT, INF_VIEWER_SCREENSHOT_THUMB, new bool(true), omINFOTYPE_BOOL, true, true);
	_myMETK.sendNotification();

	filename = kBasics::getFilename(filename,'/')+".thumb.tif";
	std::cout << "filename: " << filename << std::endl;
	_myKeystateCommunicator.setObjAttribute(keystateID,LAY_INFORMATION,INF_SCREENSHOT,&filename,omINFOTYPE_STRING,true,true);
	_myKeystateCommunicator.sendNotification();
}

void METKKeystates::setKeystatePath()
{
	string imagePath;
	imagePath = kBasics::getPath(_keystateFilename->getStringValue(),'/'); //Die Art des SLashes könnte u.U. Probleme bereiten
	_myKeystateCommunicator.setObjAttribute(OBJ_GLOBAL,LAY_KEYSTATES,INF_IMAGEPATH,new string(imagePath),omINFOTYPE_STRING,true,false);
}

string METKKeystates::applyKeystate(string keystateID, string timecode, bool singleKeystate)
{
	bool tempBool; double tempDouble; vec3 tempVec3; int tempInt;

	//-02-//Check if keystate exists
	if (!_myKeystateCommunicator.getObjAttributeBool(keystateID,LAY_GLOBAL,INF_ISKEYSTATE,tempBool))
	{
		std::cout << "Selected keystate (" << keystateID << ") is not available in ObjMgr/XML."<< std::endl;
		return "";
	}

	//-03-//Build animation script
	string script;
	if (singleKeystate)
	{
		script =+ "[Ini]\nLengthTimeUnit=0\nLengthSeconds=1\nRealTime=yes\n\n[Script]\n";
	}

	string tempStruc;
	//Add Visibility to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_VISIBILITY);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempBool = _objInfoKeystates.boolValueFld->getBoolValue();
		script = script + timecode + "'"+tempStruc+"' setVisible "+kBasics::BoolToString(tempBool)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}

	//Add Transparency to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_TRANSPARENCY);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempDouble = _objInfoKeystates.doubleValueFld->getDoubleValue();
		script = script + timecode + "'"+tempStruc+"' setTransparency "+kBasics::FloatToString(tempDouble)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}

	//Add Color to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_COLOR);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempVec3 = _objInfoKeystates.vec3ValueFld->getVec3fValue();
		script = script + timecode + "'"+tempStruc+"' setColor "+kBasics::FloatToString(tempVec3[0]*255)+","+kBasics::FloatToString(tempVec3[1]*255)+","+kBasics::FloatToString(tempVec3[2]*255)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}


	//Add Silhouette-Vis to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_SILHOUETTE);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempBool = _objInfoKeystates.boolValueFld->getBoolValue();
		script = script + timecode + "'"+tempStruc+"' setSilhouette "+kBasics::BoolToString(tempBool)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}

	//Add SilhouetteWidth to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_SILHOUETTEWIDTH);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempInt = _objInfoKeystates.int32ValueFld->getIntValue();
		script = script + timecode + "'"+tempStruc+"' setSilhouetteWidth "+kBasics::IntToString(tempInt)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}

	//Add SilhouetteColor to script
	_objInfoKeystates.objectIDFld->setStringValue(keystateID);
	_objInfoKeystates.layerIDFld->setStringValue(LAY_SILHOUETTECOLOR);
	_objInfoKeystates.firstInfoIDFld->notify();
	while (_objInfoKeystates.opSuccessfulFld->getBoolValue())
	{
		tempStruc = _objInfoKeystates.infoIDFld->getStringValue();
		_objInfoKeystates.getFld->notify();
		tempVec3 = _objInfoKeystates.vec3ValueFld->getVec3fValue();
		script = script + timecode + "'"+tempStruc+"' setSilhouetteColor "+kBasics::FloatToString(tempVec3[0]*255)+","+kBasics::FloatToString(tempVec3[1]*255)+","+kBasics::FloatToString(tempVec3[2]*255)+"\n";
		_objInfoKeystates.nextInfoIDFld->notify();
	}

	//-04-//Calculate viewpoint from keystate data and current viewpoint parameter maps
	if (!generateViewpointFromKeystate(keystateID,tempVec3))
	{
		std::cout << "No viewpoint found for keystate " << keystateID << std::endl;
	}
	else
	{
		string zoomFactor;
		_myKeystateCommunicator.getObjAttributeString(keystateID,LAY_VP,INF_ZOOM,zoomFactor);
		string strucOfInt;
		_myKeystateCommunicator.getObjAttributeString(keystateID,LAY_VP,INF_SOF,strucOfInt);

		script = script + timecode + "'Cam' campath '"+strucOfInt+"' "+kBasics::FloatToString(tempVec3[0])+";"+kBasics::FloatToString(tempVec3[1])+";"+kBasics::FloatToString(tempVec3[2])+";"+zoomFactor+"\n";
	}
	script = script + timecode + "'CAM' ROTATECAMZTO 0\n";

	_animationScript->setStringValue(script);

	//-05-//Send animation script to UMDAnimation2
	if (singleKeystate)
	{
		_myMETK.setObjAttribute(O_ANIMATION,LAY_ANIMATION_SCRIPT,INF_SCRIPT_FULLSCRIPT,(omMessage*)&script, omINFOTYPE_MESSAGE, true, false);
		_myMETK.sendNotification();
	}

	std::cout << script << std::endl;

	return script;
}


void METKKeystates::generateAnimation()
{
	//Init the ObjIterator to look for all keystates
	_objIterKeystates.layerIDsFld->setStringValue(LAY_GLOBAL);	
	_objIterKeystates.infoIDsFld->setStringValue(INF_ISKEYSTATE);
	_objIterKeystates.firstObjFld->notify();
	string keystateID;
	int stepLength = _animationStepLength->getIntValue();
	int stepPause = _animationStepPause->getIntValue();
	int stepCount = 0;

	string script = "";
	string timeCode;
	std::cout << "Start iteration" << std::endl;
	while (_objIterKeystates.opSuccessfulFld->getBoolValue())
	{		
		keystateID = _objIterKeystates.objectIDFld->getStringValue();
		std::cout << keystateID << std::endl;
		timeCode = "["+kBasics::IntToString(stepCount*(stepLength+stepPause))+","+kBasics::IntToString(stepCount*(stepLength+stepPause)+stepLength)+"] ";
		script = script + applyKeystate(keystateID,timeCode,false);
		stepCount++;
		_objIterKeystates.nextObjFld->notify();
	}

	script = "[Ini]\nLengthTimeUnit="+kBasics::IntToString(stepCount*(stepLength+stepPause)+stepLength)+"\nLengthSeconds="+kBasics::IntToString(stepCount*(stepLength+stepPause)+stepLength)+"\nRealTime="+kBasics::BoolToString(!_animationAsVideo->getBoolValue())+"\n\n[Script]\n"+script;

	_animationScript->setStringValue(script);
}


ML_END_NAMESPACE