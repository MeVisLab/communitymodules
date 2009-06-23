//----------------------------------------------------------------------------------
//! The ML module class SoSceneWriterMD.
/*!
// \file    mlSoSceneWriterMD.cpp
// \author  Konrad Mühler
// \date    2006-05-15
//
// New SceneWriter with ping when IV-file saved
*/
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlSoSceneWriterMD_H
#include "mlSoSceneWriterMD.h"
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(SoSceneWriterMD, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
SoSceneWriterMD::SoSceneWriterMD (void)
  : BaseOp(0, 0)
{
  ML_TRACE_IN("SoSceneWriterMD::SoSceneWriterMD()")

  FieldContainer *fields = getFieldContainer();

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();

  static const char* filesTypes[2] = {"ASCII Inventor","Binary Inventor"};

  _writeScene = fields->addNotify("writeScene");
  _fileSaved = fields->addNotify("fileSaved");
  (_fileName = fields->addString("fileName"))->setStringValue(".iv");
  (_inputObject = fields->addSoNode("inputObject"))->setSoNodeValue(NULL);
  _fileType    = getFieldContainer()->addEnum("fileType", filesTypes, 2);

  _fileType->setStringValue("ASCII Inventor");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void SoSceneWriterMD::handleNotification (Field *field)
{
  ML_TRACE_IN("SoSceneWriterMD::handleNotification()")

	if (field == _writeScene)
	{
		SoOutput conOut; 
		conOut.openFile(_fileName->getStringValue().c_str()); 
		if	(_fileType->getStringValue() == "Binary Inventor")
			conOut.setBinary(true);
		else
			conOut.setBinary(false);
		SoWriteAction myWriteAction(&conOut); 
		myWriteAction.apply(_inputObject->getSoNodeValue());
		_fileSaved->notify();
	} 
}

ML_END_NAMESPACE

