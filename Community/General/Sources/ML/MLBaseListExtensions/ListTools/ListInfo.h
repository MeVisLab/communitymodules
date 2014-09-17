//----------------------------------------------------------------------------------
//! This class provides a module for displaying info about BaseList derived list.
/*!
// \file    ListInfo.h
// \author  Axel Newe
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __ListInfo_H
#define __ListInfo_H



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

  
class MLBASELISTEXTENSIONS_EXPORT ListInfo : public Engine
{
  public:
          
    //! Constructor
    ListInfo ();
      
    //! Destructor
    ~ListInfo();
      
    //! Handle field notifications
    virtual void handleNotification (Field *field);
      
    //! Update fields after an initialization without handleNotification() called
    virtual void activateAttachments ();
               
  private:

    // Input fields
    BaseField*   _inListFld;   

    // UI fields
    StringField*    _listTypeFld;

    IntField*       _lengthFld;

    IntField*       _typeMinFld;
    IntField*       _typeMaxFld;
    IntField*       _numDifferentTypesFld;

    Vector3Field*   _boundingBoxMinFld;
    Vector3Field*   _boundingBoxMaxFld;
      
    // Methods  
    void _updateInfo();    // Update the info displayed in the UI

    template <typename ElementType>
    void _getBoundingBox(ListBase* inList, Vector3& boundingBoxMin, Vector3& boundingBoxMax);

    template <typename ElementType>
    void _getTypeProperties(ListBase* inList, int& listTypeMin, int& listTypeMax, int& listNumDifferentTypes);


    //! Implements the interface for the runtime type system of the ML.
    ML_MODULE_CLASS_HEADER(ListInfo);
      
};




ML_END_NAMESPACE

#endif // __ListInfo_H
