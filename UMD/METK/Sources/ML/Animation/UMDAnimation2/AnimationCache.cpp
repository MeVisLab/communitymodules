// Local includes
#include "AnimationCache.h"

ML_START_NAMESPACE


const string AnimationCache::OC_NONE = "";				
const string AnimationCache::OC_OBJECTSBYSIZE = "GETOBJECTSBYSIZE";

const string AnimationCache::OC_PARAM_NONE = "";				
const string AnimationCache::OC_PARAM_DIAMETER = "DIAMETER";
const string AnimationCache::OC_PARAM_LOWER = "LOWER";
const string AnimationCache::OC_PARAM_GREATER = "GREATER";
const string AnimationCache::OC_PARAM_BETWEEN = "BETWEEN";

//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(AnimationCache,ObjMgrClient);


AnimationCache::AnimationCache(void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("AnimationCache::AnimationCache()")
	_modMsgSenderGetMeasures = new METKMsgSender();
	_modMsgSenderGetMainAxis = new METKMsgSender();
	_modObjXMLWriter = new METKObjXMLWriter();
	
	waitForMeasures = false;
	_msgGetMeasures_done = getFieldContainer()->addBool("_msgGetMeasures_done");
	_modMsgSenderGetMeasures->getFieldContainer()->getField("done")->attachField(_msgGetMeasures_done,1);

	waitForMainAxis = false;
	_msgGetMainAxis_done = getFieldContainer()->addBool("_msgGetMainAxis_done");
	_modMsgSenderGetMainAxis->getFieldContainer()->getField("done")->attachField(_msgGetMainAxis_done,1);


	fldReady = getFieldContainer()->addNotify("ready");

	myObjMgr = new ObjMgrCommunicator();
	myAnimationParser = new AnimationParser();


	timerSendRequest = new SoTimerSensor((SoSensorCB*)AnimationCache::timerSendRequestEvent, this);
    timerSendRequest->setInterval(SbTime(1.0/100.0));
    timerSendRequest->unschedule();

	timerFldReady = new SoTimerSensor((SoSensorCB*)AnimationCache::timerFldReadyEvent, this);
    timerFldReady->setInterval(SbTime(1.0/100.0));
    timerFldReady->unschedule();

	reset();
	clearAcceptedObjectIDs();
	clearAcceptedInfoLayerNames();
}



AnimationCache::~AnimationCache()
{
	ML_TRACE_IN("AnimationCache::~AnimationCache()");
    delete _modMsgSenderGetMeasures;
	delete _modMsgSenderGetMainAxis;
	delete _modObjXMLWriter;
	delete myObjMgr;
	//delete myAnimationParser;
	delete timerSendRequest;
	delete timerFldReady;
}


void AnimationCache::handleObjMgrNotification()
{
	ML_TRACE_IN("AnimationCache::handleObjMgrNotification()")
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



void AnimationCache::handleNotification(Field *field)
{	
	ML_TRACE_IN("AnimationCache::handleNotification()")
	inherited::handleNotification(field);	

	if (field == _msgGetMeasures_done && waitForMeasures)
	{	
		//kDebug::Debug("handleNotification   _msgGetMeasures_done->getBoolValue()="+kBasics::BoolToString(_msgGetMeasures_done->getBoolValue())+"  successful="+kBasics::BoolToString(((BoolField*)_modMsgSenderGetMeasures->getFieldContainer()->getField("successful"))->getBoolValue()),kDebug::DL_HIGH);
		if (_msgGetMeasures_done->getBoolValue() && ((BoolField*)_modMsgSenderGetMeasures->getFieldContainer()->getField("successful"))->getBoolValue())
		{			
			waitForMeasures = false;
			getAllMeasures(false);
		}
	}

	else if (field == _msgGetMainAxis_done && waitForMainAxis)
	{	
		//kDebug::Debug("handleNotification   _msgGetMainAxis_done->getBoolValue()="+kBasics::BoolToString(_msgGetMainAxis_done->getBoolValue())+"  successful="+kBasics::BoolToString(((BoolField*)_modMsgSenderGetMainAxis->getFieldContainer()->getField("successful"))->getBoolValue()),kDebug::DL_HIGH);
		if (_msgGetMainAxis_done->getBoolValue())
		{			
			waitForMainAxis = false;
			globalMeasuresListCounter++; //Der wird schon hier erhöht, damit er später nicht nochmal versucht die MainAxis zu holen, wo das doch evtl. fehlgeschlagen ist
			getAllMeasures(false);
		}
	}


	else if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderGetMeasures->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderGetMainAxis->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modObjXMLWriter->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}
}


const string AnimationCache::findSingleObjectID(string objName)
{	
	ML_TRACE_IN("AnimationCache::findSingleObjectID()")
	objName = kBasics::toUp(objName);
	string* tempName = htObjectIDs->find(objName);
	string strValue = "";
	
	if (tempName==NULL)
	{		
		//Get read-only access to object container
		const omObjectContainer *oc= getConstObjContainer();

		oc = getObjContainer();
		if(oc == NULL)
		{
			kDebug::Debug("ObjContainer not found!",kDebug::DL_LOW);
			return "";
		}		
		
		omObjectContainer ::const_iterator iterObj;
		for ( iterObj = oc->begin(); iterObj!=oc->end(); iterObj++)
		{
			string objID = iterObj->first;
			omObject tempObj = iterObj->second;

			if (!(*oc).exists(objID))
			{
				kDebug::Debug("invalidObject",kDebug::DL_LOW);
				return "";
			}

			myObjMgr->getObjAttributeString(objID,LAY_IMAGE,INF_IMAGETYPE,strValue);
				
			if (kBasics::toUp(strValue) == "SEGMENTATION")
			{
				if (tempObj.hasAttribute(LAY_DESCRIPTION,INF_NAME))
				{				
					myObjMgr->getObjAttributeString(objID,LAY_DESCRIPTION,INF_NAME,strValue);				
					if (kBasics::toUp(strValue) == objName)
					{														
						htObjectIDs->insert(objName,objID);															
						listObjects.push_back(objID);
						return objID;
					}				
				}
				
				if (tempObj.hasAttribute(LAY_GLOBAL,INF_ID))
				{				
					myObjMgr->getObjAttributeString(objID,LAY_GLOBAL,INF_ID,strValue);
					if (kBasics::toUp(strValue) == objName)
					{									
						htObjectIDs->insert(objName,objID);															
						listObjects.push_back(objID);
						return objID;
					}
				}
			}
		}

		kDebug::Debug("Object not found: "+objName,kDebug::DL_LOW);
		return "";
	}	
	return *tempName;
}


void AnimationCache::reset()
{
	ML_TRACE_IN("AnimationCache::reset()")
	htMeasures = new HashTable<Measures>();
	htObjectIDs = new HashTable<string>();
	htBBRequested = new HashTable<string>();
	htMainAxisRequested = new HashTable<string>();
	listObjects.clear();
	waitForMeasures = false;
	globalMeasuresListCounter = 0;
}


void AnimationCache::getAllMeasures(bool reset)
{	
	ML_TRACE_IN("AnimationCache::getAllMeasures()")
	if (reset) globalMeasuresListCounter=0;
	bool startAni = false;
	bool stop = false;
	bool stop2 = false;
	//kDebug::Debug("AnimationCache::getAllMeasures  globalMeasuresListCounter="+kBasics::IntToString(globalMeasuresListCounter),kDebug::DL_HIGH);
	//kDebug::Debug("AnimationCache::getAllMeasures  listObjects.size()="+kBasics::IntToString(listObjects.size()),kDebug::DL_HIGH);
	if (globalMeasuresListCounter<listObjects.size())
	{
		//kDebug::Debug("listObjects.size():"+kBasics::IntToString(listObjects.size()),kDebug::DL_HIGH);
		Measures tempMeasures;
		while (!stop && !stop2)
		{
			string objName = listObjects[globalMeasuresListCounter];
			if (htBBRequested->find(objName)==NULL)
			{
				htBBRequested->insert(objName,objName);
				if (!getMeasures(objName, tempMeasures))
				{
					//std::cout << "stop = true" << std::endl;
					stop = true;
				}
			}

			if (htMainAxisRequested->find(objName)==NULL && !stop)
			{
				htMainAxisRequested->insert(objName,objName);
				//kDebug::Debug("htMainAxisRequested->instert: "+objName,kDebug::DL_HIGH);
				if (!getMainAxis(listObjects[globalMeasuresListCounter], tempMeasures))
				{
					//std::cout << "stop2 = true" << std::endl;
					stop2 = true;
				}
			}

			if (!stop && !stop2)
			{	
				//std::cout << "counter ++" << std::endl;
				globalMeasuresListCounter++;
			}


			if (globalMeasuresListCounter>=listObjects.size())
			{
				stop = true;
				startAni = true;
			}
		}
	}
	else
	{		
		//std::cout <<  "startAni = true" << endl;
		startAni = true;
	}

	if (startAni) 
	{
		//fldReady->notify();
		timerFldReady->schedule();
	}
	else if (stop)
	{
		waitForMeasures = true;
		kDebug::Debug("send Bounding Box request for  "+listObjects[globalMeasuresListCounter],kDebug::DL_LOW);
		_modMsgSenderGetMeasures->getFieldContainer()->getField("message")->setStringValue("getMeasures");
		_modMsgSenderGetMeasures->getFieldContainer()->getField("data")->setStringValue(listObjects[globalMeasuresListCounter]); //ObjectID
		timerSendRequest->schedule();
	}	
	else if (stop2)
	{			
		waitForMainAxis = true;
		kDebug::Debug("send MainAxis request for  "+listObjects[globalMeasuresListCounter],kDebug::DL_LOW);
		_modMsgSenderGetMainAxis->getFieldContainer()->getField("message")->setStringValue("exactMeasures");
		_modMsgSenderGetMainAxis->getFieldContainer()->getField("data")->setStringValue(listObjects[globalMeasuresListCounter]); //ObjectID
		timerSendRequest->schedule();
	}	
}



//Sendet zeitverzögert den Request los, weil die Antwort unmittelbar danach kommen kann.
void AnimationCache::timerSendRequestEvent(void* data, SoDataSensor*)
{	
	ML_TRACE_IN("AnimationCache::timerSendRequestEvent()")
	((AnimationCache*)data)->timerSendRequest->unschedule();
	if (((AnimationCache*)data)->waitForMeasures)
	{
		kDebug::Debug("timer send Measure "+((StringField*)((AnimationCache*)data)->_modMsgSenderGetMeasures->getFieldContainer()->getField("data"))->getStringValue(),kDebug::DL_HIGH);
		((NotifyField*)((AnimationCache*)data)->_modMsgSenderGetMeasures->getFieldContainer()->getField("send"))->notify();					
	}
	else if (((AnimationCache*)data)->waitForMainAxis)
	{
		kDebug::Debug("timer send MainAxis "+((StringField*)((AnimationCache*)data)->_modMsgSenderGetMainAxis->getFieldContainer()->getField("data"))->getStringValue(),kDebug::DL_HIGH);
		((NotifyField*)((AnimationCache*)data)->_modMsgSenderGetMainAxis->getFieldContainer()->getField("send"))->notify();
	}
}


//Sendet zeitverzögert FldReady->notify()
void AnimationCache::timerFldReadyEvent(void* data, SoDataSensor*)
{	
	ML_TRACE_IN("AnimationCache::timerFldReadyEvent()")
	((AnimationCache*)data)->timerFldReady->unschedule();	
	((NotifyField*)((AnimationCache*)data)->fldReady)->notify();					
}



bool AnimationCache::getMeasures(const string objID, Measures& tempMeasures)
{
	ML_TRACE_IN("AnimationCache::getMeasures()")
	kDebug::Debug("getMeasure   objID="+objID,kDebug::DL_HIGH);
	SbVec3f tempVec;
	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_BBMAX, tempVec))
		tempMeasures.BB_max.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_BBMIN, tempVec))
		tempMeasures.BB_min.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_BBCENTER, tempVec))
		tempMeasures.BB_boxCenter.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;
			
	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_OBJECTCENTER, tempVec))
		tempMeasures.BB_objectCenter.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	kDebug::Debug("found=true getMeasure   objID="+objID,kDebug::DL_LOW);
	return true;
}


bool AnimationCache::getMainAxis(const string objID, Measures& tempMeasures)
{
	ML_TRACE_IN("AnimationCache::getMainAxis()")
	//kDebug::Debug("getMainAxis   objID="+objID,kDebug::DL_HIGH);
	SbVec3f tempVec;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_BARYCENTER, tempVec))
		tempMeasures.baryCenter.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	float tempFloat;
	if (myObjMgr->getObjAttributeFloat((omIDType)objID,LAY_MEASURES,INF_MAXDIAMETER, tempFloat))
		tempMeasures.maxDiameter = tempFloat;
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_MIDPOINT, tempVec))
		tempMeasures.midPoint.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_XAXIS, tempVec))
		tempMeasures.xAxis.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_YAXIS, tempVec))
		tempMeasures.yAxis.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeVec3f((omIDType)objID,LAY_MEASURES,INF_ZAXIS, tempVec))
		tempMeasures.zAxis.setValue(tempVec[0],tempVec[1],tempVec[2]);
	else return false;

	if (myObjMgr->getObjAttributeFloat((omIDType)objID,LAY_MEASURES,INF_XDIAMETER, tempFloat))
		tempMeasures.xDiameter  = tempFloat;
	else return false;

	if (myObjMgr->getObjAttributeFloat((omIDType)objID,LAY_MEASURES,INF_YDIAMETER, tempFloat))
		tempMeasures.yDiameter = tempFloat;
	else return false;

	if (myObjMgr->getObjAttributeFloat((omIDType)objID,LAY_MEASURES,INF_ZDIAMETER, tempFloat))
		tempMeasures.zDiameter = tempFloat;
	else return false;

	return true;
}





SbVec3f AnimationCache::getBoundingCenter(Measures myMeasures)
{	
	ML_TRACE_IN("AnimationCache::getBoundingCenter()")
	return (myMeasures.BB_max-myMeasures.BB_min)/2.0+myMeasures.BB_min;
}


AnimationCache::Measures AnimationCache::getMasterBoundingBox(MEASURESLIST* myMeasuresList)
{
	ML_TRACE_IN("AnimationCache::getMasterBoundingBox()")
	double sX = 100000.0;
	double sY = 100000.0;
	double sZ = 100000.0;
	double lX = -100000.0;
	double lY = -100000.0;
	double lZ = -100000.0;

	Measures MasterBB;

	MEASURESLIST::const_iterator iter;	
	for ( iter = myMeasuresList->begin();iter!=myMeasuresList->end(); iter++)
	{
		//hier ein Fehler drin ... dann kannste erstmal suchen ... alles schon passiert :o)
		Measures tempMeasures = Measures(*iter);		
		if (tempMeasures.BB_min[0]<sX)		
			sX=tempMeasures.BB_min[0];					
		if (tempMeasures.BB_min[0]>lX)
			lX=tempMeasures.BB_min[0];
		if (tempMeasures.BB_min[1]<sY)
			sY=tempMeasures.BB_min[1];
		if (tempMeasures.BB_min[1]>lY)
			lY=tempMeasures.BB_min[1];
		if (tempMeasures.BB_min[2]<sZ)
			sZ=tempMeasures.BB_min[2];
		if (tempMeasures.BB_min[2]>lZ)
			lZ=tempMeasures.BB_min[2];
		if (tempMeasures.BB_max[0]<sX)
			sX=tempMeasures.BB_max[0];
		if (tempMeasures.BB_max[0]>lX)
			lX=tempMeasures.BB_max[0];
		if (tempMeasures.BB_max[1]<sY)
			sY=tempMeasures.BB_max[1];
		if (tempMeasures.BB_max[1]>lY)
			lY=tempMeasures.BB_max[1];
		if (tempMeasures.BB_max[2]<sZ)
			sZ=tempMeasures.BB_max[2];
		if (tempMeasures.BB_max[2]>lZ)
			lZ=tempMeasures.BB_max[2];
	}

	//1 = smallest values
	MasterBB.BB_min[0]=lX;
	MasterBB.BB_min[1]=lY;
	MasterBB.BB_min[2]=lZ;
	//2 = largest values
	MasterBB.BB_max[0]=sX;
	MasterBB.BB_max[1]=sY;
	MasterBB.BB_max[2]=sZ;
	
	return MasterBB;
}


//Use only, if an AnimationParser is set (i.e. in AnimationExecuter)
AnimationCache::Measures AnimationCache::getMeasuresFromHT(const string objName) //kann Name oder ID (für einzelobjekte) sein
{
	ML_TRACE_IN("AnimationCache::getMeasuresFromHT()")
	Measures* tempMeasures = htMeasures->find(objName);
	if (tempMeasures==NULL)
	{				
		//First try for normal objects
		vector<string>* objectList = new std::vector<string>;
		myAnimationParser->getObjectStringList(objName.c_str(),objectList);
		if (objectList->size()==1 && findSingleObjectID((*objectList)[0])=="")
		{
			string tempError;
			string tempCommand = (*objectList)[0];
			objectList->clear(); //damit das object command aus der liste gelöscht wird und so platz für die neuen ersetzenden objekte ist
			tempError = parseSingleObjectCommand(tempCommand,objectList);			
		}
		if (objectList!=NULL)
		{										
			MEASURESLIST* myMeasuresList = new MEASURESLIST;
			for ( size_t i = 0; i<objectList->size(); i++)
			{			
				Measures* tempMeasures2 = htMeasures->find((*objectList)[i]);
				if (tempMeasures2==NULL)
				{						
					Measures tempMeasures3; //zur Sicherheit nicht doppelt verwendet, irgendwas war da					
					if (getMeasures(findSingleObjectID((*objectList)[i]),tempMeasures3))
					{
						myMeasuresList->push_back(tempMeasures3);
					}
					else
					{
						kDebug::Debug("Invalid Measure/Measure not in ObjMgr for: " + (*objectList)[i]);
					}
				}
				else
				{					
					myMeasuresList->push_back(*tempMeasures2);
				}
			}			

			Measures MasterBB;
			MasterBB = getMasterBoundingBox(myMeasuresList);
			htMeasures->insert(objName,MasterBB);
			return MasterBB;
		}
		return *(new Measures());
	}	
	return *tempMeasures;
}




string AnimationCache::parseSingleObjectCommand(const string objectLine, std::vector<string>* returnStringList)
{
	ML_TRACE_IN("AnimationCache::parseSingleObjectCommand()")
	string parseInput = kBasics::replaceString(objectLine, "  ", " "); //remove double-spaces	
	std::vector<string> splitted;
	kBasics::splitQuotated(parseInput,' ',kBasics::QUOTATION_DOUBLE,&splitted);

	string commandStr = kBasics::toUp(splitted[0]);
	string paramStr = "";
	for (int i=1; i<splitted.size(); i++)
	{
		if (i!=1) paramStr = paramStr + " ";
		paramStr = paramStr + splitted[i];		
	}
	
	if (commandStr==OC_OBJECTSBYSIZE)
	{
		if (myAnimationParser)
		{
			std::vector<string> paramList;
			kBasics::splitQuotated(paramStr,' ',kBasics::QUOTATION_DOUBLE, &paramList); //MAX INT toDo
			if (paramList.size()>0)
			{
				std::vector<string> objectsRange;
				myAnimationParser->getObjectStringList(kBasics::trimQuotatedStr(paramList[0], kBasics::QUOTATION_DOUBLE).c_str(), &objectsRange);

				if (paramList.size()>1)
				{					
					if (kBasics::toUp(paramList[1])==OC_PARAM_DIAMETER)
					{				
						if (paramList.size()>3)
						{					
							for (int i=0; i<objectsRange.size(); i++)
							{						
								string objID = findSingleObjectID(objectsRange[i]);
								if (objID != "")
								{								
									float maxDiameter;										
									myObjMgr->getObjAttributeFloat(objID, LAY_MEASURES, INF_MAXDIAMETER, maxDiameter);
									if ((maxDiameter>=kBasics::StringToFloat(paramList[3]) && kBasics::toUp(paramList[2])==OC_PARAM_GREATER) ||
										(maxDiameter<=kBasics::StringToFloat(paramList[3]) && kBasics::toUp(paramList[2])==OC_PARAM_LOWER))
									{
										returnStringList->push_back(objID);										
									}
									else if (kBasics::toUp(paramList[2])==OC_PARAM_BETWEEN)
									{
										vector<string> range_split;
										kBasics::split(paramList[3],kScriptFrameEntry::DELIMITER_AXIS,2,&range_split);
										if (range_split.size()==2)
										{
											if (maxDiameter>=kBasics::StringToFloat(range_split[0]) &&
												maxDiameter<=kBasics::StringToFloat(range_split[1]))
											{
												returnStringList->push_back(objID);												
											}
										}
										else return kDebug::ERROR_OC_WRONGPARAMETER;

									}
								}
							}
						}
						else return kDebug::ERROR_OC_WRONGPARAMETER;
					}
				}
			}			
		}
	}
	else
	{
		return kDebug::ERROR_OC_UNKNOWNCOMMAND;
	}

	return kDebug::ERROR_NONE;
}



void AnimationCache::setObjMgr(ObjMgrCommunicator* value)
{
	ML_TRACE_IN("AnimationCache::setObjMgr()")
	myObjMgr = value;
}


void AnimationCache::setAnimationParser(AnimationParser* value)
{
	ML_TRACE_IN("AnimationCache::setAnimationParser()")
	myAnimationParser = value;
}



string AnimationCache::getObjXML(bool force)
{
	ML_TRACE_IN("AnimationCache::getObjXML()")
	if (force || _modObjXMLWriter->getFieldContainer()->getField("objXML")->getStringValue()=="")
	{
		kDebug::Debug("Create new internal ObjXML for Animation",kDebug::DL_LOW);
		((NotifyField*)_modObjXMLWriter->getFieldContainer()->getField("run"))->notify();
		//wird direkt ausgeführt ...
		//std::cout << "getObjXML direkt nach notify():" << _modObjXMLWriter->getFieldContainer()->getField("objXML")->getStringValue() << std::endl;
	}
	//std::cout << _modObjXMLWriter->getFieldContainer()->getField("objXML")->getStringValue() << std::endl;
	return _modObjXMLWriter->getFieldContainer()->getField("objXML")->getStringValue();
}


ML_END_NAMESPACE
