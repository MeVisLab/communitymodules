//----------------------------------------------------------------------------------
//! Class to handle communication with ObjMgr via get and set functions
/*!
// \file    ObjMgrCommunicator.h
// \author  Konrad Mühler
// \date    2006-11-02
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __ObjMgrCommunicator_H
#define __ObjMgrCommunicator_H


#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define __COMMON_IMPORTEXPORT
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include "XVEnterScope.h"
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "kBasics.h"
#include "kDebug.h"

ML_START_NAMESPACE

class __COMMON_IMPORTEXPORT ObjMgrCommunicator : public ObjMgrClient
{
public:

	ObjMgrCommunicator (void);

	virtual void handleObjMgrNotification();

	void setObjAttribute(const string ObjID, const string LayerID, const string InfoID, void* value, const string omInfoType, const bool createIfNotExists, const bool persistent, const bool ignoreType=false);
	bool getObjAttribute(const string ObjID, const string LayerID, const string InfoID, omAttribute& attr);
	bool getObjAttributeVec3f(const string ObjID, const string LayerID, const string InfoID, SbVec3f& vec3f);
	bool getObjAttributeVec4f(const string ObjID, const string LayerID, const string InfoID, SbVec4f& vec4f);
	bool getObjAttributeString(const string ObjID, const string LayerID, const string InfoID, string& value);
	bool getObjAttributeFloat(const string ObjID, const string LayerID, const string InfoID, float& value);
	bool getObjAttributeInt(const string ObjID, const string LayerID, const string InfoID, int& value);
	bool getObjAttributeBool(const string ObjID, const string LayerID, const string InfoID, bool& value);

private:
	
	typedef ObjMgrClient inherited;
			
	ML_CLASS_HEADER(ObjMgrCommunicator)
		
};


ML_END_NAMESPACE


#endif // __ObjMgrCommunicator_H
