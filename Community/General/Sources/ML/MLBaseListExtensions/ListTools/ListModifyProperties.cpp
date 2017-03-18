//----------------------------------------------------------------------------------
//! This class provides a module for modifying BaseList derived lists.
/*!
// \file    ListModifyProperties.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __mlDebug_H
#include "mlDebug.h"
#endif
#ifndef __ListModifyProperties_H
#include "ListModifyProperties.h"
#endif

#include "shared/MLBaseListExtensions_DataTypes.h"
#include "shared/MLBaseListExtensions_Tools.h"

#include <mlXMarkerList.h>
#include <mlStringList.h>
#include "ColoredMarkerList/ColoredMarkerList.h"
#include "IndexPairList/IndexPairList.h"


#define ML_DEBUG_ENV_NAME "ML_ListModifyProperties"

ML_START_NAMESPACE


// ------------------------------------------------------------------
// --- Class ListModifyProperties
// ------------------------------------------------------------------


ML_MODULE_CLASS_SOURCE(ListModifyProperties, Engine);


//------------------------------------------------------------------------------------------
const char* ListModifyProperties::AutoUpdateModeNames[] = { "Off", "AutoClear", "AutoUpdate" };

//------------------------------------------------------------------------------------------

ListModifyProperties::ListModifyProperties () : Engine ()
{ 
  ML_TRACE_IN("ListModifyProperties::ListModifyProperties () : Engine (), _needsCompleteRebuild(true)");
    
  handleNotificationOff();

  (_outListFld            = addBase("outList", NULL))->addAllowedType<ListBase>();
  (_inListFld             = addBase("inList", NULL))->addAllowedType<ListBase>();
  _updateFld              = addNotify("update");
  (_autoUpdateModeFld     = addEnum("updateMode", AutoUpdateModeNames, AutoUpdateModeCount))->setEnumValue(AutoUpdateModeAutoUpdate);
  _clearFld               = addNotify("clear");  

  _changeTypeValueFld     = addInt("changeTypeValue", 0);

  handleNotificationOn();
}

//------------------------------------------------------------------------------------------

ListModifyProperties::~ListModifyProperties() {
  ML_TRACE_IN("ListModifyProperties::~ListModifyProperties()");
  // Nothing to do here...
}

//------------------------------------------------------------------------------------------

void ListModifyProperties::handleNotification (Field *field) 
{
  ML_TRACE_IN("void ListModifyProperties::handleNotification (Field *field) ");

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
    bool updateRelatedFieldTouched = ( (field == _inListFld) || (field == _changeTypeValueFld) );

    if (updateRelatedFieldTouched) 
    {
      autoUpdateMode = _autoUpdateModeFld->getEnumValue();

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

void ListModifyProperties::_updateOutput() {
    
  ML_TRACE_IN("void ListModifyProperties::_updateOutput()");

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
void ListModifyProperties::_updateOutputListSpecialized()
{
  ML_TRACE_IN("void ListModifyProperties::_updateOutputListBase()");

  ListBase* newOutList = NULL;
  ListBase* inList     = static_cast<ListBase*>(_inListFld->getBaseValue());     

  if (inList)
  {    
    const RuntimeType* inListType = inList->getTypeId();  
    ML_CHECK_RUNTIME_TYPE(inListType);
    MLint changeTypeValue = _changeTypeValueFld->getIntValue();
    
    // Create new output list
    newOutList = static_cast<ListBase*>(inListType->createInstance());
    
    // If instance creation was successful
    if (newOutList)
    {             
      MLssize_t insertPos = 0;

      // copy list 0:
      for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
      {
        ItemType* inListItem = static_cast<ItemType*>(inList->getItemAt(i));
        ItemType newItem = *inListItem;

        if (inListItem)
        {
          if ( changeTypeValue != 0 )
          {
            newItem.type += static_cast<int>(changeTypeValue);
          }

          newOutList->insertItemAt(insertPos, &newItem);
          insertPos++;

        }
        else
        {
          ML_PRINT_ERROR("void ListModifyProperties::_updateOutput()", ML_BAD_BASE_FIELD_CONTENT, "Error: An input list contains elements not derived from Base.");
        }
      }
        
    } 
    else 
    {
      ML_PRINT_ERROR("void ListModifyProperties::_updateOutput()", ML_BAD_STATE, "Error: Output list could not be created.");
    }

  } // if (_inList)

  handleNotificationOff();

  ListBase* oldOutList = static_cast<ListBase*>(_outListFld->getBaseValue());
  ML_DELETE(oldOutList);

  handleNotificationOn();

  _outListFld->setBaseValue(newOutList);

}

//------------------------------------------------------------------------------------------

void ListModifyProperties::_updateOutputListBase()
{
  ML_TRACE_IN("void ListModifyProperties::_updateOutputListBase()");

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

      // Now loop through all input list items
      for (MLssize_t i = 0; i < static_cast<MLssize_t>(inList->getSize()); i++) 
      {
        BaseItem* inListItem = inList->getItemAt(i);

        if (inListItem)
        {
          // Nothing to do for BaseItem lists in this version -> just copy lists
          newOutList->insertItemAt(insertPos, inListItem);
          insertPos++;
        }
        else
        {
          ML_PRINT_ERROR("void ListModifyProperties::_updateOutput()", ML_BAD_BASE_FIELD_CONTENT, "Error: An input list contains elements not derived from Base.");
        }
      }
        
    } 
    else 
    {
      ML_PRINT_ERROR("void ListModifyProperties::_updateOutput()", ML_BAD_STATE, "Error: Output list could not be created.");
    }

  } // if (_inList)

  handleNotificationOff();

  ListBase* oldOutList = static_cast<ListBase*>(_outListFld->getBaseValue());
  ML_DELETE(oldOutList);

  handleNotificationOn();

  _outListFld->setBaseValue(newOutList);

}

//------------------------------------------------------------------------------------------

void ListModifyProperties::_deleteOutput() {
    
  ML_TRACE_IN("void ListModifyProperties::_deleteOutput()");

  ML_TRY
  {
    ListBase* outList = static_cast<ListBase*>(_outListFld->getBaseValue());
    ML_DELETE(outList);
    _outListFld->setBaseValue(NULL);
  }
  ML_CATCH_RETHROW;
}

//------------------------------------------------------------------------------------------

void ListModifyProperties::activateAttachments() {  
   
  ML_TRACE_IN("void ListModifyProperties::activateAttachments()");

  Engine::activateAttachments();
  _autoUpdateModeFld->touch();
}

//------------------------------------------------------------------------------------------



ML_END_NAMESPACE


#undef ML_DEBUG_ENV_NAME