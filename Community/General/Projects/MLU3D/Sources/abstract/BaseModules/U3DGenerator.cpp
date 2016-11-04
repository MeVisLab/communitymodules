//----------------------------------------------------------------------------------
// Base class for all modules that create a U3D object.
//
// \file    U3DGenerator.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#include "U3DGenerator.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

//! Implements code for the runtime type system of the ML.
ML_CLASS_SOURCE(U3DGenerator, U3DModule);

U3DGenerator::U3DGenerator(int numInImages, int numOutImages) : U3DModule(numInImages, numOutImages)
{
  handleNotificationOff();

  ML_CHECK_NEW(_outU3DObject, mlU3D::U3DObject());
  
  // Add output fields and set allowed types.
  (_outU3DObjectFld = addBase("outU3DObject"))->setBaseValueAndAddAllowedType(_outU3DObject);



  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

U3DGenerator::~U3DGenerator()
{  
  // Destroy data strructures here.
  _outU3DObject = NULL;
}

//////////////////////////////////////////////////////////////////////////

void U3DGenerator::activateAttachments()
{
  U3DModule::activateAttachments();
}

//////////////////////////////////////////////////////////////////////////

void U3DGenerator::handleNotification(Field* field)
{
  // Add own field handlers here

  U3DModule::handleNotification(field);
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
