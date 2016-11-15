//----------------------------------------------------------------------------------
// U3DAddModelFromWEM module.
//
// \file    U3DAddModelFromWEM.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DAddModelFromWEM_H
#define _U3DAddModelFromWEM_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DProcessor.h"

// Project includes
#include <ColoredMarkerList.h>

// ML includes
#include <mlXMarkerList.h>


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAddModelFromWEM : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DAddModelFromWEM)

public:

  //! Constructor.
  U3DAddModelFromWEM();

protected:

  //! Destructor.
  virtual ~U3DAddModelFromWEM();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();


private:

  /* FIELDS */

  //! WEM input field.
  BaseField    * _inWEMFld;

  // Fields for selecting simple, straightforward mode
  BoolField    *_simpleModeMeshFld;

  // Fields for geometry definition
  StringField  *_meshSpecificationFld;

  




  /* VARIABLES */

  //! A pointer to the input WEM.
  WEM* _inWEM;

  //! Locking variable...
  bool _isInNotificationCB;

  /* METHODS */
  void _addMeshModelData(); 

  //! Can be notified from anywhere in the network.
  static void _wemNotificationCB(void* userData, std::vector<WEMEventContainer> ecList);


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAddModelFromWEM_H