//----------------------------------------------------------------------------------
//! The ML module class Save3DFigurePDF.
/*!
// \file    SavePRC.h
// \author  Axel Newe
// \date    2015-09-25
//
// Creates PDF file with one interactive 3D scene for scientific publications.
*/
//----------------------------------------------------------------------------------


#ifndef _Save3DFigurePDF_H
#define _Save3DFigurePDF_H


// Local includes
#include "MLPDFSystem.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT Save3DFigurePDF : public Module
{
public:

  //! Constructor.
  Save3DFigurePDF();

  //-----------------------------------------------------------------------------------------------//
  //! Figure activation modes.
  //-----------------------------------------------------------------------------------------------//
  enum ACTIVATION_MODES {
    ACTIVATION_MODE_EXPLICIT_ACTIVATE = 0,
    ACTIVATION_MODE_PAGE_OPEN         = 1,
    ACTIVATION_MODE_PAGE_VISIBLE      = 2,
    NUM_ACTIVATIONMODES               = 3
  };

  //-----------------------------------------------------------------------------------------------//
  //! Figure deactivation modes.
  //-----------------------------------------------------------------------------------------------//
  enum DEACTIVATION_MODES {
    DEACTIVATION_MODE_EXPLICIT_DEACTIVATE = 0,
    DEACTIVATION_MODE_PAGE_CLOSED         = 1,
    DEACTIVATION_MODE_PAGE_INVISIBLE      = 2,
    NUM_DEACTIVATIONMODES                 = 3
  };

protected:

  //! Destructor.
  virtual ~Save3DFigurePDF();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! Filename fields
  StringField*  _u3dFilenameFld;
  StringField*  _posterImageFilenameFld;
  StringField*  _pdfFilenameFld;

  //! Page property fields
  StringField*  _pageHeaderCitationTextFld;
  StringField*  _pageHeaderHeadlineTextFld;
  BoolField*    _includeUsageHintsFld;

  //! Figure property fields
  EnumField*    _figureActivationModeFld;
  EnumField*    _figureDeactivationModeFld;
  BoolField*    _figureAnimationAutostartFld;
  BoolField*    _figureToolbarEnabledFld;
  BoolField*    _figureNavigationInterfaceEnabledFld;

  //! View property fields
  ColorField*   _viewBackgroundColorFld;

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

  //! Description fields
  StringField*  _captionFld;
  StringField*  _descriptionFld;

  //! Save notification field
  NotifyField*  _saveFld;

  //! Status fields
  StringField*   _statusFld;

  //! Progress fields
  ProgressField* _progressFld;

  //! MeVisLab Program version (must be set via Python scripting, because the full version number is not available in C++)
  StringField*   _mevislabVersionFld;


  //! PDF file property fields
  StringField*   _pdfAttrTitleFld;
  StringField*   _pdfAttrAuthorFld;
  StringField*   _pdfAttrSubjectFld;
  StringField*   _pdfAttrKeywordsFld;


  // Methods ============================================================

  void saveButtonClicked();
  void Save3DFigurePDFFile(std::string filename);

  void _calculateCameraPropertiesFromInventorCamera();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(Save3DFigurePDF)
};


ML_END_NAMESPACE

#endif // _Save3DFigurePDF_H