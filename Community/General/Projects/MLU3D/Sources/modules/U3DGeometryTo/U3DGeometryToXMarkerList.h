//----------------------------------------------------------------------------------
// U3DGeometryToXMarkerList module.
//
// \file    U3DGeometryToXMarkerList.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DGeometryToXMarkerList_H
#define _U3DGeometryToXMarkerList_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DInspector.h"

// ML includes
#include <mlXMarkerList.h>


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DGeometryToXMarkerList : public U3DInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DGeometryToXMarkerList)

public:

  //! Constructor.
  U3DGeometryToXMarkerList();

protected:

  //! Destructor.
  virtual ~U3DGeometryToXMarkerList();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();

private:

  /* FIELDS */

  BaseField  *_outPointSetXMarkersFld;

  /* VARIABLES */


  //! The XMarkerList output for point positions of point clouds
  XMarkerList       _outPointPositions;

  /* METHODS */



//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DGeometryToXMarkerList_H