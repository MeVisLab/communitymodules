//----------------------------------------------------------------------------------
//! This class provides a module for displaying info about BaseList derived list.
/*!
// \file    ListInfo.cpp
// \author  Axel Newe
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlDebug_H
#include "mlDebug.h"
#endif
#ifndef __ListInfo_H
#include "ListInfo.h"
#endif

#include "shared/MLBaseListExtensions_DataTypes.h"
#include "shared/MLBaseListExtensions_Tools.h"

#include <mlXMarkerList.h>
#include <mlStringList.h>
#include "ColoredMarkerList/ColoredMarkerList.h"
#include "IndexPairList/IndexPairList.h"


#define ML_DEBUG_ENV_NAME "ML_ListInfo"

ML_START_NAMESPACE


// ------------------------------------------------------------------
// --- Class ListInfo
// ------------------------------------------------------------------


ML_MODULE_CLASS_SOURCE(ListInfo, Engine);


//------------------------------------------------------------------------------------------

ListInfo::ListInfo () : Engine ()
{ 
  ML_TRACE_IN("ListInfo::ListInfo () : Engine (), _needsCompleteRebuild(true)");
    
  handleNotificationOff();

  (_inListFld           = addBase("inList1", NULL))->addAllowedType<ListBase>();

  _listTypeFld          = addString("listType", "");

  _lengthFld            = addInt("listLength",0);

  _typeMinFld           = addInt("typeMinValue",0);
  _typeMaxFld           = addInt("typeMaxValue",0);
  _numDifferentTypesFld = addInt("numDifferentTypes",0);

  _boundingBoxMinFld    = addVector3("boundingBoxMin",0,0,0);
  _boundingBoxMaxFld    = addVector3("boundingBoxMax",0,0,0);

  handleNotificationOn();
}

//------------------------------------------------------------------------------------------

ListInfo::~ListInfo() {
  ML_TRACE_IN("ListInfo::~ListInfo()");
  // Nothing to do here...
}

//------------------------------------------------------------------------------------------

void ListInfo::handleNotification (Field *field) 
{
  ML_TRACE_IN("void ListInfo::handleNotification (Field *field) ");

  if (field == _inListFld) 
  {
    _updateInfo();
  }

}

//------------------------------------------------------------------------------------------

void ListInfo::_updateInfo() {
    
  ML_TRACE_IN("void ListInfo::_updateOutput()");

  // Unfortunately there is no common base class for lists with "type" property... :-/

  ListBase* inListBase = static_cast<ListBase*>(_inListFld->getBaseValue());

  std::string inListTypeIdName = "(undefined)";

  int inListLength             = 0;

  int inListTypeMin            = 0;
  int inListTypeMax            = 0;
  int inListNumDifferentTypes  = 0;
  
  Vector3 inListBoundingBoxMin = Vector3(0);
  Vector3 inListBoundingBoxMax = Vector3(0);
  
  if (inListBase)
  {
    inListLength     = (int)inListBase->getSize();
    inListTypeIdName = inListBase->getTypeIdName();

    if ("XMarkerList" == inListTypeIdName) 
    {
      XMarkerList* inList = static_cast<XMarkerList*>(inListBase);

      _getBoundingBox<XMarker>(inList, inListBoundingBoxMin, inListBoundingBoxMax);
      _getTypeProperties<XMarker>(inList, inListTypeMin, inListTypeMax, inListNumDifferentTypes);
    }
    else if ("ColoredMarkerList" == inListTypeIdName)
    {
      ColoredMarkerList* inList = static_cast<ColoredMarkerList*>(inListBase);

      _getBoundingBox<ColoredMarker>(inList, inListBoundingBoxMin, inListBoundingBoxMax);
      _getTypeProperties<ColoredMarker>(inList, inListTypeMin, inListTypeMax, inListNumDifferentTypes);
    }
    else if ("IndexPairList" == inListTypeIdName)
    {
      IndexPairList* inList = static_cast<IndexPairList*>(inListBase);

      _getTypeProperties<IndexPair>(inList, inListTypeMin, inListTypeMax, inListNumDifferentTypes);
    }
  }

  _listTypeFld->setStringValue(inListTypeIdName);

  _lengthFld->setIntValue(inListLength);

  _typeMinFld->setIntValue(inListTypeMin);
  _typeMaxFld->setIntValue(inListTypeMax);
  _numDifferentTypesFld->setIntValue(inListNumDifferentTypes);

  _boundingBoxMinFld->setVector3Value(inListBoundingBoxMin);
  _boundingBoxMaxFld->setVector3Value(inListBoundingBoxMax);

}

//------------------------------------------------------------------------------------------

template <typename ElementType>
void ListInfo::_getBoundingBox(ListBase* inList, Vector3& boundingBoxMin, Vector3& boundingBoxMax)
{
  double xmin = 0;
  double ymin = 0;
  double zmin = 0;
  double xmax = 0;
  double ymax = 0;
  double zmax = 0;

  for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
  {
    ElementType* inListItem = static_cast<ElementType*>(inList->getItemAt(i));

    double x = inListItem->x();
    double y = inListItem->y();
    double z = inListItem->z();

    if (x < xmin)
    {
      xmin = x;
    }

    if (x > xmax)
    {
      xmax = x;
    }

    if (y < ymin)
    {
      ymin = y;
    }

    if (y > ymax)
    {
      ymax = y;
    }

    if (z < zmin)
    {
      zmin = z;
    }

    if (z > zmax)
    {
      zmax = z;
    }

  }

  boundingBoxMin.x = xmin;
  boundingBoxMin.y = ymin;
  boundingBoxMin.z = zmin;

  boundingBoxMax.x = xmax;
  boundingBoxMax.y = ymax;
  boundingBoxMax.z = zmax;
}

//------------------------------------------------------------------------------------------

template <typename ElementType>
void ListInfo::_getTypeProperties(ListBase* inList, int& listTypeMin, int& listTypeMax, int& listNumDifferentTypes)
{
  int typeMin = INT_MAX;
  int typeMax = INT_MIN;
  std::set<int> differentTypesSet;

  for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
  {
    ElementType* inListItem = static_cast<ElementType*>(inList->getItemAt(i));

    int inListType = (int)inListItem->type;

    if (inListType < typeMin)
    {
      typeMin = inListType;
    }

    if (inListType > typeMax)
    {
      typeMax = inListType;
    }

    differentTypesSet.insert(inListType);
  }

  listTypeMin = typeMin;
  listTypeMax = typeMax;
  listNumDifferentTypes = (int)differentTypesSet.size();
}

//------------------------------------------------------------------------------------------

void ListInfo::activateAttachments() {  
   
  ML_TRACE_IN("void ListInfo::activateAttachments()");

  Engine::activateAttachments();
}

//------------------------------------------------------------------------------------------



ML_END_NAMESPACE


#undef ML_DEBUG_ENV_NAME