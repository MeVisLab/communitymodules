//----------------------------------------------------------------------------------
// U3DSpecificationGenerator module.
//
// \file    U3DSpecificationGenerator.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Inits U3D data structure.
//----------------------------------------------------------------------------------


#ifndef _U3DSpecificationGenerator_H
#define _U3DSpecificationGenerator_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../../abstract/BaseModules/U3DGenerator.h"


ML_START_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//! Creates U3D file from WEMs, Linesets and XMarkers
class MLU3D_EXPORT U3DSpecificationGenerator : public Module
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(U3DSpecificationGenerator)

public:

  //! Constructor.
  U3DSpecificationGenerator();

protected:

  //! Destructor.
  virtual ~U3DSpecificationGenerator();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */
  StringField   *_newSpecificationFld;
  IntField      *_newSpecificationSelectedTabFld;
  BoolField     *_newSpecificationOutputValidFld;
  EnumField     *_newSpecificationTypeFld;
  StringField   *_newSpecificationObjectNameFld;
  StringField   *_newSpecificationGroupPathFld;
  BoolField     *_newSpecificationUseDefaultColorFld;
  BoolField     *_newSpecificationUseVertexColorsFld;
  BoolField     *_newSpecificationUseDefaultSpecularColorFld;
  ColorField    *_newSpecificationColorFld;
  FloatField    *_newSpecificationColorAlphaFld;
  ColorField    *_newSpecificationSpecularColorFld;
  EnumField     *_newSpecificationModelVisibilityFld;
  StringField   *_newSpecificationMetaDataKeyFld;
  StringField   *_newSpecificationMetaDataValueFld;
  StringField   *_newSpecificationWEMLabelFld;
  StringField   *_newSpecificationFibersFld;
  StringField   *_newSpecificationPositionTypesFld;
  StringField   *_newSpecificationConnectionTypesFld;


  /* METHODS */
  void _updateObjectTypeTabView();
  void _updateNewSpecification();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _U3DSpecificationGenerator_H