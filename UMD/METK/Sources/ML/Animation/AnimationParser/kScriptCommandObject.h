//----------------------------------------------------------------------------------
/*! kScriptCommandObject is the class for handling a single command object.
// \file    kScriptCommandObject.h
// \author  Konrad Mühler
// \date    2005-07-05
//
//
*/
//----------------------------------------------------------------------------------

#ifndef _KSCRIPTCOMMANDOBJECT_
#define _KSCRIPTCOMMANDOBJECT_

// Local includes
#include "AnimationSystem.h"


#include "kBasics.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm> //for string-replace
#include "StdAfx.h"

using namespace std;

//!  kScriptCommandObject is the class for handling a single command object (contains i.e. ObjectName, CommandLine and ParamStr).
class ANIMATION_EXPORT kScriptCommandObject
{
private:
  string TimeStr;
  int TimeStart,TimeEnd;
  string ObjectName;
  string CommandStr;
  string CommandLine;
  string ParamStr;
  //! \defgroup StringPos Position of single parts in an instruction
  //@{
  //static const short STRPOS_COUNT;
  static const short STRPOS_TIME; //!< TimePosition (0)
  static const short STRPOS_OBJECT; //!< ObjectPosition (1)
  static const short STRPOS_COMMAND; //!< CommandPosition (2)
  static const short STRPOS_PARAMSTR; //!< ParameterPosition (3)
  //@}

  //! \defgroup Delimiters
  //@{
  static const char DELIMITER_COMMA;
  static const char DELIMITER_SPACE;
  //@}


  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------
  bool parseStr(string);
  bool parseTimeStr(string);

public:
  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------
  kScriptCommandObject();
  kScriptCommandObject(string); //es wird sofort geparst
  kScriptCommandObject(string, string, string, string);
  bool setCommandLine(string);
  string getCommandLine();
  bool setTimeStr(string);
  string getTimeStr();
  int getTimeStart();
  int getTimeEnd();
  void setObjectName(string);
  const string getObjectName();
  void setCommandStr(string);
  string getCommandStr();
  void setParamStr(string);
  string getParamStr();
};

#endif // _KSCRIPTCOMMANDOBJECT_
