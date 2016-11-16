//----------------------------------------------------------------------------------
// U3DAddModelFromFiberSet module.
//
// \file    U3DAddModelFromFiberSet.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DAddModelFromFiberSet_H
#define _U3DAddModelFromFiberSet_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DProcessor.h"


// ML includes
#include <mlFiberSet.h>

ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAddModelFromFiberSet : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DAddModelFromFiberSet)

public:

  //! Constructor.
  U3DAddModelFromFiberSet();

protected:

  //! Destructor.
  virtual ~U3DAddModelFromFiberSet();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();


private:

  /* FIELDS */

  // Input fields for fibers
  BaseField  *_inFibersFld;

  // Fields for selecting simple, straightforward mode
  BoolField *_simpleModeLineSetFld;

  // Fields for geometry definition
  StringField  *_lineSetSpecificationFld;

  




  /* VARIABLES */

  //! The fiberset containers used for specifying the line sets
  FiberSetContainer _inFiberSetContainer;


  /* METHODS */
  void _addLineSetModelData();  


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAddModelFromFiberSet_H