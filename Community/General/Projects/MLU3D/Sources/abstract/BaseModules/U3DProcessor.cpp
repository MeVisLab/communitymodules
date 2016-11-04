//----------------------------------------------------------------------------------
// Base class for all modules that process a U3D object.
//
// \file    U3DProcessor.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#include "U3DProcessor.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

//! Implements code for the runtime type system of the ML.
ML_CLASS_SOURCE(U3DProcessor, U3DGenerator);

U3DProcessor::U3DProcessor(int numInImages, int numOutImages) : U3DGenerator(numInImages, numOutImages), _inU3DObject(NULL)
{
  handleNotificationOff();

  // Add input fields and set allowed types.
  (_inU3DObjectFld = addBase("inU3DObject"))->setBaseValueAndAddAllowedType(_inU3DObject);


  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

U3DProcessor::~U3DProcessor()
{
  // Destroy data structures here.
  if (_inU3DObject)
  {
    // Destroy any stuff added in this module 
    //_inU3DObject->removeStuff()
  }
}

//////////////////////////////////////////////////////////////////////////

void U3DProcessor::activateAttachments()
{
  U3DGenerator::activateAttachments();
}

//////////////////////////////////////////////////////////////////////////

void U3DProcessor::handleNotification(Field* field)
{
  if (field == _inU3DObjectFld)
  {
  }
  //else if (field == _applyFld) 
  //{
  //  _process();
  //}

  U3DGenerator::handleNotification(field);
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
