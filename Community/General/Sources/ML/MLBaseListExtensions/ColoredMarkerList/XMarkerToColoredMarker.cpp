//----------------------------------------------------------------------------------
//! The ML module class XMarkerToColoredMarker.
/*!
// \file    XMarkerToColoredMarker.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-02
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#ifndef __ColoredMarkerList_H
#include "ColoredMarkerList.h"
#endif
#include "XMarkerToColoredMarker.h"
#include <mlLUTFunction.h>
#include <mlLUTFloatRGBA.h>


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(XMarkerToColoredMarker, Module);

//----------------------------------------------------------------------------------

XMarkerToColoredMarker::XMarkerToColoredMarker() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  (_inXMarkerListFld = addBase("inXMarkerList"))->setBaseValueAndAddAllowedType(&_inXMarkerList);
  (_outColoredMarkerListFld = addBase("outColoredMarkerList"))->setBaseValueAndAddAllowedType(&_outColoredMarkerList);
  (_inLUTFld = addBase("inLUT"))->addAllowedType<LUTFunction>();

  (_writeDefaultColorFld = addBool("writeDefaultColor"))->setBoolValue(false);
  (_defaultColorFld = addColor("defaultColor"))->setColorValue(0,0,0);
  (_defaultAlphaFld = addFloat("defaultAlpha"))->setFloatValue(1);
  _defaultAlphaFld->setFloatMinValue(0);
  _defaultAlphaFld->setFloatMaxValue(1);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

void XMarkerToColoredMarker::handleNotification(Field* field)
{
  if (field == _inXMarkerListFld) 
  {
    ml::XMarkerList* inList = ((ml::XMarkerList*)_inXMarkerListFld->getBaseValue());

    if (inList != NULL)
    {
      _inXMarkerList = *inList;      
    }
    else
    {
      _inXMarkerList.clear();
    }

  } 

  if (field != _outColoredMarkerListFld) 
  {
    _convertXMarkersToColoredMarkers();
  }

}

//----------------------------------------------------------------------------------

//! Scan input XMarkerList and get minimum and maximum type ID #
void XMarkerToColoredMarker::_GetMinMaxTypesIDsFromInXmarkerList()
{
  int min = INT_MAX;
  int max = INT_MIN;

  for(ml::XMarkerList::iterator thisXMarker = _inXMarkerList.begin();  thisXMarker != _inXMarkerList.end();  ++thisXMarker)
  {
    int markerType = thisXMarker->type;

    if (markerType > max)
    {
      max = markerType;
    }

    if (markerType < min)
    {
      min = markerType;
    }

  }

  _inListTypeMinID = min;
  _inListTypeMaxID = max;
}


void XMarkerToColoredMarker::_convertXMarkersToColoredMarkers()
{
  _outColoredMarkerList.clear();
  Vector4 newColorRGBA(0);

  bool writeDefaultColor = _writeDefaultColorFld->getBoolValue();

  if (writeDefaultColor)
  {
    Vector3 newColor = _defaultColorFld->getColorValue();
    float   newAlpha = _defaultAlphaFld->getFloatValue();
    newColorRGBA = Vector4(newColor, newAlpha);
  }

  _GetMinMaxTypesIDsFromInXmarkerList();
  LUTFunction* inLUTFunction = mlbase_cast<LUTFunction*>(_inLUTFld->getBaseValue());

  if (inLUTFunction == NULL)
  {
    _outColoredMarkerList.fromXMarkerList(_inXMarkerList, newColorRGBA);
  }
  else
  {
    LUTFloatRGBA* lutRGBA = NULL;
    
    if ( (_inListTypeMinID < INT_MAX) && (_inListTypeMaxID > INT_MIN) )  // This might be the case when the module is created or destroyed.
    {
      lutRGBA = new LUTFloatRGBA(inLUTFunction, _inListTypeMinID, _inListTypeMaxID);
    }

    for(ml::XMarkerList::iterator thisXMarker = _inXMarkerList.begin();  thisXMarker != _inXMarkerList.end();  ++thisXMarker)
    {
      int markerType = thisXMarker->type;

      if ( (!writeDefaultColor) && (lutRGBA) )
      {
        float *color = lutRGBA->getColorAt((double)markerType);
        newColorRGBA = Vector4(color[0], color[1], color[2], color[3]);
      }

      ml::ColoredMarker newMarker(Vector3(thisXMarker->x(),thisXMarker->y(),thisXMarker->z()), markerType);
      newMarker.c() = thisXMarker->c();
      newMarker.t() = thisXMarker->t();
      newMarker.u() = thisXMarker->u();
      newMarker.rgba() = newColorRGBA;
      newMarker.setName(thisXMarker->name());

      _outColoredMarkerList.push_back(newMarker);
    }

  }

  _outColoredMarkerListFld->touch();
}

ML_END_NAMESPACE