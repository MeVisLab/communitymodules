//----------------------------------------------------------------------------------
// U3DInit module.
//
// \file    U3DInit.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


#ifndef _U3DInit_H
#define _U3DInit_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DGenerator.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DInit : public U3DGenerator
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DInit)

public:

  //! Constructor.
  U3DInit();

protected:

  //! Destructor.
  virtual ~U3DInit();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */
  NotifyField  *_initializeFld;

  // Fields for default model names 
  StringField  *_modelPrefixPointCloudsFld;
  StringField  *_modelPrefixLineSetsFld;
  StringField  *_modelPrefixMeshesFld;

  // Fields for default block identifier names 
  StringField  *_defaultViewNameFld;
  StringField  *_defaultLightNameFld;

  // Fields for default material values
  StringField  *_defaultMaterialNameFld;
  ColorField   *_defaultMaterialAmbientColorFld;
  ColorField   *_defaultMaterialDiffuseColorFld;
  ColorField   *_defaultMaterialSpecularColorFld;
  ColorField   *_defaultMaterialEmissiveColorFld;
  DoubleField  *_defaultMaterialAlphaFld;
  DoubleField  *_defaultMaterialReflectivityFld;

  // Fields for default light values
  ColorField   *_defaultLightColorFld;
  DoubleField  *_defaultLightIntensityFld;

  // Other settings 
  BoolField    *_addDefaultBoundingBoxMetaDataFld;
  BoolField    *_addDefaultViewNodeFld;
  BoolField    *_addDefaultLightNodeFld;

  /* METHODS */
  void _initializeU3D();
  void _addDefaultViewNode();
  void _addDefaultViewResource();
  void _addDefaultLightNode();
  void _addDefaultLightResource();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DInit_H