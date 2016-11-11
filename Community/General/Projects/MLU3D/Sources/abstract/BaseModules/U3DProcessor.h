//----------------------------------------------------------------------------------
// Base class for all modules that process a U3D object.
//
// \file    U3DProcessor.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "../../MLU3DSystem.h"
#include "U3DModule.h"
#include "U3DGenerator.h"
#include "U3DInspector.h"
#include "../../shared/U3DInternalFormat/U3D_Object.h"


// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

enum UPDATE_MODES {
  UPDATE_MODE_IGNORE = 0,
  UPDATE_MODE_CLEAR,
  UPDATE_MODE_UPDATE,

  NUM_UPDATE_MODES
};

const char* const updateModeStrings[NUM_UPDATE_MODES] = {
  "Ignore",
  "Auto Clear",
  "Auto Update",
};


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

  //! Update mode field
  EnumField   *_updateModeFld;

  //! Apply fields
  NotifyField *_applyFld;
  BoolField   *_autoApplyFld;

  // Reset output field
  NotifyField *_resetFld;


  /* VARIABLES */

  //! A pointer to the output U3D object.
  mlU3D::U3DObjectPtr _inU3DObject;

  //! a flag which stores if the input U3D object is valid
  bool _inU3DValid;


  /* METHODS */

  //! Called when input changes.
  virtual void handleNotification(Field *field);

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Execute module functionaylity.
  virtual void process() = 0;

private:

  /* VARIABLES */


  /* METHODS */

  //! Copy all data from input U3D to output U3D.
  void _initOutU3DFromInput();


  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(U3DProcessor)
};

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
