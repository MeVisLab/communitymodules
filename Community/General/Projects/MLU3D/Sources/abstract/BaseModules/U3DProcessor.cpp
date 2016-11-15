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
ML_ABSTRACT_MODULE_CLASS_SOURCE(U3DProcessor, U3DGenerator);

U3DProcessor::U3DProcessor(int numInImages, int numOutImages) : U3DGenerator(numInImages, numOutImages), _inU3DObject(NULL)
{
  handleNotificationOff();

  // Add input fields and set allowed types.
  (_inU3DObjectFld = addBase("inU3DObject"))->setBaseValueAndAddAllowedType(_inU3DObject);

  (_updateModeFld = addEnum("updateMode", updateModeStrings, NUM_UPDATE_MODES))->setEnumValue(UPDATE_MODE_UPDATE);

  _applyFld = addNotify("apply");
  (_autoApplyFld = addBool("autoApply"))->setBoolValue(false);

  _resetFld = addNotify("reset");

  // Init variables
  _inU3DValid = false;

  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

U3DProcessor::~U3DProcessor()
{
  // Destroy data structures here.
  if (_inU3DObject)
  {
    // Destroy any stuff added in this module 
    //__inU3DObject->removeStuff()
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
  U3DGenerator::handleNotification(field);

  if (field == _inU3DObjectFld)
  {
    _inU3DObject = NULL;

    ml::Base *inValue = _inU3DObjectFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::mlU3D::U3DObject))
      {
        ml::mlU3D::U3DObject* inU3DObject = ((ml::mlU3D::U3DObject*)inValue);
        _inU3DObject = inU3DObject;
      }
    }

    _inU3DValid = (_inU3DObject != NULL);

    int updateMode = _updateModeFld->getEnumValue();

    if (updateMode == UPDATE_MODE_CLEAR)
    {
      _initOutU3DFromInput();
    }
    else if (updateMode == UPDATE_MODE_UPDATE)
    {
      _initOutU3DFromInput();
      process();
    }
    else
    {
      // Do nothing if update mode is UPDATE_MODE_IGNORE
    }
  }

  if (field == _resetFld)
  {
    _initOutU3DFromInput();
    _outU3DObjectFld->touch();
  }

  if (field == _applyFld)
  {
    process();
  }

  if ((field == _autoApplyFld) && (_autoApplyFld->getBoolValue()) )
  {
    process();
  }
}

//////////////////////////////////////////////////////////////////////////

void U3DProcessor::_initOutU3DFromInput()
{
  if (_inU3DValid)
  {
    _outU3DObject->cloneFrom(*_inU3DObject);
  }
  else
  {
    _outU3DObject->clear();
  }

  _networkPanelStatusFld->setStringValue(_outU3DObject->getNetworkPanelInfo());
}

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
