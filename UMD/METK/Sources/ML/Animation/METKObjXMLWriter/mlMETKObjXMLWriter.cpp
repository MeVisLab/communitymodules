//----------------------------------------------------------------------------------
//! The ML module class METKObjXMLWriter.
/*!
// \file    mlMETKObjXMLWriter.cpp
// \author  Konrad Mühler
// \date    2006-12-12
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlMETKObjXMLWriter_H
#include "mlMETKObjXMLWriter.h"
#endif

//#include "../METK.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKObjXMLWriter, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
METKObjXMLWriter::METKObjXMLWriter () : inherited(0,0,ObjMgrClient::EVENTS_SELECTED)
{
	ML_TRACE_IN("METKObjXMLWriter::METKObjXMLWriter()")

	myObjMgr = new ObjMgrCommunicator();

	FieldContainer *fields = getFieldContainer();
	
	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_run = fields->addNotify("run");
	(_directory = fields->addString("directory"))->setStringValue("");
	(_filename = fields->addString("filename"))->setStringValue("");
	(_useCasePath = fields->addBool("useCasePath"))->setBoolValue(true);	
	(_objXML = fields->addString("objXML"))->setStringValue("");

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}

METKObjXMLWriter::~METKObjXMLWriter()
{
	ML_TRACE_IN("METKObjXMLWriter::~METKObjXMLWriter()");   
}

void METKObjXMLWriter::activateAttachments()
{ 
	clearAcceptedObjectIDs();
	addAcceptedObjectID(OBJ_CASE);
	addAcceptedObjectID(OBJ_COMMUNICATION);
	clearAcceptedInfoLayerNames();
	addAcceptedInfoLayerName(LAY_CASE);
	addAcceptedInfoLayerName(LAY_GLOBALEVENTS);

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}



//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKObjXMLWriter::handleNotification (Field *field)
{
	ML_TRACE_IN("METKObjXMLWriter::handleNotification()")

	inherited::handleNotification(field);

	if (field == getFieldContainer()->getField("inObjectContainer"))
	{
		if (getFieldContainer()->getField("inObjectContainer")->getDestinationField (0) == NULL)
		{
			//Verbinden aller inObjectContainer innerhalb des Moduls!!!
			//Diese Zeile hat mich fast 2 Tage und sehr viele Nerven gekostet ;-) (aus METKObjContainer geklaut)			
			getFieldContainer()->getField("inObjectContainer")->attachField(myObjMgr->getFieldContainer()->getField("inObjectContainer"),1);
		}	
	}
   
	else if (field == _run)
	{
		//std::cout << "_run" << std::endl;
		createXML();		
	}

	else if (field == _useCasePath)
	{
		if (_useCasePath->getBoolValue() && getFieldContainer()->getField("inObjectContainer")->getDestinationField (0))
		{
			string tempAttr;
			if (myObjMgr->getObjAttributeString(OBJ_CASE, LAY_CASE, INF_CASEDIR, tempAttr))
			{
				_directory->setStringValue(tempAttr);
				_filename->setStringValue("ObjectNames.xml");
			}
		}
	}
 
}




void METKObjXMLWriter::handleObjMgrNotification()
{
	ML_TRACE_IN("METKObjXMLWriter::handleObjMgrNotification()");
	omEventContainer myEventList = getEventContainer();
      	  
	//Durchiterieren der EventList
	omEventContainer::const_iterator iter;

	for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
	{
		ObjMgrEvent myEvent = (*iter);

		if (myEvent.layerID == LAY_CASE && myEvent.infoID == INF_CASEDIR && _useCasePath->getBoolValue())
		{	
            _directory->setStringValue(myEvent.newValue.getStringValue());
            _filename->setStringValue("ObjectNames.xml");
		}
		else if (myEvent.layerID == LAY_GLOBALEVENTS && myEvent.infoID == INF_CASELOADED && myEvent.newValue.getStringValue() == "Loaded")
		{
			createXML();
		}
	}
	clearEventContainer();
}


void METKObjXMLWriter::createXML()
{
	//std::cout << "createXML" << std::endl;
	string resultType, objectType, imageType, name;
	HashTable<string>* htObjDict = new HashTable<string>();
	list<string> listOfAllEntries;
	listOfAllEntries.push_back("ALL");
	htObjDict->insert("ALL","");
	string includeResultTypes = "Segmentation";
	string ignoreList = ","+string(INF_IVPOINTER)+","+string(INF_IVFULLPOINTER)+","+string(INF_NAME)+",";

	//Get read-only access to object container
	const omObjectContainer *oc= getConstObjContainer();

	oc = getObjContainer();
	if(oc == NULL)
	{
		//std::cout << "METKObjXMLWriter: ObjContainer not found!" << std::endl;
		return;
	}		
		
	//std::cout << "createXML2" << std::endl;
	omObjectContainer::const_iterator iterObj;
	for ( iterObj = oc->begin(); iterObj!=oc->end(); iterObj++)
	{
		string objID = iterObj->first;
		omObject tempObj = iterObj->second;

		if (!(*oc).exists(objID))
		{
			//kDebug::Debug("invalidObject",kDebug::DL_LOW);
			return;
		}
				
		
		if (tempObj.exists(LAY_DESCRIPTION))
		{
			resultType=""; objectType=""; imageType="";
			myObjMgr->getObjAttributeString(objID, LAY_DESCRIPTION, INF_RESULTTYPE, resultType); //Hepa
			myObjMgr->getObjAttributeString(objID, LAY_GLOBAL, INF_OBJTYPE, objectType); //METK
			myObjMgr->getObjAttributeString(objID, LAY_IMAGE, INF_IMAGETYPE, imageType); //METK				

			//Add Object to All-Object			
			//if ((string)*htObjDict->find("ALL")  != "") komma=",";
			name="";
			myObjMgr->getObjAttributeString(objID,LAY_DESCRIPTION,INF_NAME, name);
			name = kBasics::toUp(name);
         
			if (name!="" && (includeResultTypes.find(resultType)!=string::npos && resultType!="") || (objectType=="Result" && imageType=="Segmentation"))
			{
				name = name + ",";
				string old = (string)*htObjDict->find("ALL");
				string tempNewEntry = old + name;
				htObjDict->remove("ALL");
				htObjDict->insert("ALL",tempNewEntry);

				//INFOs von LAY_DESCRIPTION durchiterieren
				omAttributesVector tempDescrLay = tempObj.getLayer(LAY_DESCRIPTION);
				omAttributesVector::const_iterator iterItems;
				string infoValue;
				for (iterItems = tempDescrLay.begin(); iterItems != tempDescrLay.end(); iterItems++)
				{					
					myObjMgr->getObjAttributeString(objID,LAY_DESCRIPTION,iterItems->second.getID(),infoValue);
					infoValue=kBasics::toUp(infoValue);
					if (ignoreList.find(","+iterItems->second.getID()+",")==string::npos)
					{				
						if ((infoValue+",")!=name && infoValue!="") //verhindern das gleiches mit gleichem ersetzt wird
						{
							string tempList;
							if (htObjDict->find(infoValue))
							{
								tempList = (string)*htObjDict->find(infoValue);
								if (tempList.find(","+name)==string::npos) //verhindern, das ein Objeckt zweimal eingefügt wird
								{
									tempList = tempList + name;
									//std::cout << "tempList für "<< infoValue << ": " << tempList << std::endl;
									htObjDict->remove(infoValue);
									htObjDict->insert(infoValue,tempList);
								}
							}
							else
							{
								htObjDict->remove(infoValue);
								htObjDict->insert(infoValue,","+name);
								listOfAllEntries.push_back(infoValue);
							}
						}
					}
				}
			}         
		}		
	}	
	
	//std::cout << "createXML3" << std::endl;
	//Generate XML

	AnimationParser* myAnimationParser = new AnimationParser();
	if (_filename->getStringValue()!="")
		myAnimationParser->createNewObjectXMLFile((_directory->getStringValue()+"/"+_filename->getStringValue()).c_str());		
	string* xmlString = new string("");
	myAnimationParser->createNewObjectXMLString(xmlString);	

	//std::cout << "new XML: " << *xmlString << std::endl;

	//Oh Mann, und der ganze Mist nur, um aus einem Hashtable ein paar list<string> zu machen.
	list<string>* tempObjList = new list<string>();
	string objListString;
	list<string>::const_iterator iter;
	for ( iter = listOfAllEntries.begin(); iter!=listOfAllEntries.end(); iter++)
	{
		tempObjList->clear();

		if (htObjDict->find((string)*iter))
		{
			objListString = (string)*htObjDict->find((string)*iter);
			vector<string>* strVector = new vector<string>();
			kBasics::split(objListString,',',9999,strVector);
			for (int i=0; i<strVector->size();i++)
			{
				if ((*strVector)[i]!="")
				{
					tempObjList->push_back((*strVector)[i]);
					//std::cout << "tempObjList insert : " << (*strVector)[i] << std::endl;
				}
			}
			if (_filename->getStringValue()!="")
				myAnimationParser->addObjectToXMLFile((_directory->getStringValue()+"/"+_filename->getStringValue()).c_str(), (string)*iter, tempObjList);
			myAnimationParser->addObjectToXMLString(xmlString, (string)*iter, tempObjList);
		}
	}
	//std::cout << "createXML4" << std::endl;
	_objXML->setStringValue(*xmlString);
	//std::cout << "createXML5" << std::endl;*/
}


ML_END_NAMESPACE