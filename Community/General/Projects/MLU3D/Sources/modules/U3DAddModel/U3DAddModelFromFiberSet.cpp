//----------------------------------------------------------------------------------
// U3DAddModelFromFiberSet module.
//
// \file    U3DAddModelFromFiberSet.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddModelFromFiberSet.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE

using namespace mlU3D;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddModelFromFiberSet, U3DProcessor);


//***********************************************************************************

U3DAddModelFromFiberSet::U3DAddModelFromFiberSet() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  (_inFibersFld = addBase("inFibers"))->setBaseValueAndAddAllowedType(&_inFiberSetContainer);
  _inFibersFld->addAllowedType<FiberSetContainer::FiberSet>();

  // Add and initialize parameter fields
  (_simpleModeLineSetFld    = addBool("simpleModeLineSet"))->setBoolValue(false);
  (_lineSetSpecificationFld = addString("lineSetSpecification"))->setStringValue("");

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddModelFromFiberSet::~U3DAddModelFromFiberSet()
{
  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddModelFromFiberSet::handleNotification(Field* field)
{
  if (field == _inFibersFld)
  {
    _inFiberSetContainer.deleteAllFiberSets();

    ml::Base *inValue = _inFibersFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, FiberSetContainer))
      {
        FiberSetContainer* inFiberSetContainer = ((FiberSetContainer*)inValue);
        _inFiberSetContainer = *inFiberSetContainer;
      }
      else if (ML_BASE_IS_A(inValue, FiberSetContainer::FiberSet))
      {
        FiberSetContainer::FiberSet* inFiberSet = ((FiberSetContainer::FiberSet*)inValue);
        _inFiberSetContainer.appendFiberSet(inFiberSet);
      }
      else
      {
        // Do nothing
      }

    }

    if (_autoApplyFld->getBoolValue())
    {
      process();
    }

  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddModelFromFiberSet::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//***********************************************************************************

void U3DAddModelFromFiberSet::process()
{
  // Execute module functionaylity here.
  if (_inU3DValid)
  {
    _addLineSetModelData();
    _outU3DObject->updateGroupNodes();
    _networkPanelStatusFld->setStringValue(_outU3DObject->getNetworkPanelInfo());
  }
  else
  {
    _networkPanelStatusFld->setStringValue("U3D Input invalid");
  }

  _outU3DObjectFld->touch();
}

//***********************************************************************************

void U3DAddModelFromFiberSet::_addLineSetModelData()
{
  if (_inFiberSetContainer.getNumFiberSets() == 0)
  {
    return;
  }

  StringVector lineSetSpecificationsVector;

  if (_simpleModeLineSetFld->getBoolValue())
  {
    lineSetSpecificationsVector = U3DTools::getLineSetSpecificationsStringFromFiberSetContainer(_inFiberSetContainer);
  }
  else
  {
    lineSetSpecificationsVector = U3DTools::getModelSpecificationsStringFromUI(_lineSetSpecificationFld, "<LineSet>");
  }

  if (0 == lineSetSpecificationsVector.size())
  {
    return;
  }

  U3DTools::addLineSetModelAndGeometry(lineSetSpecificationsVector, _outU3DObject, _inFiberSetContainer);

}

//***********************************************************************************


ML_END_NAMESPACE