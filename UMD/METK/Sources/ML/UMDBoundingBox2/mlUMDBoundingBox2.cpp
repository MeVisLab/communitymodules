//----------------------------------------------------------------------------------
//! The ML module class UMDBoundingBox2.
/*!
// \file    mlUMDBoundingBox2.cpp
// \author  Konrad Mühler
// \date    2005-05-10
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlUMDBoundingBox2_H
#include "mlUMDBoundingBox2.h"
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(UMDBoundingBox2,ObjMgrClient);


UMDBoundingBox2::UMDBoundingBox2 (void) : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	handleNotificationOff();

	//doItField = getFieldContainer()->addNotify("doIt");

	bb_BoundingBoxField = getFieldContainer()->addString("bb_BoundingBox");
	bb_ObjectMinField = getFieldContainer()->addDouble("bb_ObjectMin");
	bb_ObjectMaxField = getFieldContainer()->addDouble("bb_ObjectMax");
	bb_UpToDateField = getFieldContainer()->addBool("bb_UpToDate");
	bb_UpdateField = getFieldContainer()->addNotify("bb_Update");

	addAcceptedInfoLayerName("Image");
	addAcceptedInfoLayerName("Registration");

	addAcceptedObjectID("*");

	waitFlag = false;
	waitObjectID = "";

	timerUpdate = new SoTimerSensor((SoSensorCB*)UMDBoundingBox2::timerEventUpdate, this);
    timerUpdate->setInterval(SbTime(1.0/200.0));
    timerUpdate->unschedule();


	handleNotificationOn(); 

}


void UMDBoundingBox2::handleNotification (Field *field)
{
	inherited::handleNotification(field);

	//Debug("xyz",field->getName());

	if (field == bb_UpToDateField)
	{		
		//SoDebugError::postInfo("XXXXX-BB: UpToDateField","");
		if (waitFlag && bb_UpToDateField->getBoolValue())
		{
			//SoDebugError::postInfo(("BB - UpToDateField with waitFlag   objectID:"+waitObjectID+"  BB-Field:"+bb_BoundingBoxField->getStringValue()).c_str(),"");
			waitFlag = false;

			std::vector<std::string> BBOrgSplit;
			kBasics::split(bb_BoundingBoxField->getStringValue(),' ',12,&BBOrgSplit);

			if (BBOrgSplit.size()!=12)
			{
				//Debug("WrongSplitSize in BoundingBoxField:",kBasics::IntToString(BBOrgSplit.size()));
				return;
			}


			//for (int i=0; i<12; i++)
			//{
				//Debug(kBasics::IntToString(i)," BBOrgSplit -> " + BBOrgSplit[i]);
			//}

			SbVec3f BB_XYZ1_org,BB_XYZ2_org,BB_CTU1_org,BB_CTU2_org;
			SbVec3f BB_XYZ1,BB_XYZ2,BB_CTU1,BB_CTU2;
						
			BB_XYZ1_org.setValue(kBasics::StringToInt(BBOrgSplit[0]),kBasics::StringToInt(BBOrgSplit[1]),kBasics::StringToInt(BBOrgSplit[2]));
			BB_CTU1_org.setValue(kBasics::StringToInt(BBOrgSplit[3]),kBasics::StringToInt(BBOrgSplit[4]),kBasics::StringToInt(BBOrgSplit[5]));
			BB_XYZ2_org.setValue(kBasics::StringToInt(BBOrgSplit[6]),kBasics::StringToInt(BBOrgSplit[7]),kBasics::StringToInt(BBOrgSplit[8]));
			BB_CTU2_org.setValue(kBasics::StringToInt(BBOrgSplit[9]),kBasics::StringToInt(BBOrgSplit[10]),kBasics::StringToInt(BBOrgSplit[11]));

			// Get writable access to object container
			omObjectContainer* oc = getObjContainer();
			if(oc == NULL)
			{		
				//Debug("ObjContainer","not found!");
				return;						
			} 
			omObject& obj = (*oc)[waitObjectID];


			const omAttribute& attrToWorldRegistrated = obj.getAttribute("Registration","ToWorldRegistrated");
			string tempToWorldRegistrated = attrToWorldRegistrated.getStringValue();

			tempToWorldRegistrated = kBasics::replaceString(tempToWorldRegistrated, char(10), " ");
			tempToWorldRegistrated = kBasics::replaceString(tempToWorldRegistrated, char(13), " ");
			tempToWorldRegistrated = kBasics::replaceString(tempToWorldRegistrated, "  ", " ");

			//Debug ("tempToWorldRegistrated",tempToWorldRegistrated);

			vector<string> WorldMatrixOrgSplit;
			kBasics::split(tempToWorldRegistrated,' ',16,&WorldMatrixOrgSplit);

			if (WorldMatrixOrgSplit.size()!=16)
			{
				//Debug("WrongSplitSize in ToWorldRegistrated:",kBasics::IntToString(WorldMatrixOrgSplit.size()));
				return;
			}


			SbMatrix* worldMatrix = new SbMatrix(atof(WorldMatrixOrgSplit[0].c_str()),atof(WorldMatrixOrgSplit[1].c_str()),atof(WorldMatrixOrgSplit[2].c_str()),atof(WorldMatrixOrgSplit[3].c_str()),
												 atof(WorldMatrixOrgSplit[4].c_str()),atof(WorldMatrixOrgSplit[5].c_str()),atof(WorldMatrixOrgSplit[6].c_str()),atof(WorldMatrixOrgSplit[7].c_str()),
												 atof(WorldMatrixOrgSplit[8].c_str()),atof(WorldMatrixOrgSplit[9].c_str()),atof(WorldMatrixOrgSplit[10].c_str()),atof(WorldMatrixOrgSplit[11].c_str()),
												 atof(WorldMatrixOrgSplit[12].c_str()),atof(WorldMatrixOrgSplit[13].c_str()),atof(WorldMatrixOrgSplit[14].c_str()),atof(WorldMatrixOrgSplit[15].c_str()));
					
			worldMatrix->multMatrixVec(BB_XYZ1_org,BB_XYZ1);
			worldMatrix->multMatrixVec(BB_CTU1_org,BB_CTU1);
			worldMatrix->multMatrixVec(BB_XYZ2_org,BB_XYZ2);
			worldMatrix->multMatrixVec(BB_CTU2_org,BB_CTU2);
			
			//obj["Measurements"]["WorldBoundingBox"] = bb_BoundingBoxField->getStringValue();
			string tempStr = kBasics::IntToString(BB_XYZ1[0])+" "+kBasics::IntToString(BB_XYZ1[1])+" "+kBasics::IntToString(BB_XYZ1[2])+" "+
							 kBasics::IntToString(BB_CTU1[0])+" "+kBasics::IntToString(BB_CTU1[1])+" "+kBasics::IntToString(BB_CTU1[2])+" "+
							 kBasics::IntToString(BB_XYZ2[0])+" "+kBasics::IntToString(BB_XYZ2[1])+" "+kBasics::IntToString(BB_XYZ2[2])+" "+
							 kBasics::IntToString(BB_CTU2[0])+" "+kBasics::IntToString(BB_CTU2[1])+" "+kBasics::IntToString(BB_CTU2[2]);
			
			//=create
			obj["Measurements"]["WorldBoundingBox"] = tempStr;

			//Debug("tempStr:",tempStr);


			//Debug("sbb-1","");
			sendNotification();
			//Debug("sbb-2","");
		}

	}


}


void UMDBoundingBox2::handleObjMgrNotification()
{
	const omEventContainer& myEventList = getEventContainer();
		
	omEventContainer::const_iterator iter;
	ObjMgrEvent myEvent;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		//SoDebugError::postInfo("Pong3","");
		//ObjMgrEvent myEvent = (*iter);
		myEvent = (ObjMgrEvent)(*iter);
		//Debug("Pong4","");		
		
		//Debug("myEvent.infoLayer",myEvent.infoLayer);
		//Debug("myEvent.infoID",myEvent.infoID);
		//Debug("kBasics::IntToString(myEvent.type)",kBasics::IntToString(myEvent.type));
		//Debug("myEvent.prevValue",myEvent.prevValue);
		
		//Debug("BB-Event",myEvent.infoLayer+"->"+myEvent.infoID+" Type:" + kBasics::IntToString(myEvent.type)+ "  PreValue: " + myEvent.prevValue);

		
		//Debug("BB-Event",myEvent.infoLayer+"->"+myEvent.infoID+" Type:" + kBasics::IntToString(myEvent.type)+ "  PreValue: " + myEvent.prevValue);

		if ((myEvent.infoID=="ToWorldRegistrated" || myEvent.infoID=="ObjectValue") && (myEvent.layerID=="Registration" || myEvent.layerID=="Image")) //evtl. die oder/und sachen praktischer logischer umstellen toDo
		{
			//SoDebugError::postInfo("IN-1","");
			// Get writable access to object container
			omObjectContainer* oc = getObjContainer();
			if(oc == NULL)
			{		
				mlDebug("ObjContainer not found!");
				return;						
			} 
			omObject& obj = (*oc)[myEvent.objectID];
			
			const omAttribute& attrToWorldRegistrated = obj.getAttribute("Registration","ToWorldRegistrated");
			const omAttribute& attrObjectValue = obj.getAttribute("Image","ObjectValue");
			
			//SoDebugError::postInfo("attrToWorldRegistrated.isValid()  ",kBasics::BoolToString(attrToWorldRegistrated.isValid()).c_str());
			//SoDebugError::postInfo("attrObjectValue.isValid()  ",kBasics::BoolToString(attrObjectValue.isValid()).c_str());
			//SoDebugError::postInfo("waitFlag  ",kBasics::BoolToString(waitFlag).c_str());
			if (attrToWorldRegistrated.isValid() && attrObjectValue.isValid() && !waitFlag)
			{		
				//SoDebugError::postInfo("ATTR valid","");
				//Debug("waitFlag objID:"+myEvent.objectID,"WR:*"+attrToWorldRegistrated.getStringValue()+"*   OV:*"+attrObjectValue.getStringValue()+"*");
				waitFlag = true;
				waitObjectID = myEvent.objectID;
			
				bb_ObjectMinField->setDoubleValue(atof(attrObjectValue.getStringValue().c_str()));
				bb_ObjectMaxField->setDoubleValue(atof(attrObjectValue.getStringValue().c_str()));				
			}			
		}
	}

	clearEventContainer();

	//if (waitFlag) bb_UpdateField->notify();
	if (waitFlag) timerUpdate->schedule();
}



//-------------------------------------------------------------------------------
//! 
//-------------------------------------------------------------------------------
void UMDBoundingBox2::timerEventUpdate(void* data, SoDataSensor* a)
{	
    ((UMDBoundingBox2*)data)->sendNotifyUpdate(a);
}


//-------------------------------------------------------------------------------
//! 
//-------------------------------------------------------------------------------
void UMDBoundingBox2::sendNotifyUpdate(SoDataSensor* sensor)
{
	timerUpdate->unschedule();	
	bb_UpdateField->notify();	
}


ML_END_NAMESPACE

