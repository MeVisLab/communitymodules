//----------------------------------------------------------------------------------
//! The ML module class NOP.
/*!
// \file    mlNOP.cpp
// \author  Wolf Spindler
// \date    2009-05-19
//
// No operation module for testing purposes
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlNOP.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(NOP, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
NOP::NOP ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("NOP::NOP ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

ML_END_NAMESPACE

