//----------------------------------------------------------------------------------
// Base class for all U3D modules.
//
// \file    U3DModule.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "MLU3DSystem.h"

// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE


class MLU3D_EXPORT U3DModule : public Module
{
  ML_MODULE_CLASS_HEADER(U3DModule);

public:

  //! Standard constructor.
  U3DModule(int numInImages=0, int numOutImages=0);

protected:

  //! Standard destructor.
  virtual ~U3DModule();


  /* FIELDS */

  /* STATE VARIABLES */

  /* METHODS */
  
  //! Called when input changes.
  virtual void handleNotification(Field *field);

  //! Initialize module after loading.
  virtual void activateAttachments();


private:

  /* FIELDS */

  /* STATE VARIABLES */

  /* METHODS */

};

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
