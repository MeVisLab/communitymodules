//----------------------------------------------------------------------------------
// U3DInfo module.
//
// \file    U3DInfo.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Provides info about a U3D.
//----------------------------------------------------------------------------------


#ifndef _U3DInfo_H
#define _U3DInfo_H


// Local includes
#include "MLU3DSystem.h"
#include "abstract/BaseModules/U3DInspector.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DInfo : public U3DInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DInfo)

public:

  //! Constructor.
  U3DInfo();

protected:

  //! Destructor.
  virtual ~U3DInfo();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! Field - Filename
  BoolField  *_inputValidFld;

  IntField   *_numMetaDataEntriesFld;

  IntField   *_numViewNodesFld;
  IntField   *_numGroupNodesFld;
  IntField   *_numModelNodesFld;
  IntField   *_numLightNodesFld;

  IntField   *_numPointSetsFld;
  IntField   *_numLineSetsFld;
  IntField   *_numMeshesFld;

  IntField   *_numLightResourcesFld;
  IntField   *_numViewResourcesFld;
  IntField   *_numShadersFld;
  IntField   *_numMaterialResourcesFld;
  IntField   *_numTextureResourcesFld;
  IntField   *_numMotionResourcesFld;

//  NotifyField *_updateFld;


  /* METHODS */
  void update();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DInfo_H