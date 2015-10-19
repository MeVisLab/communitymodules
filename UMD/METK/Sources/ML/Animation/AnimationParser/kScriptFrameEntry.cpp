//----------------------------------------------------------------------------------
/*!
// \file    kScriptFrameEntry.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------

#include "kScriptFrameEntry.h"


const short kScriptFrameEntry::CMD_NONE = 0;
const short kScriptFrameEntry::CMD_TRANSPARENCY = 1;
const short kScriptFrameEntry::CMD_COLOR = 2;
const short kScriptFrameEntry::CMD_VISIBLE = 3;
const short kScriptFrameEntry::CMD_QUALITY = 4;
const short kScriptFrameEntry::CMD_SELECTED = 5;
const short kScriptFrameEntry::CMD_CAM_ROTATECAMZ = 6;
const short kScriptFrameEntry::CMD_CAM_ROTATECAMZTO = 7;
const short kScriptFrameEntry::CMD_ROTATE = 9;
const short kScriptFrameEntry::CMD_MOVE = 10;
const short kScriptFrameEntry::CMD_SETCOORDSIZE = 11;
const short kScriptFrameEntry::CMD_SETBACKGROUND = 12;
const short kScriptFrameEntry::CMD_SETCREATEVIDEO = 13;
const short kScriptFrameEntry::CMD_SHIFTX = 16;
const short kScriptFrameEntry::CMD_SHIFTY = 17;
const short kScriptFrameEntry::CMD_SHIFTZ = 18;
const short kScriptFrameEntry::CMD_CLIP = 19;
const short kScriptFrameEntry::CMD_MOVECOORD = 20;
const short kScriptFrameEntry::CMD_SETSILHOUETTE = 21;
const short kScriptFrameEntry::CMD_SETSILHOUETTEWIDTH = 22;
const short kScriptFrameEntry::CMD_SETSILHOUETTECOLOR = 23;
//const short kScriptFrameEntry::CMD_NEWMOVE = 24;
const short kScriptFrameEntry::CMD_COLORNEW = 25;
const short kScriptFrameEntry::CMD_CAMFOCALDISTANCE = 26;
const short kScriptFrameEntry::CMD_CAMHEIGHT = 27;
const short kScriptFrameEntry::CMD_CAMORIENTATION = 28;
const short kScriptFrameEntry::CMD_CAMPOSITION = 29;
const short kScriptFrameEntry::CMD_VIEWPOINTMOVE = 30; //same as newmove but for new camPos feature
const short kScriptFrameEntry::CMD_FADING = 31;
const short kScriptFrameEntry::CMD_SETSURFACEDIST = 32;
const short kScriptFrameEntry::CMD_LOOKAT = 33;
const short kScriptFrameEntry::CMD_VOLREND = 34;
const short kScriptFrameEntry::CMD_SETDOUBLE = 35;
const short kScriptFrameEntry::CMD_SETSTRING = 36;
const short kScriptFrameEntry::CMD_SETVEC3 = 37;
const short kScriptFrameEntry::CMD_SETBOOL = 38;
const short kScriptFrameEntry::CMD_SETINT = 39;
const short kScriptFrameEntry::CMD_CAMPATH = 40;
const short kScriptFrameEntry::CMD_SETCAMPATH = 41;
const short kScriptFrameEntry::CMD_VIEWPOINTCLUSTERING = 42;
const short kScriptFrameEntry::CMD_SHOWCLUSTERINGPATH = 43;
const short kScriptFrameEntry::CMD_CAMPATH_VP = 44;

const short kScriptFrameEntry::CMDT_NONE = 0;
const short kScriptFrameEntry::CMDT_LAYER = 1;
const short kScriptFrameEntry::CMDT_SYSTEM = 2;

const short kScriptFrameEntry::PARAMT_NONE = 0;
const short kScriptFrameEntry::PARAMT_FLOAT = 1;
const short kScriptFrameEntry::PARAMT_V3 = 2;
const short kScriptFrameEntry::PARAMT_STRING = 3;
const short kScriptFrameEntry::PARAMT_BOOL = 4;

const string kScriptFrameEntry::PARAM_ROTATE_SAGITTAL = "SAGITTAL";
const string kScriptFrameEntry::PARAM_ROTATE_AXIAL = "AXIAL";
const string kScriptFrameEntry::PARAM_ROTATE_CORONAL = "CORONAL";

const char kScriptFrameEntry::DELIMITER_COLOR_SCRIPT = ',';
const char kScriptFrameEntry::DELIMITER_AXIS = ',';
const char kScriptFrameEntry::DELIMITER_COLOR_LAYER = ' ';
const char kScriptFrameEntry::DELIMITER_PARAMS = ' ';

const int kScriptFrameEntry::CFLAG_NONE = 0;
const int kScriptFrameEntry::CFLAG_ERROR = -1;
const int kScriptFrameEntry::CFLAG_EXECUTEVPMOVE = 1;
const int kScriptFrameEntry::CFLAG_EXECUTEVOLREND = 2;
const int kScriptFrameEntry::CFLAG_EXECUTECAMPATH = 3;
const int kScriptFrameEntry::CFLAG_EXECUTEVPCLUSTERING = 4;
const int kScriptFrameEntry::CFLAG_EXECUTECAMPATHVP = 5;


//! Constructor
kScriptFrameEntry::kScriptFrameEntry()
{
	ObjectName = "";
	ObjectID = "";
	LayerID = "";
	InfoID = "";
	CommandStr ="";
	CommandID = 0;
	CommandType = CMDT_NONE;
	ParamType = PARAMT_NONE;
	string ParamString = "";
	CurrentFrame = 0;
	FrameCount = 0;
	CommunicationFlag = CFLAG_NONE;
}


//! Copy-Constructor
kScriptFrameEntry::kScriptFrameEntry(kScriptFrameEntry& toClone)
{
	ObjectName = toClone.getObjectName();
	ObjectID = toClone.getObjectID();
	LayerID = toClone.getLayerID();
	InfoID = toClone.getInfoID();
	CommandLine = toClone.getCommandLine();
	CommandStr =toClone.getCommandStr();
	CommandID = toClone.getCommandID();
	CommandType = toClone.getCommandType();
	ParamType = toClone.getParamType();
	setParamString(toClone.getParamString());
	CurrentFrame = toClone.getCurrentFrame();
	FrameCount = toClone.getFrameCount();
	CommunicationFlag = toClone.getCommunicationFlag();
}


//! Sets object name
/*! \param value object name */
bool kScriptFrameEntry::setObjectName(string value)
{
	ObjectName = value;	
	return true;
}



//! Returns object name
/*! \return object name */
const string kScriptFrameEntry::getObjectName()
{
	return ObjectName;
}



//! Set command id by checking command string
/*! \param value command string (i.e. 'rotate')
 *  \return success
 */
bool kScriptFrameEntry::setCommandStr(string value)
{
	CommandStr = value;	
	value = kBasics::toUp(value);
	if (value=="SETTRANSPARENCY")
		CommandID = CMD_TRANSPARENCY;

	else if (value == "SETCOLOR")
		//CommandID = CMD_COLOR;
		CommandID = CMD_COLORNEW;

	else if (value == "SETVISIBLE")
		CommandID = CMD_VISIBLE;

	else if (value == "SETQUALITY")
		CommandID = CMD_QUALITY;

	else if (value == "SETSELECTED")
		CommandID = CMD_SELECTED;

	else if (value == "ROTATECAMZ")
		CommandID = CMD_CAM_ROTATECAMZ;

	else if (value == "ROTATECAMZTO")
		CommandID = CMD_CAM_ROTATECAMZTO;

	else if (value == "ROTATE")
		CommandID = CMD_ROTATE;

	else if (value == "MOVE")
		CommandID = CMD_MOVE;

	else if (value == "MOVECOORD")
		CommandID = CMD_MOVECOORD;

	else if (value =="SETCOORDSIZE")
		CommandID = CMD_SETCOORDSIZE;

	else if (value =="SETBACKGROUND")
		CommandID = CMD_SETBACKGROUND;

	else if (value =="SETCREATEVIDEO")
		CommandID = CMD_SETCREATEVIDEO;

	else if (value == "SHIFTX")
		CommandID = CMD_SHIFTX;

	else if (value == "SHIFTY")
		CommandID = CMD_SHIFTY;
	
	else if (value == "SHIFTZ")
		CommandID = CMD_SHIFTZ;

	else if (value == "SETCLIPPLANE")
		CommandID = CMD_CLIP;

	else if (value == "SETSILHOUETTE")
		CommandID = CMD_SETSILHOUETTE;

	else if (value == "SETSILHOUETTEWIDTH")
		CommandID = CMD_SETSILHOUETTEWIDTH;

	else if (value == "SETSILHOUETTECOLOR")
		CommandID = CMD_SETSILHOUETTECOLOR;

	else if (value == "NEWMOVE")
		CommandID = CMD_MOVE;

	else if (value == "SMURF")	
		CommandID = CMD_VIEWPOINTMOVE;

	else if (value == "VIEWPOINTMOVE")	
		CommandID = CMD_VIEWPOINTMOVE;

	else if (value == "VIEWPOINTCLUSTERING")	
		CommandID = CMD_VIEWPOINTCLUSTERING;

	else if (value == "SHOWCLUSTERINGPATH")	
		CommandID = CMD_SHOWCLUSTERINGPATH;

	else if (value == "FADING")	
		CommandID = CMD_FADING;

	else if (value == "SETSURFACEDIST")	
		CommandID = CMD_SETSURFACEDIST;

	else if (value == "SETCOLORNEW")
		CommandID = CMD_COLORNEW;

	else if (value == "SETCAMERAFOCALDISTANCE")
		CommandID = CMD_CAMFOCALDISTANCE;

	else if (value == "SETCAMERAHEIGHT")
		CommandID = CMD_CAMHEIGHT;

	else if (value == "SETCAMERAORIENTATION")
		CommandID = CMD_CAMORIENTATION;

	else if (value == "SETCAMERAPOSITION")
		CommandID = CMD_CAMPOSITION;

	else if (value == "LOOKAT")
		CommandID = CMD_LOOKAT;

	else if (value == "VOLREND")
		CommandID = CMD_VOLREND;

	else if (value == "SETDOUBLE")
		CommandID = CMD_SETDOUBLE;

	else if (value == "SETSTRING")
		CommandID = CMD_SETSTRING;

	else if (value == "SETVEC3")
		CommandID = CMD_SETVEC3;

	else if (value == "SETBOOL")
		CommandID = CMD_SETBOOL;

	else if (value == "SETINT")
		CommandID = CMD_SETINT;

	else if (value == "CAMPATH")
		CommandID = CMD_CAMPATH;			

	else if (value == "SETCAMPATH")
		CommandID = CMD_SETCAMPATH;			

	else if (value == "CAMPATHVP")
		CommandID = CMD_CAMPATH_VP;			

	else
	{
		CommandStr="";
		return false;
	}

	setCommandProperties();
	return true;
}



//! Return command string
/*! \return command string */
string kScriptFrameEntry::getCommandStr()
{
	return CommandStr;
}



//! Sets command line (hole instruction)
/*! \param value command line */
bool kScriptFrameEntry::setCommandLine(string value)
{
	CommandLine = value;
	return true;
}



//! Return command line
/*! \return command line */
string kScriptFrameEntry::getCommandLine()
{
	return CommandLine;
}



//! Sets parameter by splitting given parameter string
/*! \param value parameter string */
bool kScriptFrameEntry::setParamString(string value)
{	
	Params.clear();	
	kBasics::splitQuotated(value,DELIMITER_PARAMS,kBasics::QUOTATION_SINGLE,&Params); //MAX INT toDo
	ParamString = value;
	return true;
}



//! Returns hole parameter string
/*! \return parameter string */
string kScriptFrameEntry::getParamString()
{
	return ParamString;
}



//! Returns pointer to the lists of parameter
/*! \return pointer to parameter list */
std::vector<string>* kScriptFrameEntry::getParams()
{
	return &Params;	
}



bool kScriptFrameEntry::setCommunicationFlag(int value)
{
	CommunicationFlag = value;
	return true;
}


int kScriptFrameEntry::getCommunicationFlag()
{
	return CommunicationFlag;
}



//! Sets object id (ObjMgr)
/*! \param value object id as string */
bool kScriptFrameEntry::setObjectID(string value)
{
	ObjectID = value;
	return true;
}



//! Returns object id
/*! \return object id
 */
const string kScriptFrameEntry::getObjectID()
{
	return ObjectID;
}



//! Sets layer id (ObjMgr)
/*! \param  value layer id as string */
bool kScriptFrameEntry::setLayerID(string value)
{
	LayerID = value;
	return true;
}



//! Returns layer id (ObjMgr)
/*! \return layer id */
string kScriptFrameEntry::getLayerID()
{
	return LayerID;
}



//! Sets info id (ObjMgr)
/*! \param value info id as string */
bool kScriptFrameEntry::setInfoID(string value)
{
	InfoID = value;
	return true;
}



//! Returns info id (ObjMgr)
/*! \return info id */
string kScriptFrameEntry::getInfoID()
{
	return InfoID;
}



//! Sets number of current frame
/*! \param value current frame */
bool kScriptFrameEntry::setCurrentFrame(int value)
{
	CurrentFrame = value;
	return true;
}



//! Returns number of current frame
/*! \return number of current frame */
int kScriptFrameEntry::getCurrentFrame()
{
	return CurrentFrame;
}



//! Sets total number of frames for instruction
/*! \param value number of frames */
bool kScriptFrameEntry::setFrameCount(int value)
{
	FrameCount = value;
	return true;
}



//! Returns total number of frames for instruction
/*! \return total frame number */
int kScriptFrameEntry::getFrameCount()
{
	return FrameCount;
}



//! Returns type of command (CMDT_*)
/*! \return command type as short */
short kScriptFrameEntry::getCommandType()
{
	return CommandType;
}



//! Returns command id (CMD_*)
/*! \return command id */
short kScriptFrameEntry::getCommandID()
{
	return CommandID;
}



//! Returns type of parameter (PARAMT_*)
/*! \return parameter type */
short kScriptFrameEntry::getParamType()
{
	return ParamType;
}



//! Sets command properties (i.e. layer id)
bool kScriptFrameEntry::setCommandProperties()
{	
	switch (CommandID)
	{
		case CMD_TRANSPARENCY:
			//setLayerID("App_IP_3d"); //???? setLayerID("Appearance");
			setLayerID("Appearance"); //NEU METK
			setInfoID("Transparency");
			CommandType = CMDT_LAYER;
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_COLOR:
			setLayerID("Appearance");
			setInfoID("Color");
			CommandType = CMDT_LAYER;
			ParamType = PARAMT_V3;
			return true;
		case CMD_VISIBLE:
			//setLayerID("App_IP_Visprops");
			setLayerID("Appearance"); //NEU METK
			setInfoID("Visible");
			CommandType = CMDT_LAYER;
			ParamType = PARAMT_BOOL;
			return true;
		case CMD_QUALITY:
			setLayerID("Appearance");
			setInfoID("Quality");
			CommandType = CMDT_LAYER;
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_SELECTED:
			setLayerID("App_IP_Global");
			setInfoID("Selected");
			CommandType = CMDT_LAYER;
			ParamType = PARAMT_STRING;
			return true;
		case CMD_CAM_ROTATECAMZ:
		case CMD_CAM_ROTATECAMZTO:		
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_ROTATE:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_MOVE:
		case CMD_MOVECOORD:
		case CMD_SHOWCLUSTERINGPATH:
		case CMD_VIEWPOINTCLUSTERING:
		case CMD_VIEWPOINTMOVE:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_FADING:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_SETCOORDSIZE:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_SETBACKGROUND:
			CommandType = CMDT_SYSTEM;
			return true;
		case CMD_SETCREATEVIDEO:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_SHIFTX:
			CommandType = CMDT_LAYER;
			setLayerID("Transformation");
			setInfoID("ShiftX");
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_SHIFTY:
			CommandType = CMDT_LAYER;
			setLayerID("Transformation");
			setInfoID("ShiftY");
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_SHIFTZ:
			CommandType = CMDT_LAYER;
			setLayerID("Transformation");
			setInfoID("ShiftZ");
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_CLIP:
			CommandType = CMDT_SYSTEM;			
			return true;
		case CMD_SETSILHOUETTE:
			CommandType = CMDT_LAYER;
			setLayerID("Appearance");
			setInfoID("Silhouette");
			ParamType = PARAMT_BOOL;
			return true;
		case CMD_SETSILHOUETTEWIDTH:
			CommandType = CMDT_LAYER;
			setLayerID("Appearance");
			setInfoID("SilhouetteWidth");
			ParamType = PARAMT_FLOAT;
			return true;
		case CMD_SETSILHOUETTECOLOR:			
			CommandType = CMDT_LAYER;
			/*setLayerID("Appearance");
			setInfoID("SilhouetteColor");
			ParamType = PARAMT_V3;*/
			CommandType = CMDT_SYSTEM;
			return true;
		case CMD_SETSURFACEDIST:
			CommandType = CMDT_LAYER;
			setLayerID("Appearance");
			setInfoID("SurfaceDistance");
			ParamType = PARAMT_BOOL;
			return true;

		case CMD_COLORNEW:			
			CommandType = CMDT_SYSTEM;
			return true;

		case CMD_CAMFOCALDISTANCE:
		case CMD_CAMHEIGHT:
		case CMD_CAMORIENTATION:
		case CMD_CAMPOSITION:
		case CMD_LOOKAT:
		case CMD_VOLREND:
		case CMD_SETDOUBLE:
		case CMD_SETSTRING:
		case CMD_SETVEC3:
		case CMD_SETBOOL:
		case CMD_SETINT:
		case CMD_CAMPATH:
		case CMD_CAMPATH_VP:
		case CMD_SETCAMPATH:
			CommandType = CMDT_SYSTEM;
			return true;
	}
	return false;
}



//! Checks if CommandObject is valid (if command was not detected, command string is empty)
/*! \return valid or not */
bool kScriptFrameEntry::isValid()
{
	return CommandStr!="";
}
