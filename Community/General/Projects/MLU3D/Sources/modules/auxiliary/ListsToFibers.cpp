//----------------------------------------------------------------------------------
//! The ML module class ListsToFibers.
/*!
// \file    ListsToFibers.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Create fibers from positions list and connections list.
*/
//----------------------------------------------------------------------------------


// Local includes
#include "ListsToFibers.h"
//#include "../../shared/MLU3D_Defines.h"
#include "../../shared/MLU3D_MarkerListTools.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(ListsToFibers, Module);

//----------------------------------------------------------------------------------

ListsToFibers::ListsToFibers() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Input fields and allowed types.
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);

  // Output field
  (_outFibersFld = addBase("outFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

ListsToFibers::~ListsToFibers()
{
  // Destroy own dynamic data structures here
  _inLinePositions.clearList();
  _inLineConnections.clearList();
  _outFiberSetContainer.deleteAllFiberSets();
}

//----------------------------------------------------------------------------------

void ListsToFibers::handleNotification (Field* field)
{
  // _inLinePositionsFld
  if (field == _inLinePositionsFld)
  {
    ml::Base *inValue = _inLinePositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::XMarkerList))
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
        _inLinePositions = *inList;
      }
    }
    else
    {
      _inLinePositions.clear();
    }

    _createFibers();
  }

  // _inLineConnectionsFld
  if (field == _inLineConnectionsFld)
  {
    ml::IndexPairList* inList = ((ml::IndexPairList*)_inLineConnectionsFld->getBaseValue());

    if (inList != NULL)
    {
      _inLineConnections = (*inList);
    }
    else
    {
      _inLineConnections.clear();
    }

    _createFibers();
  }
}

//----------------------------------------------------------------------------------

void ListsToFibers::_createFibers()
{
  mlU3D::U3DMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(_outFiberSetContainer, _inLinePositions, _inLineConnections);

  _outFibersFld->touch();
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE