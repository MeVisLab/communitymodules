//----------------------------------------------------------------------------------
// Base class for all modules that process a U3D object.
//
// \file    U3DProcessor.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "MLU3DSystem.h"
#include "U3DModule.h"
#include "../shared/U3DInternalFormat/U3D_Object.h"
#include "abstract/BaseModules/U3DGenerator.h"
#include "abstract/BaseModules/U3DInspector.h"


// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

class MLU3D_EXPORT U3DProcessor : public U3DGenerator
{
public:

  //! Standard constructor.
  U3DProcessor(int numInImages = 0, int numOutImages = 0);

protected:

  //! Standard destructor.
  virtual ~U3DProcessor();


  /* FIELDS */

  //! U3D object input field.
  BaseField* _inU3DObjectFld;


  /* VARIABLES */

  //! A pointer to the output U3D object.
  mlU3D::U3DObjectPtr _inU3DObject;


  /* METHODS */

  //! Called when input changes.
  virtual void handleNotification(Field *field);

  //! Initialize module after loading.
  virtual void activateAttachments();


private:


  /* VARIABLES */


  /* METHODS */


  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(U3DProcessor)
};

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
