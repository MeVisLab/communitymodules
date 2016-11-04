//----------------------------------------------------------------------------------
// Base class for all U3D modules.
//
// \file    U3DModule.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#include "U3DModule.h"


ML_START_NAMESPACE

ML_MODULE_CLASS_SOURCE(U3DModule, Module);

//////////////////////////////////////////////////////////////////////////

U3DModule::U3DModule(int numInImages, int numOutImages) : Module(numInImages, numOutImages)
{
  handleNotificationOff();

  // Add fields here


  // Init vars here


  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

U3DModule::~U3DModule()
{
  // Destroy data structures here.
}

//////////////////////////////////////////////////////////////////////////

void U3DModule::handleNotification(Field *field)
{
  // Add own field handlers here


  Module::handleNotification(field);
}

//////////////////////////////////////////////////////////////////////////

void U3DModule::activateAttachments()
{
  Module::activateAttachments();
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
