#include "../AnimationExecuter.h"

ML_START_NAMESPACE

void AnimationExecuter::commandRotateCam(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	//Params: Degrees
	if (tempEntryParams->size()>=1)
	{						
		string interpoltype = "BEZIER2";						

		double angle = atof((*tempEntryParams)[0].c_str())*2.0*kBasics::PI/360.0; //gesamtwinkel in bogenmass											

		if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAM_ROTATECAMZ)
			getAngleByLastValue(currentEntry->getCommandLine(), interpoltype, currentEntry->getCurrentFrame(), currentEntry->getFrameCount(), angle);						
		else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_CAM_ROTATECAMZTO)
		{								
			//Dieser ausgefeilte Algorithmus hier sorgt dafür dass der kürzere Drehweg genommen wird und nicht immer stur im Uhrzeigersinn gedreht wird
			double targetAngle = mainCamera.getUpVecAngle()-angle;
			double sign = +1.0;
			if (mainCamera.getUpVecAngle()>kBasics::PI)
			{
				targetAngle = 2*kBasics::PI - targetAngle;
				sign = -1.0;
			}
			angle = sign * getRotationAngleByType(-targetAngle, interpoltype, currentEntry->getCurrentFrame(), currentEntry->getFrameCount());							
		}						

		mainCamera.rotateCam(angle);
		writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr		
	}
}


void AnimationExecuter::commandRotate(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	//Params: Object Direction Degrees
	if (tempEntryParams->size()>=3)
	{
		string interpoltype = "BEZIER2";

		double angle = atof((*tempEntryParams)[2].c_str())*kBasics::PI/180.0; //Gesamtwinkel in Bogenmass						
		getAngleByLastValue(currentEntry->getCommandLine(), interpoltype, currentEntry->getCurrentFrame(), currentEntry->getFrameCount(), angle);

		SbVec3f rotAxis;
		if ((*tempEntryParams)[1]==kScriptFrameEntry::PARAM_ROTATE_SAGITTAL)
			rotAxis.setValue(1.0,0.0,0.0); //+x
		else if ((*tempEntryParams)[1]==kScriptFrameEntry::PARAM_ROTATE_CORONAL)
			rotAxis.setValue(0.0,-1.0,0.0); //-y
		else if ((*tempEntryParams)[1]==kScriptFrameEntry::PARAM_ROTATE_AXIAL)
			rotAxis.setValue(0.0,0.0,-1.0); //-z
		else
		{
			//Achse splitten
			vector<string>* axis_split = new vector<string>;
			kBasics::split((*tempEntryParams)[1],kScriptFrameEntry::DELIMITER_AXIS,3,axis_split);

			if (axis_split->size()==3)
				rotAxis.setValue(atof((*axis_split)[0].c_str()),atof((*axis_split)[1].c_str()),atof((*axis_split)[2].c_str()));
			else 
				rotAxis.setValue(1.0,0.0,0.0); //+x

			delete axis_split;
		}				

		//BoundingBox
		string tempObjName = kBasics::trimQuotatedStr((string)(*tempEntryParams)[0], kBasics::QUOTATION_SINGLE);

		vector<string>* splittedLookAt = new vector<string>;
		kBasics::split(tempObjName,kScriptFrameEntry::DELIMITER_AXIS,3,splittedLookAt);

		SbVec3f newLookAt;
		if (splittedLookAt->size()<3)
		{
			AnimationCache::Measures MasterBB = myCache->getMeasuresFromHT(tempObjName);
			newLookAt = myCache->getBoundingCenter(MasterBB);
		}
		else
			newLookAt = SbVec3f(kBasics::StringToInt((*splittedLookAt)[0]),kBasics::StringToInt((*splittedLookAt)[1]),kBasics::StringToInt((*splittedLookAt)[2]));

		mainCamera.setLookAt(newLookAt);

		mainCamera.rotatePosition(rotAxis,angle,newLookAt);
		writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr
		writePosition(mainCamera.getPosition()); // Schreibt position in ObjMgr	

		delete splittedLookAt;
	}
	else kDebug::Debug("rotate -> to few arguments",kDebug::DL_HIGH);
}



void AnimationExecuter::commandCamFocalDistance(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()==1) setDoubleValueInterpol(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,(*tempEntryParams)[0],currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
}



void AnimationExecuter::commandCamHeight(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()==1) setDoubleValueInterpol(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,(*tempEntryParams)[0],currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
}


void AnimationExecuter::commandCamPosition(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()==1) setV3ValueInterpol(fldAnimationViewerName->getStringValue(), LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_POSITION,(*tempEntryParams)[0],currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
}

void AnimationExecuter::commandCamOrientation(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()==1)
	{
		mainCamera.setOrientation(getCamOrientation());		
		mainCamera.setPosition(getCamPosition());	

		//Ziel Orientation Splitten
		SbRotation targetOrientation;
		vector<string>* splittedOrientation = new vector<string>;
		kBasics::split((string)(*tempEntryParams)[0],kScriptFrameEntry::DELIMITER_AXIS,4,splittedOrientation);
		if (splittedOrientation->size()==4)
			targetOrientation = SbRotation(SbVec3f(kBasics::StringToFloat((*splittedOrientation)[0]),kBasics::StringToFloat((*splittedOrientation)[1]),kBasics::StringToFloat((*splittedOrientation)[2])),kBasics::StringToFloat((*splittedOrientation)[3]));
		else
			targetOrientation = SbRotation(SbVec3f(0,0,1),0);

		SbRotation tempOrientation;
		SbVec3f tempCurrentOrientationVec;
		float tempCurrentOrientationRad;
		mainCamera.getOrientation().getValue(tempCurrentOrientationVec,tempCurrentOrientationRad);
		SbVec3f targetOrientationVec;
		float targetOrientationRad;
		targetOrientation.getValue(targetOrientationVec,targetOrientationRad);
		tempOrientation.setValue(SbVec3f(tempCurrentOrientationVec[0] + (( targetOrientationVec[0] ) - tempCurrentOrientationVec[0] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ),
			tempCurrentOrientationVec[1] + (( targetOrientationVec[1] ) - tempCurrentOrientationVec[1] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ),
			tempCurrentOrientationVec[2] + (( targetOrientationVec[2] ) - tempCurrentOrientationVec[2] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 )),
			tempCurrentOrientationRad + (( targetOrientationRad ) - tempCurrentOrientationRad ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ));

		mainCamera.setOrientation(tempOrientation);

		writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr

		delete splittedOrientation;
	}
}



void AnimationExecuter::commandLookAt(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()==1)
	{
		mainCamera.setOrientation(getCamOrientation());		
		mainCamera.setPosition(getCamPosition());	

		SbVec3f tempCurrentLookAt = mainCamera.getLookAt();
		SbVec3f tempPosition = mainCamera.getPosition();

		//Ziel LookAt Splitten
		SbVec3f targetLookAt;						
		SbVec3f BBCenter;
		vector<string>* splittedLookAt = new vector<string>;
		kBasics::split((string)(*tempEntryParams)[0],kScriptFrameEntry::DELIMITER_AXIS,3,splittedLookAt);
		if (splittedLookAt->size()<3)
		{									
			AnimationCache::Measures MasterBB = myCache->getMeasuresFromHT((string)(*tempEntryParams)[0]);
			BBCenter = myCache->getBoundingCenter(MasterBB);																			
		}
		else
		{
			BBCenter = SbVec3f(kBasics::StringToInt((*splittedLookAt)[0]),kBasics::StringToInt((*splittedLookAt)[1]),kBasics::StringToInt((*splittedLookAt)[2]));		
		}

		SbVec3f newLookAt = tempCurrentLookAt + (BBCenter-tempCurrentLookAt) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );

		mainCamera.setCamPosition(tempPosition,newLookAt);

		writeOrientation(mainCamera.getOrientation()); // Schreibt Cam Orientation in ObjMgr
		writePosition(mainCamera.getPosition()); // Schreibt Cam Position in ObjMgr					

		double focalDistance = (mainCamera.getPosition() - mainCamera.getLookAt()).length();
		mainCamera.setFocalDistance(focalDistance);
		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&focalDistance,omINFOTYPE_DOUBLE,true,true);

		delete splittedLookAt;
	}
}

void AnimationExecuter::commandCampathVP(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter){
	//Im ersten Frame einer viewpointmove-Anweisung werden alle weiteren Positionen etc. für die übrigen Frames der Anweisung berechnet
	//Dabei wird für alle weiteren Frames der Anweisung auch das CFLAG_EXECUTEVPMOVE gesetzt					
	if (!gotCamPath && currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP){
		//currentEntry->setCommunicationFlag(kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP);
		kDebug::Debug("calculate path command",kDebug::DL_LOW);
		waitingFor_getCamPath = true;
		gotCamPath = false;
		returnFrame = frameNr;
		returnEntry = FrameEntryListIter;

		//Get target object from parameter 0
		string targetObjName = "X";
		if (currentEntry->getParams()->size()>0){
			targetObjName	= kBasics::trimQuotatedStr((string)(*currentEntry->getParams())[0], kBasics::QUOTATION_SINGLE);
			vector<string>* objectList = new std::vector<string>;
			myAnimationParser->getObjectStringList(targetObjName.c_str(),objectList);			
			if (objectList->size()>0)
				targetObjName = myCache->findSingleObjectID((*objectList)[0]);
			delete objectList;
		}

		kDebug::Debug("target object: "+targetObjName,kDebug::DL_LOW);
		string WeightVis = "30";
		string WeightReg = "50";
		string WeightCam = "50";
		string WeightEnt = "50";
		string WeightNum = "5";
		string WeightImp = "50";
		string WeightSta = "30";
		string ThresSta = "0.7";
		string WeightSta2 = "50";
		string ThresSta2 = "0.7";
		string WeightViewplaneDist = "50";
		string WeightSilhouette = "50";
		string WeightImageCenter = "50";

		//Wurden Parameter angegeben, so werden diese und nicht die obigen Standardwerte genommen
		if (currentEntry->getParams()->size()>=3){
			string weights = kBasics::trimQuotatedStr((string)(*currentEntry->getParams())[2],kBasics::QUOTATION_SINGLE);
			vector<string>* splittedWeights = new vector<string>;
			kBasics::split(weights,' ',13,splittedWeights);
			//Boah das ist mal schlechter Code
			if (splittedWeights->size()>=1) WeightVis = (string)(*splittedWeights)[0];
			if (splittedWeights->size()>=2) WeightReg = (string)(*splittedWeights)[1];
			if (splittedWeights->size()>=3) WeightCam = (string)(*splittedWeights)[2];
			if (splittedWeights->size()>=4) WeightEnt = (string)(*splittedWeights)[3];
			if (splittedWeights->size()>=5) WeightNum = (string)(*splittedWeights)[4];
			if (splittedWeights->size()>=6) WeightImp = (string)(*splittedWeights)[5];
			if (splittedWeights->size()>=7) WeightSta = (string)(*splittedWeights)[6];
			if (splittedWeights->size()>=8) ThresSta = (string)(*splittedWeights)[7];
			if (splittedWeights->size()>=9) WeightSta2 = (string)(*splittedWeights)[8];
			if (splittedWeights->size()>=10) ThresSta2 = (string)(*splittedWeights)[9];
			if (splittedWeights->size()>=11) WeightViewplaneDist = (string)(*splittedWeights)[10];
			if (splittedWeights->size()>=12) WeightSilhouette = (string)(*splittedWeights)[11];
			if (splittedWeights->size()>=13) WeightImageCenter = (string)(*splittedWeights)[12];

			delete splittedWeights;
		}

		string data = targetObjName+" " + WeightVis + " " + WeightReg + " " + WeightCam + " " + WeightEnt + " " + WeightNum + " " + WeightImp + " " + WeightSta + " " + ThresSta + " " + WeightSta2 + " " + ThresSta2+ " " + WeightViewplaneDist + " " + WeightSilhouette + " " + WeightImageCenter;
		kDebug::Debug("send CamPath request for "+targetObjName + "   data = "+data+"   FRAME="+kBasics::IntToString(frameNr),kDebug::DL_HIGH);
		_modMsgSenderGetCamPath->getFieldContainer()->getField("data")->setStringValue(data);
		_modMsgSenderGetCamPath->getFieldContainer()->getField("message")->setStringValue("solverCamPath");
		((NotifyField*)_modMsgSenderGetCamPath->getFieldContainer()->getField("send"))->notify();
	}else{						
		//Die schon vorher berechnet Werte werden für diesen Frame nur noch ausgeführt
		writeCalculatedViewpathValues(tempEntryParams, currentEntry, frameNr, FrameEntryListIter, currentEntry->getCommandID());
	}
	gotCamPath = false; //Dies veranlasst setFrame() am Ende anzuhalten und darauf zuwarten, dass vom VP-Modul eine Antwort kommt
}


void AnimationExecuter::commandViewpointMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter)
{
	//Im ersten Frame einer viewpointmove-Anweisung werden alle weiteren Positionen etc. für die übrigen Frames der Anweisung berechnet
	//Dabei wird für alle weiteren Frames der Anweisung auch das CFLAG_EXECUTEVPMOVE gesetzt					
	if ((!gotCamPos) && currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTEVPMOVE)
	{
		kDebug::Debug("calculate viewpoint command",kDebug::DL_LOW);
		waitingFor_getCamPos = true;
		gotCamPos = false;
		returnFrame = frameNr;
		returnEntry = FrameEntryListIter;

		//Get target object from parameter 0
		string targetObjName = "X";
		if (currentEntry->getParams()->size()>0)
		{
			targetObjName	= kBasics::trimQuotatedStr((string)(*currentEntry->getParams())[0], kBasics::QUOTATION_SINGLE);
			vector<string>* objectList = new std::vector<string>;
			myAnimationParser->getObjectStringList(targetObjName.c_str(),objectList);			
			if (objectList->size()>0)
				targetObjName = myCache->findSingleObjectID((*objectList)[0]);
			delete objectList;
		}

		kDebug::Debug("target object: "+targetObjName,kDebug::DL_LOW);

		string WeightVis = "1";
		string WeightReg = "50";
		string WeightCam = "1";
		string WeightEnt = "1";
		string WeightNum = "1";
		string WeightImp = "50";
		string WeightSta = "1";
		string ThresSta = "0.9";
		string WeightSta2 = "1";
		string ThresSta2 = "0.9";
		string WeightViewplaneDist = "1";
		string WeightSilhouette = "1";
		string WeightImageCenter = "1";

		//Wurden Parameter angegeben, so werden diese und nicht die obigen Standardwerte genommen
		if (currentEntry->getParams()->size()>=3)
		{
			string weights = kBasics::trimQuotatedStr((string)(*currentEntry->getParams())[2],kBasics::QUOTATION_SINGLE);
			vector<string>* splittedWeights = new vector<string>;
			kBasics::split(weights,' ',13,splittedWeights);
			//Boah das ist mal schlechter Code
			if (splittedWeights->size()>=1) WeightVis = (string)(*splittedWeights)[0];
			if (splittedWeights->size()>=2) WeightReg = (string)(*splittedWeights)[1];
			if (splittedWeights->size()>=3) WeightCam = (string)(*splittedWeights)[2];
			if (splittedWeights->size()>=4) WeightEnt = (string)(*splittedWeights)[3];
			if (splittedWeights->size()>=5) WeightNum = (string)(*splittedWeights)[4];
			if (splittedWeights->size()>=6) WeightImp = (string)(*splittedWeights)[5];
			if (splittedWeights->size()>=7) WeightSta = (string)(*splittedWeights)[6];
			if (splittedWeights->size()>=8) ThresSta = (string)(*splittedWeights)[7];
			if (splittedWeights->size()>=9) WeightSta2 = (string)(*splittedWeights)[8];
			if (splittedWeights->size()>=10) ThresSta2 = (string)(*splittedWeights)[9];
			if (splittedWeights->size()>=11) WeightViewplaneDist = (string)(*splittedWeights)[10];
			if (splittedWeights->size()>=12) WeightSilhouette = (string)(*splittedWeights)[11];
			if (splittedWeights->size()>=13) WeightImageCenter = (string)(*splittedWeights)[12];

			delete splittedWeights;
		}

		string data = targetObjName+" " + WeightVis + " " + WeightReg + " " + WeightCam + " " + WeightEnt + " " + WeightNum + " " + WeightImp + " " + WeightSta + " " + ThresSta + " " + WeightSta2 + " " + ThresSta2+ " " + WeightViewplaneDist + " " + WeightSilhouette + " " + WeightImageCenter;
		kDebug::Debug("send CamPos request for "+targetObjName + "   data = "+data,kDebug::DL_HIGH);
		_modMsgSenderGetCamPos->getFieldContainer()->getField("data")->setStringValue(data);
		_modMsgSenderGetCamPos->getFieldContainer()->getField("message")->setStringValue("solver");
		((NotifyField*)_modMsgSenderGetCamPos->getFieldContainer()->getField("send"))->notify();
	}
	else
	{						
		//Die schon vorher berechnet Werte werden für diesen Frame nur noch ausgeführt
		writeCalculatedViewpointValues(tempEntryParams, currentEntry, frameNr, FrameEntryListIter, currentEntry->getCommandID());
	}
	gotCamPos = false; //Dies veranlasst setFrame() am Ende anzuhalten und darauf zuwarten, dass vom VP-Modul eine Antwort kommt
}


void AnimationExecuter::commandViewpointClustering(vector<string>* /*tempEntryParams*/, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter){
	//Im ersten Frame einer viewpointclustering-Anweisung werden alle weiteren Positionen etc. für die übrigen Frames der Anweisung berechnet
	//Dabei wird für alle weiteren Frames der Anweisung auch das CFLAG_EXECUTEVPCLUSTERING gesetzt					
	if ((!gotClusteredCamPos) && currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTECAMPATH){// ::CFLAG_EXECUTEVPCLUSTERING){
		waitingFor_clusteredCamPos=true;
		gotClusteredCamPos=true;
		returnFrame = frameNr;
		returnEntry = FrameEntryListIter;
		std::vector<string> params=*(currentEntry->getParams());
		params.erase(params.begin());
		std::list<kBasics::optionstruct> options;
		options.push_back(kBasics::optionstruct("structures",-1,'s'));
		options.push_back(kBasics::optionstruct("objects",-1,'o'));
		options.push_back(kBasics::optionstruct("weightvis",1,'v'));
		options.push_back(kBasics::optionstruct("weightreg",1,'r'));
		options.push_back(kBasics::optionstruct("weightcam",1,'c'));
		options.push_back(kBasics::optionstruct("weightent",1,'e'));
		options.push_back(kBasics::optionstruct("weightnum",1,'n'));
		options.push_back(kBasics::optionstruct("weightimp",1,'i'));
		options.push_back(kBasics::optionstruct("weightsta",1,'a'));
		options.push_back(kBasics::optionstruct("thressta",1,'t'));
		options.push_back(kBasics::optionstruct("weightsta2",1,'w'));
		options.push_back(kBasics::optionstruct("thressta2",1,'m'));
		options.push_back(kBasics::optionstruct("weightviewplanedist",1,'p'));
		options.push_back(kBasics::optionstruct("weightsilhouette",1,'h'));
		options.push_back(kBasics::optionstruct("weightimagecenter",1,'g'));
		std::string value;
		std::map<char,std::string> values;
		values['s']="";values['o']="";values['v']="1";
		values['r']="1";values['c']="1";values['e']="1";
		values['n']="1";values['i']="1";values['a']="1";
		values['t']="0.9";values['w']="1";values['m']="0.9";
		values['p']="1";values['h']="1";values['g']="1";
		char c;
		while(-1!=(c=kBasics::getOptions(params,options,value))){
			switch(c){
				case 's':{
					std::vector<std::string> tmpStructures;
					kBasics::split(value,' ',-1,&tmpStructures);
					if(!tmpStructures.empty()){
						std::vector<std::string> structureGroupElements;
						myAnimationParser->getObjectStringList(tmpStructures[0].c_str(),&structureGroupElements);
						for(int j=0;j<structureGroupElements.size();j++){
							values['o']+=" "+myCache->findSingleObjectID(structureGroupElements[j]);
						}
						values['o']=kBasics::trimString(values['o']);
					}
					break;
				}
				case 'o':	case 'v':
				case 'r':	case 'c':	case 'e':
				case 'n':	case 'i':	case 'a':
				case 't':	case 'w':	case 'm':
				case 'p':	case 'h':
				case 'g':values[c]=value;break;
			}
		}
		std::ostringstream os;
		std::list<kBasics::optionstruct>::iterator it=options.begin();
		while(it!=options.end()){
			os << "--" << kBasics::trimString(it->name) << " ";
			if(!(kBasics::trimString(values[it->shortname]).empty())){
				os << kBasics::trimString(values[it->shortname]) << " ";
			}
			it++;
		}
		string data = os.str();
		std::cout << "XXX:" << data << std::endl;
		kDebug::Debug("calculate viewpointclustering command:"+data,kDebug::DL_LOW);
		//kDebug::Debug("send CamPos request for "+targetObjName + "   data = "+data,kDebug::DL_HIGH);
		_modMsgSenderClusteredCamPos->getFieldContainer()->getField("data")->setStringValue(data);
		_modMsgSenderClusteredCamPos->getFieldContainer()->getField("message")->setStringValue("clustering");
		((NotifyField*)_modMsgSenderClusteredCamPos->getFieldContainer()->getField("send"))->notify();
	}
	gotClusteredCamPos=false;
}

void AnimationExecuter::commandShowClusteringPath(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter){
	bool found=true;
	if(currentEntry->getParams()->size()==1){
		found=false;
		std::cout << "CLUSTERRESULT:" << clusteredResult << std::endl;
		std::vector<std::string> mehrere;
		kBasics::split(clusteredResult,'#',-1,&mehrere);
		std::string par="";
		for(int i=0;i<mehrere.size();i++){
			std::vector<std::string> teile;
			kBasics::split(mehrere[i],' ',4,&teile);
			if(teile.size()==4){
				vec3 pos;
				vec4 orient;
				double height;
				std::vector<std::string> tmp;
				kBasics::split(teile[0],';',3,&tmp);
				if(tmp.size()==3){
					for(int i=0;i<3;i++){
						pos[i]=kBasics::StringToFloat(tmp[i]);
					}
					std::vector<std::string> tmp;
					kBasics::split(teile[1],';',4,&tmp);
					if(tmp.size()==4){
						for(int i=0;i<4;i++){
							orient[i]=kBasics::StringToFloat(tmp[i]);
						}
						//kBasics::split(teile[3],";",-1,&tmp);
						height=kBasics::StringToFloat(teile[2]);
						std::ostringstream os;
						os << pos[0] << ";" << pos[1] << ";" << height << "," << orient[0] << ";" << orient[1] << ";" << orient[2] << ";" << orient[3] << ",";
						os << teile[3] << " ";
						par+=os.str();
						std::cout << "PARAMS:" << par << std::endl;
					}
				}
			}
			break;
		}
		if(!par.empty()){
			par="'ALL' "+kBasics::trimString(par);
			found=true;
		}
		currentEntry->setParamString(par);
	}
	if(found){
		if(!myCamPath){
			myCamPath=new LinearCampath(myObjMgr,fldAnimationViewerName,&myList,&mainCamera,myCache,&testPositions,fldTestSphereCenter,fldTestSphereRadius,fldTestTargetPosition,fldDrawCameraPath,"Linear");
		}
		std::cout << "CAMPATHAUFRUF" << std::endl;
		myCamPath->writeCalculatedCamPathValues(tempEntryParams, currentEntry, frameNr, FrameEntryListIter, currentEntry->getCommandID());
	}
}


void AnimationExecuter::commandMoveCoord(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	//Params:   position orientation height focalDist
	if (tempEntryParams->size()>=4)
	{					
		SbVec3f targetPosition;					
		vector<string>* splittedPosition = new vector<string>;
		kBasics::split((string)(*tempEntryParams)[0],kScriptFrameEntry::DELIMITER_AXIS,3,splittedPosition);
		if (splittedPosition->size()==3)
			targetPosition = SbVec3f(kBasics::StringToFloat((*splittedPosition)[0]),kBasics::StringToFloat((*splittedPosition)[1]),kBasics::StringToFloat((*splittedPosition)[2]));
		else
			targetPosition = SbVec3f(0,0,0);

		SbRotation targetOrientation;
		vector<string>* splittedOrientation = new vector<string>;
		kBasics::split((string)(*tempEntryParams)[1],kScriptFrameEntry::DELIMITER_AXIS,4,splittedOrientation);
		if (splittedOrientation->size()==4)
			targetOrientation = SbRotation(SbVec3f(kBasics::StringToFloat((*splittedOrientation)[0]),kBasics::StringToFloat((*splittedOrientation)[1]),kBasics::StringToFloat((*splittedOrientation)[2])),kBasics::StringToFloat((*splittedOrientation)[3]));
		else
			targetOrientation = SbRotation(SbVec3f(0,0,1),0);

		float targetHeight = kBasics::StringToFloat((string)(*tempEntryParams)[2]);

		float targetFocalDistance = kBasics::StringToFloat((string)(*tempEntryParams)[3]);

		SbVec3f tempPosition;
		SbVec3f tempCurrentPosition = mainCamera.getPosition();
		tempPosition[0] = tempCurrentPosition[0] + (( targetPosition[0] ) - tempCurrentPosition[0] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
		tempPosition[1] = tempCurrentPosition[1] + (( targetPosition[1] ) - tempCurrentPosition[1] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
		tempPosition[2] = tempCurrentPosition[2] + (( targetPosition[2] ) - tempCurrentPosition[2] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );

		mainCamera.setPosition(tempPosition);

		SbRotation tempOrientation;
		SbVec3f tempCurrentOrientationVec;
		float tempCurrentOrientationRad;
		mainCamera.getOrientation().getValue(tempCurrentOrientationVec,tempCurrentOrientationRad);
		SbVec3f targetOrientationVec;
		float targetOrientationRad;
		targetOrientation.getValue(targetOrientationVec,targetOrientationRad);
		tempOrientation.setValue(SbVec3f(tempCurrentOrientationVec[0] + (( targetOrientationVec[0] ) - tempCurrentOrientationVec[0] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ),
			tempCurrentOrientationVec[1] + (( targetOrientationVec[1] ) - tempCurrentOrientationVec[1] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ),
			tempCurrentOrientationVec[2] + (( targetOrientationVec[2] ) - tempCurrentOrientationVec[2] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 )),
			tempCurrentOrientationRad + (( targetOrientationRad ) - tempCurrentOrientationRad ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 ));

		mainCamera.setOrientation(tempOrientation);

		writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr
		writePosition(mainCamera.getPosition()); // Schreibt position in ObjMgr

		double tempFocalDistance;
		double tempCurrentFocalDistance = (mainCamera.getPosition() - mainCamera.getLookAt()).length();
		tempFocalDistance = tempCurrentFocalDistance + ( targetFocalDistance - tempCurrentFocalDistance ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );						
		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&tempFocalDistance,omINFOTYPE_DOUBLE,true,true);

		double tempHeight = mainCamera.getHeight() + ( targetHeight - mainCamera.getHeight()) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
		mainCamera.setHeight(tempHeight);
		myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,&tempHeight,omINFOTYPE_DOUBLE,true,true);

		delete splittedPosition;
		delete splittedOrientation;
	}
}



void AnimationExecuter::commandMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter)
{
	if (tempEntryParams->size()==4)
	{
		kDebug::Debug("oldMove",kDebug::DL_HIGH);
		oldMove(tempEntryParams,currentEntry);
	}
	else
	{
		writeCalculatedViewpointValues(tempEntryParams, currentEntry, frameNr, FrameEntryListIter, currentEntry->getCommandID());
	}
}


/**	Write the orientation to the ObjMgr
\param  orientation 
\return void  
*/
void AnimationExecuter::writeOrientation(SbRotation orientation)
{
	ML_TRACE_IN("AnimationExecuter::writeOrientation()");
	SbVec3f axis;
	float x,y,z,radians;

	orientation.getValue(axis,radians);
	axis.getValue(x,y,z);

	vec4* tempVec4 = new vec4((double)x,(double)y,(double)z,(double)radians);
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_ORIENTATION,tempVec4,omINFOTYPE_VEC4,true,true);
	delete tempVec4;
}


/**	Write the position to the ObjMgr
\param  position 
\return void  
*/
void AnimationExecuter::writePosition(SbVec3f position)
{
	ML_TRACE_IN("AnimationExecuter::writePosition()");
	vec3* tempVec3 = new vec3(position[0],position[1],position[2]);
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_POSITION,tempVec3,omINFOTYPE_VEC3,true,true);
	delete tempVec3;
}

//! Führt einen alten Move-Befehl aus, bei dem sich linear auf den Zielpunkt hinbewegt wird.\n
//!	Dieser Zielpunkt ist gegeben durch BoundingBox des Objektes und 2 Kugelkoordinaten
//! \param  tempEntryParams 
//! \param  currentEntry 
//! \return void  
void AnimationExecuter::oldMove(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry)
{	
	ML_TRACE_IN("AnimationExecuter::oldMove()");
	string tempObjName = kBasics::trimQuotatedStr((string)(*tempEntryParams)[0], kBasics::QUOTATION_SINGLE);

	vector<string>* splittedLookAt = new vector<string>;
	kBasics::split(tempObjName,kScriptFrameEntry::DELIMITER_AXIS,3,splittedLookAt);	

	SbVec3f BBCenter;
	double radius;	
	if (splittedLookAt->size()<3)
	{									
		AnimationCache::Measures MasterBB = myCache->getMeasuresFromHT(tempObjName);				
		BBCenter = myCache->getBoundingCenter(MasterBB);																	
		radius = (BBCenter-MasterBB.BB_min).length();
	}
	else
	{
		BBCenter = SbVec3f(kBasics::StringToInt((*splittedLookAt)[0]),kBasics::StringToInt((*splittedLookAt)[1]),kBasics::StringToInt((*splittedLookAt)[2]));
		radius = 1;
	}

	double targetHeight = 2.0*radius;
	if (tempEntryParams->size()>=4)
		targetHeight = targetHeight * atof(((string)(*tempEntryParams)[3]).c_str());

	SbVec3f targetPosition = BBCenter;						
	targetPosition[2]=targetPosition[2]+radius; //genau die entfernung des Kugelradius' vom BBCenter (=targetPos) entfernt.

	//Rotation der Target-Position aus dem Original heraus
	double angle;						

	//Rotation sagittal = x-Axis
	if (tempEntryParams->size()>=2)
	{
		angle = atof((*tempEntryParams)[1].c_str())*2.0*kBasics::PI/360.0;
		SbRotation* posRot = new SbRotation(SbVec3f(1.0,0.0,0.0),angle);
		posRot->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;
		delete posRot;
	}

	//Rotation axial = -z-Axis
	if (tempEntryParams->size()>=3)
	{
		angle = atof((*tempEntryParams)[2].c_str())*2.0*kBasics::PI/360.0;
		SbRotation* posRot = new SbRotation(SbVec3f(0.0,0.0,-1.0),angle);
		posRot->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;
		delete posRot;
	}													

	SbVec3f tempPosition;
	SbVec3f tempCurrentPosition = mainCamera.getPosition();
	tempPosition[0] = tempCurrentPosition[0] + (( targetPosition[0] ) - tempCurrentPosition[0] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
	tempPosition[1] = tempCurrentPosition[1] + (( targetPosition[1] ) - tempCurrentPosition[1] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
	tempPosition[2] = tempCurrentPosition[2] + (( targetPosition[2] ) - tempCurrentPosition[2] ) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );

	SbVec3f tempCurrentLookAt = mainCamera.getLookAt();
	SbVec3f newLookAt = tempCurrentLookAt + (BBCenter-tempCurrentLookAt) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );

	mainCamera.setCamPosition(tempPosition,newLookAt);

	writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr
	writePosition(mainCamera.getPosition()); // Schreibt position in ObjMgr					

	double focalDistance = (mainCamera.getPosition() - mainCamera.getLookAt()).length();
	mainCamera.setFocalDistance(focalDistance);
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&focalDistance,omINFOTYPE_DOUBLE,true,true);

	double tempHeight = mainCamera.getHeight() + ( targetHeight - mainCamera.getHeight()) / (double)( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,&tempHeight,omINFOTYPE_DOUBLE,true,true);
	mainCamera.setHeight(tempHeight);

	delete splittedLookAt;
}

//! Führt die im ersten Frame berechneten Werte (Position etc.) für einen newMove aus.\n
//! Dieser kann sowohl ein viewpoint-Move sein aber auch ein Move auf Kugelkoordinaten nur entlang der Bounding-Sphere.
//! \param  tempEntryParams 
//! \param  currentEntry 
//! \param  frameNr 
//! \param  FrameEntryListIter 
//! \param  commandID 
//! \return void  
void AnimationExecuter::writeCalculatedViewpathValues(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID)
{	
	ML_TRACE_IN("AnimationExecuter::writeCalculatedViewpathValues()");

	if (currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP){			
		preCalcPathPositions(currentEntry,frameNr,commandID);
		//Params werden nochmals abgefragt (für den ersten Frame der Anweisung haben sie sich ja geändert)
		currentEntry = myList.FrameList[frameNr]->EntryList[FrameEntryListIter];
		tempEntryParams = currentEntry->getParams();												
	}

	//Wenn die frames ordentlich in preCalcMovePositions() gesetzt wurden, können die neu berechneten Parameter auch hier zur Ausführung der Bewegung genutzt werden					
	if (currentEntry->getCommunicationFlag()==kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP){					
		//Debug("tempEntryParamsSize:"+kBasics::IntToString(tempEntryParams->size()),DL_HIGH);
		//Debug("Params:"+currentEntry->getParamString(),DL_HIGH);
		if (tempEntryParams->size()==3)
		{
			SbVec3f tempPosition = kBasics::StringToVec3f((*tempEntryParams)[1],',');
			SbVec3f tempCurrentLookAt = mainCamera.getLookAt();			
			SbVec3f newLookAt = kBasics::StringToVec3f((*tempEntryParams)[0],',');
			mainCamera.setCamPosition(tempPosition,newLookAt);
			writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr
			writePosition(mainCamera.getPosition()); // Schreibt position in ObjMgr					

			double focalDistance = (mainCamera.getPosition() - mainCamera.getLookAt()).length();			
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&focalDistance,omINFOTYPE_DOUBLE,true,true);
			mainCamera.setFocalDistance(focalDistance);

			double tempHeight = kBasics::StringToDouble((*tempEntryParams)[2]);
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,&tempHeight,omINFOTYPE_DOUBLE,true,true);
			mainCamera.setHeight(tempHeight);
		}
	}
}
void AnimationExecuter::preCalcPathPositionParts(kScriptFrameEntry* frameEntry, int globalFrameNr, int frameFrom, int frameTo, SbVec3f targetPosition, SbVec3f targetLookAt,double targetHeight,SbVec3f &currentPosition,SbVec3f &currentLookAt,double &currentHeight,SbVec3f &BBCenter, double radius){
	fldTestCamPosition->setStringValue(kBasics::Vec3fToString(currentPosition));

	// 02 - Get cross point with sphere as first anchor for the hole path
	SbVec3f crossPoint, tempVec3;
	tempVec3 = (-BBCenter)+currentPosition;
	tempVec3.normalize();
	tempVec3 = tempVec3*radius;
	tempVec3 = BBCenter + tempVec3;
	crossPoint = tempVec3;
	fldTestCrossPoint->setStringValue(kBasics::Vec3fToString(crossPoint));
	
	// 03 - Calc length of arc on sphere surface
	SbVec3f vec1target, vec2crosspoint, rotationVec;
	vec1target = targetPosition - BBCenter;
	vec2crosspoint = crossPoint - BBCenter;
	rotationVec = vec2crosspoint.cross(vec1target);
	double sphere_angle;
	double tempQuot = vec1target.dot(vec2crosspoint) / (vec1target.length()*vec2crosspoint.length());
	if (tempQuot > 1.0 ) tempQuot = 1.0;
	else if (tempQuot < -1.0 ) tempQuot = -1.0;
	sphere_angle = acos( tempQuot );
	fldTestSphereAngle->setDoubleValue(sphere_angle);
	//Vektor zeigt von der currenPos zur Sphere
	SbVec3f vecToSphere = BBCenter - currentPosition + vec2crosspoint;

	// XX - Calc positions for every frame
	double distance;
	SbVec3f nextPosition;
	double nextHeight;
	SbVec3f nextLookAt;	
	double maxHeight = (currentPosition - targetPosition).length();
	SbVec3f vectorToTarget = targetPosition - currentPosition;
	for (int i=frameFrom; i<=frameTo; i++){		
		//Die Distanz entspricht immer der Distanz vom Startpunkt bis zum aktuellen Frame und NICHT nur der Distanz für diesen einen Frame!
		distance = getRotationAngleByType(maxHeight,"LINEAR",i-frameFrom,frameTo-frameFrom);//frameEntry->getFrameCount());
		double prozdist=distance/maxHeight;
		nextPosition=currentPosition+prozdist*vectorToTarget;
		SbVec3f direction=nextPosition-(BBCenter);
		nextPosition+=(fldTestSphereRadius->getDoubleValue()/direction.length()-1.0)*direction;
		nextLookAt = currentLookAt - (currentLookAt - targetLookAt) * prozdist;
		nextHeight = currentHeight - (currentHeight - targetHeight) * (distance/maxHeight);
		if(prozdist<0.5){
			//nextHeight+=radius*2.0*prozdist;
		}else{
			//nextHeight+=radius*2.0*(1.0-prozdist);
		}
		testPositions.push_back(nextPosition);
		//Write camera values as parameter in corresponding FrameEntry
		int j = myList.FrameList[globalFrameNr+i]->getEntryIDByCommandLine(frameEntry->getCommandLine());
		if (j!=-1){	
			string ParamStr = kBasics::Vec3fToString(nextLookAt,',');
			ParamStr += " " + kBasics::Vec3fToString(nextPosition,',');
			ParamStr += " " + kBasics::DoubleToString(nextHeight);
			myList.FrameList[globalFrameNr+i]->EntryList[j]->setParamString(ParamStr);
			myList.FrameList[globalFrameNr+i]->EntryList[j]->setCommunicationFlag(kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP);
			if (fldDrawCameraPath->getBoolValue()){
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Position"+kBasics::leadingZero(i,6), new vec3(nextPosition[0],nextPosition[1],nextPosition[2]), omINFOTYPE_VEC3, true, false);
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Height"+kBasics::leadingZero(i,6), new double(radius), omINFOTYPE_DOUBLE, true, false);
				//Calc Orientation
				kCamera tempCam;
				tempCam.setCamPosition(nextPosition,nextLookAt);
				SbVec3f axis; float x,y,z,radians;				
				tempCam.getOrientation().getValue(axis,radians);
				axis.getValue(x,y,z);
				vec4* tempVec4 = new vec4((double)x,(double)y,(double)z,(double)radians);
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Orientation"+kBasics::leadingZero(i,6), tempVec4, omINFOTYPE_VEC4, true, false);
				delete tempVec4;
			}
		}else{
			kDebug::Debug("Frame entry which should be there isnt :-(",kDebug::DL_HIGH);
		}
	}
	if (fldDrawCameraPath->getBoolValue()) myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "PositionCount", new int(frameEntry->getFrameCount()), omINFOTYPE_INT32, true, false);
	testBBCenter = BBCenter;
	testTargetHeight = 2.0*radius;
	currentPosition=nextPosition;
	currentLookAt=nextLookAt;
}
void AnimationExecuter::preCalcPathPositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int commandID){
	ML_TRACE_IN("AnimationExecuter::preCalcPathPositions()");
	testPositions.clear();
	if (commandID != kScriptFrameEntry::CMD_CAMPATH_VP){
		kDebug::Debug ("NO CAMPATH_VP Command!",kDebug::DL_HIGH);
		return;
	}
	std::list<SbVec3f> targetPositions;
	int countPositions;
	myObjMgr->getObjAttributeInt("CameraSolver", "Camera", "Count", countPositions);
	if(countPositions<=0){
		return;
	}
	for(int i=0;i<countPositions;i++){
		std::ostringstream os;
		os << "Position" << i;
		SbVec3f targetPosition;
		myObjMgr->getObjAttributeVec3f("CameraSolver", "Camera", os.str(), targetPosition);
		targetPositions.push_back(targetPosition);
		kDebug::Debug("get Position from ObjMgr::CameraSolver = "+kBasics::Vec3fToString(targetPosition),kDebug::DL_HIGH);
		fldTestTargetPosition->setStringValue(kBasics::Vec3fToString(targetPosition));
	}
	SbVec3f targetLookAt;
	float targetHeight=2.0;
	string tempObjName = kBasics::trimQuotatedStr((string)(*frameEntry->getParams())[0], kBasics::QUOTATION_SINGLE);
	AnimationCache::Measures lookAtBB = myCache->getMeasuresFromHT(tempObjName);		
	targetLookAt = myCache->getBoundingCenter(lookAtBB);
	targetHeight = fldCampathVPSize->getFloatValue()*(targetLookAt-lookAtBB.BB_min).length(); //Durchmesser lokale Kugel
	int k=0;
	int framesPerPart=frameEntry->getFrameCount()/targetPositions.size();
	std::list<SbVec3f>::iterator it=targetPositions.begin();
	int lastFrame=-1;
	SbVec3f tp;
	SbVec3f currentPosition = mainCamera.getPosition();
	double currentHeight = mainCamera.getHeight();
	SbVec3f currentLookAt = mainCamera.getLookAt();
	double newHeight=currentHeight;
	double allFrames=frameEntry->getFrameCount();
	double percentageFrames=framesPerPart/allFrames;
	double heightDiff=currentHeight-targetHeight;
	// 01 - Build surrounding sphere ('All')
	SbVec3f BBCenter;
	double radius,halfRadius;	
	AnimationCache::Measures MasterBB;
	MasterBB = myCache->getMeasuresFromHT("ALL"); // watch for uppercase
	BBCenter = myCache->getBoundingCenter(MasterBB);
	radius = (BBCenter-MasterBB.BB_min).length();
	fldTestSphereCenter->setStringValue(kBasics::Vec3fToString(BBCenter));
	fldTestSphereRadius->setDoubleValue(radius);
	if (fldDrawCameraPath->getBoolValue()){
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereCenter", new vec3(BBCenter[0],BBCenter[1],BBCenter[2]), omINFOTYPE_VEC3, true, false);
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereRadius", new double(radius), omINFOTYPE_DOUBLE, true, false);
	}
	halfRadius=radius/2.0;
	int min=countPositions/3;
	int max=countPositions-min;
	while(it!=targetPositions.end()){
		tp=*it;
		if(k<min){
			preCalcPathPositionParts(frameEntry,globalFrameNr,k*framesPerPart,(k+1)*framesPerPart-1,tp,targetLookAt,radius-(1.0-(double)(k+1)/(double)min)*radius,currentPosition,currentLookAt,newHeight,BBCenter,radius);
			newHeight=radius-(1.0-(double)(k+1)/(double)min)*radius;
		}else if(k>max){
			preCalcPathPositionParts(frameEntry,globalFrameNr,k*framesPerPart,(k+1)*framesPerPart-1,tp,targetLookAt,radius-((double)(k-max)/(double)min)*radius,currentPosition,currentLookAt,newHeight,BBCenter,radius);
			newHeight=radius-((double)(k-max)/(double)min)*radius;
		}else{
			preCalcPathPositionParts(frameEntry,globalFrameNr,k*framesPerPart,(k+1)*framesPerPart-1,tp,targetLookAt,radius,currentPosition,currentLookAt,radius,BBCenter,radius);
		}
		lastFrame=(k+1)*framesPerPart-1;
		it++;
		k++;
	}
	if(lastFrame>=0 && lastFrame<=frameEntry->getFrameCount()){
		newHeight+=percentageFrames*heightDiff;
		preCalcPathPositionParts(frameEntry,globalFrameNr,lastFrame,frameEntry->getFrameCount(),tp,targetLookAt,targetHeight,currentPosition,currentLookAt,newHeight,BBCenter,radius);
	}
}

void AnimationExecuter::writeCalculatedViewpointValues(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID)
{	
	ML_TRACE_IN("AnimationExecuter::writeCalculatedViewpointValues()");

	if (currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTEVPMOVE)
	{			
		preCalcMovePositions(currentEntry,frameNr,commandID);
		//Params werden nochmals abgefragt (für den ersten Frame der Anweisung haben sie sich ja geändert)
		currentEntry = myList.FrameList[frameNr]->EntryList[FrameEntryListIter];
		tempEntryParams = currentEntry->getParams();												
	}

	//Wenn die frames ordentlich in preCalcMovePositions() gesetzt wurden, können die neu berechneten Parameter auch hier zur Ausführung der Bewegung genutzt werden					
	if (currentEntry->getCommunicationFlag()==kScriptFrameEntry::CFLAG_EXECUTEVPMOVE)
	{					
		//Debug("tempEntryParamsSize:"+kBasics::IntToString(tempEntryParams->size()),DL_HIGH);
		//Debug("Params:"+currentEntry->getParamString(),DL_HIGH);
		if (tempEntryParams->size()==3)
		{
			SbVec3f tempPosition = kBasics::StringToVec3f((*tempEntryParams)[1],',');
			SbVec3f tempCurrentLookAt = mainCamera.getLookAt();			
			SbVec3f newLookAt = kBasics::StringToVec3f((*tempEntryParams)[0],',');
			mainCamera.setCamPosition(tempPosition,newLookAt);
			writeOrientation(mainCamera.getOrientation()); // Schreibt orientation in ObjMgr
			writePosition(mainCamera.getPosition()); // Schreibt position in ObjMgr					

			double focalDistance = (mainCamera.getPosition() - mainCamera.getLookAt()).length();			
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&focalDistance,omINFOTYPE_DOUBLE,true,true);
			mainCamera.setFocalDistance(focalDistance);

			double tempHeight = kBasics::StringToDouble((*tempEntryParams)[2]);
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,&tempHeight,omINFOTYPE_DOUBLE,true,true);
			mainCamera.setHeight(tempHeight);
		}
	}
}

//! Berechnet die Positionen für einen viewpointMove im vorraus für alle Frames
//! \param  frameEntry 
//! \param  globalFrameNr 
//! \param  commandID 
//! \return void  
void AnimationExecuter::preCalcMovePositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int commandID)
{
	ML_TRACE_IN("AnimationExecuter::preCalcMovePositions()");
	testPositions.clear();

	if (commandID == kScriptFrameEntry::CMD_MOVE && frameEntry->getParams()->size()<5)
	{
		kDebug::Debug ("To few parameters for newMove-Command!",kDebug::DL_HIGH);
		return;
	}
	else if ((commandID == kScriptFrameEntry::CMD_VIEWPOINTMOVE || commandID == kScriptFrameEntry::CMD_VIEWPOINTCLUSTERING) && frameEntry->getParams()->size()<3)
	{
		kDebug::Debug ("To few parameters for ViewpointMove-Command!",kDebug::DL_HIGH);
		return;
	}


	// 00 - Get current values

	SbVec3f currentPosition = mainCamera.getPosition();
	fldTestCamPosition->setStringValue(kBasics::Vec3fToString(currentPosition));

	SbVec3f currentLookAt = mainCamera.getLookAt();
	//???Das ist falsch ... da so nicht das durch verschiebung veränderte lookAt rausbekommen wird
	//SbVec3f currentLookAt = mainCamera.getPosition()+mainCamera.getLookDir();

	double currentHeight = mainCamera.getHeight();


	// 01 - Build surrounding sphere ('All')

	SbVec3f BBCenter;
	double radius;	
	AnimationCache::Measures MasterBB;
	if (commandID == kScriptFrameEntry::CMD_MOVE)
		MasterBB = myCache->getMeasuresFromHT(kBasics::toUp((*frameEntry->getParams())[1])); // watch for uppercase
	else
		MasterBB = myCache->getMeasuresFromHT("ALL"); // watch for uppercase

	BBCenter = myCache->getBoundingCenter(MasterBB);
	kDebug::Debug("BBCenter = "+kBasics::Vec3fToString(BBCenter),kDebug::DL_LOW);
	radius = (BBCenter-MasterBB.BB_min).length();

	kDebug::Debug("radius:"+kBasics::DoubleToString(radius),kDebug::DL_LOW);
	fldTestSphereCenter->setStringValue(kBasics::Vec3fToString(BBCenter));
	fldTestSphereRadius->setDoubleValue(radius);

	if (fldDrawCameraPath->getBoolValue())
	{
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereCenter", new vec3(BBCenter[0],BBCenter[1],BBCenter[2]), omINFOTYPE_VEC3, true, false);
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereRadius", new double(radius), omINFOTYPE_DOUBLE, true, false);
	}



	// 02 - Get cross point with sphere as first anchor for the hole path

	SbVec3f crossPoint, tempVec3;
	tempVec3 = (-BBCenter)+currentPosition;
	tempVec3.normalize();
	tempVec3 = tempVec3*radius;
	tempVec3 = BBCenter + tempVec3;
	crossPoint = tempVec3;

	fldTestCrossPoint->setStringValue(kBasics::Vec3fToString(crossPoint));
	kDebug::Debug("crossPoint:"+kBasics::Vec3fToString(crossPoint),kDebug::DL_LOW);


	// 03 - Get target Postiotion from a) to polar coords and BBCenter (same as old cmd_move) or b) New CamPosition from Neugi in ObjMgr
	// Example 90 45
	SbVec3f targetPosition;
	if (commandID == kScriptFrameEntry::CMD_MOVE)
	{
		targetPosition = BBCenter;						
		targetPosition[2]=targetPosition[2]+radius; //genau die entfernung des Kugelradius' vom BBCenter (=targetPos) entfernt.

		//Rotation der Target-Position aus dem Original heraus
		double angle;						

		//Rotation sagittal = x-Axis
		angle = atof((*frameEntry->getParams())[2].c_str())*2.0*kBasics::PI/360.0;
		SbRotation* posRot1 = new SbRotation(SbVec3f(1.0,0.0,0.0),angle);
		posRot1->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;

		//Rotation axial = -z-Axis
		angle = atof((*frameEntry->getParams())[3].c_str())*2.0*kBasics::PI/360.0;
		SbRotation* posRot2 = new SbRotation(SbVec3f(0.0,0.0,-1.0),angle);
		posRot2->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;

		delete posRot1; delete posRot2;
	}
	else //Neugis position
	{		
		myObjMgr->getObjAttributeVec3f("CameraSolver","Camera","Position", targetPosition);
		kDebug::Debug("get Position from ObjMgr::CameraSolver = "+kBasics::Vec3fToString(targetPosition),kDebug::DL_HIGH);
	}

	fldTestTargetPosition->setStringValue(kBasics::Vec3fToString(targetPosition));



	// 04 - Calc length of arc on sphere surface
	// b = radius *  arc(alpha)
	SbVec3f vec1target, vec2crosspoint, rotationVec;
	vec1target = targetPosition - BBCenter;
	vec2crosspoint = crossPoint - BBCenter;
	rotationVec = vec2crosspoint.cross(vec1target);

	//kDebug::Debug("vec1target:"+kBasics::Vec3fToString(vec1target),DL_HIGH);
	//kDebug::Debug("vec2crosspoint:"+kBasics::Vec3fToString(vec2crosspoint),DL_HIGH);

	double sphere_angle;
	//kDebug::Debug("vec1target.dot(vec2crosspoint):"+kBasics::DoubleToString(vec1target.dot(vec2crosspoint)),DL_HIGH);
	//kDebug::Debug("vec1target.length()*vec2crosspoint.length():"+kBasics::DoubleToString(vec1target.length()*vec2crosspoint.length()),DL_HIGH);
	double tempQuot = vec1target.dot(vec2crosspoint) / (vec1target.length()*vec2crosspoint.length());
	if (tempQuot > 1.0 ) tempQuot = 1.0;
	else if (tempQuot < -1.0 ) tempQuot = -1.0;

	//kDebug::Debug("tempQuot:"+kBasics::DoubleToString(tempQuot),DL_HIGH);
	sphere_angle = acos( tempQuot );


	fldTestSphereAngle->setDoubleValue(sphere_angle);

	//kDebug::Debug("sphere_angle:"+kBasics::DoubleToString(sphere_angle),kDebug::DL_HIGH);
	//kDebug::Debug("radius:"+kBasics::DoubleToString(radius),kDebug::DL_HIGH);

	double lengthArc = radius * sphere_angle;	
	//kDebug::Debug("lengthArc:"+kBasics::DoubleToString(lengthArc),kDebug::DL_HIGH);

	//Vektor zeigt von der currenPos zur Sphere
	SbVec3f vecToSphere = BBCenter - currentPosition + vec2crosspoint;
	double lengthToSphere = vecToSphere.length();
	//kDebug::Debug("lengthToSphere:"+kBasics::DoubleToString(lengthToSphere),kDebug::DL_HIGH);



	// 05 - Get lookAtPosition and calc cam height

	SbVec3f targetLookAt;
	float targetHeight;
	vector<string>* splittedLookAt = new vector<string>;
	string tempObjName;
	if (commandID == kScriptFrameEntry::CMD_MOVE || commandID == kScriptFrameEntry::CMD_VIEWPOINTMOVE)
	{
		tempObjName = kBasics::trimQuotatedStr((string)(*frameEntry->getParams())[0], kBasics::QUOTATION_SINGLE);
	}
	else
	{
		tempObjName = kBasics::trimQuotatedStr((string)(*frameEntry->getParams())[1], kBasics::QUOTATION_SINGLE);
		//kDebug::Debug("lookAt = " + tempObjName,kDebug::DL_HIGH);
	}
	kBasics::split(tempObjName,kScriptFrameEntry::DELIMITER_AXIS,3,splittedLookAt);

	if (splittedLookAt->size()<3) //newMove und Smurf
	{										
		AnimationCache::Measures lookAtBB = myCache->getMeasuresFromHT(tempObjName);				
		targetLookAt = myCache->getBoundingCenter(lookAtBB);
		string paramHeight;
		if (commandID == kScriptFrameEntry::CMD_VIEWPOINTMOVE)
		{
			paramHeight = (string)(*frameEntry->getParams())[1];

		}
		else //NewMove und ganz altes Move
		{
			paramHeight = (string)(*frameEntry->getParams())[4];
		}
		targetHeight = kBasics::StringToDouble(paramHeight) * (targetLookAt-lookAtBB.BB_min).length(); //Durchmesser lokale Kugel
	}
	else //ganz altes Move
	{
		targetLookAt = SbVec3f(kBasics::StringToInt((*splittedLookAt)[0]),kBasics::StringToInt((*splittedLookAt)[1]),kBasics::StringToInt((*splittedLookAt)[2]));		
		targetHeight = kBasics::StringToDouble((string)(*frameEntry->getParams())[4]);
	}
	//kDebug::Debug("targetHeight:"+kBasics::FloatToString(targetHeight),kDebug::DL_HIGH);



	// XX - Calc positions for every frame

	double distance;
	SbVec3f nextPosition;
	double nextHeight;
	SbVec3f nextLookAt;	
	//kDebug::Debug("frameCount:"+kBasics::IntToString(frameEntry->getFrameCount()),DL_HIGH);

	for (int i=0; i<=frameEntry->getFrameCount(); i++)
	{		
		//Die Distanz entspricht immer der Distanz vom Startpunkt bis zum aktuellen Frame und NICHT nur der Distanz für diesen einen Frame!
		distance = getRotationAngleByType(lengthToSphere+lengthArc,"BEZIER2",i,frameEntry->getFrameCount());
		//Debug("distance at "+ kBasics::IntToString(i) + " : "+kBasics::DoubleToString(distance),DL_HIGH);

		if (distance<=lengthToSphere)
		{
			nextPosition = currentPosition + (distance/lengthToSphere)*vecToSphere;
			//Debug("nextPosition at "+ kBasics::IntToString(i) + " : " + kBasics::Vec3fToString(nextPosition),DL_HIGH);			
		}
		else
		{
			//Prozentualer Anteil am zurückzulegenden Winkel
			double anglePart = ((distance-lengthToSphere)/lengthArc);
			double angleToRotate = anglePart * sphere_angle;
			//Debug("anglePart: "+kBasics::DoubleToString(anglePart)+ "   angelToRotate:"+kBasics::DoubleToString(angleToRotate),DL_HIGH);

			//Get point on sphere by rotation crossPoint Vector
			SbRotation* surfaceRotate = new SbRotation(rotationVec,angleToRotate);
			surfaceRotate->multVec(vec2crosspoint,nextPosition);
			nextPosition = nextPosition + BBCenter;
			//Debug("nextPosition at "+ kBasics::IntToString(i) + " : " + kBasics::Vec3fToString(nextPosition),DL_HIGH);			
			delete surfaceRotate;
		}

		nextLookAt = currentLookAt - (currentLookAt - targetLookAt) * (distance / (lengthToSphere+lengthArc) );
		nextHeight = currentHeight - (currentHeight - targetHeight) * (distance / (lengthToSphere+lengthArc) );		

		testPositions.push_back(nextPosition);

		//Write camera values as parameter in corresponding FrameEntry

		//Debug("globalFrameNr:"+kBasics::IntToString(globalFrameNr), DL_HIGH);
		//Debug("i:"+kBasics::IntToString(i), DL_HIGH);
		//Debug("frameEntry->getCommandLine()"+frameEntry->getCommandLine(),DL_HIGH);
		int j = myList.FrameList[globalFrameNr+i]->getEntryIDByCommandLine(frameEntry->getCommandLine());
		//Debug("j:"+kBasics::IntToString(j), DL_HIGH);
		if (j!=-1)
		{	
			string ParamStr;
			ParamStr = kBasics::Vec3fToString(nextLookAt,',');
			ParamStr = ParamStr + " " + kBasics::Vec3fToString(nextPosition,',');
			ParamStr = ParamStr + " " + kBasics::DoubleToString(nextHeight);

			myList.FrameList[globalFrameNr+i]->EntryList[j]->setParamString(ParamStr);
			myList.FrameList[globalFrameNr+i]->EntryList[j]->setCommunicationFlag(kScriptFrameEntry::CFLAG_EXECUTEVPMOVE);

			//Debug ("setParamString for frame:"+kBasics::IntToString(globalFrameNr+i)+"  Entry:"+kBasics::IntToString(j)+"  ParamStr: *"+ParamStr+"*",DL_HIGH);
			//kDebug::Debug("fldDrawCameraPath->getBoolValue() = " + kBasics::BoolToString(fldDrawCameraPath->getBoolValue()),kDebug::DL_HIGH);
			if (fldDrawCameraPath->getBoolValue())
			{
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Position"+kBasics::leadingZero(i,6), new vec3(nextPosition[0],nextPosition[1],nextPosition[2]), omINFOTYPE_VEC3, true, false);
			}
		}
		else kDebug::Debug("Frame entry which should be there isnt :-(",kDebug::DL_HIGH);		
	}

	if (fldDrawCameraPath->getBoolValue()) myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "PositionCount", new int(frameEntry->getFrameCount()), omINFOTYPE_INT32, true, false);

	testBBCenter = BBCenter;
	testTargetHeight = 2.0*radius;

	delete splittedLookAt;
}

/**	Returns a double value by given target value, currten Frame and intepoltype\n
interpoltype can be "BEZIER2" or linear
\param  targetValue 
\param  interpoltype 
\param  currentFrame 
\param  frameCount 
\return double  
*/
double AnimationExecuter::getRotationAngleByType(const double targetValue, const string& interpoltype, const int& currentFrame, const int& frameCount)
{
	ML_TRACE_IN("AnimationExecuter::getRotationAngleByType()");
	double angle = 0.0;
	double t_next, Q_y, Q_y_next;
	if (interpoltype=="BEZIER2")
	{
		t_next = (double)(((double)currentFrame+1.0) / (double)((double)frameCount+1.0));
		Q_y = Bezier( 0.0 );
		Q_y_next = Bezier( t_next );
		angle = (Q_y_next - Q_y) * targetValue;
	}
	else //linear
		angle = targetValue * ((currentFrame+1.0) / ((double)frameCount+1.0)); //linear	

	return angle;
}



/**	Diese Funktion (lastAngle und die HashTable) sind dazu da, den jeweils letzten gesetzen Wert der Anweisung zu erfahren.\n
Dies ist für den Fall wichtig, wenn im RealTime-Betrieb Frames übersprungen wurden und der jeweilige Wert nicht einfach um einen festen, frameanzahlabhängigen Betrag erhöht werden darf (sondern mehr).
\param  CommandLine 
\param  interpoltype 
\param  currentFrame 
\param  frameCount 
\param  angle 
\return void  
*/
void AnimationExecuter::getAngleByLastValue(const string CommandLine, const string interpoltype, const int currentFrame, const int frameCount, double& angle)
{
	ML_TRACE_IN("AnimationExecuter::getAngleByLastValue()");
	double lastAngle;
	string* strLastValue = myList.htLastValues->find(CommandLine);
	if (strLastValue!=NULL)
		lastAngle = kBasics::StringToDouble(*strLastValue);
	else
		lastAngle = 0.0;

	double targetAngle = getRotationAngleByType(angle, interpoltype, currentFrame, frameCount);	
	angle = targetAngle - lastAngle; //angle = zu rotierende Differenz

	myList.htLastValues->remove(CommandLine);
	myList.htLastValues->insert(CommandLine,kBasics::DoubleToString(lastAngle+angle));
}


//! Berechnet eine einfache Bezier-Funktion
//! \param  t 
//! \return double  
double AnimationExecuter::Bezier(const double t)
{
	ML_TRACE_IN("AnimationExecuter::Bezier()");
	//Bezier1:
	//	double value = (1.0-t)*(1.0-t)*(1.0-t)*0.0
	//	   +3.0*t*(1.0-t)*(1.0-t)*0.0
	//	   +3.0*t*t*(1.0-t)*1.0
	//	   +t*t*t*1.0;
	double value = (1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*0.0
		+6.0*t*(1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*0.0
		+15.0*t*t*(1.0-t)*(1.0-t)*(1.0-t)*(1.0-t)*0.0
		+20.0*t*t*t*(1.0-t)*(1.0-t)*(1.0-t)*0.5
		+15.0*t*t*t*t*(1.0-t)*(1.0-t)*1.0
		+6.0*t*t*t*t*t*(1.0-t)*1.0
		+t*t*t*t*t*t*1.0;
	return value;
}




/** Get current CamPosition from ObjMgr
\return SbVec3f  
*/
SbVec3f AnimationExecuter::getCamPosition()
{
	ML_TRACE_IN("AnimationExecuter::getCamPosition()");
	string tempAttr;	
	if (myObjMgr->getObjAttributeString((omIDType)fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_POSITION, tempAttr))
	{
		vector<string>* splitted = new vector<string>; //wär das kein Pointer, würde er zu 80% abstürzen (alter, nicht neu verifizierter Kommentar)		
		kBasics::split(tempAttr,kScriptFrameEntry::DELIMITER_COLOR_LAYER,3,splitted);
		SbVec3f tempVec;				
		tempVec.setValue(kBasics::StringToDouble((*splitted)[0]),kBasics::StringToDouble((*splitted)[1]),kBasics::StringToDouble((*splitted)[2]));
		kDebug::Debug("position get from viewer: "+kBasics::Vec3fToString(tempVec),kDebug::DL_HIGH);
		delete splitted;
		return tempVec;
	}
	return SbVec3f(0,0,0);
}



/**	Get current CamOrientation fomr ObjMgr
\return SbRotation
*/
SbRotation AnimationExecuter::getCamOrientation()
{
	ML_TRACE_IN("AnimationExecuter::getCamOrientation()");
	string tempAttr;
	if (myObjMgr->getObjAttributeString((omIDType)fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_ORIENTATION, tempAttr))
	{
		vector<string>* splitted = new vector<string>; //wär das kein Pointer, würde er zu 80% abstürzen (alter, nicht neu verifizierter Kommentar)		
		kBasics::split(tempAttr,kScriptFrameEntry::DELIMITER_COLOR_LAYER,4,splitted);
		SbRotation tempVec;				
		tempVec.setValue(SbVec3f(kBasics::StringToDouble((*splitted)[0]),kBasics::StringToDouble((*splitted)[1]),kBasics::StringToDouble((*splitted)[2])),kBasics::StringToDouble((*splitted)[3]));
		delete splitted;
		return tempVec;
	}
	return SbRotation(0,0,0,0);
}



/**	Get currentCamHeight from ObjMgr
\return double
*/
double AnimationExecuter::getCamHeight()
{
	ML_TRACE_IN("AnimationExecuter::getCamHeight()");
	string tempAttr;
	if (myObjMgr->getObjAttributeString((omIDType)fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT, tempAttr))
	{
		return kBasics::StringToDouble(tempAttr);
	}
	return 0;
}



/**	Get current focal distance of cam from ObjMgr
\return double
*/
double AnimationExecuter::getCamFocalDistance()
{
	ML_TRACE_IN("AnimationExecuter::getCamFocalDistance()");
	string tempAttr;
	if (myObjMgr->getObjAttributeString((omIDType)fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE, tempAttr))
		return kBasics::StringToDouble(tempAttr);
	return 0;
}




ML_END_NAMESPACE
