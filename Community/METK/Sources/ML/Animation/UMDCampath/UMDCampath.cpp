#include "UMDCampath.h"
#include "kDebug.h"
#include "UMDAnimation2/AnimationExecuter.h"

ML_START_NAMESPACE
AbstractCampath::AbstractCampath(ObjMgrCommunicator* omc,StringField* favn,kScriptFrameList* m,kCamera* mc,
								 AnimationCache* am,vector<SbVec3f>* tp,Vec3fField* ftsc,DoubleField* ftsr,
								 Vec3fField* fttp,BoolField* fdcp,const std::string& pn){
									 myObjMgr=omc;
									 fldAnimationViewerName=favn;
									 myList=m;
									 mainCamera=mc;
									 myCache=am;
									 testPositions=tp;
									 fldTestSphereCenter=ftsc;
									 fldTestSphereRadius=ftsr;
									 fldTestTargetPosition=fttp;
									 fldDrawCameraPath=fdcp;
									 pathName=pn;
									 framesPerScene=0;
									 maxHeight=0.0;									 
}

AbstractCampath::~AbstractCampath(){}

void AbstractCampath::writeCalculatedCamPathValues(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter, int commandID){	
	ML_TRACE_IN("GoogleCampath::writeCalculatedCamPathValues()");

	if (currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTECAMPATH)
	{	
		preCalcCamPathPositions(currentEntry,frameNr,commandID);
		//Params werden nochmals abgefragt (für den ersten Frame der Anweisung haben sie sich ja geändert)
		currentEntry = myList->FrameList[frameNr]->EntryList[FrameEntryListIter];
		tempEntryParams = currentEntry->getParams();												
	}

	//Wenn die frames ordentlich in preCalcCamPathPositions() gesetzt wurden, können die neu berechneten Parameter auch hier zur Ausführung der Bewegung genutzt werden
	if (currentEntry->getCommunicationFlag()==kScriptFrameEntry::CFLAG_EXECUTECAMPATH)
	{			
		if (tempEntryParams->size()==3)
		{
			SbVec3f tempPosition = kBasics::StringToVec3f((*tempEntryParams)[1],',');
			SbVec3f tempCurrentLookAt = mainCamera->getLookAt();			
			SbVec3f newLookAt = kBasics::StringToVec3f((*tempEntryParams)[0],',');
			mainCamera->setCamPosition(tempPosition,newLookAt);
			writeOrientation(mainCamera->getOrientation()); // Schreibt orientation in ObjMgr
			writePosition(mainCamera->getPosition()); // Schreibt position in ObjMgr					

			double focalDistance = (mainCamera->getPosition() - mainCamera->getLookAt()).length();			
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_FOCALDISTANCE,&focalDistance,omINFOTYPE_DOUBLE,true,true);
			mainCamera->setFocalDistance(focalDistance);

			double tempHeight = kBasics::StringToDouble((*tempEntryParams)[2]);
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_HEIGHT,&tempHeight,omINFOTYPE_DOUBLE,true,true);
			mainCamera->setHeight(tempHeight);
		}
	}
}

void AbstractCampath::preCalcCamPathPositions(kScriptFrameEntry* frameEntry, int globalFrameNr, int /*commandID*/){
	ML_TRACE_IN("GoogleCampath::preCalcCamPathPositions()");
	testPositions->clear();

	//1. Testen und Auslesen der Parameter
	readParameter(frameEntry);
	//2. Build surrounding sphere ('All')
	SbVec3f BBCenter;
	double radius;	
	AnimationCache::Measures MasterBB;
	MasterBB = myCache->getMeasuresFromHT("ALL"); // watch for uppercase
	BBCenter = myCache->getBoundingCenter(MasterBB);	
	radius = (BBCenter-MasterBB.BB_min).length();

	//Durch die Ausgabe der Koordinaten an die TestSphere, kann man optisch im 3D überprüfen, ob die BB richtig ermittelt wurde
	fldTestSphereCenter->setStringValue(kBasics::Vec3fToString(BBCenter));
	fldTestSphereRadius->setDoubleValue(radius);
	if (fldDrawCameraPath->getBoolValue())
	{
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereCenter", new vec3(BBCenter[0],BBCenter[1],BBCenter[2]), omINFOTYPE_VEC3, true, false);
		myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "SphereRadius", new double(radius), omINFOTYPE_DOUBLE, true, false);
	}


	SbVec3f currentPosition,currentLookAt,targetPosition,targetLookAt;
	double currentHeight=-1.0,targetHeight;
	int frameFrom=0;
	int frameTo=framesPerScene;
	string tempObjName=objectName;

	AnimationCache::Measures lookAtBB = myCache->getMeasuresFromHT(tempObjName);
	AnimationCache::MEASURESLIST myMeasureList;

	for(int oid=0;oid<scenePosition.objects.size();oid++)
	{
		tempObjName=scenePosition.objects[oid];
		if(!tempObjName.empty()){
			AnimationCache::Measures lookAtBBTmp = myCache->getMeasuresFromHT(tempObjName);
			myMeasureList.push_back(lookAtBBTmp);
		}
	}
	if(!myMeasureList.empty())
	{
		lookAtBB=myCache->getMasterBoundingBox(&myMeasureList);
	}

	//3. Get current values
	currentPosition = mainCamera->getPosition();
	currentLookAt = mainCamera->getLookAt();
	currentHeight = mainCamera->getHeight();

	//4. Get target Postiotion from polar coords and BBCenter OR take the given one
	if (scenePosition.polarCoord)
	{
		targetPosition = BBCenter;						
		targetPosition[2]=targetPosition[2]+radius; //genau die entfernung des Kugelradius' vom BBCenter (=targetPos) entfernt.

		//Rotation der Target-Position aus dem Original heraus
		double angle;
		double xAxis=scenePosition.position[0];
		double zAxis=scenePosition.position[1];		
		//Rotation sagittal = x-Axis
		angle = xAxis*2.0*kBasics::PI/360.0;
		SbRotation* posRot1 = new SbRotation(SbVec3f(1.0,0.0,0.0),angle);
		posRot1->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;
		//Rotation axial = -z-Axis
		angle = zAxis*2.0*kBasics::PI/360.0;
		SbRotation* posRot2 = new SbRotation(SbVec3f(0.0,0.0,-1.0),angle);
		posRot2->multVec(targetPosition-BBCenter,targetPosition);
		targetPosition = targetPosition + BBCenter;
		delete posRot1; delete posRot2;
	}
	else
	{
		targetPosition[0] = scenePosition.position[0];
		targetPosition[1] = scenePosition.position[1];
		targetPosition[2] = scenePosition.position[2];
	}

	double height = scenePosition.height;

	//Testweise Ausgabe der Zielposition um auch das optisch durch ne Kugel o.ä. zu testen
	fldTestTargetPosition->setStringValue(kBasics::Vec3fToString(targetPosition));
	//5. Get lookAtPosition and calc cam height
	targetLookAt = myCache->getBoundingCenter(lookAtBB);	
	//targetHeight = height * (targetLookAt-lookAtBB.BB_min).length(); //Durchmesser lokale Kugel
	targetHeight = 2* height * (targetLookAt-MasterBB.BB_min).length(); //Durchmesser global ALL-Kugel
	//6. Calc positions for every frame
	calcPositions(&BBCenter,frameEntry,globalFrameNr,currentHeight,targetHeight,&currentPosition,&targetPosition,&currentLookAt,&targetLookAt,frameFrom,frameTo);

	frameTo+=framesPerScene;
	if (fldDrawCameraPath->getBoolValue()) myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "PositionCount", new int(frameEntry->getFrameCount()), omINFOTYPE_INT32, true, false);
	currentPosition=targetPosition;
	currentLookAt=targetLookAt;
	currentHeight=targetHeight;
}

void AbstractCampath::calcPositions(SbVec3f* bbc,kScriptFrameEntry* frameEntry,int globalFrameNr,double currentHeight,double targetHeight,SbVec3f* currentPosition,SbVec3f* targetPosition,SbVec3f* currentLookAt,SbVec3f* targetLookAt,int frameFrom,int frameTo){
	if(frameFrom<0 || frameTo>frameEntry->getFrameCount() || frameTo<frameFrom)
		return;	
	maxHeight = (*currentPosition - *targetPosition).length();
	double distance,nextHeight,prozdist;
	SbVec3f vectorToTarget = *targetPosition - *currentPosition;
	SbVec3f nextPosition,nextLookAt;
	int framecnt=frameTo-frameFrom;
	if(maxHeight<=0.0)
	{
		nextPosition=*targetPosition;
		nextLookAt=*targetLookAt;
		nextHeight=targetHeight;
		for (int i=frameFrom; i<=frameTo; i++)
		{
			testPositions->push_back(nextPosition);
			setParameters(globalFrameNr,i,frameEntry,nextHeight,&nextLookAt,&nextPosition);
		}
	}
	else
	{
		for (int i=frameFrom; i<=frameTo; i++)
		{
			distance = AnimationExecuter::getRotationAngleByType(maxHeight,"BEZIER2",i-frameFrom,framecnt);
			prozdist=distance/maxHeight;
			internCalcPositions(bbc,currentHeight,targetHeight,currentPosition,currentLookAt,targetLookAt,prozdist,&vectorToTarget,nextPosition,nextLookAt,nextHeight);
			testPositions->push_back(nextPosition);
			//Write camera values as parameter in corresponding FrameEntry
			setParameters(globalFrameNr,i,frameEntry,nextHeight,&nextLookAt,&nextPosition);
		}
	}
}

bool AbstractCampath::testParameters(kScriptFrameEntry* frameEntry)
{
	if(frameEntry->getParams()->size()!=2){
		kDebug::Debug("Wrong number of parameter in UMDCampath",kDebug::DL_NONE);
		kDebug::Debug(frameEntry->getParamString(),kDebug::DL_NONE);
		for(int i=0;i<frameEntry->getParams()->size();i++){
			kDebug::Debug((*frameEntry->getParams())[i],kDebug::DL_NONE);
		}
		kDebug::Debug("Usage: [time1,time2] 'Cam' campath 'Object' Point(X,Y,Height)",kDebug::DL_NONE);
		return false;	
	}
	return true;
}

void AbstractCampath::readParameter(kScriptFrameEntry* frameEntry)
{
	//Parameter:	StructureName alpha;beta;zoom
	//ODER:			StructureName x;y;z;zoom
	if(!testParameters(frameEntry))
		return;
	objectName=(*frameEntry->getParams())[0];
	std::vector<std::string> bothparams;
	kBasics::split((*frameEntry->getParams())[1],',',-1,&bothparams);
	if(bothparams.size()>=1)
	{
		std::vector<std::string> actvector;
		kBasics::split(bothparams[0],';',-1,&actvector);		
		if(actvector.size()==3)
		{
			vec3 pos(kBasics::StringToDouble(actvector[0]),kBasics::StringToDouble(actvector[1]),0);
			double height = kBasics::StringToDouble(actvector[2]);
			vec4 v(0,0,0,0);
			std::vector<std::string> objs;
			if(bothparams.size()>=2){
				std::vector<std::string> orientvector;
				kBasics::split(bothparams[1],';',4,&orientvector);
				if(orientvector.size()==4){
					for(int g=0;g<4;g++){
						v[g]=kBasics::StringToFloat(orientvector[g]);
					}
					if(bothparams.size()>=3){
						kBasics::split(bothparams[2],';',-1,&objs);
					}
				}
			}
			ScenePosition sp(pos,height,v,true,objs);
			scenePosition=sp;
		}
		else if (actvector.size()==4)
		{
			vec3 pos(kBasics::StringToDouble(actvector[0]),kBasics::StringToDouble(actvector[1]),kBasics::StringToDouble(actvector[2]));
			double height = kBasics::StringToDouble(actvector[3]);
			vec4 v(0,0,0,0);
			std::vector<std::string> objs;
			if(bothparams.size()>=2){
				std::vector<std::string> orientvector;
				kBasics::split(bothparams[1],';',4,&orientvector);
				if(orientvector.size()==4){
					for(int g=0;g<4;g++){
						v[g]=kBasics::StringToFloat(orientvector[g]);
					}
					if(bothparams.size()>=3){
						kBasics::split(bothparams[2],';',-1,&objs);
					}
				}
			}
			ScenePosition sp(pos,height,v,false,objs);
			scenePosition=sp;			
		}
		else
		{
			std::cout << "ERROR in campath: use alpha;beta;zoom or x;y;z;zoom" << std::endl;
		}
	}
	calcFramesPerScene(frameEntry->getFrameCount());
}

void AbstractCampath::calcFramesPerScene(int count)
{
	framesPerScene=count;
}

void AbstractCampath::setParameters(int globalFrameNr,int i,kScriptFrameEntry* frameEntry,double nextHeight,SbVec3f* nextLookAt,SbVec3f* nextPosition){	
	if(i+globalFrameNr>=myList->FrameList.size())
		return;
	int j = myList->FrameList[globalFrameNr+i]->getEntryIDByCommandLine(frameEntry->getCommandLine());
	if (j!=-1) {
		if (!nextLookAt || ! nextPosition)
		{
			std::cout << "NULL nextLookAt  or NULL nextPosition ... return " << std::endl;
		}
		string ParamStr;
		ParamStr = kBasics::Vec3fToString(*nextLookAt,',');
		ParamStr = ParamStr + " " + kBasics::Vec3fToString(*nextPosition,',');
		ParamStr = ParamStr + " " + kBasics::DoubleToString(nextHeight);
		myList->FrameList[globalFrameNr+i]->EntryList[j]->setParamString(ParamStr);
		myList->FrameList[globalFrameNr+i]->EntryList[j]->setCommunicationFlag(kScriptFrameEntry::CFLAG_EXECUTECAMPATH);

		if (fldDrawCameraPath->getBoolValue()){
			/**Berechnung der Debugkamerapositionen mit Höhenangaben**/
			/*SbVec3f v3f=(*nextPosition)-(*nextLookAt);
			vec3 v((*nextPosition)[0],(*nextPosition)[1],(*nextPosition)[2]);
			v3f.normalize();
			v[0]+=v3f[0]*nextHeight;
			v[1]+=v3f[1]*nextHeight;
			v[2]+=v3f[2]*nextHeight;
			myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Position"+kBasics::leadingZero(i,6), &v, omINFOTYPE_VEC3, true, false);*/


			//Berechnung der Debugkamerapositionen mit Höhenangaben
			//Eine Höhe = BBox-Höhe liegt auf der Kugeloberfläche
			if (nextPosition)
			{
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Position"+kBasics::leadingZero(i,6), new vec3((*nextPosition)[0],(*nextPosition)[1],(*nextPosition)[2]), omINFOTYPE_VEC3, true, false);
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Height"+kBasics::leadingZero(i,6), new double(nextHeight), omINFOTYPE_DOUBLE, true, false);
				//myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Color",new vec3(1,1,1),omIn omINFOTYPE_VEC3,true,false);
				//Calc Orientation
				kCamera tempCam;
				tempCam.setCamPosition(*nextPosition,*nextLookAt);
				SbVec3f axis; float x,y,z,radians;				
				tempCam.getOrientation().getValue(axis,radians);
				axis.getValue(x,y,z);
				vec4* tempVec4 = new vec4((double)x,(double)y,(double)z,(double)radians);
				myObjMgr->setObjAttribute(O_ANIMATION, "DebugCameraPath", "Orientation"+kBasics::leadingZero(i,6), tempVec4, omINFOTYPE_VEC4, true, false);
				delete tempVec4;
			}
		}
	}
	else kDebug::Debug("Frame entry which should be there isnt :-(",kDebug::DL_HIGH);
}

void AbstractCampath::writeOrientation(SbRotation orientation)
{
	ML_TRACE_IN("AbstractCampath::writeOrientation()");
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
void AbstractCampath::writePosition(SbVec3f position)
{
	ML_TRACE_IN("AbstractCampath::writePosition()");
	vec3* tempVec3 = new vec3(position[0],position[1],position[2]);
	myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_CAMERA,INF_VIEWER_CAMERA_POSITION,tempVec3,omINFOTYPE_VEC3,true,true);
	delete tempVec3;
}

ML_END_NAMESPACE
