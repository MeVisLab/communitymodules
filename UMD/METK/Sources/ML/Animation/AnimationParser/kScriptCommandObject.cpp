//----------------------------------------------------------------------------------
/*!
// \file    kScriptCommandObject.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------

#include "kScriptCommandObject.h"


//const short kScriptCommandObject::STRPOS_COUNT		= 4;
const short kScriptCommandObject::STRPOS_TIME		= 0;
const short kScriptCommandObject::STRPOS_OBJECT		= 1;
const short kScriptCommandObject::STRPOS_COMMAND	= 2;
const short kScriptCommandObject::STRPOS_PARAMSTR	= 3;

const char kScriptCommandObject::DELIMITER_COMMA = ',';
const char kScriptCommandObject::DELIMITER_SPACE = ' ';


//! Constructor
kScriptCommandObject::kScriptCommandObject()
{
	TimeStr = "";
	ObjectName = "";
	CommandStr = "";
	ParamStr = "";
}



//! Constructor - Directly parsing
/*! \param vLine command line */
kScriptCommandObject::kScriptCommandObject(string vLine)
{
	setCommandLine(vLine);
	//parseStr(vLine);
}



//! Constructor
/*! \param vTimeStr time string (i.e. [0,10])
 *  \param vObjectName
 *  \param vCommandStr
 *  \param vParamStr
 */
kScriptCommandObject::kScriptCommandObject(string vTimeStr, string vObjectName, string vCommandStr, string vParamStr)
{
	TimeStr = vTimeStr;
	ObjectName = kBasics::trimQuotatedStr(vObjectName, kBasics::QUOTATION_SINGLE);
	CommandStr = vCommandStr;
	ParamStr = vParamStr;
}



//! Sets command line
/*! \param vLine */
bool kScriptCommandObject::setCommandLine(string vLine)
{
	CommandLine = vLine;
	return parseStr(vLine);
}



//! Returns command line
/*! \return command line */
string kScriptCommandObject::getCommandLine()
{
	return CommandLine;
}



//! Sets time string
/*! \param value time string (i.e. [0,10])
 *  \return success
 */
bool kScriptCommandObject::setTimeStr(string value)
{
	TimeStr = value;	
	return parseTimeStr(TimeStr);
}



//! Returns time string
/*! \return time string */
string kScriptCommandObject::getTimeStr()
{
	return TimeStr;
}



//! Returns start time
/*! \return start time in time units */
int kScriptCommandObject::getTimeStart()
{
	return TimeStart;
}



//! Returns end time
/*! \return end time in time units */
int kScriptCommandObject::getTimeEnd()
{
	return TimeEnd;
}



//! Sets object name
/*! \param value object name */
void kScriptCommandObject::setObjectName(string value)
{
	ObjectName = kBasics::trimQuotatedStr(value, kBasics::QUOTATION_SINGLE);
}



//! Returns object name
/*! \return object name */
const string kScriptCommandObject::getObjectName()
{
	return ObjectName;
}



//! Sets command string (i.e. 'rotate')
/*! \param value command string */
void kScriptCommandObject::setCommandStr(string value)
{
	CommandStr = value;
}



//! Returns command string
/*! \return command string */
string kScriptCommandObject::getCommandStr()
{
	return CommandStr;
}



//! Sets parameter string
/*! \param value parameter string */
void kScriptCommandObject::setParamStr(string value)
{
	ParamStr = value;
}



//! Returns parameter string
/*! \return parameter string */
string kScriptCommandObject::getParamStr()
{
	return ParamStr;
}



//! Parse the input instruction by splitting it in its parts
/*! \param parseInput instruction string
	\return success
 */
bool kScriptCommandObject::parseStr(string parseInput)
{			
	parseInput = kBasics::replaceString(parseInput, "  ", " "); //remove double-spaces	
	std::vector<string> splitted;
	kBasics::splitQuotated(parseInput,DELIMITER_SPACE,kBasics::QUOTATION_SINGLE,&splitted);

	bool allOk = true;
	if (splitted.size()>STRPOS_TIME) allOk = allOk & setTimeStr(splitted[STRPOS_TIME]);
	
	if (splitted.size()>STRPOS_OBJECT)
	{
		splitted[STRPOS_OBJECT]=kBasics::trimQuotatedStr(splitted[STRPOS_OBJECT], kBasics::QUOTATION_SINGLE);
		setObjectName(splitted[STRPOS_OBJECT]);
	}
	if (splitted.size()>STRPOS_COMMAND) setCommandStr(splitted[STRPOS_COMMAND]);
	string ParamStr = "";
	for (unsigned int i=STRPOS_PARAMSTR; i<splitted.size(); i++)
	{
		if (i!=STRPOS_PARAMSTR) ParamStr = ParamStr + " ";
		ParamStr = ParamStr + splitted[i];		
	}
		
	setParamStr(ParamStr);	
	return allOk;
}



//! Parses a time stirng and sets start and end time in time units
/*! \param parseInput time string (i.e. [0,10])
 *  \return success
 */
bool kScriptCommandObject::parseTimeStr(string parseInput)
{	
	if (parseInput!="*")
	{
		vector<string> splitted;
		kBasics::split(kBasics::eraseStrPart("[",kBasics::eraseStrPart("]",parseInput)),DELIMITER_COMMA,2,&splitted);
		if (splitted.size()==2)
		{
			TimeStart = kBasics::StringToInt(splitted[0]);
			TimeEnd = kBasics::StringToInt(splitted[1]);
			//bool x = !(TimeStart==0 && splitted[0]!="0");
			return !((TimeStart==0 && splitted[0]!="0") || (TimeEnd==0 && splitted[1]!="0")); //TimeStart is 0 when the input is not an int
		}
		else if (splitted.size()==1)
		{
			TimeStart = kBasics::StringToInt(splitted[0]);
			TimeEnd = kBasics::StringToInt(splitted[0]);
			return !(TimeStart==0 && splitted[0]!="0"); //TimeStart is 0 when the input is not an int
		}
		else
		{
			cout << "\nZeiterkennungsfehler\n\n";
			return false;
		}
	}
	return true;
}
