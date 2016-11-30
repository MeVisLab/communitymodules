//----------------------------------------------------------------------------------
// U3DAddTextureFromImage module.
//
// \file    U3DAddTextureFromImage.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Adds a model to a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DAddTextureFromImage_H
#define _U3DAddTextureFromImage_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DProcessor.h"



ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DAddTextureFromImage : public U3DProcessor
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DAddTextureFromImage)

public:

  //! Constructor.
  U3DAddTextureFromImage();

protected:

  //! Destructor.
  virtual ~U3DAddTextureFromImage();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Execute module functionaylity.
  virtual void process();


private:

  /* FIELDS */
  

  




  /* VARIABLES */



  /* METHODS */



//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DAddTextureFromImage_H