//----------------------------------------------------------------------------------
// U3DManager module.
//
// \file    U3DManager.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DManager_H
#define _U3DManager_H


// Local includes
#include "MLU3DSystem.h"
#include "abstract/BaseModules/U3DProcessor.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DManager : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DManager)

public:

  //! Constructor.
  U3DManager();

protected:

  //! Destructor.
  virtual ~U3DManager();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */




  /* METHODS */



//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DManager_H