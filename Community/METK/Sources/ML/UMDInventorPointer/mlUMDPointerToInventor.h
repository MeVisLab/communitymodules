//----------------------------------------------------------------------------------
//! The ML module class UMDPointerToInventor.
/*!
// \file    mlUMDPointerToInventor.h
// \author  Konrad
// \date    2006-02-02
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDPointerToInventor_H
#define __mlUMDPointerToInventor_H


// Local includes
#ifndef __UMDInventorPointerSystem_H
#include "UMDInventorPointerSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <Inventor/errors/SoDebugError.h>

#include <ObjMgr/mlObjMgrClient.h>

class SoGroup;

ML_START_NAMESPACE


//! 
class UMDINVENTORPOINTER_EXPORT UMDPointerToInventor : public ObjMgrClient
{
public:

	//! Constructor.
	UMDPointerToInventor (void);
   ~UMDPointerToInventor (void);

	//! Handle field changes of the field \c field.
	virtual void handleNotification (Field *field);

	virtual void handleObjMgrNotification();

	virtual void activateAttachments();

	// ----------------------------------------------------------
	//@{ \name Module field declarations
	// ----------------------------------------------------------

	//! 
	IntField *fieldPointer;
	
	SoNodeField* outInventor;
	StringField* fieldObjectID;
	StringField* fieldLayerID;
	StringField* fieldInfoID;	

	//@}

private:

	typedef ObjMgrClient inherited;

	__int64 _myPointer;

	//! Implements interface for the runtime type system of the ML.
	ML_CLASS_HEADER(UMDPointerToInventor)

	void getPointer();
};


ML_END_NAMESPACE

#endif // __mlUMDPointerToInventor_H


