//----------------------------------------------------------------------------------
/*!
// \file    kScriptFrameList.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------


#include "kScriptFrameList.h"

//const short kScriptFrameList::FRAMERATE = 14;


//! Constructor
kScriptFrameList::kScriptFrameList() { }



//! Creates a frame list with the given number of frames
/*! \param frameCount */
bool kScriptFrameList::createFrameList(int value)
{
	frameCount = value;
	//framerate = 7;
	FrameList.clear(); // empty vector
	FrameList.resize(frameCount); // set size to new frameCount	
	for (int i=0; i<frameCount; i++) //create a frame object for every frame
	{
		FrameList[i] = new kScriptFrameObject();
	}

	ImportantFrames.clear(); // empty vector
	ImportantFrames.resize(frameCount); // set size to new frameCount

	htLastValues = new HashTable<string>();
	return true;
}



//! Sets iniObject
/*! \param vIniObject */
bool kScriptFrameList::setIniObject(kScriptIniObject vIniObject)
{
	iniObject = vIniObject;
	//calc FrameCount and create new List
	createFrameList(iniObject.getLengthSeconds()*framerate+1); //+1 da es immer ein Frame mehr ist; und damit es bei 0 sec mind. 1 frame ist
	return true;
}



//! Inserts a command in the FrameList
/*! The command will be spread over the frames for this command
 *  \param commandObj
 *  \return a failure number
 */
int kScriptFrameList::insertCommand(kScriptCommandObject* commandObj)
{	
	int startFrame = commandObj->getTimeStart()*(iniObject.getLengthSeconds()/iniObject.getLengthTimeUnit())*framerate;
	int endFrame = commandObj->getTimeEnd()*(iniObject.getLengthSeconds()/iniObject.getLengthTimeUnit())*framerate;

	kScriptFrameEntry* tempFrameEntry = new kScriptFrameEntry();
	if (!tempFrameEntry->setCommandStr(commandObj->getCommandStr())) return 2;
	if (!tempFrameEntry->setObjectName(commandObj->getObjectName())) return 3;	
	
	tempFrameEntry->setCommandLine(commandObj->getCommandLine());
	tempFrameEntry->setParamString(commandObj->getParamStr());
	tempFrameEntry->setFrameCount(endFrame-startFrame);

	//std::cout << "insertCommand " << tempFrameEntry->getCommandLine() << std::endl;
	for (int i=startFrame; i<=endFrame; i++)
	{
		kScriptFrameEntry* tempFrameEntry2 = new kScriptFrameEntry(*tempFrameEntry);
		//std::cout << "insertCommand2 " << tempFrameEntry2->getCommandLine() << std::endl;
		tempFrameEntry2->setCurrentFrame(i-startFrame);
		FrameList[i]->addFrameEntry(tempFrameEntry2);
	}

	// set the end and start frame of insturciotn as an important frame
	// start frame is important for moving-instructions
	ImportantFrames[endFrame] = true;
	ImportantFrames[startFrame] = true;
	return 0;
}



//! Prints the frames ... for debugging etc.
string kScriptFrameList::printList()
{
	string output = "";
	for (unsigned int i=0; i<FrameList.size();i++)
	{
		output =  output + "Frame: " + kBasics::IntToString(i);
		
		list<kScriptFrameEntry>::const_iterator iter;
		for (unsigned int j=0; j<FrameList[i]->EntryList.size();j++)
		{		
			if (!FrameList[i]->EntryList[j]->isValid())
			{
				output = output + " --- " + kBasics::IntToString(i) + "," + kBasics::IntToString(j);
			}
			else
			{
				string xyz = FrameList[i]->EntryList[j]->getObjectName();				
			}			
		}
	}
	return output;
}



void kScriptFrameList::cleanUp()
{
	for (unsigned int i=0; i<FrameList.size();i++)
	{				
		list<kScriptFrameEntry>::const_iterator iter;
		for (unsigned int j=0; j<FrameList[i]->EntryList.size();j++)
		{					
			delete FrameList[i]->EntryList[j];
		}
		FrameList[i]->EntryList.clear();
		delete FrameList[i];
	}
	FrameList.clear();
}


void kScriptFrameList::setFramerate(int value)
{
	framerate = value;
}