//----------------------------------------------------------------------------------
//! The ML module class UMDInventorToPointer.
/*!
// \file    mlUMDInventorToPointer.h
// \author  Konrad
// \date    2006-02-01
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDInventorToPointer_H
#define __mlUMDInventorToPointer_H


// Local includes
#ifndef __UMDInventorPointerSystem_H
#include "UMDInventorPointerSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif


// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/errors/SoDebugError.h>
#include "XVLeaveScope.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "kBasics.h"


ML_START_NAMESPACE


//! 
class UMDINVENTORPOINTER_EXPORT UMDInventorToPointer : public ObjMgrClient
{
public:

	//! Constructor.
	UMDInventorToPointer (void);
   ~UMDInventorToPointer (void);

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);	
	virtual void activateAttachments();
	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//! 
	IntField* _pointer;
	
	SoNodeField* inInventor;
	StringField* fieldObjectID;
	StringField* fieldLayerID;
	StringField* fieldInfoID;

	NotifyField* fieldRefresh;

	//@}

private:

	typedef ObjMgrClient inherited;

	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(UMDInventorToPointer)


	void writePointer();

};


ML_END_NAMESPACE

#endif // __mlUMDInventorToPointer_H


