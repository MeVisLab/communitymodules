//----------------------------------------------------------------------------------
//! The ML module class VCGLineSetDefinition.
/*!
// \file    VCGLineSetDefinition.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __VCGLineSetDefinition_H
#define __VCGLineSetDefinition_H


// Local includes
#include "MLVectorcardiogramSystem.h"

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>

ML_START_NAMESPACE


//! 
class MLVECTORCARDIOGRAM_EXPORT VCGLineSetDefinition : public Module
{
public:

  //! Constructor.
  VCGLineSetDefinition();

  //! Destructor.
  ~VCGLineSetDefinition();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // Input/output fields
  BaseField* _inLinePositionsFld;

  // Ui fields
  BoolField*    _singleLoopFld;
  StringField*  _linesetDefinitionFld;
  ColorField*   _linesetStartColorFld;
  ColorField*   _linesetEndColorFld;

  // Globals
  XMarkerList   _inLinePositions;


  // Methods
  void _updateOutput();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(VCGLineSetDefinition)
};


ML_END_NAMESPACE

#endif // __VCGLineSetDefinition_H