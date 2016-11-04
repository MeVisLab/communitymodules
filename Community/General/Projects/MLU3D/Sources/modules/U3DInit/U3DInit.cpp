//----------------------------------------------------------------------------------
// U3DInit module.
//
// \file    U3DInit.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DInit.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DInit, U3DGenerator);


//***********************************************************************************


U3DInit::U3DInit() : U3DGenerator()
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


U3DInit::~U3DInit()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DInit::handleNotification (Field* field)
{
  /*
  if (field == _myFld) 
  {
    action();
  } 
  */

  // call parent class and handle apply/autoApply and in/outputs
  U3DGenerator::handleNotification(field);
}


//***********************************************************************************


void U3DInit::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DGenerator::activateAttachments();
}


//***********************************************************************************


ML_END_NAMESPACE