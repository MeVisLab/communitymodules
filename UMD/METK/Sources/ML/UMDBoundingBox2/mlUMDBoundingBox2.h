//----------------------------------------------------------------------------------
//! The ML ObjMgrClient module UMDBoundingBox2.
/*!
// \file    mlUMDBoundingBox2.h
// \author  Konrad Mühler
// \date    2005-05-10
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDBoundingBox2_H
#define __mlUMDBoundingBox2_H


// Local includes
#ifndef __UMDBoundingBox2System_H
#include "UMDBoundingBox2System.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>

#include <kBasics.h>

#include <Inventor/SbLinear.h> //for SbVec3f
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>



ML_START_NAMESPACE


//! The ML ObjMgrClient module UMDBoundingBox2.
class UMDBOUNDINGBOX2_EXPORT UMDBoundingBox2 : public ObjMgrClient
{
public:

  UMDBoundingBox2 (void);

  StringField* bb_BoundingBoxField;
  DoubleField* bb_ObjectMinField;
  DoubleField* bb_ObjectMaxField;
  BoolField* bb_UpToDateField;
  NotifyField* bb_UpdateField;


  virtual void handleNotification (Field *field);
  virtual void handleObjMgrNotification();


private:

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(UMDBoundingBox2)

  typedef ObjMgrClient inherited;

  bool waitFlag;
  std::string waitObjectID;


  SoTimerSensor* timerUpdate;                 //!< Timer um das Update-Event ein klein wenig später zu senden
  static void timerEventUpdate(void* data, SoDataSensor* a);  //!< Sends the Update-Event by calling sendNotifyUpdate
  void sendNotifyUpdate(SoDataSensor* sensor);        //!< Sends the Update-Event (called by timerEventUpdate)


};


ML_END_NAMESPACE

#endif // __mlUMDBoundingBox2_H


