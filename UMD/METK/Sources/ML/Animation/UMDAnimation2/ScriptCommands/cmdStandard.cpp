#include "../AnimationExecuter.h"

ML_START_NAMESPACE

// Introduce a constant to avoid implicit constructor call in comparison which is not allowed under some circumstances in ISO c++.
omIDType defaultOMID = omID_DEFAULT;

void AnimationExecuter::commandLayer(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	if ((currentEntry->getObjectID() != defaultOMID) && (currentEntry->getObjectID()!=""))
	{
		if (currentEntry->getParamType() == kScriptFrameEntry::PARAMT_FLOAT) //INT wird als Float behandelt ...
			setDoubleValueInterpol(currentEntry->getObjectID(),currentEntry->getLayerID(),currentEntry->getInfoID(),(*tempEntryParams)[0],currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
		else if (currentEntry->getParamType() == kScriptFrameEntry::PARAMT_V3)
			setV3ValueInterpol(currentEntry->getObjectID(), currentEntry->getLayerID(),currentEntry->getInfoID(),(*tempEntryParams)[0],currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
		else if (currentEntry->getParamType() == kScriptFrameEntry::PARAMT_STRING)
			myObjMgr->setObjAttribute(currentEntry->getObjectID(), currentEntry->getLayerID(),currentEntry->getInfoID(),(void*)&((*tempEntryParams)[0]),omINFOTYPE_STRING,true,true);
		else if (currentEntry->getParamType() == kScriptFrameEntry::PARAMT_BOOL)
			if (kBasics::toUp((*tempEntryParams)[0]) == "TRUE")
				myObjMgr->setObjAttribute(currentEntry->getObjectID(), currentEntry->getLayerID(),currentEntry->getInfoID(),new bool(true),omINFOTYPE_BOOL,true,true);
			else
				myObjMgr->setObjAttribute(currentEntry->getObjectID(), currentEntry->getLayerID(),currentEntry->getInfoID(),new bool(false),omINFOTYPE_BOOL,true,true);
	}
	else
	{
		kDebug::Debug("Object not found " + currentEntry->getObjectName()+"  --  "+currentEntry->getObjectID(),kDebug::DL_LOW);
	}

}



void AnimationExecuter::commandCreateVideo(vector<string>* tempEntryParams/*, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()>0) globalCreateVideo = ((string)(*tempEntryParams)[0]) == "TRUE";
}



void AnimationExecuter::commandBackground(vector<string>* tempEntryParams/*, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter*/)
{
	if (tempEntryParams->size()>=1)
	{
		vector<string>* splittedTarget = new vector<string>;
		kBasics::split((string)(*tempEntryParams)[0],kScriptFrameEntry::DELIMITER_COLOR_SCRIPT,3,splittedTarget);

		if (splittedTarget->size()==3)
		{	
			vec3* tempVec3 = new vec3(kBasics::StringToDouble((*splittedTarget)[0])/255.0,kBasics::StringToDouble((*splittedTarget)[1])/255.0,kBasics::StringToDouble((*splittedTarget)[2])/255.0);
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGCOLOR, tempVec3,omINFOTYPE_VEC3,true,true);
			delete tempVec3;
		}

		if (tempEntryParams->size()>=2)
		{							
			if (((string)(*tempEntryParams)[1])=="RAMP")
			{
				myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYWIDTH,new double(1.0),omINFOTYPE_DOUBLE,true,true);
				myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYCENTER,new double(0.5),omINFOTYPE_DOUBLE,true,true);
			}
			else
			{
				myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYWIDTH,new double(0),omINFOTYPE_DOUBLE,true,true);
				myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYCENTER,new double(0),omINFOTYPE_DOUBLE,true,true);
			}
		}
		else
		{
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYWIDTH,new double(0),omINFOTYPE_DOUBLE,true,true);
			myObjMgr->setObjAttribute(fldAnimationViewerName->getStringValue(),LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_BGGREYCENTER,new double(0),omINFOTYPE_DOUBLE,true,true);
		}

		delete splittedTarget;
	}
}



void AnimationExecuter::commandColor(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	string objID = currentEntry->getObjectID();
	string layerID = LAY_APPEARANCE;
	string infoID = INF_COLOR;					
	interpolColor(objID, layerID, infoID,(*tempEntryParams)[0], currentEntry);
}



void AnimationExecuter::commandSilhouetteColor(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	string objID = currentEntry->getObjectID();
	string layerID = LAY_APPEARANCE;
	string infoID = INF_SILHOUETTECOLOR;
	interpolColor(objID, layerID, infoID,(*tempEntryParams)[0],currentEntry);
}





void AnimationExecuter::commandSetType(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry/*, int frameNr, size_t FrameEntryListIter*/)
{
	//Parameter: ObjID LayerID InfoID Value
	//Quotationmarks can be ' or " or nothing
	//std::cout << "SET COMMAND  " << currentEntry->getParamString() << std::endl;
	if (tempEntryParams->size()==4)
	{
		string objID = kBasics::trimQuotatedStr(kBasics::trimQuotatedStr((*tempEntryParams)[0],kBasics::QUOTATION_SINGLE),kBasics::QUOTATION_DOUBLE);
		string layerID = kBasics::trimQuotatedStr(kBasics::trimQuotatedStr((*tempEntryParams)[1],kBasics::QUOTATION_SINGLE),kBasics::QUOTATION_DOUBLE);
		string infoID = kBasics::trimQuotatedStr(kBasics::trimQuotatedStr((*tempEntryParams)[2],kBasics::QUOTATION_SINGLE),kBasics::QUOTATION_DOUBLE);
		string valueStr = kBasics::trimQuotatedStr(kBasics::trimQuotatedStr((*tempEntryParams)[3],kBasics::QUOTATION_SINGLE),kBasics::QUOTATION_DOUBLE);

		if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETDOUBLE)
		{
			setDoubleValueInterpol(objID,layerID,infoID,valueStr,currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
		}
		else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETSTRING)
		{
			myObjMgr->setObjAttribute(objID,layerID,infoID,(void*)&(valueStr),omINFOTYPE_STRING,true,true);
		}
		else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETVEC3)
		{
			setV3ValueInterpol(objID,layerID,infoID,valueStr,currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
		}
		else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETBOOL)
		{
			if (kBasics::toUp(valueStr) == "TRUE" || valueStr =="1" )
				myObjMgr->setObjAttribute(objID,layerID,infoID,new bool(true),omINFOTYPE_BOOL,true,true);
			else
				myObjMgr->setObjAttribute(objID,layerID,infoID,new bool(false),omINFOTYPE_BOOL,true,true);
		}
		else if (currentEntry->getCommandID() == kScriptFrameEntry::CMD_SETINT)
		{
			setIntegerValueInterpol(objID,layerID,infoID,valueStr,currentEntry->getCurrentFrame(), currentEntry->getFrameCount());
		}

	}
	else
	{
		std::cout << "Parameter count in setFloat, setString, setVec3 or setBool != 4" << std::endl;
	}
}



/** Interpoliert einen Double-Wert anhand des aktuellen Frames und schreibt ihn in den ObjMgr
\param  objID 
\param  layerID 
\param  infoID 
\param  targetValue 
\param  curFrame 
\param  frameCount 
\return void
*/
void AnimationExecuter::setDoubleValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount)
{	
	ML_TRACE_IN("AnimationExecuter::setDoubleValueInterpol()");
	string oldValue = "";
	string tempAttr;
	if (myObjMgr->getObjAttributeString(objID, layerID, infoID, tempAttr)) oldValue = tempAttr;		
	if (oldValue=="") oldValue = "0";	
	double newValue = atof(oldValue.c_str()) + ( atof(targetValue.c_str()) - atof(oldValue.c_str()) ) / ( frameCount - curFrame + 1.0 );
	myObjMgr->setObjAttribute(objID, layerID, infoID, new double(newValue),omINFOTYPE_DOUBLE,true,true);
}


/** Interpoliert einen Integer-Wert anhand des aktuellen Frames und schreibt ihn in den ObjMgr
\param  objID 
\param  layerID 
\param  infoID 
\param  targetValue 
\param  curFrame 
\param  frameCount 
\return void
*/
void AnimationExecuter::setIntegerValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount)
{	
	ML_TRACE_IN("AnimationExecuter::setIntegerValueInterpol()");
	string oldValue = "";
	string tempAttr;
	if (myObjMgr->getObjAttributeString(objID, layerID, infoID, tempAttr)) oldValue = tempAttr;		
	if (oldValue=="") oldValue = "0";	
	int newValue = atoi(oldValue.c_str()) + ( atoi(targetValue.c_str()) - atoi(oldValue.c_str()) ) / ( frameCount - curFrame + 1.0 );
	myObjMgr->setObjAttribute(objID, layerID, infoID, new int(newValue),omINFOTYPE_INT32,true,true);
}



/** Interpoliert einen V3-Wert anhand des aktuellen Frames und schreibt ihn in den ObjMgr
\param  objID 
\param  layerID 
\param  infoID 
\param  targetValue 
\param  curFrame 
\param  frameCount 
\return void
*/
void AnimationExecuter::setV3ValueInterpol(const string objID, const string layerID, const string infoID, const string targetValue, const int curFrame, const int frameCount)
{	
	ML_TRACE_IN("AnimationExecuter::setV3ValueInterpol()");
	string oldValue = "";	
	string tempAttr;
	if (myObjMgr->getObjAttributeString(objID, layerID, infoID, tempAttr)) oldValue = tempAttr;
	vector<string>* splittedOld2 = new vector<string>; //wär das kein Pointer, würde er zu 80% abstürzen
	vector<string>* splittedTarget2 = new vector<string>;		
	if (oldValue!="")		
		kBasics::split(oldValue,kScriptFrameEntry::DELIMITER_COLOR_LAYER,3,splittedOld2);
	else
		kBasics::split(targetValue,kScriptFrameEntry::DELIMITER_COLOR_SCRIPT,3,splittedOld2);
	kBasics::split(targetValue,kScriptFrameEntry::DELIMITER_COLOR_SCRIPT,3,splittedTarget2);

	if (splittedTarget2->size()==3 && splittedOld2->size()==3)
	{
		double colorDevide = 1;
		if (infoID==INF_COLOR) colorDevide = 255.0; //Holy shit ... this is a very bad hack :-(
		double newValueC1 = atof((*splittedOld2)[0].c_str()) + ( atof((*splittedTarget2)[0].c_str())/colorDevide - atof((*splittedOld2)[0].c_str()) ) / ( frameCount - curFrame + 1.0 );
		double newValueC2 = atof((*splittedOld2)[1].c_str()) + ( atof((*splittedTarget2)[1].c_str())/colorDevide - atof((*splittedOld2)[1].c_str()) ) / ( frameCount - curFrame + 1.0 );
		double newValueC3 = atof((*splittedOld2)[2].c_str()) + ( atof((*splittedTarget2)[2].c_str())/colorDevide - atof((*splittedOld2)[2].c_str()) ) / ( frameCount - curFrame + 1.0 );		
		myObjMgr->setObjAttribute(objID, layerID, infoID, new vec3(newValueC1,newValueC2,newValueC3),omINFOTYPE_VEC3,true,true);
	}

	delete splittedOld2; delete splittedTarget2;
}



/** Interpoliert einen RGB-Farbwert (255er Werte) im HSV-Raum
\param  objID 
\param  layerID 
\param  infoID 
\param  param255 
\param  currentEntry 
\return void
*/ 
void AnimationExecuter::interpolColor(string objID, string layerID, string infoID, string param255, kScriptFrameEntry* currentEntry)
{
	ML_TRACE_IN("AnimationExecuter::interpolColor()");
	string oldValueRGB = "";	
	string tempAttr;
	if (myObjMgr->getObjAttributeString(objID, layerID, infoID, tempAttr)) oldValueRGB = tempAttr;

	vector<string>* splittedOldRGB = new vector<string>; //wär das kein Pointer, würde er zu 80% abstürzen
	vector<string>* splittedTargetRGB = new vector<string>;	

	float colorDevideOld = 1;
	if (oldValueRGB!="")		
		kBasics::split(oldValueRGB,kScriptFrameEntry::DELIMITER_COLOR_LAYER,3,splittedOldRGB);
	else
	{	
		colorDevideOld = 255;
		kBasics::split(param255,kScriptFrameEntry::DELIMITER_COLOR_SCRIPT,3,splittedOldRGB);
	}

	float colorDevideTarget = 255;
	kBasics::split(param255,kScriptFrameEntry::DELIMITER_COLOR_SCRIPT,3,splittedTargetRGB);

	if (splittedTargetRGB->size()==3 && splittedOldRGB->size()==3)
	{		
		SbVec3f oldHSV;
		SbVec3f targetHSV;
		oldHSV = kBasics::RGB_to_HSV(SbVec3f(kBasics::StringToFloat((*splittedOldRGB)[0])/colorDevideOld,
			kBasics::StringToFloat((*splittedOldRGB)[1])/colorDevideOld,
			kBasics::StringToFloat((*splittedOldRGB)[2])/colorDevideOld));
		targetHSV = kBasics::RGB_to_HSV(SbVec3f(kBasics::StringToFloat((*splittedTargetRGB)[0])/colorDevideTarget,
			kBasics::StringToFloat((*splittedTargetRGB)[1])/colorDevideTarget,
			kBasics::StringToFloat((*splittedTargetRGB)[2])/colorDevideTarget));

		float newValueC1 = oldHSV[0] + ( targetHSV[0] - oldHSV[0] ) / ( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
		float newValueC2 = oldHSV[1] + ( targetHSV[1] - oldHSV[1] ) / ( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );
		float newValueC3 = oldHSV[2] + ( targetHSV[2] - oldHSV[2] ) / ( currentEntry->getFrameCount() - currentEntry->getCurrentFrame() + 1.0 );		

		SbVec3f newRGB = kBasics::HSV_to_RGB(SbVec3f(newValueC1,newValueC2,newValueC3));			
		myObjMgr->setObjAttribute(objID, layerID, infoID, new vec3(newRGB[0],newRGB[1],newRGB[2]),omINFOTYPE_VEC3,true,true);
	}

	delete splittedOldRGB; delete splittedTargetRGB;
}

ML_END_NAMESPACE