//----------------------------------------------------------------------------------
// U3DGeometryToWEM module.
//
// \file    U3DGeometryToWEM.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DGeometryToWEM_H
#define _U3DGeometryToWEM_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DInspector.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DGeometryToWEM : public U3DInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DGeometryToWEM)

public:

  //! Constructor.
  U3DGeometryToWEM();

protected:

  //! Destructor.
  virtual ~U3DGeometryToWEM();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();

private:

  /* FIELDS */

  BaseField  *_outMeshWEMFld;

  /* VARIABLES */

  //! The WEM used for visualizing the meshes
  WEM*              _outWEM;


  /* METHODS */


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DGeometryToWEM_H