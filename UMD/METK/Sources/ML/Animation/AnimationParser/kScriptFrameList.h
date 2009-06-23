//----------------------------------------------------------------------------------
/*!
// \file    kScriptFrameList.h
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef _KSCRIPTFRAMELIST_
#define _KSCRIPTFRAMELIST_

// Local includes
#include "AnimationSystem.h"


#include <string>
#include <vector>
#include <iostream>

#include "kBasics.h"
#include "kScriptFrameObject.h"
#include "kScriptIniObject.h"
#include "kScriptCommandObject.h"
#include "kScriptFrameEntry.h"
#include "HashTable.h"
#include "StdAfx.h"

using namespace std;


//! kScriptFrameList is the class for handling a list of frame objects (one object per frame).
//! A single frame list is equal to a complete animation
class ANIMATION_EXPORT kScriptFrameList
{
private:				

	int framerate;

public:								
	
	typedef std::vector<kScriptFrameObject*>::size_type size_type;

	//static const short FRAMERATE;					//!< Framerate for animation (video, not real-time)	
	
	std::vector<kScriptFrameObject*> FrameList;		//!< list of frame object
	std::vector<bool> ImportantFrames;				//!< list of important frames (frames, have to be rendered during real-time animation
	kScriptIniObject iniObject;						//!< iniObject for hole frame list = hole animation		
	HashTable<string>* htLastValues;				//!< Die jeweils letzten gesetzten Werte eines Command (relative Werte). Identifizierung durch hash(CommandLine);	
	int frameCount;

	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------

	kScriptFrameList();
	bool createFrameList(int value);
	bool setIniObject(kScriptIniObject);
	int insertCommand(kScriptCommandObject*);
	string printList();		

	void cleanUp();
	void setFramerate(int value);
};

#endif // _KSCRIPTFRAMELIST_