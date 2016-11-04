//----------------------------------------------------------------------------------
// U3DManager module.
//
// \file    U3DManager.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DManager.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DManager, U3DProcessor);


//***********************************************************************************


U3DManager::U3DManager() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.

  // Add output fields to the module and set allowed types.

  // Add and initialize parameter fields
  
  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DManager::~U3DManager()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DManager::handleNotification (Field* field)
{
  /*
  if (field == _myFld) 
  {
    action();
  } 
  */

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}


//***********************************************************************************


void U3DManager::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}


//***********************************************************************************


ML_END_NAMESPACE