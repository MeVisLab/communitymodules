//----------------------------------------------------------------------------------
/*!
// \file    kScriptIniObject.h
// \author  Konrad Mühler
// \date    2005-07-05
//
//
*/
//----------------------------------------------------------------------------------

#ifndef _KSCRIPTINIOBJECT_
#define _KSCRIPTINIOBJECT_

// Local includes
#include "AnimationSystem.h"

#include <string>
#include <iostream>
#include <vector>
#include "kBasics.h"
#include "StdAfx.h"

using namespace std;

//!  kScriptIniObject is the class for information about a script like length or realtime.
class ANIMATION_EXPORT kScriptIniObject
{
private:
  int LengthTimeUnit; //!< length in time units
  float LengthSeconds; //!< length in seconds
  bool RealTime; //!< realtime or not
  bool Loop; //!< Loop realplay or not (loop is only in realtime mode possible

  //! \defgroup ParameterNames PN
  //@{
  static const string PN_LENGTHTIMEUNIT;
  static const string PN_LENGTHSECONDS;
  static const string PN_REALTIME;
  //@}

  static const int INI_EMPTY_VALUE;
  static const char DELIMITER_EQUAL;

public:

  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------

  kScriptIniObject();
  kScriptIniObject(int vLengthTimeUnit, float vLengthSeconds, bool vRealTime, bool vLoop);
  void setLengthTimeUnit(int value);
  int getLengthTimeUnit();
  void setLengthSeconds(float value);
  float getLengthSeconds();
  void setRealTime(bool value);
  bool getRealTime();
  void setLoop(bool value);
  bool getLoop();

  bool parseStr(string parseInput);
  bool getIntegrity();
};

#endif // _KSCRIPTINIOBJECT_
