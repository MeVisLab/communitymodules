//----------------------------------------------------------------------------------
// Base class for all odules that consume a U3D object.
//
// \file    U3DInspector.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#include "U3DInspector.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////

//! Implements code for the runtime type system of the ML.
ML_ABSTRACT_MODULE_CLASS_SOURCE(U3DInspector, U3DModule);

U3DInspector::U3DInspector(int numInImages, int numOutImages) : U3DModule(numInImages, numOutImages), _inU3DObject(NULL)
{
  handleNotificationOff();

  // Add input fields and set allowed types.
  (_inU3DObjectFld = addBase("inU3DObject"))->setBaseValueAndAddAllowedType(_inU3DObject);

  // Add and initialize parameter fields
  (_inputValidFld = addBool("inputValid"))->setBoolValue(false);

  _applyFld      = addNotify("apply");
  (_autoApplyFld = addBool("autoApply"))->setBoolValue(false);

  // Init variables
  _inU3DValid = false;

  handleNotificationOn();
}

//////////////////////////////////////////////////////////////////////////

U3DInspector::~U3DInspector()
{
  // Destroy data structures here.
  if (_inU3DObject)
  {
    // Destroy any stuff added in this module 
    //_inU3DObject->removeStuff()
  }
}

//////////////////////////////////////////////////////////////////////////

void U3DInspector::activateAttachments()
{
  U3DModule::activateAttachments();
}

//////////////////////////////////////////////////////////////////////////

void U3DInspector::handleNotification(Field* field)
{
  U3DModule::handleNotification(field);

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
    _inputValidFld->setBoolValue(_inU3DValid);

    if (_autoApplyFld->getBoolValue())
    {
      process();
    }
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


ML_END_NAMESPACE

