//----------------------------------------------------------------------------------
//! The ML module class METKIntraOPViewpoint.
/*!
// \file    mlMETKIntraOPViewpoint.cpp
// \author  Konrad Mühler
// \date    2007-07-30
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlMETKIntraOPViewpoint.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKIntraOPViewpoint,ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKIntraOPViewpoint::METKIntraOPViewpoint (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKIntraOPViewpoint::METKIntraOPViewpoint()")

	myObjMgr = new ObjMgrCommunicator();
	myCache = new AnimationCache();
	myObjIterator = new ObjConstrainedIterator();

	FieldContainer *fields = getFieldContainer();

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_GenerateVPForNewTumor = fields->addNotify("GenerateVPForNewTumor");
	(_NewTumorCenter = fields->addVec3f("NewTumorCenter"))->setVec3fValue(vec3(0,0,0));
	(_NewTumorRadius = fields->addDouble("NewTumorRadius"))->setDoubleValue(1.0);
	_GenerateVPForAllTumors = fields->addNotify("GenerateVPForAllTumors");
	_init = fields->addNotify("init");
	_initDone = fields->addBool("initDone");
	_initDone->setBoolValue(false);		

	waitingFor_getAllMeasures = false;
	_fs_cache_getAllMeasures = new FieldSensor(METKIntraOPViewpoint::fieldChanged_cache_getAllMeasures, this);
	_fs_cache_getAllMeasures->attachField(myCache->getFieldContainer()->getField("ready"));	

	_modMsgSenderGetCamPos = new METKMsgSender();
	_msgGetCamPos_done = getFieldContainer()->addBool("_msgGetCamPos_done");
	_modMsgSenderGetCamPos->getFieldContainer()->getField("done")->attachField(_msgGetCamPos_done,1);	
	waitingFor_getCamPos = false;
	gotCamPos = false;	

	kDebug::setDebugLevel(kDebug::DL_NONE);

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}

METKIntraOPViewpoint::~METKIntraOPViewpoint (void)
{
	ML_TRACE_IN("METKIntraOPViewpoint::~METKIntraOPViewpoint()")
    delete myObjMgr;
	delete myCache;
	delete _modMsgSenderGetCamPos;
	delete _fs_cache_getAllMeasures;
}



//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKIntraOPViewpoint::handleNotification (Field *field)
{
	ML_TRACE_IN("METKIntraOPViewpoint::handleNotification()")

	//Verbinden des Caches, des ObjCommunicator und des MsgSenders für Neugis CamPos
	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjIterator->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(myCache->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderGetCamPos->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}
	
	else if (field == _init)
	{
		myObjIterator->layerIDsFld->setStringValue(LAY_DESCRIPTION);
		myObjIterator->infoIDsFld->setStringValue(INF_STRUCTURE);
		myObjIterator->useInfoValueFld->setBoolValue(true);
		myObjIterator->infoValueFld->setStringValue("IntraOPTumorDummy");
		
		listDummys.clear();
		myObjIterator->firstObjFld->notify();
		while (myObjIterator->opSuccessfulFld->getBoolValue())
		{
			//std::cout << "Add: " << myObjIterator->objectIDFld->getStringValue() << std::endl;
			myCache->findSingleObjectID(myObjIterator->objectIDFld->getStringValue());
			listDummys.push_back(myObjIterator->objectIDFld->getStringValue());
			myObjIterator->nextObjFld->notify();
		}		
		
		_initDone->setBoolValue(false);		
		myCache->getAllMeasures(true);
		waitingFor_getAllMeasures=true;	
	}

	else if (field == _GenerateVPForAllTumors)
	{
		//Generate BB for new tumor
		AnimationCache::Measures bbTumor;
		bbTumor.BB_boxCenter[0] = _NewTumorCenter->getVec3fValue()[0]; bbTumor.BB_boxCenter[1] = _NewTumorCenter->getVec3fValue()[1]; bbTumor.BB_boxCenter[2] = _NewTumorCenter->getVec3fValue()[2];
		vec3 tempVec3;
		vec3 centerCorner = vec3(1,1,1);
		centerCorner.normalize();
		tempVec3 = _NewTumorCenter->getVec3fValue() - (centerCorner*sqrt(3*_NewTumorRadius->getDoubleValue()*_NewTumorRadius->getDoubleValue()));
		bbTumor.BB_min[0] =  tempVec3[0]; bbTumor.BB_min[1] =  tempVec3[1]; bbTumor.BB_min[2] =  tempVec3[2];
		tempVec3 = _NewTumorCenter->getVec3fValue() + (centerCorner*sqrt(3*_NewTumorRadius->getDoubleValue()*_NewTumorRadius->getDoubleValue()));
		bbTumor.BB_max[0] =  tempVec3[0]; bbTumor.BB_max[1] =  tempVec3[1]; bbTumor.BB_max[2] =  tempVec3[2];

		std::cout << "Tumor-BB = " << kBasics::Vec3fToString(bbTumor.BB_min) << " --- " << kBasics::Vec3fToString(bbTumor.BB_max) << std::endl;
		std::cout << "Tumor-BB-Center = " << kBasics::Vec3fToString(bbTumor.BB_boxCenter) << std::endl;

		//Searching for intersecting Dummys
		std::list<string>::const_iterator iter;
		AnimationCache::Measures tempMeasures;
		string objID;
		bool intersect=false;
		double totalVolume = 0.0;
		intersectingDummys.clear();
		for (iter=listDummys.begin(); iter!=listDummys.end(); iter++)
		{
			objID = string(*iter);			
			myCache->getMeasures(objID,tempMeasures);
			//std::cout << objID <<  " center: " << kBasics::Vec3fToString(tempMeasures.BB_boxCenter) << "  max: " << kBasics::Vec3fToString(tempMeasures.BB_max) << "  min: " << kBasics::Vec3fToString(tempMeasures.BB_min) << std::endl;

			//Check for intersection with Tumor-BB
			intersect=false;

			//for X: |centerX1-centerX2| < widthX1/2 + widthX2/2  -> intersect
			if ( fabs(bbTumor.BB_boxCenter[0]-tempMeasures.BB_boxCenter[0]) < ( fabs(bbTumor.BB_max[0]-bbTumor.BB_min[0])/2 + fabs(tempMeasures.BB_max[0]-tempMeasures.BB_min[0])/2 ) )
			{
				//for Y
				if ( fabs(bbTumor.BB_boxCenter[1]-tempMeasures.BB_boxCenter[1]) < ( fabs(bbTumor.BB_max[1]-bbTumor.BB_min[1])/2 + fabs(tempMeasures.BB_max[1]-tempMeasures.BB_min[1])/2 ) )
				{
					//for Z
					if ( fabs(bbTumor.BB_boxCenter[2]-tempMeasures.BB_boxCenter[2]) < ( fabs(bbTumor.BB_max[2]-bbTumor.BB_min[2])/2 + fabs(tempMeasures.BB_max[2]-tempMeasures.BB_min[2])/2 ) )
					{
						intersect = true;
					}
				}				
			}

			if (intersect)
			{
				std::cout << "intersect with  " << objID << std::endl;
				//Calc volume				
				double gap;
				double volume=1.0;
				for (int i=0; i<3; i++)
				{
					gap = max(bbTumor.BB_boxCenter[i],tempMeasures.BB_boxCenter[i]) 
						  - min(bbTumor.BB_boxCenter[i],tempMeasures.BB_boxCenter[i])
						  - fabs(bbTumor.BB_max[i]-bbTumor.BB_min[i])/2.0
						  - fabs(tempMeasures.BB_max[i]-tempMeasures.BB_min[i])/2.0;
					
					//Schnittgröße auf maximale Ausdehnung der Boxen in der jeweiligen Richtung begrenzen
					//für kleinere Werte klappt die Formel oben
					if (fabs(gap)>min(fabs(bbTumor.BB_max[i]-bbTumor.BB_min[i]),fabs(tempMeasures.BB_max[i]-tempMeasures.BB_min[i])))
					{
						gap = min(fabs(bbTumor.BB_max[i]-bbTumor.BB_min[i]),fabs(tempMeasures.BB_max[i]-tempMeasures.BB_min[i]));
					}	

					volume *= fabs(gap);
					//std::cout << "max X center: " << max(bbTumor.BB_boxCenter[i],tempMeasures.BB_boxCenter[i]) << std::endl;
					//std::cout << "min X center: " << min(bbTumor.BB_boxCenter[i],tempMeasures.BB_boxCenter[i]) << std::endl;
					std::cout << "gap " << gap << std::endl;
				}

				totalVolume += volume;
				intersectDummy tempDummy;
				tempDummy.objID = objID;
				tempDummy.volume = volume;
				intersectingDummys.push_back(tempDummy);

				std::cout << "Volume:" << volume << std::endl;
			}
			else //no intersection
			{
				myObjMgr->setObjAttribute(objID,LAY_APPEARANCE,INF_TRANSPARENCY,new double(0.95),omINFOTYPE_DOUBLE,true,true);
				myObjMgr->setObjAttribute(objID,LAY_APPEARANCE,INF_COLOR,new vec3(.8,.8,.8),omINFOTYPE_VEC3,true,true);
			}
		}

		//Setting Appearance of all intersecting Dummys
		std::list<intersectDummy>::const_iterator iter2;
		intersectDummy currentDummy;
		double rate = 0.0;
		string viewpointRequest = "";
		for (iter2=intersectingDummys.begin(); iter2!=intersectingDummys.end(); iter2++)
		{
			currentDummy = intersectDummy(*iter2);
			rate = currentDummy.volume/totalVolume;
			viewpointRequest += currentDummy.objID+","+kBasics::DoubleToString(rate)+";";
			myObjMgr->setObjAttribute(currentDummy.objID,LAY_APPEARANCE,INF_TRANSPARENCY,new double(0.9-rate),omINFOTYPE_DOUBLE,true,true);
			myObjMgr->setObjAttribute(currentDummy.objID,LAY_APPEARANCE,INF_COLOR,new vec3(1,0,0),omINFOTYPE_VEC3,true,true);
			myObjMgr->setObjAttribute(currentDummy.objID,LAY_APPEARANCE,INF_VISIBLE,new bool(true),omINFOTYPE_BOOL,true,true);
		}

		std::cout << "viewpointRequest: " << viewpointRequest << std::endl;
		
		myObjMgr->sendNotification();		
	}
}



void METKIntraOPViewpoint::handleObjMgrNotification()
{
	ML_TRACE_IN("METKIntraOPViewpoint::handleObjMgrNotification()")
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;	
 	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);
		//if (myEvent.objectID==fldAnimationViewerName->getStringValue() && myEvent.layerID==LAY_VIEWER_PROPERTIES && myEvent.infoID==INF_VIEWER_PROPERTIES_VIEWERSTATUS)
	}
	clearEventContainer();
}



void METKIntraOPViewpoint::activateAttachments()
{ 
	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}



//! Wird aufgerufen, wenn alle Informationen im Cache vorliegen und es losgehen kann
//! \param  usrData 
//! \param  sensor 
//! \return void  
void METKIntraOPViewpoint::fieldChanged_cache_getAllMeasures(void *usrData, void* )
{	
	ML_TRACE_IN("METKIntraOPViewpoint::fieldChanged_cache_getAllMeasures()")
	METKIntraOPViewpoint* myObj = (METKIntraOPViewpoint*) usrData;
	if (myObj)
	{		
		//if (myObj->waitingFor_getAllMeasures) myObj->startAnimation();
		if (myObj->waitingFor_getAllMeasures)
		{
			std::cout << "All Measures done!" << std::endl;
			myObj->_initDone->setBoolValue(true);
		}
	}	
}



ML_END_NAMESPACE

