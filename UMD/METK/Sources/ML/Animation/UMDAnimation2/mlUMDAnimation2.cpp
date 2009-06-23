//----------------------------------------------------------------------------------
//! The ML module class UMDAnimation2.
/*!
// \file    mlUMDAnimation2.cpp
// \author  Konrad Mühler
// \date    2005-05-04
//
//
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlUMDAnimation2_H	    
#include "mlUMDAnimation2.h"
#endif

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(UMDAnimation2,ObjMgrClient);


const char* UMDAnimation2::pathTypeStrings[UMDAnimation2::NUM_PATHTYPE] = { "Use script path", "Use module path", "Use case path", "Other path" };
const char* UMDAnimation2::pathTypeStringsObjXML1[UMDAnimation2::NUM_PATHTYPEO1] = { "Use script path", "Use module path", "Use case path", "Other path", "Automatic via ObjMgr" };
const char* UMDAnimation2::scriptPathTypeStrings[UMDAnimation2::NUM_SCRIPTPATHTYPE] = { "Use module path", "Use case path", "Other path" };
const string UMDAnimation2::AEX_MAIN = "Main";
const string UMDAnimation2::AEX_TRANSFER = "Transfer";


UMDAnimation2::UMDAnimation2 (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("UMDAnimation2::UMDAnimation2()")

	offScreen = new SomnOffscreenRenderer();

	fldModulePath = getFieldContainer()->addString("modulePath");
	fldModulePath -> setStringValue("");

	fldPathTypeScriptPath = getFieldContainer()->addEnum("pathTypeScriptPath", scriptPathTypeStrings, NUM_SCRIPTPATHTYPE);
	fldPathTypeScriptPath -> setEnumValue(0);
	fldScriptPath = getFieldContainer()->addString("scriptPath");
	fldScriptPath -> setStringValue("");
	fldScriptFile = getFieldContainer()->addString("scriptFile");
	fldScriptFile -> setStringValue("Scripts/ENT/init-script.txt");

	fldPathTypeObjectNamesPath1 = getFieldContainer()->addEnum("pathTypeObjectNamesPath1", pathTypeStringsObjXML1, NUM_PATHTYPEO1);
	fldPathTypeObjectNamesPath1 -> setEnumValue(PTO1_AUTO);
	fldObjectNamesPath1 = getFieldContainer()->addString("objectNamesPath1");
	fldObjectNamesPath1 -> setStringValue("");
	fldObjectNamesFile1 = getFieldContainer()->addString("objectNamesFile1");
	fldObjectNamesFile1 -> setStringValue("ObjectNames.xml");

	fldPathTypeObjectNamesPath2 = getFieldContainer()->addEnum("pathTypeObjectNamesPath2", pathTypeStrings, NUM_PATHTYPE);
	fldPathTypeObjectNamesPath2 -> setEnumValue(0);	
	fldUseObjectNamesFile2 = getFieldContainer()->addBool("useObjectNamesFile2");
	fldUseObjectNamesFile2 -> setBoolValue(false);
	fldObjectNamesPath2 = getFieldContainer()->addString("objectNamesPath2");
	fldObjectNamesPath2 -> setStringValue("");
	fldObjectNamesFile2 = getFieldContainer()->addString("objectNamesFile2");
	fldObjectNamesFile2 -> setStringValue("ObjectNames_static.xml");

	fldPathTypeHighLevelCommandsPath = getFieldContainer()->addEnum("pathTypeHighLevelCommandsPath", pathTypeStrings, NUM_PATHTYPE);
	fldPathTypeHighLevelCommandsPath -> setEnumValue(0);
	fldHighLevelCommandsPath = getFieldContainer()->addString("highLevelCommandsPath");
	fldHighLevelCommandsPath -> setStringValue("");
	fldHighLevelCommandsFile = getFieldContainer()->addString("highLevelCommandsFile");
	fldHighLevelCommandsFile -> setStringValue("HighLevel-Commands.xml");

	fldScriptExecute = getFieldContainer()->addNotify("ScriptExecute");

	fldCommandLine = getFieldContainer()->addString("CommandLine");
	fldCommandLineExecute = getFieldContainer()->addNotify("CommandLineExecute");

	fldScriptArea = getFieldContainer()->addString("ScriptArea");
	fldScriptAreaExecute = getFieldContainer()->addNotify("ScriptAreaExecute");

	fldReset = getFieldContainer()->addNotify("Reset");

	fldAnimationViewerName = getFieldContainer()->addString("AnimationViewerName");
	fldAnimationViewerName->setStringValue("METKViewer3D");

	static const char* debugStates[3] = {"None","High","Low"};
	fldDebugState = getFieldContainer()->addEnum("debugState", debugStates, 3);
	fldDebugState->setStringValue("None");
	kDebug::setDebugLevel(fldDebugState->getStringValue());

	fldClearMeasureCache = getFieldContainer()->addBool("clearMeasureCacheForEachScript");
	fldClearMeasureCache->setBoolValue(true);

	fldTestSphereCenter = getFieldContainer()->addVec3f("testSphereCenter");
	fldTestSphereRadius = getFieldContainer()->addDouble("testSphereRadius");
	fldTestCrossPoint = getFieldContainer()->addVec3f("testCrossPoint");
	fldTestCamPosition = getFieldContainer()->addVec3f("testCamPosition");
	fldTestTargetPosition = getFieldContainer()->addVec3f("testTargetPosition");
	fldTestSphereAngle = getFieldContainer()->addDouble("testSphereAngle");

	fldDrawCameraPath = getFieldContainer()->addBool("drawCameraPath");
	fldDrawCameraPath->setBoolValue(false);

	fldEnableSelection = getFieldContainer()->addBool("enableSelection");
	fldEnableSelection->setBoolValue(false);

	fldPauseAnimation = getFieldContainer()->addNotify("pauseAnimation");
	fldContinueAnimation = getFieldContainer()->addNotify("continueAnimation");
	fldStopAnimation = getFieldContainer()->addNotify("stopAnimation");

	fldPlayStatus = getFieldContainer()->addString("playStatus");
	fldPlayStatus->setStringValue("");

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
	fldOffScreenFolder->setStringValue("c:/temp");
	fldOffScreenRender = getFieldContainer()->addNotify("offScreenRender");
	fldOffScreenInit = getFieldContainer()->addNotify("offScreenInit");
	fldOffScreenFinishAVI = getFieldContainer()->addNotify("offScreenFinishAVI");
	fldVideoXSize = getFieldContainer()->addInt("videoXSize");
	fldVideoXSize->setIntValue(400);
	((SoSFShort*)offScreen->getField("bufferSizeX"))->setValue(fldVideoXSize->getIntValue());
	fldVideoYSize = getFieldContainer()->addInt("videoYSize");
	fldVideoYSize->setIntValue(400);
	((SoSFShort*)offScreen->getField("bufferSizeY"))->setValue(fldVideoYSize->getIntValue());
	fldSaveFrames = getFieldContainer()->addBool("saveFrames");
	fldSaveFrames->setBoolValue(false);
	((SoSFBool*)offScreen->getField("saveImage"))->setValue(fldSaveFrames->getBoolValue());
	fldSaveVideo = getFieldContainer()->addBool("saveVideo");
	fldSaveVideo->setBoolValue(true);
	((SoSFBool*)offScreen->getField("saveAVI"))->setValue(fldSaveVideo->getBoolValue());

	((SoSFEnum*)offScreen->getField("bufferType"))->setValue(1); //=PBUFFER
	
	fldUseOffscreenRenderer = getFieldContainer()->addBool("useOffscreenRenderer");
	fldUseOffscreenRenderer->setBoolValue(true);


	inInventor = getFieldContainer()->addSoNode("inInventor");


	currentAnimation = new AnimationExecuter();
	currentAnimation->setName(AEX_MAIN);
	currentAnimation->setParent(this);
	createFieldConnectionsForAnimationExecuter(currentAnimation,true);

	transferAnimation = new AnimationExecuter();
	transferAnimation->setName(AEX_TRANSFER);
	transferAnimation->setParent(this);
	createFieldConnectionsForAnimationExecuter(transferAnimation,false);

	myObjMgr = new ObjMgrCommunicator();

	//Der FieldSensor ist nötig, um auf Ereignisse von Inventormodulen (Bspw. Ready-Trigger) von internen Inventormodulen zu hören
	_fieldSensor = new SoFieldSensor(UMDAnimation2::fieldChangedCB, this);
	_fieldSensor->setPriority(0);
	_fieldSensor->attach(offScreen->getField("readyTrigger"));

	addAcceptedObjectID(O_ANIMATION);
	addAcceptedObjectID(OBJ_COMMUNICATION);
	addAcceptedObjectID(O_CSO);
	addAcceptedInfoLayerName("*");
}



//field-connection for an AnimationExecuter
void UMDAnimation2::createFieldConnectionsForAnimationExecuter(AnimationExecuter* aex, bool inToOut)
{
	ML_TRACE_IN("UMDAnimation2::createFieldConnectionsForAnimationExecuter()")
	if (aex)
	{	
		//From Out to In
		fldClearMeasureCache->attachField(aex->getFieldContainer()->getField("clearMeasureCache"),1);
		fldDrawCameraPath->attachField(aex->getFieldContainer()->getField("drawCameraPath"),1);
		fldAnimationViewerName->attachField(aex->getFieldContainer()->getField("animationViewerName"),1);
		fldAnimationSpeed->attachField(aex->getFieldContainer()->getField("animationSpeed"),1);
		fldCampathVPSize->attachField(aex->getFieldContainer()->getField("campathVPSize"),1);
		fldWriteLowLevelScript->attachField(aex->getFieldContainer()->getField("writeLowLevelScript"),1);
		fldFramerate->attachField(aex->getFieldContainer()->getField("framerate"),1);
		fldOffScreenReady->attachField(aex->getFieldContainer()->getField("offScreenReady"),1);
		fldOffScreenFolder->attachField(aex->getFieldContainer()->getField("offScreenFolder"),1);		
		fldUseOffscreenRenderer->attachField(aex->getFieldContainer()->getField("useOffscreenRenderer"),1);
		
		//Weil das konnektiere nallein nicht reicht, müssen die Werte nochmals von Hand gesetzt werden		
		aex->getFieldContainer()->getField("clearMeasureCache")->setStringValue(fldClearMeasureCache->getStringValue());		
		aex->getFieldContainer()->getField("drawCameraPath")->setStringValue(fldDrawCameraPath->getStringValue());		
		aex->getFieldContainer()->getField("animationViewerName")->setStringValue(fldAnimationViewerName->getStringValue());		
		aex->getFieldContainer()->getField("animationSpeed")->setStringValue(fldAnimationSpeed->getStringValue());
		aex->getFieldContainer()->getField("campathVPSize")->setStringValue(fldCampathVPSize->getStringValue());
		aex->getFieldContainer()->getField("writeLowLevelScript")->setStringValue(fldWriteLowLevelScript->getStringValue());
		aex->getFieldContainer()->getField("framerate")->setStringValue(fldFramerate->getStringValue());
		aex->getFieldContainer()->getField("offScreenFolder")->setStringValue(fldOffScreenFolder->getStringValue());
		aex->getFieldContainer()->getField("useOffscreenRenderer")->setStringValue(fldUseOffscreenRenderer->getStringValue());
		

		//From In to Out
		if (inToOut)
		{
			handleNotificationOff(); //Damit die Videofelder nicht gleich getouched werden
			aex->getFieldContainer()->getField("testSphereCenter")->attachField(fldTestSphereCenter,1);
			aex->getFieldContainer()->getField("testSphereRadius")->attachField(fldTestSphereRadius,1);
			aex->getFieldContainer()->getField("testCrossPoint")->attachField(fldTestCrossPoint,1);
			aex->getFieldContainer()->getField("testCamPosition")->attachField(fldTestCamPosition,1);
			aex->getFieldContainer()->getField("testTargetPosition")->attachField(fldTestTargetPosition,1);
			aex->getFieldContainer()->getField("testSphereAngle")->attachField(fldTestSphereAngle,1);
			aex->getFieldContainer()->getField("playStatus")->attachField(fldPlayStatus,1);
			aex->getFieldContainer()->getField("offScreenRender")->attachField(fldOffScreenRender,1);
			aex->getFieldContainer()->getField("offScreenFilename")->attachField(fldOffScreenFilename,1);
			aex->getFieldContainer()->getField("offScreenInit")->attachField(fldOffScreenInit,1);
			aex->getFieldContainer()->getField("offScreenFinishAVI")->attachField(fldOffScreenFinishAVI,1);
			handleNotificationOn();
		}
	}
}

void UMDAnimation2::handleNotification(Field *field)
{	
	ML_TRACE_IN("UMDAnimation2::handleNotification()")
	inherited::handleNotification(field);	

	if (field == fldScriptExecute || field == fldCommandLineExecute || (field == fldScriptAreaExecute && fldScriptArea->getStringValue()!=""))
	{	
		currentAnimation->ResetAll(fldClearMeasureCache->getBoolValue(),fldClearMeasureCache->getBoolValue());
		
		string scriptPath, scriptFile; 
		string objectNamesPath1, objectNamesFile1;
		string objectNamesPath2, objectNamesFile2;
		string commandsPath, commandsFile;						
		calcFolders(scriptPath, scriptFile, objectNamesPath1, objectNamesFile1, objectNamesPath2, objectNamesFile2, commandsPath, commandsFile);
		//Parsing the script
		string failureScript = AnimationParser::ERROR_NONE;
		if (field == fldCommandLineExecute || field == fldScriptAreaExecute)
		{
			//ofstream fileTemp((scriptPath+"/TempHL.txt").c_str());
			stringstream tempStream;
			
			if (field == fldCommandLineExecute)
			{
				tempStream << "[Ini]" << endl << "LengthTimeUnit=0" << endl << "LengthSeconds=0" << endl << "Realtime=yes" << endl << "[Script]" << endl;
				tempStream << "[0] 'System' setCreateVideo false" << endl;
				tempStream << "[0] " << fldCommandLine->getStringValue();
			}
			else
				tempStream << fldScriptArea->getStringValue();			

			//fileTemp.close();

			/*failureScript = currentAnimation->parseScript((scriptPath+"/TempHL.txt").c_str(),
										(scriptPath+"/LowLevel.txt").c_str(),
										(commandsPath+commandsFile).c_str(),
										(fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_AUTO),
										(objectNamesPath1+objectNamesFile1).c_str(),
										(objectNamesPath2+objectNamesFile2).c_str());*/
			failureScript = currentAnimation->parseScript(tempStream,
										(scriptPath+"/LowLevel.txt").c_str(),
										(commandsPath+commandsFile).c_str(),
										(fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_AUTO),
										(objectNamesPath1+objectNamesFile1).c_str(),
										(objectNamesPath2+objectNamesFile2).c_str());
		}
		else //fldScriptExecute
		{
			ifstream tempScript((scriptPath+scriptFile).c_str());
			stringstream tempStream; //string tempStr;
			//tempScript >> tempStr; //KRITISCH GEHT DAS ODER MACHT ES NUR DIE 1. ZEILE

			//tempStream << tempStr;

			char l[256];
			while(tempScript.getline(l,256))
			{
				tempStream << l << endl;
			}

			failureScript = currentAnimation->parseScript(tempStream,
										(scriptPath+"/LowLevel.txt").c_str(),
										(commandsPath+commandsFile).c_str(),
										(fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_AUTO),
										(objectNamesPath1+objectNamesFile1).c_str(),
										(objectNamesPath2+objectNamesFile2).c_str());
		}

		if (failureScript!=AnimationParser::ERROR_NONE) kDebug::Debug("failureScript: "+failureScript,kDebug::DL_HIGH);
		currentAnimation->runAnimation();
	}

	else if (field == fldReset)
	{		
		currentAnimation->ResetAll(true,true);
	}

	else if (field == fldAnimationViewerName)
	{		
		addAcceptedInfoLayerName(fldAnimationViewerName->getStringValue());
	}

	else if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-)									
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(currentAnimation->getFieldContainer()->getField("inObjectContainer"),1);
			getFieldContainer()->getField("inObjectContainer")->attachField(transferAnimation->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}

	else if (field == inInventor)
	{
		if (inInventor->getDestinationField(0)==NULL)
		{
			kDebug::Debug("Verbinde OffScreenRenderer mit Aussenwelt",kDebug::DL_LOW);
			//inInventor->attachField(offScreen->getField("m_fldScene"),1);
			((SoSFNode*)offScreen->getField("m_fldScene"))->setValue(inInventor->getSoNodeValue());
		}
	}

	else if (field == fldDebugState)
	{		
		kDebug::setDebugLevel(fldDebugState->getStringValue());
	}

	else if (field == fldPauseAnimation)
	{
		if (currentAnimation) currentAnimation->pauseAnimation();
	}

	else if (field == fldContinueAnimation)
	{
		if (fldPlayStatus->getStringValue()==AnimationExecuter::AS_PAUSE)
		{
			//Animation hin zur Ausgangsposition
			transferAnimation->ResetAll(fldClearMeasureCache->getBoolValue(),fldClearMeasureCache->getBoolValue());
			
			//!!! DAS MUSS UNBEDINGT AUF STRINGS UMGESTELLT WERDEN!!!
			string scriptPath, scriptFile; 
			string objectNamesPath1, objectNamesFile1;
			string objectNamesPath2, objectNamesFile2;
			string commandsPath, commandsFile;						

			calcFolders(scriptPath, scriptFile, objectNamesPath1, objectNamesFile1, objectNamesPath2, objectNamesFile2, commandsPath, commandsFile);

			stringstream tempScript;
			//ofstream fileTemp((scriptPath+"/TempHL_Transfer.txt").c_str());		
			tempScript << "[Ini]" << endl << "LengthTimeUnit=1" << endl << "LengthSeconds=1" << endl << "Realtime=yes" << endl << "[Script]" << endl;
			tempScript << "[0,1] 'Cam' moveCoord " 
					 << kBasics::replaceString(kBasics::Vec3fToString(currentAnimation->mainCamera.getPosition())," ",",") << " "
					 << kBasics::replaceString(kBasics::SbRotationToString(currentAnimation->mainCamera.getOrientation())," ",",") << " "
					 << kBasics::FloatToString(currentAnimation->mainCamera.getHeight()) << " "
					 << kBasics::FloatToString(currentAnimation->mainCamera.getFocalDistance())
					 << std::endl;			
			//fileTemp.close();

			string failureScript = transferAnimation->parseScript(tempScript,
										(scriptPath+"/LowLevel_Transfer.txt").c_str(),
										(commandsPath+commandsFile).c_str(),
										(fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_AUTO),
										(objectNamesPath1+objectNamesFile1).c_str(),
										(objectNamesPath2+objectNamesFile2).c_str());

			if (failureScript!=AnimationParser::ERROR_NONE)
				kDebug::Debug("transfer script failure: "+failureScript,kDebug::DL_HIGH);

			transferAnimation->runAnimation();
		}
	}

	else if (field == fldStopAnimation)
	{
		currentAnimation->stopAnimation();
	}

	//Übertragung von Ereignissen aus den AnimationExecutern nach draußen an den OffscreenRenderer
	else if (field == fldOffScreenFilename)
	{
		if (offScreen)
		{
			kDebug::Debug("filname set to "+fldOffScreenFilename->getStringValue(),kDebug::DL_LOW);
			offScreen->getField("fldFilename")->set(fldOffScreenFilename->getStringValue().c_str());
		}
	}

	else if (field == fldOffScreenRender)
	{
		if (offScreen)
		{			
			((SoSFTrigger*)offScreen->getField("m_flRender"))->touch();
		}
	}

	else if (field == fldOffScreenInit)
	{
		if (offScreen)
		{			
			((SoSFTrigger*)offScreen->getField("m_flCreateBuffer"))->touch();
		}
	}

	else if (field == fldOffScreenFinishAVI)
	{
		if (offScreen)
		{			
			((SoSFTrigger*)offScreen->getField("finishAVI"))->touch();
		}
	}

	else if (field == fldVideoXSize)
	{
		if (offScreen) ((SoSFShort*)offScreen->getField("bufferSizeX"))->setValue(fldVideoXSize->getIntValue());
	}

	else if (field == fldVideoYSize)
	{
		if (offScreen) ((SoSFShort*)offScreen->getField("bufferSizeY"))->setValue(fldVideoYSize->getIntValue());
	}

	else if (field == fldSaveFrames)
	{
		if (offScreen) ((SoSFBool*)offScreen->getField("saveImage"))->setValue(fldSaveFrames->getBoolValue());
	}
    
	else if (field == fldSaveVideo)
	{
		if (offScreen) ((SoSFBool*)offScreen->getField("saveAVI"))->setValue(fldSaveVideo->getBoolValue());
	}

	else if (field == fldOffScreenFolder)
	{
		if (offScreen) ((SoSFString*)offScreen->getField("aviFilename"))->setValue((fldOffScreenFolder->getStringValue()+"/Animation.avi").c_str());
	}

	else if (field == fldFramerate)
	{
		if (offScreen) ((SoSFInt32*)offScreen->getField("aviFramerate"))->setValue(fldFramerate->getIntValue());
	}    
}




void UMDAnimation2::handleObjMgrNotification()
{
	ML_TRACE_IN("UMDAnimation2::handleObjMgrNotification()")
	omEventContainer myEventList = getEventContainer();
	bool objSelected = false;
	string objSelectedID = "";	
	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;	
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);

		if (myEvent.objectID==O_ANIMATION && myEvent.layerID==LAY_ANIMATION_SCRIPT)
		{
			string tempAttr;
			if (myObjMgr->getObjAttributeString(myEvent.objectID, myEvent.layerID, myEvent.infoID, tempAttr))
			{
				if (myEvent.infoID==INF_SCRIPT_FULLSCRIPT)
				{
					fldScriptArea->setStringValue(tempAttr);
					fldScriptAreaExecute->notify();
				}
				else if (myEvent.infoID==INF_SCRIPT_SCRIPTFILE)
				{
					fldScriptFile->setStringValue(tempAttr);
					fldScriptExecute->notify();
				}
				else if (myEvent.infoID==INF_SCRIPT_OBJNAMESPATH)
				{
					fldObjectNamesPath1->setStringValue(tempAttr);
				}
			}
		}

		else if (myEvent.objectID==O_ANIMATION && myEvent.layerID==LAY_ANIMATION_GLOBAL && myEvent.infoID==INF_ANIMATION_RESET)
		{
			fldReset->notify();
		}

		else if (fldEnableSelection->getBoolValue() && myEvent.objectID==O_CSO && myEvent.layerID==LAY_SELECTEDOBJECT && myEvent.infoID==INF_OBJECTID) 
		{		
			kDebug::Debug("Selected",kDebug::DL_HIGH);

			string tempAttr;
			if (myObjMgr->getObjAttributeString(myEvent.objectID, myEvent.layerID, myEvent.infoID, tempAttr))
			{	
				if (tempAttr!="")
				{					
					objSelected = true;
					objSelectedID = tempAttr;
				}
			}
		}

		else if (myEvent.objectID==OBJ_COMMUNICATION && myEvent.layerID==LAY_GLOBALEVENTS && myEvent.infoID==INF_CASELOADED)
		{
			fldReset->notify();
		}
	}
	clearEventContainer();
	if (objSelected) ObjectSelected(objSelectedID);
}


//See 3.1.3. Module Persistence and Overloading activateAttachments()
//in MLGuide
//----
//Diese Funktion wird nicht ausgeführt, wenn das Modul per C++ in ML in ein Netz eingefügt wird, sondern scheinbar nur,
//wenn das Modul in ein "echtes" MeVisLab-Netz eingefügt wird.
void UMDAnimation2::activateAttachments()
{
	ML_TRACE_IN("UMDAnimation2::activateAttachments()")
	kDebug::setDebugLevel(fldDebugState->getStringValue());

	//Die Connections müssen hier erneut erstellt werden, weil sonst z.B. gespeicherte ViewerNamen in Netzwerken nicht an die Untermodule weiterpropagiert werden
	if (currentAnimation)
	{
		createFieldConnectionsForAnimationExecuter(currentAnimation,true);
	}
	if (transferAnimation)
	{
		createFieldConnectionsForAnimationExecuter(transferAnimation,false);
	}

	if (offScreen) ((SoSFBool*)offScreen->getField("saveAVI"))->setValue(fldSaveVideo->getBoolValue());
	if (offScreen) ((SoSFShort*)offScreen->getField("bufferSizeX"))->setValue(fldVideoXSize->getIntValue());
	if (offScreen) ((SoSFShort*)offScreen->getField("bufferSizeY"))->setValue(fldVideoYSize->getIntValue());
	if (offScreen) ((SoSFBool*)offScreen->getField("saveImage"))->setValue(fldSaveFrames->getBoolValue());
	if (offScreen) ((SoSFBool*)offScreen->getField("saveAVI"))->setValue(fldSaveVideo->getBoolValue());
	if (offScreen) ((SoSFString*)offScreen->getField("aviFilename"))->setValue((fldOffScreenFolder->getStringValue()+"/Animation.avi").c_str());
	if (offScreen) ((SoSFInt32*)offScreen->getField("aviFramerate"))->setValue(fldFramerate->getIntValue());	    
	   
	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}



void UMDAnimation2::calcFolders(string& scriptPath, string& scriptFile, string& objectNamesPath1, string& objectNamesFile1, string& objectNamesPath2, string& objectNamesFile2, string& commandsPath, string& commandsFile)
{
	ML_TRACE_IN("UMDAnimation2::calcFolders()")
	if (fldScriptFile->getStringValue().find(":")==-1) // relative Path
	{
		if (fldPathTypeScriptPath->getEnumValue()==SPT_MODULEPATH)
		{
			scriptPath = fldModulePath->getStringValue();
		}
		else if (fldPathTypeScriptPath->getEnumValue()==SPT_CASEPATH)
		{
			string tempValue;
			if (myObjMgr->getObjAttributeString(O_CASEOBJECT,LAY_CASEOBJECT_CASE,INF_CASEOBJECT_CASE_DIRECTORY,tempValue))
			{
				scriptPath = tempValue;
			}
		}
		else
		{
			scriptPath = fldScriptPath->getStringValue();
		}

		scriptFile = scriptPath+"/"+fldScriptFile->getStringValue();
	}
	else //global Path
	{
		scriptFile = fldScriptFile->getStringValue();
	}

	scriptFile = kBasics::replaceString(scriptFile,char(92),"/");
	scriptPath = kBasics::getPath(scriptFile,'/');		
	scriptFile = kBasics::replaceString(scriptFile, scriptPath, "");

	//Build path and file for first ObjectNames.xml
	if (fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_SCRIPTPATH)
	{
		objectNamesPath1 = scriptPath;
	}
	else if (fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_MODULEPATH)
	{
		objectNamesPath1 = fldModulePath->getStringValue();
	}
	else if (fldPathTypeObjectNamesPath1->getEnumValue()==PTO1_CASEPATH)
	{
		string tempValue;
		if (myObjMgr->getObjAttributeString(O_CASEOBJECT,LAY_CASEOBJECT_CASE,INF_CASEOBJECT_CASE_DIRECTORY,tempValue))
		{
			objectNamesPath1 = tempValue;
		}
	}
	else
	{
		objectNamesPath1 = fldObjectNamesPath1->getStringValue();
	}

	objectNamesFile1 = "/"+fldObjectNamesFile1->getStringValue();


	//Build path and file for second (ObjectNames_static.xml)
	if (!fldUseObjectNamesFile2->getBoolValue())
	{
		objectNamesPath2 = "";
		objectNamesFile2 = "";			
	}
	else
	{	
		objectNamesFile2 = "/" + fldObjectNamesFile2->getStringValue();			

		if (fldPathTypeObjectNamesPath2->getEnumValue()==PT_SCRIPTPATH)
		{
			objectNamesPath2 = scriptPath;
		}
		else if (fldPathTypeObjectNamesPath2->getEnumValue()==PT_MODULEPATH)
		{
			objectNamesPath2 = fldModulePath->getStringValue();
		}
		else if (fldPathTypeObjectNamesPath2->getEnumValue()==PT_CASEPATH)
		{	
			string tempValue;
			if (myObjMgr->getObjAttributeString(O_CASEOBJECT,LAY_CASEOBJECT_CASE,INF_CASEOBJECT_CASE_DIRECTORY,tempValue))
			{
				objectNamesPath2 = tempValue;
			}
		}
		else
		{
			objectNamesPath2 = fldObjectNamesPath2->getStringValue();
		}				
	}

	//Build path and file for commands file
	if (fldPathTypeHighLevelCommandsPath->getEnumValue()==PT_SCRIPTPATH)
	{
		commandsPath = scriptPath;
	}
	else if (fldPathTypeHighLevelCommandsPath->getEnumValue()==PT_MODULEPATH)
	{
		commandsPath = fldModulePath->getStringValue();
	}
	else if (fldPathTypeHighLevelCommandsPath->getEnumValue()==PT_CASEPATH)
	{
		string tempValue;
		if (myObjMgr->getObjAttributeString(O_CASEOBJECT,LAY_CASEOBJECT_CASE,INF_CASEOBJECT_CASE_DIRECTORY,tempValue))
		{
			commandsPath = tempValue;
		}
	}
	else
	{
		commandsPath = fldHighLevelCommandsPath->getStringValue();
	}

	commandsFile = "/" + fldHighLevelCommandsFile->getStringValue();

	kDebug::Debug("scriptPath+scriptFile:"+scriptPath+scriptFile,kDebug::DL_HIGH);
	kDebug::Debug("objectNamesPath1+objectNamesFile1:"+objectNamesPath1+objectNamesFile1,kDebug::DL_HIGH);
	kDebug::Debug("objectNamesPath2+objectNamesFile2:"+objectNamesPath2+objectNamesFile2,kDebug::DL_HIGH);
	kDebug::Debug("commandsPath+commandsFile:"+commandsPath+commandsFile,kDebug::DL_HIGH);
}				




void UMDAnimation2::ObjectSelected(string objectID)
{
	ML_TRACE_IN("UMDAnimation2::ObjectSelected()")
	kDebug::Debug("ObjectSelected "+objectID,kDebug::DL_HIGH);
	
	string scriptPath, scriptFile; 
	string objectNamesPath1, objectNamesFile1;
	string objectNamesPath2, objectNamesFile2;
	string commandsPath, commandsFile;						

	calcFolders(scriptPath, scriptFile, objectNamesPath1, objectNamesFile1, objectNamesPath2, objectNamesFile2, commandsPath, commandsFile);

	myAnimationParser = new AnimationParser();
	myAnimationParser->createNewObjectXMLFile((scriptPath+"/ObjectNames-Temp.xml").c_str());
	myAnimationParser->createNewCommandXMLFile((scriptPath+"/HighLevel-Commands-empty.xml").c_str());
				
	omObjectContainer* myOC = getObjContainer();
	if(myOC == NULL)
	{
		kDebug::Debug("ObjContainer not found!",kDebug::DL_LOW);
		return;
	}

	omObject& myObj = (*myOC)[objectID];
	if (!myObj.isValid())
		kDebug::Debug("myObj beim Selektieren scheint mir nicht valid zu sein :-(",kDebug::DL_HIGH);
	
	string objName = "";
	myObjMgr->getObjAttributeString(objectID, LAY_DESCRIPTION,INF_NAME, objName);
	kDebug::Debug("objName: "+objName,kDebug::DL_HIGH);

	list<string>* tempList = new list<string>;
	tempList->push_back(objName);	
	myAnimationParser->addObjectToXMLFile(scriptPath+"/ObjectNames-Temp.xml", "SELECTEDOBJECT", tempList);
	delete tempList;
	
	string tempError;	
	tempError = myAnimationParser->setCommandXMLFileName((scriptPath+"/HighLevel-Commands-empty.xml").c_str());
	if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError,kDebug::DL_HIGH);

	tempError = myAnimationParser->setObjectXML1FileName((scriptPath+"/ObjectNames-Temp.xml").c_str());	
	if (tempError!=AnimationParser::ERROR_NONE) kDebug::Debug(tempError,kDebug::DL_HIGH);

	string parseFailure = myAnimationParser->openAndParseHLScript((scriptPath+"/SELECT-default.txt").c_str(), (scriptPath+"/LowLevel.txt").c_str());

	if (parseFailure==AnimationParser::ERROR_NONE)
	{
		kDebug::Debug("Open file",kDebug::DL_HIGH);
		ifstream fileHL((scriptPath+"/LowLevel.txt").c_str());
		if (fileHL)
		{
			stringstream temp;
			temp << fileHL.rdbuf();
			
			fldScriptArea->setStringValue(temp.str());
			fldScriptAreaExecute->notify();
			string tempStr=temp.str(); //to avoid Compiler Warning (level 4) C4238
			myObjMgr->setObjAttribute(O_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_FULLSCRIPT, (omMessage*)&tempStr, omINFOTYPE_MESSAGE, true, false);
			sendNotification();			
		}
	}
	
	kDebug::Debug("parseFailure: "+parseFailure,kDebug::DL_HIGH);	
}



void UMDAnimation2::animationFinished(string name)
{
	ML_TRACE_IN("UMDAnimation2::animationFinished()")
	kDebug::Debug("Animation finished with name: "+name, kDebug::DL_HIGH);
	if (name==AEX_TRANSFER)
	{
		currentAnimation->continueAnimation();
	}
}




//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void UMDAnimation2::fieldChangedCB(void *data, SoSensor* sens)
{
	((UMDAnimation2*) data)->fieldChanged((SoFieldSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void UMDAnimation2::fieldChanged(SoFieldSensor* sensor)
{
	// Get the field which caused the notification.
	SoField* field = sensor->getTriggerField();
  
	if ( offScreen )
	{
		if ( field == offScreen->getField("readyTrigger"))
		{
			//cout << "READY TRIGGER NODE SENSOR" << endl;
			fldOffScreenReady->notify();
		}
	}
}



ML_END_NAMESPACE