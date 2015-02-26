#include "../AnimationExecuter.h"

ML_START_NAMESPACE

void AnimationExecuter::commandClip(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int /*frameNr*/, size_t /*FrameEntryListIter*/)
{
	//Params:					
	//on/off left/right/... percent toClip (notToClip) (aab/oab)
	if (tempEntryParams->size()>=4)
	{	
		//Parameter 1: on/off
		bool onOff = false;
		onOff=((*tempEntryParams)[0]=="ON");

		//Parameter 2: left/right/...
		//noch nicht
		string clipPlane;
		if ((*tempEntryParams)[1]=="LEFT")
			clipPlane = LAY_CLIPPINGLEFT;
		else if ((*tempEntryParams)[1]=="RIGHT")
			clipPlane = LAY_CLIPPINGRIGHT;
		else if ((*tempEntryParams)[1]=="TOP")
			clipPlane = LAY_CLIPPINGTOP;
		else if ((*tempEntryParams)[1]=="BOTTOM")
			clipPlane = LAY_CLIPPINGBOTTOM;
		else if ((*tempEntryParams)[1]=="REAR")
			clipPlane = LAY_CLIPPINGREAR;
		else clipPlane = LAY_CLIPPINGFRONT; //Front is default																			

		myObjMgr->setObjAttribute(O_CLIPPING,clipPlane,INF_CLIPPING_ON,new string((*tempEntryParams)[0]),omINFOTYPE_STRING,true,true);

		//Parameter 5: listNotToClip						
		vector<string>* listNotToClip = new vector<string>;
		if (tempEntryParams->size()>=5)
			myAnimationParser->getObjectStringList(kBasics::trimQuotatedStr((string)(*tempEntryParams)[4], kBasics::QUOTATION_SINGLE).c_str(),listNotToClip);

		//Parameter 4: listToClip, listForBB						
		HashTable<string>* listToClip = new HashTable<string>;						
		myAnimationParser->getObjectStringList(kBasics::trimQuotatedStr((string)(*tempEntryParams)[3], kBasics::QUOTATION_SINGLE).c_str(),listToClip);

		for ( size_t i = 0; i<listNotToClip->size(); i++)
			listToClip->remove((*listNotToClip)[i]);

		//Parameter 6: aab/oab

		//Transform ClippingBox
		//Get BoundingBox for objects to clip

		AnimationCache::Measures MasterBB = myCache->getMeasuresFromHT((string)(*tempEntryParams)[3]);
		SbVec3f BBCenter = myCache->getBoundingCenter(MasterBB);

		//Translation
		myObjMgr->setObjAttribute(O_CLIPPING,L_GLOBAL,INF_CLIPPING_TRANSLATION,new vec3(BBCenter[0],BBCenter[1],BBCenter[2]),omINFOTYPE_VEC3,true,true);

		//Scale
		//Scale nimmt sowieso nachher bei der ClipBox den größten der 3 werte
		SbVec3f tempScale;
		tempScale[0] = std::abs(MasterBB.BB_max[0]-MasterBB.BB_min[0])/2;
		tempScale[1] = std::abs(MasterBB.BB_max[1]-MasterBB.BB_min[1])/2;
		tempScale[2] = std::abs(MasterBB.BB_max[2]-MasterBB.BB_min[2])/2;
		myObjMgr->setObjAttribute(O_CLIPPING,L_GLOBAL,INF_CLIPPING_SCALE,new vec3(tempScale[0],tempScale[1],tempScale[2]),omINFOTYPE_VEC3,true,true);

		//Parameter 3: percent
		//left 
		//x +1 clip nothing
		//x -1 clip all
		string vecString;
		if (clipPlane == LAY_CLIPPINGLEFT)
			vecString = kBasics::FloatToString(1-2*kBasics::StringToFloat((string)(*tempEntryParams)[2]))+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0");
		else if (clipPlane == LAY_CLIPPINGRIGHT)
			vecString = kBasics::FloatToString(-1+2*kBasics::StringToFloat((string)(*tempEntryParams)[2]))+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0");
		else if (clipPlane == LAY_CLIPPINGTOP)
			vecString = string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+kBasics::FloatToString(1-2*kBasics::StringToFloat((string)(*tempEntryParams)[2]));
		else if (clipPlane == LAY_CLIPPINGBOTTOM)
			vecString = string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+kBasics::FloatToString(-1+2*kBasics::StringToFloat((string)(*tempEntryParams)[2]));
		else if (clipPlane == LAY_CLIPPINGREAR)							
			vecString = string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+kBasics::FloatToString(1-2*kBasics::StringToFloat((string)(*tempEntryParams)[2]))+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0");
		else
			vecString = string("0")+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+kBasics::FloatToString(-1+2*kBasics::StringToFloat((string)(*tempEntryParams)[2]))+kScriptFrameEntry::DELIMITER_COLOR_SCRIPT+string("0");

		setV3ValueInterpol(O_CLIPPING, clipPlane,INF_CLIPPING_TRANSLATION, vecString, currentEntry->getCurrentFrame(), currentEntry->getFrameCount());

		//Nur beim ersten Frame einer setClipPlane-Anweisung soll er alle Clipping-Eigenschaften zurücksetzen.
		//Testen mit Anweisungen a la [1,10]
		if (currentEntry->getCurrentFrame()==0)
		{
			kDebug::Debug("Reset all Clipping to false",kDebug::DL_HIGH);
			//Get read-only access to object container
			const omObjectContainer *oc= getConstObjContainer();

			oc = getObjContainer();
			if(oc != NULL)
			{		
				omObjectContainer ::const_iterator iterObj;
				//definition in mlObjMgrObjectContainer.h: typedef std::map<omIDType, omObject> objVec;

				for ( iterObj = oc->begin(); iterObj!=oc->end(); iterObj++)
				{
					omObject tempObj = iterObj->second;
					//Als Typ für das Feld BOOL zu nehmen scheiterte daran, das nicht existente Felder bei z.B. viewer-Objekten auch TRUE zurückgeben wenn man nach dem Attribut fragt

					omAttribute attr2 = tempObj.getAttribute(LAY_DESCRIPTION,INF_NAME);
					if (attr2.getStringValue()!="")
					{
						if ((listToClip->find(kBasics::toUp(attr2.getStringValue()))!=NULL))
						{
							myObjMgr->setObjAttribute(tempObj.getID(),LAY_APPEARANCE,INF_CLIPPING,new string("ON"),omINFOTYPE_STRING,true,true);
							sendNotification();
						}
						else
						{
							if (tempObj.hasAttribute(LAY_APPEARANCE,INF_CLIPPING))
							{
								omAttribute attr1 = tempObj.getAttribute(LAY_APPEARANCE,INF_CLIPPING);
								if (kBasics::toUp(attr1.getStringValue()) == "ON")
								{			
									myObjMgr->setObjAttribute(tempObj.getID(),LAY_APPEARANCE,INF_CLIPPING,new string("OFF"),omINFOTYPE_STRING,true,true);
									sendNotification();
								}											
							}
						}
					}
				}
			}
		}

		delete listNotToClip; delete listToClip;
		sendNotification();
	}
	else kDebug::Debug("setClipPlane -> to few arguments",kDebug::DL_HIGH);					
}


ML_END_NAMESPACE