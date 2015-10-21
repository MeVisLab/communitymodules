//----------------------------------------------------------------------------------
//! This class provides a module for modifying BaseList derived lists.
/*!
// \file    ListModifyProperties.h
// \author  Axel Newe
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __ListModifyProperties_H
#define __ListModifyProperties_H



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

  
class MLBASELISTEXTENSIONS_EXPORT ListModifyProperties : public Engine
{
  public:
          
    //! Constructor
    ListModifyProperties ();
      
    //! Destructor
    ~ListModifyProperties();
      
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

    IntField*    _changeTypeValueFld;

    static const char* AutoUpdateModeNames[];
      
    // Methods
    
    void _updateOutput();  // Update the output list          
    void _deleteOutput();  // Delete the output list               

    template <typename ItemType>
    void _updateOutputListSpecialized();   // Update output list if input is specialized list

    void _updateOutputListBase();          // Update output list if input is unknown ListBase list

    //! Implements the interface for the runtime type system of the ML.
    ML_MODULE_CLASS_HEADER(ListModifyProperties);
      
};




ML_END_NAMESPACE

#endif // __ListModifyProperties_H
