//----------------------------------------------------------------------------------
// U3DLoad module.
//
// \file    U3DLoad.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


#ifndef _U3DLoad_H
#define _U3DLoad_H


// Local includes
#include "MLU3DSystem.h"
#include "abstract/BaseModules/U3DGenerator.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DLoad : public U3DGenerator
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DLoad)

public:

  //! Constructor.
  U3DLoad();

protected:

  //! Destructor.
  virtual ~U3DLoad();

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

#endif // _U3DLoad_H