//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase.h
// \author  Axel Newe (axel.newe@fau.de)
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

  //------------------------------------
  // Methods for assembling PDF document
  //------------------------------------

  // Document handling
  void pdfDoc_SetYAxisReference(bool reference);
  const bool pdfDoc_GetYAxisReference();
  void pdfDoc_StoreYAxisReference();
  void pdfDoc_RestoreYAxisReference();
  void pdfDoc_SetDefaultYAxisReference(bool reference);
  void pdfDoc_RestoreDefaultYAxisReference();

  // Page handling
  void pdfDoc_AddPage();
  void pdfDoc_AddPage(float width, float height);
  void pdfDoc_AddPage(mlPDF::PAGESIZES pageSize, mlPDF::PAGEDIRECTIONS pageDirection);
  void pdfDoc_SetGlobalPageMarginsMM(float leftMargin, float topMargin, float rightMargin, float bottomMargin);
  void pdfDoc_SetGlobalPageMarginsInch(float leftMargin, float topMargin, float rightMargin, float bottomMargin);
  void pdfDoc_SetGlobalPageMarginsPixels(float leftMargin, float topMargin, float rightMargin, float bottomMargin);
  const float pdfDoc_GetPageMinX(bool ignoreMargins = false);
  const float pdfDoc_GetPageMinY(bool ignoreMargins = false);
  const float pdfDoc_GetPageMaxX(bool ignoreMargins = false);
  const float pdfDoc_GetPageMaxY(bool ignoreMargins = false);
  const float pdfDoc_GetPageMaxWidth(bool ignoreMargins = false);
  const float pdfDoc_GetPageMaxHeight(bool ignoreMargins = false);
  const float pdfDoc_GetPageRemainingHeight(float yPos, bool ignoreMargins = false);
  const float pdfDoc_GetPageRemainingHeightFromTop(float yPos, bool ignoreMargins = false);

  // Text and font handling
  void pdfDoc_SetCurrentFont(HPDF_Font font);
  void pdfDoc_SetCurrentFont(HPDF_Font font, float fontSize);
  void pdfDoc_SetCurrentFontSize(float fontSize);
  const float pdfDoc_GetCurrentFontHeight();
  void pdfDoc_WriteTextAt(float x, float y, std::string text, bool ignoreMargins = false);
  void pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, bool ignoreMargins = false);

  // Resources handling
  mlPDF::IMAGE   pdfDoc_LoadImageFromFile(std::string fileName);
  mlPDF::MODEL3D pdfDoc_Load3DModelDataFromFile(std::string fileName);

  // Image handling
  void pdfDoc_AddImage(float x, float y, float width, float height, mlPDF::IMAGE image, bool ignoreMargins = false);
  void pdfDoc_AddImage(float x, float y, float width, float height, std::string imageFilename, bool ignoreMargins = false);

  // 3D scenes handling
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal = "");
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor);
  void pdfDoc_3DView_SetLightingScheme(mlPDF::VIEW3D view, mlPDF::LIGHTING_SCHEMES lightingScheme);
  void pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, float r, float g, float b);
  void pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, Vector3 color);
  void pdfDoc_3DView_SetPerspectiveCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float fieldOfView, float cameraRollDegrees = 0);
  void pdfDoc_3DView_SetOrthogonalCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float scaleFactor, float cameraRollDegrees = 0);
  void pdfDoc_3DView_AddVisibleNode(mlPDF::VIEW3D view, std::string nodeName, float transparency, bool visibility);
  void pdfDoc_3DModel_AddView(mlPDF::MODEL3D model, mlPDF::VIEW3D view);
  void pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, std::string viewName);
  void pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, mlPDF::VIEW3D view);
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, bool ignoreMargins = false);
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, mlPDF::IMAGE posterImage, bool ignoreMargins = false);
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, std::string posterFilename, bool ignoreMargins = false);
  void pdfDoc_3DScene_SetActivationProperties(mlPDF::SCENE3D scene, std::string activationCode, std::string deactivationCode, bool toolbarEnabled = false, bool navigationInterfaceOpened = false, bool animationAutoStart = false);

  //------------------------------------
  // Tool methods 
  //------------------------------------

  //! Calculate PDF camera properties
  void calculateCameraPropertiesFromInventorCamera();
  void calculateDefaultCameraProperties();

  /* FIELDS */

  //! Field - Output filename
  StringField   *mlPDFFileNameFld;

  //! Field - Save button notification
  NotifyField   *saveFld;

  //! Status message
  StringField   *statusFld;
  BoolField*     successFld;

  //! Progress bar.
  ProgressField *progressFld;

  //! PDF file property fields
  StringField   *pdfAttrTitleFld;
  StringField   *pdfAttrAuthorFld;
  StringField   *pdfAttrSubjectFld;
  StringField   *pdfAttrKeywordsFld;

  //! PDF view camera fields
  Vector3Field*  cameraCenterOfOrbitFld;
  Vector3Field*  cameraCenterToCameraFld;
  FloatField*    cameraRadiusOfOrbitFld;
  FloatField*    cameraRollAngleFld;
  FloatField*    cameraFOVAngleFld;

  //! Inventor camera fields (needed for calculation of PDF view camera from Inventor camera settings)
  NotifyField*   calculateCameraFromInventorSceneFld;
  BoolField*     autoCalculateCameraFromInventorScene;
  Vector3Field*  inventorCameraPositionFld;
  Vector4Field*  inventorCameraOrientationFld;
  FloatField*    inventorCameraFocalDistanceFld;
  FloatField*    inventorCameraHeightFld;

  //! The main document container
  HPDF_Doc  pdfDocument;

  //! Document content containersk
  std::vector<mlPDF::PAGE>    pdfDocPages;
  mlPDF::PAGE                 pdfDocCurrentPage;
  std::vector<mlPDF::IMAGE>   pdfDocImages;
  std::vector<mlPDF::MODEL3D> pdfDoc3DScenes;
  
  //! General PDF properties
  mlPDF::BuiltInFontsStruct buildInFonts;
  mlPDF::PageSizeInfoStruct pageSizeInfo;

private:
  
  //! Initialize internal PDF document container
  void _initPDFDocument();
  void _initFonts();

  // Get current font and properties
  HPDF_Font _currentFont;
  HPDF_REAL _currrentFontSize;
  HPDF_REAL _currentFontHeight;

  // Vertical reference direction
  bool _defaultYAxisReferenceIsFromTop;
  bool _currentYAxisReferenceIsFromTop;
  bool _previousYAxisReferenceIsFromTop;

  // Internal tool methods
  const float     _getYPosFromTop(float y, bool ignoreMargins = false);
  const HPDF_Rect _getPageRect(float x, float y, float width, float height, bool ignoreMargins = false);
  const float     _getPageX(float x, bool ignoreMargins = false);
  const float     _getPageY(float y, bool ignoreMargins = false);
  const HPDF_REAL _getFontHeight(HPDF_Font& font, HPDF_REAL size);

  // Page margins
  float _globalPageMarginLeft;
  float _globalPageMarginRight;
  float _globalPageMarginTop;
  float _globalPageMarginBottom;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFCreatorBase)

};


ML_END_NAMESPACE

#endif // _PDF_PDFCreatorBase_H