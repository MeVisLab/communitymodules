//----------------------------------------------------------------------------------
/*!
// \file    kScriptFrameObject.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------


#include "kScriptFrameObject.h"

//! Constructor
kScriptFrameObject::kScriptFrameObject() 
{		
	//EntryIDList = new HashTable<int>();
}

kScriptFrameObject::~kScriptFrameObject()
{	
	//if (EntryIDList) { delete EntryIDList; EntryIDList = 0; }
}

//! Adds a new entry to the list
bool kScriptFrameObject::addFrameEntry(kScriptFrameEntry* value)
{	
	//Verwerfen doppelter Einträge ... bei Transparency geht es, bei boolschen Werten nicht ...
	//Optimierung ... wenn Object+Command schon in diesem Frame (und es ein LAYER-Befehl ist) , dann verwerfen.
	bool addCommand = true;
	//std::cout << "   -- CID=" << value->getCommandID() << "   ONAME=" << value->getObjectName() <<  std::endl;
	for (unsigned i=0; i<EntryList.size(); i++)
	{
		if (EntryList[i]->getCommandID()==value->getCommandID() && kBasics::toUp(EntryList[i]->getObjectName())==kBasics::toUp(value->getObjectName()) && EntryList[i]->getCommandType()==kScriptFrameEntry::CMDT_LAYER)
		{
			addCommand = false;
			//std::cout << "Kill " << value->getCommandLine() << " ... duplicate" << std::endl;
		}
	}
	if (addCommand)
	{
		EntryList.push_back(value);
		//std::cout << "Use " << value->getCommandLine() << std::endl;		
		//EntryIDList->insert(value->getCommandStr(),EntryList.size()-1);
	}
	return true;
}


int kScriptFrameObject::getEntryIDByCommandLine(const string commandLine)
{
	if (commandLine=="") return -1;
	for (unsigned int i=0; i<EntryList.size(); i++)
	{
		if (EntryList[i]->getCommandLine()==commandLine)
		{
			return i;
		}
	}
	return -1;
}