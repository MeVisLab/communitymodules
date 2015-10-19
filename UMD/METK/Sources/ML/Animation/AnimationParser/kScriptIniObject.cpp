//----------------------------------------------------------------------------------
/*!
// \file    kScriptIniObject.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------

#include "kScriptIniObject.h"

const string kScriptIniObject::PN_LENGTHTIMEUNIT = "LENGTHTIMEUNIT";
const string kScriptIniObject::PN_LENGTHSECONDS = "LENGTHSECONDS";
const string kScriptIniObject::PN_REALTIME = "REALTIME";
const int kScriptIniObject::INI_EMPTY_VALUE = -1;
const char kScriptIniObject::DELIMITER_EQUAL = '=';


//! Constructor
kScriptIniObject::kScriptIniObject()
{
	LengthTimeUnit=INI_EMPTY_VALUE;
	LengthSeconds=INI_EMPTY_VALUE;
	RealTime = false;
	Loop = false;
}



//! Constructor - create iniObject with given parameter
/*! \param vLengthTimeUnit
 *  \param vLengthSeconds
 *  \param vRealTime */
kScriptIniObject::kScriptIniObject(int vLengthTimeUnit, float vLengthSeconds, bool vRealTime, bool vLoop)
{
	kScriptIniObject();
	LengthTimeUnit = vLengthTimeUnit;
	LengthSeconds = vLengthSeconds;
	RealTime = vRealTime;
	Loop = vLoop;
}



//! Sets the length of animation in time units
/*! \param value time units */
void kScriptIniObject::setLengthTimeUnit(int value)
{
	LengthTimeUnit = value;	
}



//! Returns the length of animation in time units
/*! \return time units */
int kScriptIniObject::getLengthTimeUnit()
{
	return LengthTimeUnit;
}



//! Sets the length of animation in seconds
/*! \param seconds */
void kScriptIniObject::setLengthSeconds(float value)
{
	LengthSeconds = value;	
}



//! Returns the length of animation in seconds
/*! \return seconds */
float kScriptIniObject::getLengthSeconds()
{
	return LengthSeconds;
}



//! Sets if the animation is in realtime or not
/*! \param realtime or not */
void kScriptIniObject::setRealTime(bool value)
{
	RealTime = value;	
}



//! Returns if the animation is in realtime
/*! \return realtime or not */
bool kScriptIniObject::getRealTime()
{
	return RealTime;
}


//! Sets if the animation is playied back in loop mode
/*! \param loop or not */
void kScriptIniObject::setLoop(bool value)
{
	Loop = value;	
}



//! Returns if the animation is playied back in loop mode
/*! \return loop or not */
bool kScriptIniObject::getLoop()
{
	return Loop;
}


//! Parses a single line from the ini part of script
/*! \param script line */
bool kScriptIniObject::parseStr(string parseInput)
{			
	vector<string> splitted;
	kBasics::split(parseInput,DELIMITER_EQUAL,2,&splitted);

	if (splitted.size()==2)
	{
		if (kBasics::toUp(splitted[0])==PN_LENGTHTIMEUNIT)
		{
			LengthTimeUnit = kBasics::StringToInt(splitted[1]);
		}
		else if (kBasics::toUp(splitted[0])==PN_LENGTHSECONDS)
		{
			LengthSeconds = atof(splitted[1].c_str());
		}
		else if (kBasics::toUp(splitted[0])==PN_REALTIME)
		{
			RealTime = (kBasics::toUp(splitted[1])=="YES" || kBasics::toUp(splitted[1])=="LOOP");
			Loop = kBasics::toUp(splitted[1])=="LOOP";
		}
	}	
	return true;
}



//! Checks, if LengthTimEUnit and LengthSeconds are set
/*! \return iniObject is ok or not */
bool kScriptIniObject::getIntegrity()
{
	return (LengthTimeUnit!=INI_EMPTY_VALUE && LengthSeconds!=INI_EMPTY_VALUE);
}