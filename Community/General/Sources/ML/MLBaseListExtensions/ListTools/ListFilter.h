//----------------------------------------------------------------------------------
//! This class provides a module for filtering BaseList derived lists.
/*!
// \file    ListFilter.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __ListFilter_H
#define __ListFilter_H



// ML includes
#ifndef __MLBaseListExtensionsSystem_H
#include "MLBaseListExtensionsSystem.h"
#endif
#ifndef __mlModuleIncludes_H
#include "mlModuleIncludes.h"
#endif
#ifndef __mlEngine_H
#include "mlEngine.h"
#endif

// Local includes
#ifndef __mlListBase_H
#include "mlListBase.h"
#endif
#ifndef __mlListContainer_H
#include "mlListContainer.h"
#endif

#include "shared/MLBaseListExtensions_DataTypes.h"


ML_START_NAMESPACE

  
class MLBASELISTEXTENSIONS_EXPORT ListFilter : public Engine
{
  public:
          
    //! Constructor
    ListFilter ();
      
    //! Destructor
    ~ListFilter();
      
    //! Handle field notifications
    virtual void handleNotification (Field *field);
      
    //! Update fields after an initialization without handleNotification() called
    virtual void activateAttachments ();
               
  private:

    // Input / output fields
    BaseField*   _outListFld;    
    BaseField*   _inListFld;     
      
    // UI fields
    NotifyField* _updateFld;             
    EnumField*   _autoUpdateModeFld;      
    NotifyField* _clearFld;               

    StringField* _allowedTypesFld;
    StringField* _allowedNamesFld;

    static const char* AutoUpdateModeNames[];
      
    // Methods
    
    void _updateOutput();  // Update the output list          
    void _deleteOutput();  // Delete the output list               

    template <typename ItemType>
    void _updateOutputListSpecialized();   // Update output list if input is specialized list

    void _updateOutputListBase();          // Update output list if input is unknown ListBase list

    bool _itemPropertyAllowed(std::string itemProperty, StringVector allowedProperties);  // Checks if item property is in list of allowed item properies

    //! Implements the interface for the runtime type system of the ML.
    ML_MODULE_CLASS_HEADER(ListFilter);
      
};




ML_END_NAMESPACE

#endif // __ListFilter_H
