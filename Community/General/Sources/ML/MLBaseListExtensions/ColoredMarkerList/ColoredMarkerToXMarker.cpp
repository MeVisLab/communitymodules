//----------------------------------------------------------------------------------
//! The ML module class ColoredMarkerToXMarker.
/*!
// \file    ColoredMarkerToXMarker.cpp
// \author  Axel Newe
// \date    2014-09-02
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#ifndef __ColoredMarkerList_H
#include "ColoredMarkerList.h"
#endif
#include "ColoredMarkerToXMarker.h"
#include "mlLUTFunction.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(ColoredMarkerToXMarker, Module);

//----------------------------------------------------------------------------------

ColoredMarkerToXMarker::ColoredMarkerToXMarker() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  (_inColoredMarkerListFld = addBase("inColoredMarkerList"))->setBaseValueAndAddAllowedType(&_inColoredMarkerList);
  (_outXMarkerListFld = addBase("outXMarkerList"))->setBaseValueAndAddAllowedType(&_outXMarkerList);


  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

void ColoredMarkerToXMarker::handleNotification(Field* field)
{
  if (field == _inColoredMarkerListFld) 
  {
    ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)_inColoredMarkerListFld->getBaseValue());

    if (inList != NULL)
    {
      _inColoredMarkerList = *inList;
    }
    else
    {
      _inColoredMarkerList.clear();
    }

  } 

  _convertColoredMarkersToXMarkers();

}

//----------------------------------------------------------------------------------


void ColoredMarkerToXMarker::_convertColoredMarkersToXMarkers()
{
  _outXMarkerList = _inColoredMarkerList.toXMarkerList();
  _outXMarkerListFld->touch();
}

ML_END_NAMESPACE