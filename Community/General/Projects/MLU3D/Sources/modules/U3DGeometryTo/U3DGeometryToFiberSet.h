//----------------------------------------------------------------------------------
// U3DGeometryToFiberSet module.
//
// \file    U3DGeometryToFiberSet.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DGeometryToFiberSet_H
#define _U3DGeometryToFiberSet_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DInspector.h"

// ML includes
#include <mlFiberSet.h>


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DGeometryToFiberSet : public U3DInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DGeometryToFiberSet)

public:

  //! Constructor.
  U3DGeometryToFiberSet();

protected:

  //! Destructor.
  virtual ~U3DGeometryToFiberSet();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();

private:

  /* FIELDS */

  BaseField  *_outLineSetFibersFld;

  /* VARIABLES */


  //! The fiberset containers used for visualizing the line sets
  FiberSetContainer _outFiberSetContainer;


  /* METHODS */


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DGeometryToFiberSet_H