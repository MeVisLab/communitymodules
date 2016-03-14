//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    PDFGenerator.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_PDFGenerator_H
#define _PDF_PDFGenerator_H 


// Local includes
#include "MLPDFSystem.h"
#include"../../shared/MLPDF_Defines.h"

// ML includes
#include <mlModuleIncludes.h>

// Include all libhpdf headers
#include <hpdf.h>
#include <hpdf_3dmeasure.h>
#include <hpdf_annotation.h>
#include <hpdf_catalog.h>
#include <hpdf_conf.h>
#include <hpdf_consts.h>
#include <hpdf_destination.h>
#include <hpdf_doc.h>
#include <hpdf_encoder.h>
#include <hpdf_encrypt.h>
#include <hpdf_encryptdict.h>
#include <hpdf_error.h>
#include <hpdf_exdata.h>
#include <hpdf_ext_gstate.h>
#include <hpdf_font.h>
#include <hpdf_fontdef.h>
#include <hpdf_gstate.h>
#include <hpdf_image.h>
#include <hpdf_info.h>
#include <hpdf_list.h>
#include <hpdf_mmgr.h>
#include <hpdf_namedict.h>
#include <hpdf_objects.h>
#include <hpdf_outline.h>
#include <hpdf_page_label.h>
#include <hpdf_pages.h>
#include <hpdf_pdfa.h>
#include <hpdf_streams.h>
#include <hpdf_types.h>
#include <hpdf_u3d.h>
#include <hpdf_utils.h>
#include <hpdf_version.h>


ML_START_NAMESPACE


//! 
class MLPDF_EXPORT PDFGenerator : public Module
{
public:

  //! Constructor.
  PDFGenerator();

protected:

  //! Destructor.
  virtual ~PDFGenerator();

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
  //! Returns true if assembly was successful, false otherwise.
  virtual bool assemblePDFDocument() = 0;

  //====================================
  // Methods for assembling PDF document
  //====================================

  //------------------
  // Document handling
  //------------------

  //! Set the compression mode
  void pdfDoc_SetCompressionMode(const unsigned int compressionFlags);

  //! Set the password
  void pdfDoc_SetPasswords(std::string editPassword, std::string openPassword = "");

  //! Set permissions
  //! Attention: A password must be set PRIOR to setting permissions.
  //! If no password has been set when pdfDoc_SetPermissions() is invoked,
  //! a random password of 64 characters (512 bit) length is set!
  void pdfDoc_SetPermissions(const unsigned int permissionFlags);

  //! Set the y-axis reference (i.e., where the y-axis zero coordinate shall be: at the top of the page or at the bottom).
  void pdfDoc_SetYAxisReference(bool reference);

  //! Get the y-axis reference (i.e., where the y-axis zero coordinate is: at the top of the page or at the bottom).
  bool pdfDoc_GetYAxisReference() const;

  //! Store the current y-axis reference to a stack buffer.
  void pdfDoc_StoreYAxisReference();

  //! Restore the y-axis reference from stack buffer. Returns default value (mlPDF::YAXIS_REFERENCE_DEFAULT) if buffer is empty.
  void pdfDoc_RestoreYAxisReference();

  //! Specify a document-global default value for the y-axis reference.
  void pdfDoc_SetDefaultYAxisReference(bool reference);

  //! Restore the y-axis reference to document-global default value.
  void pdfDoc_RestoreDefaultYAxisReference();

  //--------------
  // Page handling
  //--------------

  //! Add a page to the document and to the global page pool (pdfDocCurrentPage property). Also sets the pdfDocCurrentPage property to the new page.
  void pdfDoc_AddPage();
  void pdfDoc_AddPage(float width, float height);
  void pdfDoc_AddPage(mlPDF::PAGE_SIZES pageSize, mlPDF::PAGE_DIRECTIONS pageDirection);

  //! Set  the page margins.
  void pdfDoc_SetGlobalPageMarginsMM(float leftMargin, float topMargin, float rightMargin, float bottomMargin);
  void pdfDoc_SetGlobalPageMarginsInch(float leftMargin, float topMargin, float rightMargin, float bottomMargin);
  void pdfDoc_SetGlobalPageMarginsPixels(float leftMargin, float topMargin, float rightMargin, float bottomMargin);

  //! Get minimum value for x coordinate of current page.
  float pdfDoc_GetPageMinX(bool ignoreMargins = false) const;

  //! Get minimum value for y coordinate of current page.
  float pdfDoc_GetPageMinY(bool ignoreMargins = false) const;

  //! Get maximum value for x coordinate of current page.
  float pdfDoc_GetPageMaxX(bool ignoreMargins = false) const;

  //! Get maximum value for x coordinate of current page.
  float pdfDoc_GetPageMaxY(bool ignoreMargins = false) const;

  //! Get x coordinate of center of current page.
  float pdfDoc_GetPageCenterX(bool ignoreMargins = false) const;

  //! Get y coordinate of center of current page.
  float pdfDoc_GetPageCenterY(bool ignoreMargins = false) const;

  //! Get maximum width of current page.
  float pdfDoc_GetPageMaxWidth(bool ignoreMargins = false) const;

  //! Get maximum height of current page.
  float pdfDoc_GetPageMaxHeight(bool ignoreMargins = false) const;

  //! Get remaining page height from given y coordinate.
  float pdfDoc_GetPageRemainingHeight(float yPos, bool ignoreMargins = false) const;

  //-----------------------
  // Text and font handling
  //-----------------------

  //! Set current font.
  void pdfDoc_SetCurrentFont(HPDF_Font font);
  void pdfDoc_SetCurrentFont(HPDF_Font font, float fontSize);
  void pdfDoc_SetCurrentFontSize(float fontSize);

  //! Get current font height.
  float pdfDoc_GetCurrentFontHeight() const;

  //! Write a single line of text at the specified position.
  void pdfDoc_WriteTextAt(float x, float y, std::string text, bool ignoreMargins = false);
  void pdfDoc_WriteTextAt(float x, float y, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins = false);

  //! Write text into a specified area.
  void pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, bool ignoreMargins = false);
  void pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_ALIGNMENTS alignment, bool ignoreMargins = false);
  void pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins = false);
  void pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_RENDERMODES renderMode, mlPDF::TEXT_ALIGNMENTS alignment, bool ignoreMargins = false);

  //-------------------
  // Resources handling
  //-------------------

  //! Load image (PNG or JPG format) from file and add it to global image pool (pdfDocImages property).
  mlPDF::IMAGE   pdfDoc_LoadImageFromFile(std::string fileName);

  //! Load 3D model (U3D format) from file and add it to global model pool (pdfDoc3DScenes property).
  mlPDF::MODEL3D pdfDoc_Load3DModelDataFromFile(std::string fileName);

  //---------------
  // Image handling
  //---------------

  //! Add image at the specified position and with the specified size.
  void pdfDoc_AddImage(float x, float y, float width, float height, mlPDF::IMAGE image, bool ignoreMargins = false);
  void pdfDoc_AddImage(float x, float y, float width, float height, std::string imageFilename, bool ignoreMargins = false);

  //-------------------
  // 3D scenes handling
  //-------------------

  //! Create a 3D view for a 3D model.
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal = "");
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor);
  mlPDF::VIEW3D pdfDoc_3DModel_CreateViewFromSpecificationString(std::string specificationString);

  //! Set lighting scheme property for 3D view.
  void pdfDoc_3DView_SetLightingScheme(mlPDF::VIEW3D view, mlPDF::LIGHTING_SCHEMES lightingScheme);

  //! Set background color property for 3D view.
  void pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, float r, float g, float b);
  void pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, Vector3 color);

  //! Set render mode for 3D view.
  void pdfDoc_3DView_SetRenderMode(mlPDF::VIEW3D view, mlPDF::MODEL_RENDERMODES renderMode);

  //! Set camera for 3D view.
  void pdfDoc_3DView_SetPerspectiveCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float fieldOfView, float cameraRollDegrees = 0);
  void pdfDoc_3DView_SetOrthogonalCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float scaleFactor, float cameraRollDegrees = 0);

  //! Add model node property to a 3D view.
  void pdfDoc_3DView_AddVisibleNode(mlPDF::VIEW3D view, std::string nodeName, float transparency, bool visibility);
  void pdfDoc_3DView_AddVisibleNodeFromSpecificationString(mlPDF::VIEW3D view, std::string specificationString);
  void pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(mlPDF::VIEW3D view, std::string specificationString);

  //! Add a 3D views to 3D model.
  void pdfDoc_3DModel_AddView(mlPDF::MODEL3D model, mlPDF::VIEW3D view);
  void pdfDoc_3DModel_AddViewFromSpecificationString(mlPDF::MODEL3D model, std::string specificationString);
  void pdfDoc_3DModel_AddAllViewsFromSpecificationString(mlPDF::MODEL3D model, std::string specificationString);

  //! Set the default 3D view of a 3D model.
  void pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, std::string viewName);
  void pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, mlPDF::VIEW3D view);

  //! Add a 3D scene to the document. If a poster image is specified, this image will be added to the image pool (pdfDocImages property).
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, bool ignoreMargins = false);
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, mlPDF::IMAGE posterImage, bool ignoreMargins = false);
  mlPDF::SCENE3D pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, std::string posterFilename, bool ignoreMargins = false);

  //! Set the activation properties of a 3D scene.
  void pdfDoc_3DScene_SetActivationProperties(mlPDF::SCENE3D scene, std::string activationCode, std::string deactivationCode, bool toolbarEnabled = false, bool navigationInterfaceOpened = false, bool animationAutoStart = false);

  //------------------
  // Graphics handling
  //------------------

  //! Set the stroke color for lines.
  void pdfDoc_SetStrokeColor(float r, float g, float b);
  void pdfDoc_SetStrokeColor(Vector3 color);

  //! Set the fill color.
  void pdfDoc_SetFillColor(float r, float g, float b);
  void pdfDoc_SetFillColor(Vector3 color);

  //! Add a line drawing.
  void pdfDoc_AddLine(float startX, float startY, float endX, float endY, float lineWidth, bool ignoreMargins = false);

  //! Add an arc drawing.
  void pdfDoc_AddOutlineArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlineArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddOutlineArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlineArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);

  //! Add a pie drawing.
  void pdfDoc_AddOutlinePieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledPieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlinePieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddOutlinePieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledPieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlinePieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins = false);

  //! Add a circle drawing.
  void pdfDoc_AddOutlineCircle(float centerX, float centerY, float radius, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledCircle(float centerX, float centerY, float radius, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlineCircle(float centerX, float centerY, float radius, float lineWidth, bool ignoreMargins = false);

  //! Add an ellipse drawing.
  void pdfDoc_AddOutlineEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlineEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineWidth, bool ignoreMargins = false);

  //! Add a rectangle drawing.
  void pdfDoc_AddOutlineRectangle(float x, float y, float width, float height, float lineWidth, bool ignoreMargins = false);
  void pdfDoc_AddFilledRectangle(float x, float y, float width, float height, bool ignoreMargins = false);
  void pdfDoc_AddFilledOutlineRectangle(float x, float y, float width, float height, float lineWidth, bool ignoreMargins = false);

  /* FIELDS */

  //! Field - Output filename
  StringField   *pdfFilenameFld;

  //! Field - Output filename
  StringField   *resourcesPathFld;

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

  //! View specifiactions field
  StringField*   viewSpecificationsFld;

  //! The main document container
  HPDF_Doc  pdfDocument;

  //! Document content containers
  std::vector<mlPDF::PAGE>    pdfDocPages;
  mlPDF::PAGE                 pdfDocCurrentPage;
  std::vector<mlPDF::IMAGE>   pdfDocImages;
  std::vector<mlPDF::MODEL3D> pdfDoc3DScenes;

  //! General PDF properties
  mlPDF::BuiltInFontsStruct buildInFonts;
  mlPDF::PageSizeInfoStruct pageSizeInfo;

  //! Assembly error message container
  std::string assemblyErrorMessage;

  //! Helper variable that stores the current x pos for placing elements.
  //! Is automatically set to zero if a new page starts.
  float pdfDoc_CurrentXPos;

  //! Helper variable that stores the current y pos for placing elements.
  //! Is automatically set to zero if a new page starts.
  float pdfDoc_CurrentYPos;

private:
  
  // Initialize internal PDF document container
  void _initPDFDocument();
  void _initFonts();

  // Get current font and properties
  HPDF_Font _currentFont;
  HPDF_REAL _currrentFontSize;
  HPDF_REAL _currentFontHeight;

  // Vertical reference direction
  bool _currentYAxisReferenceIsFromTop;
  bool _defaultYAxisReferenceIsFromTop;
  std::vector<bool> _previousYAxisReferenceIsFromTop;

  // Internal tool methods
  void              _checkCoordinate(float& smaller, float& larger);
  void              _checkAngle(float& startAngle, float& endAngle);
  float             _getYPosFromTop(float y, bool ignoreMargins = false) const;
  HPDF_Rect         _getPageRect(float x, float y, float width, float height, bool ignoreMargins = false) const;
  float             _getPageX(float x, bool ignoreMargins = false) const;
  float             _getPageY(float y, bool ignoreMargins = false) const;
  HPDF_REAL         _getFontHeight(HPDF_Font& font, HPDF_REAL size) const;
  std::string       _getRandomPassword(const unsigned int passwordLength) const;

  // Page margins
  float _globalPageMarginLeft;
  float _globalPageMarginRight;
  float _globalPageMarginTop;
  float _globalPageMarginBottom;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFGenerator)

};


ML_END_NAMESPACE

#endif // _PDF_PDFGenerator_H
