// Local includes
#include "AnimationExecuter.h"

#include "mlUMDAnimation2.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(AnimationExecuter,ObjMgrClient);


const string AnimationExecuter::AS_PLAY = "Play";
const string AnimationExecuter::AS_PAUSE = "Pause";
const string AnimationExecuter::AS_STOP = "Stop";


/**	Constructor für AnimationExecuter\n
- Erstellung eines eigenen ObjMgrCommunicator\n
- Erstellung eines eigenen AnimationCache\n
*/
AnimationExecuter::AnimationExecuter() : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("AnimationExecuter::AnimationExecuter()");
	myObjMgr = new ObjMgrCommunicator();
	myCache = new AnimationCache();
	myAnimationParser = new AnimationParser();

	realtimeTimer = new SoTimerSensor((SoSensorCB*)AnimationExecuter::realtimeTimerEvent, this);
	realtimeTimer->setInterval(SbTime(1.0/5000.0)); //So hoch getimed ... war der entscheidende Hemmungsfaktor 2006-01-13 bei der Erhöhung der fps bei realTime Animations
	realtimeTimer->unschedule();

	videoFrameTimer = new SoTimerSensor((SoSensorCB*)AnimationExecuter::videoFrameTimerEvent, this);
	videoFrameTimer->setInterval(SbTime(1.0/1000.0));
	videoFrameTimer->unschedule();

	mainCamera.setNormPlump(SbVec3f(0.0,0.0,1.0));
	mainCamera.setUpVecAngle(0.0); //muss sein, da sonst undefiniert ... wird gebraucht, bevor das erstemal der eigentliche UpVecAngle bestimmt wird
	mainCamera.setLookAt(SbVec3f(0,0,0));	

	waitingFor_getAllMeasures = false;
	_fs_cache_getAllMeasures = new FieldSensor(AnimationExecuter::fieldChanged_cache_getAllMeasures, this);
	_fs_cache_getAllMeasures->attachField(myCache->getFieldContainer()->getField("ready"));	

	_modMsgSenderGetCamPos = new METKMsgSender();
	_msgGetCamPos_done = getFieldContainer()->addBool("_msgGetCamPos_done");
	_modMsgSenderGetCamPos->getFieldContainer()->getField("done")->attachField(_msgGetCamPos_done,1);	
	waitingFor_getCamPos = false;
	gotCamPos = false;	
	
	_modMsgSenderGetCamPath = new METKMsgSender();
	_msgGetCamPath_done = getFieldContainer()->addBool("_msgGetCamPath_done");
	_modMsgSenderGetCamPath->getFieldContainer()->getField("done")->attachField(_msgGetCamPath_done,1);	
	waitingFor_getCamPath = false;
	gotCamPath = false;

	_modMsgSenderClusteredCamPos = new METKMsgSender();
	_msgClusteredCamPos_done = getFieldContainer()->addBool("_msgClusteredCamPos_done");
	_modMsgSenderClusteredCamPos->getFieldContainer()->getField("done")->attachField(_msgClusteredCamPos_done,1);	
	waitingFor_clusteredCamPos = false;
	gotClusteredCamPos = false;	
	wroteClusteredCamPos = false;

	_modMsgSenderExecFading = new METKMsgSender();
	_msgExecFading_done = getFieldContainer()->addBool("_msgExecFading_done");
	_modMsgSenderExecFading->getFieldContainer()->getField("done")->attachField(_msgExecFading_done,1);	
	waitingFor_execFading = false;
	gotFading = false;	

	fldAnimationViewerName = getFieldContainer()->addString("animationViewerName");
	fldAnimationViewerName->setStringValue("");
	fldClearMeasureCache = getFieldContainer()->addBool("clearMeasureCache");
	fldClearMeasureCache->setBoolValue(true);
	fldTestSphereCenter = getFieldContainer()->addVec3f("testSphereCenter");
	fldTestSphereRadius = getFieldContainer()->addDouble("testSphereRadius");
	fldTestCrossPoint = getFieldContainer()->addVec3f("testCrossPoint");
	fldTestCamPosition = getFieldContainer()->addVec3f("testCamPosition");
	fldTestTargetPosition = getFieldContainer()->addVec3f("testTargetPosition");
	fldTestSphereAngle = getFieldContainer()->addDouble("testSphereAngle");
	fldDrawCameraPath = getFieldContainer()->addBool("drawCameraPath");
	fldDrawCameraPath->setBoolValue(false);

	fldPlayStatus = getFieldContainer()->addString("playStatus");
	fldPlayStatus->setStringValue(AS_STOP);
	fldAnimationSpeed = getFieldContainer()->addFloat("animationSpeed");
	fldAnimationSpeed->setFloatValue(1.0);	

	fldCampathVPSize = getFieldContainer()->addFloat("campathVPSize");
	fldCampathVPSize->setFloatValue(1.0);

	fldWriteLowLevelScript = getFieldContainer()->addBool("writeLowLevelScript");
	fldWriteLowLevelScript->setBoolValue(false);

	fldFramerate = getFieldContainer()->addInt("framerate");
	fldFramerate->setIntValue(7);

	fldOffScreenReady = getFieldContainer()->addNotify("offScreenReady");
	fldOffScreenFilename = getFieldContainer()->addString("offScreenFilename");
	fldOffScreenFolder = getFieldContainer()->addString("offScreenFolder");		
	fldOffScreenRender = getFieldContainer()->addNotify("offScreenRender");
	fldOffScreenInit = getFieldContainer()->addNotify("offScreenInit");
	fldOffScreenFinishAVI = getFieldContainer()->addNotify("offScreenFinishAVI");

	fldUseOffscreenRenderer = getFieldContainer()->addBool("useOffscreenRenderer");
	fldUseOffscreenRenderer->setBoolValue(true);

	ResetAll(true);		

	addAcceptedObjectID(fldAnimationViewerName->getStringValue());   
	addAcceptedInfoLayerName(LAY_VIEWER_PROPERTIES);
	myCamPath=0;
}




AnimationExecuter::~AnimationExecuter()
{
	ML_TRACE_IN("AnimationExecuter::~AnimationExecuter()");
	delete myObjMgr;
	delete myCache;
	//delete myAnimationParser;
	delete realtimeTimer;
	delete videoFrameTimer;
	delete _modMsgSenderGetCamPos;
	delete _modMsgSenderGetCamPath;
	delete _modMsgSenderClusteredCamPos;
	delete _fs_cache_getAllMeasures;
	delete _modMsgSenderExecFading;
	delete myCamPath;
}


/**	handleNotification vom AnimationExecuter
*/
void AnimationExecuter::handleNotification(Field *field)
{
	ML_TRACE_IN("AnimationExecuter::handleNotification()");
	inherited::handleNotification(field);

	//Verbinden des Caches, des ObjCommunicator und des MsgSenders für Neugis CamPos
	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)			
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(myCache->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderGetCamPos->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderGetCamPath->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderClusteredCamPos->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(_modMsgSenderExecFading->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}

	//Weitermachen, nachdem die beste CamPos gesetzt wurde
	else if (field == _msgGetCamPos_done && waitingFor_getCamPos)
	{
		if (_msgGetCamPos_done->getBoolValue() && ((BoolField*)_modMsgSenderGetCamPos->getFieldContainer()->getField("successful"))->getBoolValue())
		{
			//continue
			kDebug::Debug("continue ...");			
			waitingFor_getCamPos = false;
			gotCamPos = true;
			setFrame(returnFrame,returnEntry);
		}
	}

	//Weitermachen, nachdem der beste CamPath gesetzt wurde
	else if (field == _msgGetCamPath_done && waitingFor_getCamPath)
	{
		if (_msgGetCamPath_done->getBoolValue() && ((BoolField*)_modMsgSenderGetCamPath->getFieldContainer()->getField("successful"))->getBoolValue())
		{
			//continue
			kDebug::Debug("continue CamPathVP");			
			waitingFor_getCamPath = false;
			gotCamPath = true;
			setFrame(returnFrame,returnEntry);
		}
	}

	//Weitermachen, nachdem die beste ClusteringPos gesetzt wurde
	else if(field == _msgClusteredCamPos_done && waitingFor_clusteredCamPos){
		if(_msgClusteredCamPos_done->getBoolValue() && ((BoolField*)_modMsgSenderClusteredCamPos->getFieldContainer()->getField("successful"))->getBoolValue()){
			waitingFor_clusteredCamPos = false;
			gotClusteredCamPos = true;
			setFrame(returnFrame,returnEntry);
		}
	}

	//Weitermachen, nachdem Fading ausgeführt wurde
	else if (field == _msgExecFading_done && waitingFor_execFading)
	{
		if (_msgExecFading_done->getBoolValue() && ((BoolField*)_modMsgSenderExecFading->getFieldContainer()->getField("successful"))->getBoolValue())
		{
			//continue
			kDebug::Debug("continue ...");			
			waitingFor_execFading = false;
			gotFading = true;
			setFrame(returnFrame,returnEntry);
		}
	}


	else if (field == fldOffScreenReady && isWaitingForRenderingReady)
	{		
		isWaitingForRenderingReady = false;		
		if (!myList.iniObject.getRealTime()) videoFrameTimer->schedule();//setFrame(currentFrame);
	}

	else if (field == fldAnimationViewerName)
	{
		clearAcceptedObjectIDs();
		addAcceptedObjectID(fldAnimationViewerName->getStringValue());   
	}
}



/** Behandlung der ObjMgr-Events\n
Behandelte Events:\n
- ViewerStatus: ready -> wahlweise nächster Frame (wenn nicht realtime) oder Erzeugung eines Videoes
\return void  
*/
void AnimationExecuter::handleObjMgrNotification()
{
	ML_TRACE_IN("AnimationExecuter::handleObjMgrNotification()");
	omEventContainer myEventList = getEventContainer();

	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;	
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);
		//Viewer-Status changed
		if (myEvent.objectID==fldAnimationViewerName->getStringValue() && myEvent.layerID==LAY_VIEWER_PROPERTIES && myEvent.infoID==INF_VIEWER_PROPERTIES_VIEWERSTATUS)
		{
			string tempAttr;
			if (myObjMgr->getObjAttributeString(myEvent.objectID, myEvent.layerID, myEvent.infoID, tempAttr))
			{
				if (tempAttr=="Ready" && isWaitingForRenderingReady)
				{											
					if (fldUseOffscreenRenderer->getBoolValue()==false)
					{
						isWaitingForRenderingReady = false;														
						if (globalCreateVideo)
						{								
							myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_CAPTUREFRAME,new omMessage("ping"),omINFOTYPE_MESSAGE,true,false);
							sendNotification();
						}																						
						if (!myList.iniObject.getRealTime()) videoFrameTimer->schedule();//setFrame(currentFrame);
					}
				}
			}
		}
	}
	clearEventContainer();
}



/** Startet die Animation
\return void  
*/
void AnimationExecuter::startAnimation()
{
	ML_TRACE_IN("AnimationExecuter::startAnimation()");

	//Jetzt sind alle Daten da und das ganze kann noch mal nach ObjectCommands geparst werden.
	kDebug::Debug("Name in AEX: "+fldAnimationViewerName->getStringValue(),kDebug::DL_LOW);
	kDebug::Debug("Speed in AEX: "+fldAnimationSpeed->getStringValue(),kDebug::DL_LOW);

	//Suche nach möglichen ObjectCommands
	parseObjectCommands();

	//Am Start werden einmal die Koordinaten eingeholt ... der ExViewer korrigiert sie eigenmächtig nach ... und das passt uns hier nicht in den Kram
	mainCamera.setHeight(getCamHeight());		
	mainCamera.setOrientation(getCamOrientation());		
	mainCamera.setPosition(getCamPosition());	
	mainCamera.setFocalDistance(getCamFocalDistance());		

	//Frame-Based Version	
	if (!myList.iniObject.getRealTime())
	{
		globalCreateVideo = true; //wird erstmal gesetzt; kann durch Skript verändert werden
		//Abbruch möglicherweise laufender Recordingprozesse

		if (fldUseOffscreenRenderer->getBoolValue())
		{
			fldOffScreenInit->notify();
		}

		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_CANCELRECORDING,new omMessage("ping"),omINFOTYPE_MESSAGE,true,false);
		sendNotification();			
		currentFrame = 0;
		fldPlayStatus->setStringValue(AS_PLAY);
		//setFrame(currentFrame);
		videoFrameTimer->schedule();
	}
	//Real-Time Version
	else 
	{		
		globalCreateVideo = false;
		globalDurchlaufCounter = 0;
		currentFrame = -1; //damit auch bei 0-Frame (also genau 1 Frame) Scripts currentFrame!=lastFrame ist		
		setFrame(currentFrame);		
		//Die Startzeit wird erst nach dem ersten Frame genommen; so können zeitaufwendige Prozesse dort ablaufen
		globalStartTime = kBasics::getCurrentTime();
		fldPlayStatus->setStringValue(AS_PLAY);
		lastFrameTime = globalStartTime;		
		realtimeTimer->schedule();
	}
}



/** Ersetzt ein ObjectCommand durch die einzelnen ermittelten Objekte.\n
Diese werde aus dem Cache mit parseSingleObjectCommand(...) ermittelt.\n
Jeder Frame, über den sich ein solches ObjcetCommand erstreckt wird um die Anzahl der Anweisungen pro ersetztes Objekt vergrößert und die Anweisungen werden zu jedem FrameObjekt hinzugefügt
\return int  
*/
void AnimationExecuter::parseObjectCommands()
{
	ML_TRACE_IN("AnimationExecuter::parseObjectCommands()");
	for (int i=0;i<myCache->myList_ObjectCommands.size();i++)
	{		
		int FrameNr = myCache->myList_ObjectCommands[i].first;
		int EntryNr = myCache->myList_ObjectCommands[i].second;

		//Parse Object
		kScriptFrameEntry* tempCommand = myList.FrameList[FrameNr]->EntryList[EntryNr];

		//So hier hat er jetzt schonmal die Liste der Objekte:
		vector<string> objectList;
		myCache->parseSingleObjectCommand(tempCommand->getObjectName(),&objectList);

		if (objectList.size()==0)
		{
			kDebug::Debug(" ----- Error in parseObjectCommands - empty objectList " + tempCommand->getObjectName() + " --- this can be a wrong ObjectCommand or an unknown object (e.g. from setBaseColors)" ,kDebug::DL_HIGH);
		}
		else
		{
			//Resize EntryList for this frame
			int oldSize = myList.FrameList[FrameNr]->EntryList.size();
			if (objectList.size()>0) myList.FrameList[FrameNr]->EntryList.resize(myList.FrameList[FrameNr]->EntryList.size()+objectList.size()-1); //-1 für das wegfallende Commad				

			//Save entry to change/split
			kScriptFrameEntry* entryToSplit = myList.FrameList[FrameNr]->EntryList[EntryNr];

			//Copy Entries einen nach vorne
			int j;
			for (j=EntryNr; j<oldSize-1; j++) myList.FrameList[FrameNr]->EntryList[j]=myList.FrameList[FrameNr]->EntryList[j+1];			

			//Sets the new entries and their object names 
			if (objectList.size()>0)
			{
				for (j=oldSize-1;j<myList.FrameList[FrameNr]->EntryList.size();j++)
				{
					kScriptFrameEntry* tempEntry = new kScriptFrameEntry(*entryToSplit);
					myList.FrameList[FrameNr]->EntryList[j]=tempEntry;
					string tempXYZ = objectList[j-(oldSize-1)];
					myList.FrameList[FrameNr]->EntryList[j]->setObjectName(tempXYZ); //XXXX!!!!
					myList.FrameList[FrameNr]->EntryList[j]->setObjectID(tempXYZ);
				}	
			}

			//Correct possible other EntryNr with ObjectCommands in this frame
			for (j=i+1; j<myCache->myList_ObjectCommands.size();j++)
			{
				if (FrameNr==myCache->myList_ObjectCommands[j].first)
				{
					if (myCache->myList_ObjectCommands[j].second>EntryNr) //Das ist immer der Fall! wenn alles richtig geht
					{
						myCache->myList_ObjectCommands[j].second = myCache->myList_ObjectCommands[j].second - 1;
					}
				}		
			}

			//Fall: Kein Object in Liste
			//Nachdem die Anweisung gelöscht wurde und alles nach vorn verschoben wurde kann die size auch verkleinert werden
			if (objectList.size()==0) myList.FrameList[FrameNr]->EntryList.resize(myList.FrameList[FrameNr]->EntryList.size()-1);
		}
	}	
}



/**	Sucht zu jedem Objekt die ID im ObjMgr raus.\n
Sollte keine ID gefunden werden, so wird das Objekt als potentielles ObjectCommand der Liste myList_ObjectCommands im Cache hinzugefügt.
\return int
*/
void AnimationExecuter::findObjectIDs()
{	
	ML_TRACE_IN("AnimationExecuter::findObjectIDs()");
	myCache->myList_ObjectCommands.clear();
	for (kScriptFrameList::size_type i = 0; i<myList.FrameList.size(); i++)
	{
		for (size_t j = 0; j<myList.FrameList[i]->EntryList.size();j++)
		{
			if (kBasics::toUp(myList.FrameList[i]->EntryList[j]->getObjectName())!="SYSTEM" &&
				kBasics::toUp(myList.FrameList[i]->EntryList[j]->getObjectName())!="CAM" &&
				myList.FrameList[i]->EntryList[j]->getObjectName()!="")
			{
				//Find ObjectID by name				
				myList.FrameList[i]->EntryList[j]->setObjectID("");
				string tempObjID = myCache->findSingleObjectID(myList.FrameList[i]->EntryList[j]->getObjectName());
				kDebug::Debug("getObjectName()="+myList.FrameList[i]->EntryList[j]->getObjectName()+"  => "+tempObjID+" (tempObjID)", kDebug::DL_LOW);
				if (tempObjID!="") myList.FrameList[i]->EntryList[j]->setObjectID(tempObjID);				
				else
				{
					if (myList.FrameList[i]->EntryList[j]->getObjectName()!="")
					{											
						myCache->myList_ObjectCommands.push_back(AnimationCache::int_pair(i,j));
					}
					else kDebug::Debug("Object not found " + myList.FrameList[i]->EntryList[j]->getObjectName()+";  It was not interpreted as an object-command and was not found in the ObjMgr.",kDebug::DL_HIGH);
				}
			}
		}		
	}	
}



/** Setzt den AnimationExecuter zurück\n
Es wird eine neuer AnimationParser erstellt.\n
\param  refreshMeasuresCache - entscheidet, ob auch der Cache (ObjectID-Zuordnungen etc.) gelöscht werden soll
\return void   
*/
void AnimationExecuter::ResetAll(bool refreshMeasuresCache, bool createNewParser)
{
	ML_TRACE_IN("AnimationExecuter::ResetAll()");
	kDebug::Debug("ResetAll refreshMeasuresCache="+kBasics::BoolToString(refreshMeasuresCache),kDebug::DL_LOW);
	if (createNewParser || myAnimationParser==NULL)
	{
		myAnimationParser = new AnimationParser();
	}

	myCache->setAnimationParser(myAnimationParser);

	isWaitingForRenderingReady = false;		
	waitingFor_getCamPos = false;
	waitingFor_getCamPath = false;
	waitingFor_clusteredCamPos = false;
	waitingFor_execFading = false;
	globalCreateVideo = true;

	if (refreshMeasuresCache)
	{
		kDebug::Debug("refresh Measures Cache in ResetAll of AnimationExecuter",kDebug::DL_LOW);
		myCache->reset();
		myCache->getObjXML(true); //force creation of ObjXML
	}
}



/** Führt alle Anweisungen eines Frames aus.\n
Die Abarbeitung eines Frame kann zwischenzeitlich auch unterbrochen werden. Mit Hilfe der FrameNr und des FrameEntryListIter kann an die entsprechende Stelle zurückgesprungen werden.\n
Dies wird nötig, wenn zwischenzeitlich ein optimaler viewpoint ermittelt werden muss. 
\param  frameNr Nummer des Frames, dessen Anweisungen ausgeführt werden sollen
\param  FrameEntryListIter Iteratorposition einer möglichen Anweisung(Entry) innerhalb des Frames (default = 0)
\return bool  
*/
bool AnimationExecuter::setFrame(int frameNr, size_t FrameEntryListIter)
{		
	ML_TRACE_IN("AnimationExecuter::setFrame()");
	//std::cout << "frame:" << frameNr <<  "  waitingFor_getCamPos=" << waitingFor_getCamPos << std::endl;
	if (fldPlayStatus->getStringValue()==AS_STOP) return false;
	if ((kScriptFrameList::size_type)frameNr<myList.FrameList.size())
	{
		if (!myList.iniObject.getRealTime()) kDebug::Debug("--- Frame: " + kBasics::IntToString(frameNr)+ "   of "+kBasics::IntToString(myList.FrameList.size()-1),kDebug::DL_NONE);

		kScriptFrameEntry* currentEntry;
		vector<string>* tempEntryParams;
		while (FrameEntryListIter<myList.FrameList[frameNr]->EntryList.size() && !waitingFor_getCamPos && !waitingFor_execFading && !waitingFor_clusteredCamPos && !waitingFor_getCamPath)
		{
			currentEntry = myList.FrameList[frameNr]->EntryList[FrameEntryListIter];
			//kDebug::Debug("currentEntry->getObjectName()="+currentEntry->getObjectName()+";  currentEntry->getObjectID()"+currentEntry->getObjectID()+";  frameNr:"+kBasics::IntToString(frameNr)+"   FrameEntryListIter:"+kBasics::IntToString(FrameEntryListIter)+"   ParamStr:*"+currentEntry->getParamString()+"*  currentEntry->getCurrentFrame()"+kBasics::IntToString(currentEntry->getCurrentFrame()),DL_LOW);			
			tempEntryParams = currentEntry->getParams(); //Die Größe wird später nicht abgefragt. Da zerreißt es mich hier höllisch, wenn ich auf ..[0] zugreife und gar kein Parameter existiert ... da muss man mal irgendein Exception-Konzept erarbeiten

			if (currentEntry->getCommandType()==kScriptFrameEntry::CMDT_LAYER)
			{				
				commandLayer(tempEntryParams, currentEntry);
			}

			else if (currentEntry->getCommandType() == kScriptFrameEntry::CMDT_SYSTEM)
			{
				if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAM_ROTATECAMZ ||
					currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAM_ROTATECAMZTO)
				{
					commandRotateCam(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_ROTATE)
				{
					commandRotate(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMFOCALDISTANCE)
				{
					commandCamFocalDistance(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMHEIGHT)
				{
					commandCamHeight(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMORIENTATION)
				{					
					commandCamOrientation(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_LOOKAT)
				{					
					commandLookAt(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMPOSITION)
				{
					commandCamPosition(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_MOVE)
				{						
					commandMove(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_VIEWPOINTMOVE)
				{
					commandViewpointMove(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_VIEWPOINTCLUSTERING)
				{
					if(!gotClusteredCamPos){
						commandViewpointClustering(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
					}else{
						if(!wroteClusteredCamPos){
							wroteClusteredCamPos=true;
							myObjMgr->getObjAttributeString("CameraSolver","Camera","ClusteringResult",clusteredResult);
						}
						//currentEntry->setCommandStr("CAMPATH");
					}
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_VOLREND)
				{
					commandVolRend(currentEntry, frameNr);
				}

				else if (currentEntry->getCommandID() ==  kScriptFrameEntry::CMD_MOVECOORD)
				{					
					commandMoveCoord(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() ==  kScriptFrameEntry::CMD_SETCREATEVIDEO)
				{
					commandCreateVideo(tempEntryParams);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETBACKGROUND)
				{
					commandBackground(tempEntryParams);
				}
				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_COLORNEW)
				{						
					commandColor(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETSILHOUETTECOLOR)
				{	
					commandSilhouetteColor(tempEntryParams, currentEntry);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CLIP)
				{
					commandClip(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}			

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_FADING)
				{
					commandFading(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SHOWCLUSTERINGPATH)
				{
					//waitingFor_clusteredCamPos=false;
					commandShowClusteringPath(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMPATH_VP){
					commandCampathVP(tempEntryParams, currentEntry, frameNr, FrameEntryListIter);
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAMPATH)
				{
					//std::cout << "Es wird eine CamPath-Anweisung ausgefuehrt.  FrameNr:" << frameNr << std::endl;
					//Die schon vorher berechnet Werte werden für diesen Frame ausgeführt
					//In der Funktion werden sie wenn nötig vorberechnet
					if(!myCamPath){
						myCamPath=new LinearCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,"Linear");
					}
					myCamPath->writeCalculatedCamPathValues(tempEntryParams, currentEntry, frameNr, FrameEntryListIter, currentEntry->getCommandID());
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETCAMPATH){
					if(currentEntry->getParams()->size()==1){
						std::string type=(string)(*currentEntry->getParams())[0];
						if(!myCamPath || myCamPath->pathName!=type){
							if(myCamPath){
								std::cout << "IM DELETE CAMPATH" << std::endl;
								delete myCamPath;
							}
							if(type=="Parable"){
								myCamPath=new GoogleCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,type);
							}else if(type=="Rectangle"){
								myCamPath=new RectangleCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,type);
							}else if(type=="ParableRectangle"){
								myCamPath=new GoogleRectangleCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,type);
							}else if(type=="Height"){
								myCamPath=new HeightCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,type);
							}else{
								myCamPath=new LinearCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,type);
							}
						}
					}
				}

				else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETDOUBLE ||
					currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETSTRING ||
					currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETVEC3 ||
					currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETBOOL ||
					currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETINT)
				{
					commandSetType(tempEntryParams, currentEntry);
				}
			}

			else
			{
				kDebug::Debug("NO CMDT"+currentEntry->getCommandStr()+"..."+kBasics::IntToString(currentEntry->getCommandID()),kDebug::DL_HIGH);
			}

			if (!waitingFor_getCamPos && !waitingFor_execFading && !waitingFor_clusteredCamPos && !waitingFor_getCamPath) FrameEntryListIter++;
		}

		mainCamera.setUpVecAngle(0.0);
		if (!waitingFor_getCamPos && !waitingFor_execFading && !waitingFor_clusteredCamPos && !waitingFor_getCamPath)
		{			
			if (!myList.iniObject.getRealTime())
			{
				currentFrame++;				
				if (fldUseOffscreenRenderer->getBoolValue()==false)
				{
					myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS, new bool(true), omINFOTYPE_BOOL, true, false); //Das muss immer wieder eingeschaltet werden, weil z.B. fading es ausschaltet, wenn fading fertig ist.
					myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_VIEWERSTATUS,new omMessage("Waiting"),omINFOTYPE_MESSAGE,true,false);
					myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT,new omMessage("ping"),omINFOTYPE_MESSAGE,true,false);
				}
			}

			sendNotification();		
			isWaitingForRenderingReady = true;
		}	
		/*else
		{
			std::cout << "waitingFor_getCamPos=" << waitingFor_getCamPos << "   waitingFor_execFading=" << waitingFor_execFading << "waitingFor_clusteredCamPos=" << waitingFor_clusteredCamPos << "waitingFor_getCamPath=" << waitingFor_getCamPath << std::endl;
		}*/
	}

	if ((kScriptFrameList::size_type)frameNr>=myList.FrameList.size() || currentFrame>=myList.FrameList.size())
	{
		isWaitingForRenderingReady = false;
		fldPlayStatus->setStringValue(AS_STOP);
		kDebug::Debug("Fertich :o)",kDebug::DL_NONE);
		wroteClusteredCamPos=false;
		gotClusteredCamPos=false;
		if (globalCreateVideo)
		{
			kDebug::Debug("Create video",kDebug::DL_NONE);
			if (fldUseOffscreenRenderer->getBoolValue())
				fldOffScreenFinishAVI->notify();
			else
				myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_CREATEAVI,new omMessage("ping"),omINFOTYPE_MESSAGE,true,false);
		}
		myObjMgr->setObjAttribute(O_ANIMATION, "Script", "Status", new omMessage("ready"), omINFOTYPE_MESSAGE, true, false);
		sendNotification();		
	}	
	//gotClusteredCamPos=false;
	//wroteClusteredCamPos=false;
	return true;
}



void AnimationExecuter::videoFrameTimerEvent(void* data, SoDataSensor*)
{	
	((AnimationExecuter*)data)->videoFrameTimer->unschedule();
	((AnimationExecuter*)data)->setFrame(((AnimationExecuter*)data)->currentFrame);

	//NOCH DURCH EIGENEN TIMER REALISIEREN(???)
	if (((AnimationExecuter*)data)->isWaitingForRenderingReady) //Nur, wenn er mit dem Warten auf einen neuen Frame aus setFrame kommt, soll auch gerendert werden
	{
		///////////?
		if (((AnimationExecuter*)data)->fldUseOffscreenRenderer->getBoolValue())
		{
			((AnimationExecuter*)data)->fldOffScreenFilename->setStringValue(((AnimationExecuter*)data)->fldOffScreenFolder->getStringValue()+"/frame"+kBasics::leadingZero(((AnimationExecuter*)data)->currentFrame,5)+".bmp");
			((AnimationExecuter*)data)->fldOffScreenRender->notify();
		}
	}
}


void AnimationExecuter::realtimeTimerEvent(void* data, SoDataSensor*)
{	
	ML_TRACE_IN("AnimationExecuter::realtimeTimerEvent()");
	((AnimationExecuter*)data)->setRealtimeTimerAction();
}


/**	Wird nach jedem Timer-Tick einer Realtime Animation aufgerufen und setzt entsprechende Frames
\return void
*/
void AnimationExecuter::setRealtimeTimerAction()
{	
	ML_TRACE_IN("AnimationExecuter::setRealtimeTimerAction()");
	globalDurchlaufCounter++;

	int lastFrame = currentFrame;
	currentFrame = lastFrame + kBasics::round((kBasics::getCurrentTime()-lastFrameTime)*fldAnimationSpeed->getFloatValue()*fldFramerate->getIntValue());

	if (currentFrame<0) currentFrame=0;

	if (currentFrame>=myList.frameCount)
	{				
		currentFrame=myList.frameCount;
		realtimeTimer->unschedule();
		//Debug("set remaining Important Frames ...  "+myList.ImportantFrames.size()="+kBasics::IntToString(myList.ImportantFrames.size())+"   currentFrame="+kBasics::IntToString(currentFrame),DL_HIGH);				
		for (std::vector<bool>::size_type i = currentFrame; i<myList.ImportantFrames.size(); i++)		
		{
			if (myList.ImportantFrames[i]) setFrame(i);
		}		
		sendNotification();

		globalEndTime = kBasics::getCurrentTime();		

		std::stringstream sstrTime;
		sstrTime << (globalEndTime - globalStartTime);		

		std::stringstream sstrTemp1, sstrTemp2;
		sstrTemp1 << globalDurchlaufCounter;
		sstrTemp2 << (globalDurchlaufCounter/(globalEndTime - globalStartTime));
		kDebug::Debug("Zeit (s): "+sstrTime.str() + "   Frames: "+sstrTemp1.str()+"  fps: "+sstrTemp2.str(),kDebug::DL_HIGH);
		myObjMgr->setObjAttribute(O_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_STATUS, new omMessage("ready"), omINFOTYPE_MESSAGE, true, false);
		sendNotification();

		fldPlayStatus->setStringValue(AS_STOP);

		//call Parent to say animation has finished
		if (parent && !myList.iniObject.getLoop())
		{
			try
			{
				((UMDAnimation2*)parent)->animationFinished(animationExecuterName);			
			}			
			catch (...)
			{
				kDebug::Debug("Error calling UMDAnimation2 parent",kDebug::DL_HIGH);
			}
		}
		else if (myList.iniObject.getLoop())
		{
			startAnimation();
		}
	}
	else if (currentFrame!=lastFrame)
	{
		lastFrameTime = kBasics::getCurrentTime();

		bool importantFrameRendered = false;
		for (int i = 0; i<currentFrame; i++)
		{
			if (myList.ImportantFrames[i])
			{
				myList.ImportantFrames[i]=false;
				setFrame(i);
				importantFrameRendered = true;
			}			
		}
		if (importantFrameRendered) sendNotification();

		if (!importantFrameRendered /*&& lastFrame<currentFrame*/) setFrame(currentFrame);
	}
}



//! Parst ein Script gegeben durch die benötigten Dateien
//! \param  HLFilename 
//! \param  LLFilename 
//! \param  CommandXMLFilename 
//! \param  ObjectXML1Auto 
//! \param  ObjectXMLFilename1 
//! \param  ObjectXMLFilename2 
//! \return string possible parseFailure 
string AnimationExecuter::parseScript(stringstream& HLStream, const char* LLFilename,
									  const char* CommandXMLFilename, 
									  const bool ObjectXML1Auto, const char* ObjectXMLFilename1, const char* ObjectXMLFilename2)
{	
	ML_TRACE_IN("AnimationExecuter::parseScript()");
	string tempError;

	if (myAnimationParser->getCommandXMLFileName()!=kBasics::CharToString(CommandXMLFilename))
	{
		kDebug::Debug("Parsing CommandXML-File: "+*CommandXMLFilename,kDebug::DL_LOW);
		tempError = myAnimationParser->setCommandXMLFileName(CommandXMLFilename);	
		if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError,kDebug::DL_HIGH);
	}

	if (!ObjectXML1Auto)
	{		
		if (myAnimationParser->getObjectXML1FileName()!=kBasics::CharToString(ObjectXMLFilename1))
		{
			kDebug::Debug("Parsing ObjectXML-File: "+*ObjectXMLFilename1,kDebug::DL_LOW);
			tempError = myAnimationParser->setObjectXML1FileName(ObjectXMLFilename1);
			if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError, kDebug::DL_HIGH);
		}
	}
	else
	{		
		//std::cout << myCache->getObjXML() << std::endl;		
		if (myAnimationParser->getObjectXML1String()!=myCache->getObjXML())
		{
			kDebug::Debug("Parsing automatic ObjectXML",kDebug::DL_LOW);
			tempError = myAnimationParser->setObjectXML1String(myCache->getObjXML());
			if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError, kDebug::DL_HIGH);		
		}
	}

	if (myAnimationParser->getObjectXML2FileName()!=kBasics::CharToString(ObjectXMLFilename2))
	{
		kDebug::Debug("Parsing Object-2-XML-File: "+*ObjectXMLFilename2,kDebug::DL_LOW);
		tempError = myAnimationParser->setObjectXML2FileName(ObjectXMLFilename2);
		if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError, kDebug::DL_HIGH);
	}

	string parseFailure;
	stringstream LLStream;
	parseFailure = myAnimationParser->openAndParseHLStream(HLStream, LLStream);
	if (fldWriteLowLevelScript->getBoolValue())
	{		
		std::cout << "LLFilename:" << LLFilename << std::endl;
		ofstream fileLL(LLFilename);			
		if (!fileLL)
		{
			std::cout << "Error opening lowlevel file / file not found? filnename:" << LLFilename << std::endl;
		}
		else
		{
			std::cout << "Write LowLevel file  filnename:" << LLFilename << std::endl;
			//fileLL.clear();			
			//std::string tempStr;
			//LLStream >> tempStr; 
			//fileLL << tempStr;
			//fileLL << LLStream.str();						
			fileLL << LLStream.rdbuf();
			LLStream.seekg(0,ios::beg); //Move the get pointer for the stream to beginning, so that the parser starts reading from beginning			
			fileLL.close();
		}						
	}

	if (parseFailure!=AnimationParser::ERROR_NONE) return parseFailure;	

	kScriptIniObject myIniObject;
	list<kScriptCommandObject>* myLLCommandList = new list<kScriptCommandObject>;	
	parseFailure = myAnimationParser->openAndParseLLStream(LLStream, &myIniObject, myLLCommandList);	
	if (parseFailure!=AnimationParser::ERROR_NONE) return parseFailure;

	myList.cleanUp();	
	myList.setFramerate(fldFramerate->getIntValue());
	myList.setIniObject(myIniObject);	

	list<kScriptCommandObject>::const_iterator iter;

	//HIER KÖNNTE NOCH OPTIMIERT WERDEN :::: ????? DURCH POINTER DIE EFFEKTIV ZERSTÖRT WERDEN
	kScriptCommandObject tempCommandObj;	
	for ( iter = myLLCommandList->begin();iter!=myLLCommandList->end(); iter++)
	{		
		tempCommandObj = kScriptCommandObject(*iter);
		int insertFailure = myList.insertCommand(&tempCommandObj); //Wenn es hier zu Fehlern kommen, kann es sein, das myList nicht initialisiert wurde		
		if (insertFailure!=0) kDebug::Debug("Command nicht in Liste eingefügt   code:"+kBasics::IntToString(insertFailure)+"   "+tempCommandObj.getCommandStr() + "," + tempCommandObj.getObjectName(),kDebug::DL_HIGH);		
	}		

	int tempFramerate = fldFramerate->getIntValue();
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_FRAMERATE,(void*)&tempFramerate,omINFOTYPE_INT32,true,false);	
	return parseFailure;
}



//! Wird aufgerufen, wenn alle Informationen im Cache vorliegen und es losgehen kann
//! \param  usrData 
//! \param  sensor 
//! \return void  
void AnimationExecuter::fieldChanged_cache_getAllMeasures(void *usrData, void* )
{	
	ML_TRACE_IN("AnimationExecuter::fieldChanged_cache_getAllMeasures()");
	AnimationExecuter* myObj = (AnimationExecuter*) usrData;
	if (myObj)
	{		
		if (myObj->waitingFor_getAllMeasures) myObj->startAnimation();
	}	
}


//! Setzt einen ObjCommunicator für diesen AnimationExecuter
//! \param  value 
//! \return void  
void AnimationExecuter::setObjMgr(ObjMgrCommunicator* value)
{
	ML_TRACE_IN("AnimationExecuter::setObjMgr()");
	myObjMgr = value;
	//fraglich .. damit biege ich den eingang des objmgr um, oder?
	getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
}



//! Setzt einen AnimationCache für diesen AnimationExecuter
//! \param  value 
//! \return void  
void AnimationExecuter::setAnimationCache(AnimationCache* value)
{
	ML_TRACE_IN("AnimationExecuter::setAnimationCache()");
	myCache = value;
	getFieldContainer()->getField("inObjectContainer")->attachField(myCache->getFieldContainer()->getField("inObjectContainer"),1);
}



//! Setzt einen AnimationParser für diesen AnimationExecuter
//! \param  value 
//! \return void  
void AnimationExecuter::setAnimationParser(AnimationParser* value)
{
	ML_TRACE_IN("AnimationExecuter::setAnimationParser()");
	myCache->setAnimationParser(value);
}



//! Setzt die Protokolleigenschaft des Viewers in Abhängigkeit einer realtime-Animation.\n
//! Wenn ein Video aufgezeichnet wird, muss der Viewer protokollieren, wenn er einen Frame fertig gerendert hat.
//! \return void  
void AnimationExecuter::setViewerProtocol()
{
	ML_TRACE_IN("AnimationExecuter::setViewerProtocol()");
	if (myList.iniObject.getRealTime())
		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS,new bool(false),omINFOTYPE_BOOL,true,false);
	//UNBEDINGT KLÄREN WENN OFFSCREEN RENDERING GEHT
	/*////*/	else if (fldUseOffscreenRenderer->getBoolValue()==false)
	{
		/*////*/		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS,new bool(true),omINFOTYPE_BOOL,true,false);	
	}
}



//! Startet eine Animation. Wird von außen aufgerufen.
//! \return void  
void AnimationExecuter::runAnimation()
{		
	ML_TRACE_IN("AnimationExecuter::runAnimation()");
	setViewerProtocol();
	gotClusteredCamPos=false;
	wroteClusteredCamPos=false;
	waitingFor_clusteredCamPos=false;
	//send refresh request to viewer (the old way, not via METKMessenger)
	//This operation takes about a half second ... longer than getting all IDs
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),L_GLOBAL,LAY_GLOBAL_REFRESH,new string("1"),omINFOTYPE_MESSAGE,true,false);
	sendNotification();

	findObjectIDs();		
	kDebug::Debug("get all needed Measures (BoundingBox, diameter, ...)",kDebug::DL_HIGH);
	myCache->getAllMeasures(true);
	waitingFor_getAllMeasures=true;	
}


//! Animation pausiert. Wird von außen aufgerufen
//! \return void  
void AnimationExecuter::pauseAnimation()
{
	ML_TRACE_IN("AnimationExecuter::pauseAnimation()");
	//Erstmal nur für realtimer-Version
	//currentFrame bleibt ja erhalten
	if (myList.iniObject.getRealTime() && fldPlayStatus->getStringValue() == AS_PLAY)
	{
		realtimeTimer->unschedule();
		_pauseAtTimeDiff = kBasics::getCurrentTime() - globalStartTime;		
		fldPlayStatus->setStringValue(AS_PAUSE);
	}
}



//! Setzt eine Animation nach einer Pause fort. Wird von außen aufgerufen.
//! \return void  
void AnimationExecuter::continueAnimation()
{
	ML_TRACE_IN("AnimationExecuter::continueAnimation()");
	if (fldPlayStatus->getStringValue()==AS_PAUSE)
	{
		globalStartTime = kBasics::getCurrentTime() - _pauseAtTimeDiff;
		lastFrameTime = kBasics::getCurrentTime(); 
		fldPlayStatus->setStringValue(AS_PLAY);
		realtimeTimer->schedule();	
	}
}



//! Stoppt eine Animation. Wird von außen aufgerufen.
//! \return void  
void AnimationExecuter::stopAnimation()
{
	ML_TRACE_IN("AnimationExecuter::stopAnimation()");
	fldPlayStatus->setStringValue(AS_STOP);
	realtimeTimer->unschedule();
}

ML_END_NAMESPACE