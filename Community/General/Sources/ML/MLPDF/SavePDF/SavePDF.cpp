//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    SavePDF.cpp
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PDF file.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SavePDF.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(SavePDF, Module);

//----------------------------------------------------------------------------------

SavePDF::SavePDF() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate page data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

}

//----------------------------------------------------------------------------------

void SavePDF::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.

  // Avoid warning
  if (field == NULL) {}
}

//----------------------------------------------------------------------------------

void SavePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

ML_END_NAMESPACE