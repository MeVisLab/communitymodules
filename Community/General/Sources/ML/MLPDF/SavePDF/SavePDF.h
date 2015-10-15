//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    SavePRC.h
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PDF file.
*/
//----------------------------------------------------------------------------------


#ifndef _SavePDF_H
#define _SavePDF_H


// Local includes
#include "MLPDFSystem.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT SavePDF : public Module
{
public:

  //! Constructor.
  SavePDF();

protected:

  //! Destructor.
  virtual ~SavePDF();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! Field - Filename
  StringField   *_mlPDFFileNameFld;
  StringField   *_mlU3DFileNameFld;

  //! Field - Save notification
  NotifyField   *_saveFld;

  //! Status message
  StringField   *_statusFld;

  //! Progress bar.
  ProgressField *_progressFld;

  // Inventor camera fields
  NotifyField*   _calculateCameraFromInventorSceneFld;
  BoolField*     _autoCalculateCameraFromInventorScene;
  Vector3Field*  _inventorCameraPositionFld;
  Vector4Field*  _inventorCameraOrientationFld;
  FloatField*    _inventorCameraFocalDistanceFld;
  FloatField*    _inventorCameraHeightFld;

  // PDF view camera fields
  Vector3Field*  _cameraCenterOfOrbitFld;
  Vector3Field*  _cameraCenterToCameraFld;
  FloatField*    _cameraRadiusOfOrbitFld;
  FloatField*    _cameraRollAngleFld;
  FloatField*    _cameraFOVAngleFld;


  //! MeVisLab Program version (must be set via Python scripting, because the full version number is not available in C++)
  StringField * _mevislabVersionFld;


  //! PDF file property fields
  StringField   *_pdfAttrTitleFld;
  StringField   *_pdfAttrAuthorFld;
  StringField   *_pdfAttrSubjectFld;
  StringField   *_pdfAttrKeywordsFld;


  // Methods ============================================================

  void saveButtonClicked();
  void savePDFFile(std::string filename);

  void _calculateCameraPropertiesFromInventorCamera();


  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePDF)
};


ML_END_NAMESPACE

#endif // _SavePDF_H