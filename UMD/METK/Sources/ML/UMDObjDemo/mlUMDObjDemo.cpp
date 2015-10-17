//----------------------------------------------------------------------------------
//! The ML module class UMDObjDemo.
/*!
// \file    mlUMDObjDemo.cpp
// \author  Konrad Mühler
// \date    2005-04-22
//
// Example for a single ObjModule
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlUMDObjDemo_H
#include "mlUMDObjDemo.h"
#endif



ML_START_NAMESPACE


// This macro implements some functions and methods for the runtime system and
// for the initialization of this class.
ML_CLASS_SOURCE(UMDObjDemo,ObjMgrClient);


//----------------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------------
UMDObjDemo::UMDObjDemo () : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	addAcceptedInfoLayerName("*");    
	addAcceptedObjectID("*");

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}


//-------------------------------------------------------------------------------
// Called on notification from ObjMgr.
//-------------------------------------------------------------------------------
void UMDObjDemo::handleObjMgrNotification()
{
	omEventContainer myEventList = getEventContainer();

	mlDebug("ping");
	
	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;
	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);		
		
		if (myEvent.layerID!="Test")
		{			
			// Get writable access to object container
			omObjectContainer* oc = getObjContainer();
			if(oc == NULL)
			{		
				mlDebug("ObjContainer not found!");
			}
			else
			{

				omObject& obj = (*oc)[myEvent.objectID];	

				if (!obj.isValid())
				{				
					mlDebug("invalidObject");
				}
				else
				{

					const omAttribute& attr1 = obj.getAttribute("Test","EventCount");
			
					if (attr1.isValid())
					{			
						//Erhöhung der EventCount des jeweiligen Objektes um 1
						std::stringstream sstr;
						sstr << atoi(((std::string)obj["Test"]["EventCount"]).c_str())+1;				
						obj["Test"]["EventCount"] = sstr.str();
						sendNotification();
					}
					else
					{
						obj["Test"]["EventCount"] = "0";
						sendNotification();
					}
				}
			}
		}
	}

	clearEventContainer();	
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void UMDObjDemo::handleNotification (Field *field)
{
	inherited::handleNotification(field);
}



ML_END_NAMESPACE

