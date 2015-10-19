//----------------------------------------------------------------------------------
//! ML ObjMgrClient module UMDObjDemo is an example for a single ObjMgrModule.
/*!
// \file    mlUMDObjDemo.h
// \author  Konrad Mühler
// \date    2005-04-22
//
// Example for a single ObjModule
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDObjDemo_H
#define __mlUMDObjDemo_H


// Local includes
#ifndef __UMDObjDemoSystem_H
#include "UMDObjDemoSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif


// Include base-class header.
#include <ObjMgr/mlObjMgrClient.h>

// Include most ml specific things.
#include <mlOperatorIncludes.h>


#include <string>
#include <sstream>


ML_START_NAMESPACE


//! ML ObjMgrClient module UMDObjDemo is an example for a single ObjMgrModule.
class UMDOBJDEMO_EXPORT UMDObjDemo : public ObjMgrClient
{
private:
  typedef ObjMgrClient inherited;

    //Macro for declaring methods for the runtime system
    ML_CLASS_HEADER(UMDObjDemo)

public:

  //Constructor.
  UMDObjDemo (void);

  //Called when input changes.
  virtual void handleNotification(Field *field);

  // Called on notification from ObjMgr.
  virtual void handleObjMgrNotification();

};


ML_END_NAMESPACE

#endif // __mlUMDObjDemo_H


