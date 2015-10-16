//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase.h
// \author  Axel Newe
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_PDFCreatorBase_H
#define _PDF_PDFCreatorBase_H 


// Local includes
#include "MLPDFSystem.h"
#include"../MLPDF_Defines.h"

// ML includes
#include <mlModuleIncludes.h>

// Include all libhpdf headers
#include "hpdf.h"
#include "hpdf_3dmeasure.h"
#include "hpdf_annotation.h"
#include "hpdf_catalog.h"
#include "hpdf_conf.h"
#include "hpdf_consts.h"
#include "hpdf_destination.h"
#include "hpdf_doc.h"
#include "hpdf_encoder.h"
#include "hpdf_encrypt.h"
#include "hpdf_encryptdict.h"
#include "hpdf_error.h"
#include "hpdf_exdata.h"
#include "hpdf_ext_gstate.h"
#include "hpdf_font.h"
#include "hpdf_fontdef.h"
#include "hpdf_gstate.h"
#include "hpdf_image.h"
#include "hpdf_info.h"
#include "hpdf_list.h"
#include "hpdf_mmgr.h"
#include "hpdf_namedict.h"
#include "hpdf_objects.h"
#include "hpdf_outline.h"
#include "hpdf_page_label.h"
#include "hpdf_pages.h"
#include "hpdf_pdfa.h"
#include "hpdf_streams.h"
#include "hpdf_types.h"
#include "hpdf_u3d.h"
#include "hpdf_utils.h"
#include "hpdf_version.h"


ML_START_NAMESPACE


//! 
class MLPDF_EXPORT PDFCreatorBase : public Module
{
public:

  //! Constructor.
  PDFCreatorBase();

protected:

  //! Destructor.
  virtual ~PDFCreatorBase();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification(Field* field);

  //! Handle Save button click
  void saveButtonClicked();

  //! Assemble PDF document
  //! This is the main method for all derived modules.
  //! It is called by the saveButtonClicked() method.
  //! Add all code that assembles the actual PDF document here!
  virtual void assemblePDFDocument() = 0;

  // Methods for assembling PDf doc
  void docAddPage();
  void docAddPage(float width, float height);
  void docAddPage(mlPDF::PAGESIZES pageSize, mlPDF::PAGEDIRECTIONS pageDirection);
  void docSetCurrentFont(HPDF_Font font);
  void docSetCurrentFont(HPDF_Font font, float fontSize);
  void docSetCurrentFontSize(float fontSize);
  void docWriteTextAt(float x, float y, std::string text);
  void doc3DLoadModelDatasFromFfile();
  void doc3DCreateView();
  void doc3DAddView(mlPDF::PDF3DSCENE scene, mlPDF::PDF3DVIEW view);
  void doc3DSetDefaultView(mlPDF::PDF3DSCENE scene, std::string viewName);
  mlPDF::PDF3DANNOTATION doc3DCreateAnnotation(float left, float bottom, float right, float top, mlPDF::PDF3DSCENE scene);
  void doc3DSetActivationProperties(mlPDF::PDF3DANNOTATION annotation, std::string activationCode, std::string deactivationCode, bool toolbarEnabled = false, bool navigationInterfaceOpened = false, bool animationAutoStart = false);


  //! Calculate PDF camera properties
  void calculateCameraPropertiesFromInventorCamera();
  void calculateDefaultCameraProperties();

  /* FIELDS */

  //! Field - Output filename
  StringField   *_mlPDFFileNameFld;

  //! Field - Save button notification
  NotifyField   *_saveFld;

  //! Status message
  StringField   *_statusFld;
  BoolField*     _successFld;

  //! Progress bar.
  ProgressField *_progressFld;

  //! PDF file property fields
  StringField   *_pdfAttrTitleFld;
  StringField   *_pdfAttrAuthorFld;
  StringField   *_pdfAttrSubjectFld;
  StringField   *_pdfAttrKeywordsFld;

  //! PDF view camera fields
  Vector3Field*  _cameraCenterOfOrbitFld;
  Vector3Field*  _cameraCenterToCameraFld;
  FloatField*    _cameraRadiusOfOrbitFld;
  FloatField*    _cameraRollAngleFld;
  FloatField*    _cameraFOVAngleFld;

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  NotifyField*   _calculateCameraFromInventorSceneFld;
  BoolField*     _autoCalculateCameraFromInventorScene;
  Vector3Field*  _inventorCameraPositionFld;
  Vector4Field*  _inventorCameraOrientationFld;
  FloatField*    _inventorCameraFocalDistanceFld;
  FloatField*    _inventorCameraHeightFld;

  //! The main document container
  HPDF_Doc  pdfDocument;

  //! Document content containers
  std::vector<HPDF_Page> pdfDocPages;
  HPDF_Page              pdfDocCurrentPage;
  
  //! General PDF properties
  mlPDF::BuiltInFontsStruct buildInFonts;
  mlPDF::PageSizeInfoStruct pageSizeInfo;

private:

  //! Initialize internal PDF document container
  void _initPDFDocument();
  void _initFonts();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFCreatorBase)

  HPDF_Font _currentFont;
  HPDF_REAL _currrentFontSize;
};


ML_END_NAMESPACE

#endif // _PDF_PDFCreatorBase_H