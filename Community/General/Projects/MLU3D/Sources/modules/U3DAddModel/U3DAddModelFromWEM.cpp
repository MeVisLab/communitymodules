//----------------------------------------------------------------------------------
// U3DAddModelFromWEM module.
//
// \file    U3DAddModelFromWEM.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DAddModelFromWEM.h"
#include "../../shared/MLU3D_Tools.h"
#include "../../shared/MLU3D_Constants.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE

using namespace mlU3D;

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DAddModelFromWEM, U3DProcessor);


//***********************************************************************************

U3DAddModelFromWEM::U3DAddModelFromWEM() : U3DProcessor()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  _inWEM = NULL;

  // Add input fields to the module and set allowed types.
  (_inWEMFld = addBase("inWEM"))->setBaseValueAndAddAllowedType(_inWEM);

  // Add and initialize parameter fields
  (_simpleModeMeshFld    = addBool("simpleModeMesh"))->setBoolValue(false);
  (_meshSpecificationFld = addString("meshSpecification"))->setStringValue("");

  // Init global variables
  _isInNotificationCB = false;

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

U3DAddModelFromWEM::~U3DAddModelFromWEM()
{
  if (_inWEM)
  {
    _inWEM->removeNotificationObserver(_wemNotificationCB, this);
  }

  // Destroy own dynamic data structures here
}

//***********************************************************************************

void U3DAddModelFromWEM::handleNotification(Field* field)
{
  if (field == _inWEMFld)
  {

    if (!_isInNotificationCB)
    {
      WEM::removeNotificationObserverFromAllWEMs(_wemNotificationCB, this);
    }

    _inWEM = mlbase_cast<WEM*>(_inWEMFld->getBaseValue());

    // if the input is valid process it
    // if the input is NULL call process when we are not the owner of the list
    // if we are the owner, not calling process keeps the output valid
    if (_inWEM)
    {
      if (_autoApplyFld->getBoolValue())
      {
        process();
      }

      if (!_isInNotificationCB)
      {
        _inWEM->addNotificationObserver(_wemNotificationCB, this);
      }
    }
  }

  // call parent class and handle apply/autoApply and in/outputs
  U3DProcessor::handleNotification(field);
}

//***********************************************************************************

void U3DAddModelFromWEM::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DProcessor::activateAttachments();
}

//***********************************************************************************

void U3DAddModelFromWEM::_wemNotificationCB(void* userData, std::vector<WEMEventContainer> ecList)
{
  U3DAddModelFromWEM* thisp = static_cast<U3DAddModelFromWEM*>(userData);

  thisp->_isInNotificationCB = true;

  bool shouldProcess = false;

  const unsigned int listSize = static_cast<unsigned int>(ecList.size());

  for (unsigned int i = 0; i < listSize; i++)
  {
    // process on any notification
    const WEMEventContainer& ec = ecList[i]; // read only

    if (ec.notificationType & WEM_NOTIFICATION_SELECTION)
    {
      // do nothing
      //shouldProcess = true;
    }
    if (ec.notificationType & WEM_NOTIFICATION_FINISHED)
    {
      // do nothing
      //shouldProcess = true;
    }
    if (ec.notificationType& WEM_NOTIFICATION_REPAINT)
    {
      // do nothing
      //shouldProcess = true;
    }
    if (ec.notificationType & WEM_NOTIFICATION_INTERACTION_INIT)
    {
      // do nothing
      //shouldProcess = true;
    }
  }

  if (shouldProcess)
  {
    thisp->process();
  }

  thisp->_isInNotificationCB = false;
}

//***********************************************************************************

void U3DAddModelFromWEM::process()
{
  // Execute module functionaylity here.
  if (_inU3DValid)
  {
    _addMeshModelData();
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

void U3DAddModelFromWEM::_addMeshModelData()
{
  if (!_inWEM)
  {
    return;
  }

  bool simpleMode = _simpleModeMeshFld->getBoolValue();
  StringVector meshSpecificationsVector;

  if (simpleMode)
  {
    meshSpecificationsVector = U3DTools::getMeshSpecificationsStringFromWEM(_inWEM);
  }
  else
  {
    meshSpecificationsVector = U3DTools::getModelSpecificationsStringFromUI(_meshSpecificationFld, "<Mesh>");
  }

  if (0 == meshSpecificationsVector.size())
  {
    return;
  }

  U3DTools::addMeshModelAndGeometry(meshSpecificationsVector, _outU3DObject, _inWEM);
}

//***********************************************************************************


ML_END_NAMESPACE