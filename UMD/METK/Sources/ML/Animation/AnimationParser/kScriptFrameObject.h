//----------------------------------------------------------------------------------
//! kScriptFrameObject ist a representation of one frame with the list of all instructions.
/*!
// \file    kScriptFrameObject.h
// \author  Konrad Mühler
// \date    2005-07-05
//
//
*/
//----------------------------------------------------------------------------------

#ifndef _KSCRIPTFRAMEOBJECT_
#define _KSCRIPTFRAMEOBJECT_

// Local includes
#include "AnimationSystem.h"

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "kBasics.h"
#include "HashTable.h"
#include "kScriptFrameEntry.h"
#include "StdAfx.h"

using namespace std;

//! kScriptFrameObject ist a representation of one frame with the list of all instructions.
class ANIMATION_EXPORT kScriptFrameObject
{
private:

public:
  std::vector<kScriptFrameEntry*> EntryList;  //!< list of commands for this frame
  //HashTable<int>* EntryIDList;        //!< IDs of the commands in the EntryList

  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------

  kScriptFrameObject();
  ~kScriptFrameObject();
  bool addFrameEntry(kScriptFrameEntry*);
  int getEntryIDByCommandLine(const string commandLine);
};

#endif // _KSCRIPTFRAMEOBJECT_