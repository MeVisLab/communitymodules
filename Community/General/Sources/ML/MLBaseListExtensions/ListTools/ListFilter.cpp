//----------------------------------------------------------------------------------
//! This class provides a module for filtering BaseList derived lists.
/*!
// \file    ListFilter.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlDebug_H
#include "mlDebug.h"
#endif
#ifndef __ListFilter_H
#include "ListFilter.h"
#endif

#include "shared/MLBaseListExtensions_DataTypes.h"
#include "shared/MLBaseListExtensions_Tools.h"

#include <mlXMarkerList.h>
#include <mlStringList.h>
#include "ColoredMarkerList/ColoredMarkerList.h"
#include "IndexPairList/IndexPairList.h"


#define ML_DEBUG_ENV_NAME "ML_ListFilter"

ML_START_NAMESPACE


// ------------------------------------------------------------------
// --- Class ListFilter
// ------------------------------------------------------------------


ML_MODULE_CLASS_SOURCE(ListFilter, Engine);


//------------------------------------------------------------------------------------------
const char* ListFilter::AutoUpdateModeNames[] = { "Off", "AutoClear", "AutoUpdate" };

//------------------------------------------------------------------------------------------

ListFilter::ListFilter () : Engine ()
{ 
  ML_TRACE_IN("ListFilter::ListFilter () : Engine (), _needsCompleteRebuild(true)");
    
  handleNotificationOff();

  (_outListFld            = addBase("outList", NULL))->addAllowedType<ListBase>();
  (_inListFld             = addBase("inList", NULL))->addAllowedType<ListBase>();
  _updateFld              = addNotify("update");
  (_autoUpdateModeFld     = addEnum("updateMode", AutoUpdateModeNames, AutoUpdateModeCount))->setEnumValue(AutoUpdateModeAutoUpdate);
  _clearFld               = addNotify("clear");  

  _allowedTypesFld        = addString("allowedTypes","");
  _allowedNamesFld        = addString("allowedNames","");

  handleNotificationOn();
}

//------------------------------------------------------------------------------------------

ListFilter::~ListFilter() {
  ML_TRACE_IN("ListFilter::~ListFilter()");
  // Nothing to do here...
}

//------------------------------------------------------------------------------------------

void ListFilter::handleNotification (Field *field) 
{
  ML_TRACE_IN("void ListFilter::handleNotification (Field *field) ");

  if (field == _updateFld) 
  {
    _updateOutput();
  }
  else if (field == _clearFld)
  {
    _deleteOutput();
  }
  else
  {    
    int autoUpdateMode = _autoUpdateModeFld->getEnumValue();
    bool updateRelatedFieldTouched = ( (field == _inListFld) || (field == _allowedTypesFld) || (field == _allowedNamesFld) );

    if (updateRelatedFieldTouched) 
    {
      int autoUpdateMode = _autoUpdateModeFld->getEnumValue();

      if (autoUpdateMode == AutoUpdateModeAutoClear)
      {
        _deleteOutput();
      }

      if (autoUpdateMode == AutoUpdateModeAutoUpdate)
      {
        _updateOutput();
      }
    }

    if ( (field == _autoUpdateModeFld) && (autoUpdateMode == AutoUpdateModeAutoUpdate) )
    {
      _updateOutput();
    }

  }
}

//------------------------------------------------------------------------------------------

void ListFilter::_updateOutput() {
    
  ML_TRACE_IN("void ListFilter::_updateOutput()");

  // Unfortunately there is no common base class for lists with "type" property... :-/

  ListBase*   inList = static_cast<ListBase*>(_inListFld->getBaseValue());

  if (inList)
  {

    std::string inListTypeIdName(inList->getTypeIdName());

    if ("XMarkerList" == inListTypeIdName)
    {
      _updateOutputListSpecialized<XMarker>();
    }
    else if ("ColoredMarkerList" == inListTypeIdName)
    {
      _updateOutputListSpecialized<ColoredMarker>();
    }
    else if ("IndexPairList" == inListTypeIdName)
    {
      _updateOutputListSpecialized<IndexPair>();
    }
    else
    {
      _updateOutputListBase();
    }

  }
  else
  {
    _deleteOutput();
  }

}


//------------------------------------------------------------------------------------------

template <typename ItemType>
void ListFilter::_updateOutputListSpecialized()
{
  ML_TRACE_IN("void ListFilter::_updateOutputListBase()");

  ListBase* newOutList = NULL;
  ListBase* inList     = static_cast<ListBase*>(_inListFld->getBaseValue());     

  if (inList)
  {    
    const RuntimeType* inListType = inList->getTypeId();  
    ML_CHECK_RUNTIME_TYPE(inListType);
    
    // Create new output list
    newOutList = static_cast<ListBase*>(inListType->createInstance());
    
    // If instance creation was successful
    if (newOutList)
    {             
      MLssize_t insertPos = 0;
      StringVector allowedTypes = stringSplit(_allowedTypesFld->getStringValue(), ",", false);
      StringVector allowedNames = stringSplit(_allowedNamesFld->getStringValue(), ",", false);

      // Now loop through all input list items
      for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
      {
        ItemType* inListItem = static_cast<ItemType*>(inList->getItemAt(i));

        if (inListItem)
        {
          std::string itemName(inListItem->name());
          std::string itemType = intToString((int)inListItem->type);

          if ( (_itemPropertyAllowed(itemName, allowedNames)) && (_itemPropertyAllowed(itemType, allowedTypes)) )
          {
            newOutList->insertItemAt(insertPos, inListItem);
            insertPos++;
          }

        }
        else
        {
          ML_PRINT_ERROR("void ListFilter::_updateOutput()", ML_BAD_BASE_FIELD_CONTENT, "Error: An input list contains elements not derived from Base.");
        }
      }
        
    } 
    else 
    {
      ML_PRINT_ERROR("void ListFilter::_updateOutput()", ML_BAD_STATE, "Error: Output list could not be created.");
    }

  } // if (_inList)

  handleNotificationOff();

  ListBase* oldOutList = static_cast<ListBase*>(_outListFld->getBaseValue());
  ML_DELETE(oldOutList);

  handleNotificationOn();

  _outListFld->setBaseValue(newOutList);

}

//------------------------------------------------------------------------------------------

void ListFilter::_updateOutputListBase()
{
  ML_TRACE_IN("void ListFilter::_updateOutputListBase()");

  ListBase* newOutList = NULL;
  ListBase* inList     = static_cast<ListBase*>(_inListFld->getBaseValue());     

  if (inList)
  {    
    const RuntimeType* inListType = inList->getTypeId();  
    ML_CHECK_RUNTIME_TYPE(inListType);
    
    // Create new output list
    newOutList = static_cast<ListBase*>(inListType->createInstance());
    
    // If instance creation was successful
    if (newOutList)
    {             
      MLssize_t insertPos = 0;
      StringVector allowedNames = stringSplit(_allowedNamesFld->getStringValue(), ",", false);

      // Now loop through all input list items
      for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
      {
        BaseItem* inListItem = inList->getItemAt(i);

        if (inListItem)
        {
          std::string itemName(inListItem->name());

          if (_itemPropertyAllowed(itemName, allowedNames))
          {
            newOutList->insertItemAt(insertPos, inListItem);
            insertPos++;
          }

        }
        else
        {
          ML_PRINT_ERROR("void ListFilter::_updateOutput()", ML_BAD_BASE_FIELD_CONTENT, "Error: An input list contains elements not derived from Base.");
        }
      }
        
    } 
    else 
    {
      ML_PRINT_ERROR("void ListFilter::_updateOutput()", ML_BAD_STATE, "Error: Output list could not be created.");
    }

  } // if (_inList)

  handleNotificationOff();

  ListBase* oldOutList = static_cast<ListBase*>(_outListFld->getBaseValue());
  ML_DELETE(oldOutList);

  handleNotificationOn();

  _outListFld->setBaseValue(newOutList);

}

//------------------------------------------------------------------------------------------

bool ListFilter::_itemPropertyAllowed(std::string itemProperty, StringVector allowedProperties)
{
  bool result = false;

  if (allowedProperties.size() == 0)
  {
    result = true;
  }
  else
  {
    for (size_t i = 0; i < allowedProperties.size(); i++)
    {
      if (itemProperty == allowedProperties[i])
      {
        result = true;
        break;
      }
    }
  }

  return result;
}

//------------------------------------------------------------------------------------------

void ListFilter::_deleteOutput() {
    
  ML_TRACE_IN("void ListFilter::_deleteOutput()");

  ML_TRY
  {
    ListBase* outList = static_cast<ListBase*>(_outListFld->getBaseValue());
    ML_DELETE(outList);
    _outListFld->setBaseValue(NULL);
  }
  ML_CATCH_RETHROW;
}

//------------------------------------------------------------------------------------------

void ListFilter::activateAttachments() {  
   
  ML_TRACE_IN("void ListFilter::activateAttachments()");

  Engine::activateAttachments();
  _autoUpdateModeFld->touch();
}

//------------------------------------------------------------------------------------------



ML_END_NAMESPACE


#undef ML_DEBUG_ENV_NAME