//----------------------------------------------------------------------------------
// U3DAddModelFromMarkerList module.
//
// \file    U3DAddModelFromMarkerList.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DAddModelFromMarkerList_H
#define _U3DAddModelFromMarkerList_H


// Local includes
#include "../../../MLU3DSystem.h"
#include "../../../abstract/BaseModules/U3DProcessor.h"

// Project includes
#include <ColoredMarkerList.h>

// ML includes
#include <mlXMarkerList.h>


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAddModelFromMarkerList : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DAddModelFromMarkerList)

public:

  //! Constructor.
  U3DAddModelFromMarkerList();

protected:

  //! Destructor.
  virtual ~U3DAddModelFromMarkerList();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();


private:

  /* FIELDS */

  // Input fields for positions
  BaseField *_inPositionsFld;

  // Fields for selecting simple, straightforward mode
  BoolField *_simpleModePointSetFld;

  // Fields for geometry definition
  StringField  *_pointCloudSpecificationFld;

  




  /* VARIABLES */

  //! The XMarkerList input for point positions of point clouds
  ml::XMarkerList _inPositions;


  /* METHODS */
  void _addPointSetModelData();  


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAddModelFromMarkerList_H