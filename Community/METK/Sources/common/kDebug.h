//----------------------------------------------------------------------------------
//! Class to ...
/*!
// \file    kDebug.h
// \author  Konrad Mühler
// \date    2006-11-03
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __kDebug_H
#define __kDebug_H

#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define __COMMON_IMPORTEXPORT
#endif


#include <string>
#include <iostream>
#include "mlDebug.h"

using namespace std;

class __COMMON_IMPORTEXPORT kDebug
{
public:
	//! \name DebugLevel
	//! Comments will be written in the console if theire DebugLevel is higher than the global DebugLevel
	//@{
		static short DebugLevel;
		static const short DL_HIGH;
		static const short DL_LOW;		
		static const short DL_NONE;
		static void Debug(std::string debugStr, short level = DL_HIGH);
		static void setDebugLevel(short level);
		static void setDebugLevel(string level);
	//@}


	//! \name Error Strings
	//@{		
		static const string ERROR_NONE;				//!< No error, all ok
		static const string ERROR_OC_UNKNOWNCOMMAND;//!< CommandString is unknown for parsed object command
		static const string ERROR_OC_WRONGPARAMETER;//!< Some error in the parameter for an object command
	//@}

	static bool debugOn;

};

#endif // __kDebug_H
