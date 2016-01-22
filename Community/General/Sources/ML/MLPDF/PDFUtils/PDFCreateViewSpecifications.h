//----------------------------------------------------------------------------------
//! The ML module class PDFCreateViewSpecifications.
/*!
// \file    PDFCreateViewSpecifications.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-01-11
//
// Create PDF view specification string.
*/
//----------------------------------------------------------------------------------


#pragma once


#ifndef _PDFCreateViewSpecifications_H
#define _PDFCreateViewSpecifications_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/MLPDF_Tools.h"


ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

class MLPDF_EXPORT PDFCreateViewSpecifications : public Module
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFCreateViewSpecifications)

public:

  //! Constructor.
  PDFCreateViewSpecifications();

protected:

  //! Destructor.
  virtual ~PDFCreateViewSpecifications();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! PDF view camera fields
  Vector3Field*  _cameraCenterOfOrbitFld;
  Vector3Field*  _cameraCenterToCameraFld;
  FloatField*    _cameraRadiusOfOrbitFld;
  FloatField*    _cameraRollAngleFld;
  FloatField*    _cameraFOVAngleFld;

  //! Fields for creating views
  StringField*   _viewSpecificationsFld;
  StringField*   _newViewDisplayNameFld;
  ColorField*    _newViewBackgroundColorFld;
  EnumField*     _newViewLightingSchemeFld;
  EnumField*     _newViewRenderModeFld;
  NotifyField*   _addNewViewFld;
  NotifyField*   _clearViewsFld;

  /* METHODS */

  // Methods for creating views ==============================================

  void _createNewView();
  void _clearViews();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _PDFCreateViewSpecifications_H