//----------------------------------------------------------------------------------
//! The ML ObjMgr client module class METKObjXMLWriter.
/*!
// \file    mlMETKObjXMLWriter.h
// \author  Konrad Mühler
// \date    2006-12-12
//
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlMETKObjXMLWriter_H
#define __mlMETKObjXMLWriter_H

// Local includes
#include "AnimationSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>

#include "ObjMgrCommunicator.h"
#include "METK.h"
#include "kBasics.h"
#include "HashTable.h"
#include "AnimationParser/AnimationParser.h"

ML_START_NAMESPACE


//! The ML ObjMgr client module class METKObjXMLWriter.
class ANIMATION_EXPORT METKObjXMLWriter : public ObjMgrClient
{
public:

  //! Constructor.
  METKObjXMLWriter ();
  ~METKObjXMLWriter (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  virtual void handleObjMgrNotification();

  virtual void activateAttachments();

private:

  typedef ObjMgrClient inherited;

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //!
  NotifyField *_run;

  //!
  StringField *_directory;

  //!
  StringField *_filename;

  //!
  BoolField *_useCasePath;

  //!
  StringField *_objXML;


  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(METKObjXMLWriter)


  void createXML();

  ObjMgrCommunicator* myObjMgr;

};


ML_END_NAMESPACE

#endif // __mlMETKObjXMLWriter_H
