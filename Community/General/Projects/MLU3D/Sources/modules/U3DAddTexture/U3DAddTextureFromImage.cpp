//----------------------------------------------------------------------------------
// U3DAddTextureFromImage module.
//
// \file    U3DAddTextureFromImage.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddTextureFromImage.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"


ML_START_NAMESPACE

using namespace mlU3D;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddTextureFromImage, U3DProcessor);


//***********************************************************************************

U3DAddTextureFromImage::U3DAddTextureFromImage() : U3DProcessor(1,0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.

  // Add and initialize parameter fields

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddTextureFromImage::~U3DAddTextureFromImage()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddTextureFromImage::handleNotification(Field* field)
{
  
  if (field == getInputImageField(0))
  {
    mlInfo(__FUNCTION__) << "Input image field touched.";
  }
  

  if (_autoApplyFld->getBoolValue())
  {
    process();
  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddTextureFromImage::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//***********************************************************************************

void U3DAddTextureFromImage::process()
{
  // Execute module functionaylity here.
  if (_inU3DValid)
  {

    //_networkPanelStatusFld->setStringValue(_outU3DObject->getNetworkPanelInfo());
  }
  else
  {
    _networkPanelStatusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}


//***********************************************************************************


ML_END_NAMESPACE