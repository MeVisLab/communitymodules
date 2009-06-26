//----------------------------------------------------------------------------------
//! kScriptFrameEntry is the class for handling a single frame entry.
/*!
// \file    kScriptFrameEntry.h
// \author  Konrad Mühler
// \date    2005-07-05
//
//
*/
//----------------------------------------------------------------------------------

#ifndef _KSCRIPTFRAMEENTY_
#define _KSCRIPTFRAMEENTY_

// Local includes
#include "AnimationSystem.h"


#include <string>
#include <iostream>
#include <vector>
#include "kBasics.h"
#include "StdAfx.h"

using namespace std;

//! kScriptFrameEntry is the class for handling a single frame entry (contains i.e. command id, number of frames for instruction, current frame for this instruction).
class ANIMATION_EXPORT kScriptFrameEntry
{
private:
  string ObjectName; //!< Name of object (without qmarks)
  string ObjectID; //!< ID of Object in ObjMgr
  string LayerID; //!< Layer for propery adressed by command
  string InfoID;//!< InfoLayer for property adressed by command
  string CommandStr; //!< command string (not instruction!) ... i.e. 'rotate'
  string ParamString; //!< parameter string
  std::vector<string> Params; //!< splitted parameters

  int CommunicationFlag; //!< a flag that is used to communicate several states of a frame like "Can be used for NEWMOVE" or "Error-DoNotExecute this frame"

  string CommandLine; //!<  total instruction string
  short CommandID; //!< id of command (i.e. CMD_ROTATE)
  short CommandType; //!< type of command (i.e. CMDT_SYSTEM)
  short ParamType; //!< type of paramerer (i.e. PARAMT_STRING) ... for LAYER-commands only

  int CurrentFrame; //!< current number of frame (0 = first frame of instruction)
  int FrameCount; //!< total number of frames for instruction


  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------
  bool setCommandProperties();

public:
  //! \defgroup Commands CMD
  //@{
  static const short CMD_NONE;
  static const short CMD_TRANSPARENCY;
  static const short CMD_COLOR;
  static const short CMD_VISIBLE;
  static const short CMD_QUALITY;
  static const short CMD_SELECTED;
  static const short CMD_CAM_ROTATECAMZ;
  static const short CMD_CAM_ROTATECAMZTO;
  static const short CMD_CAM_ROTATESMURF;
  static const short CMD_ROTATE;
  static const short CMD_MOVE;
  static const short CMD_MOVECOORD;
  static const short CMD_SETCOORDSIZE;
  static const short CMD_SETBACKGROUND;
  static const short CMD_SETCREATEVIDEO;
  static const short CMD_SHIFTX;
  static const short CMD_SHIFTY;
  static const short CMD_SHIFTZ;
  static const short CMD_CLIP;
  static const short CMD_SETSILHOUETTE;
  static const short CMD_SETSILHOUETTEWIDTH;
  static const short CMD_SETSILHOUETTECOLOR;
  static const short CMD_COLORNEW;
  static const short CMD_CAMFOCALDISTANCE;
  static const short CMD_CAMHEIGHT;
  static const short CMD_CAMORIENTATION;
  static const short CMD_CAMPOSITION;
  static const short CMD_VIEWPOINTMOVE;
  static const short CMD_VIEWPOINTCLUSTERING;
  static const short CMD_SHOWCLUSTERINGPATH;
  static const short CMD_FADING;
  static const short CMD_SETSURFACEDIST;
  static const short CMD_LOOKAT;
  static const short CMD_VOLREND;
  static const short CMD_SETDOUBLE;
  static const short CMD_SETSTRING;
  static const short CMD_SETVEC3;
  static const short CMD_SETBOOL;
  static const short CMD_SETINT;
  static const short CMD_CAMPATH;
  static const short CMD_SETCAMPATH;
  static const short CMD_CAMPATH_VP;
  //@}


  //! \defgroup CommandTypes CMDT
  //@{
  static const short CMDT_NONE;
  static const short CMDT_LAYER;
  static const short CMDT_SYSTEM;
  //@}


  //! \defgroup ParameterTypes PARAMT
  //@{
  static const short PARAMT_NONE;
  static const short PARAMT_FLOAT;
  static const short PARAMT_INT;
  static const short PARAMT_V3;
  static const short PARAMT_STRING;
  static const short PARAMT_BOOL;
  //@}


  //! \defgroup Parameter PARAM
  //@{
  static const string PARAM_ROTATE_SAGITTAL; //!< string value for sagittal
  static const string PARAM_ROTATE_AXIAL; //!< string value for axial
  static const string PARAM_ROTATE_CORONAL; //!< string value for coronal
  //@}



  //! \defgroup Delimiters for splitting
  //@{
  static const char DELIMITER_COLOR_SCRIPT;
  static const char DELIMITER_AXIS;
  static const char DELIMITER_COLOR_LAYER;
  static const char DELIMITER_PARAMS;
  //@}

  //! \defgroup Communication Flags
  //@{
  static const int CFLAG_NONE;
  static const int CFLAG_ERROR;
  static const int CFLAG_EXECUTEVPMOVE;
  static const int CFLAG_EXECUTEVPCLUSTERING;
  static const int CFLAG_EXECUTEVOLREND;
  static const int CFLAG_EXECUTECAMPATH;
  static const int CFLAG_EXECUTECAMPATHVP;
  //@}


  //--------------------------------------------------------
  //Functions
  //--------------------------------------------------------
  kScriptFrameEntry();
  kScriptFrameEntry(kScriptFrameEntry& toClone);

  bool setObjectID(string);
  const string getObjectID();
  bool setObjectName(string);
  const string getObjectName();
  bool setCommandStr(string);
  string getCommandStr();
  bool setCommandLine(string);
  string getCommandLine();
  bool setParamString(string value);
  string getParamString();
  std::vector<string>* getParams();

  bool setCommunicationFlag(int value);
  int getCommunicationFlag();


  bool setLayerID(string);
  string getLayerID();
  bool setInfoID(string);
  string getInfoID();

  bool setCurrentFrame(int value);
  int getCurrentFrame();
  bool setFrameCount(int value);
  int getFrameCount();

  short getCommandType();
  short getCommandID();
  short getParamType();

  bool isValid();
};

#endif // _KSCRIPTFRAMEENTY_




