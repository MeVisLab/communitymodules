// Local includes
#include "kDebug.h"

const short kDebug::DL_NONE = 3;
const short kDebug::DL_HIGH = 2;
const short kDebug::DL_LOW = 1;

const string kDebug::ERROR_NONE = "";
const string kDebug::ERROR_OC_UNKNOWNCOMMAND = "Command string is unknown for parsed object command!";
const string kDebug::ERROR_OC_WRONGPARAMETER = "Some error in the parameter for an object command";

bool kDebug::debugOn = true;
short kDebug::DebugLevel = DL_LOW;


void kDebug::Debug(string debugStr, short level)
{
	if (level>=DebugLevel && debugOn)
	{		
		std::cout << debugStr << std::endl;
		//mlDebug(debugStr);
	}
}


void kDebug::setDebugLevel(short level)
{
	DebugLevel = level;
}

void kDebug::setDebugLevel(string level)
{
	if (level=="None") setDebugLevel(DL_NONE);
	else if (level=="High") setDebugLevel(DL_HIGH);
	else if (level=="Low") setDebugLevel(DL_LOW);	
}