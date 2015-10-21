//----------------------------------------------------------------------------------
//! The ML module class Save3DFigurePDF.
/*!
// \file    Save3DFigurePDF.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-09-25
//
// Creates PDF file with one interactive 3D scene for scientific publications.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "Save3DFigurePDF.h"
#include "../shared/MLPDF_Tools.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"

// ThirdParty includes
#include "hpdf.h"
#include "hpdf_u3d.h"

// ML includes
#include "mlUnicode.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(Save3DFigurePDF, Module);

//----------------------------------------------------------------------------------

Save3DFigurePDF::Save3DFigurePDF() : PDFCreatorBase()
{

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_u3dFilenameFld         = addString("u3dFilename"))->setStringValue("");
  (_posterImageFilenameFld = addString("posterImageFilename"))->setStringValue("");

  (_pageHeaderCitationTextFld = addString("pageHeaderCitationText"))->setStringValue("");
  (_pageHeaderHeadlineTextFld = addString("pageHeaderHeadlineText"))->setStringValue("");
  (_includeUsageHintsFld      = addBool("includeUsageHints"))->setBoolValue(true);

  (_figureActivationModeFld             = addEnum("figureActivationMode", mlPDF::ACTIVATION_MODE_STRINGS, mlPDF::NUM_ACTIVATIONMODES))->setEnumValue(mlPDF::ACTIVATION_MODE_EXPLICIT_ACTIVATE);
  (_figureDeactivationModeFld           = addEnum("figureDeactivationMode", mlPDF::DEACTIVATION_MODE_STRINGS, mlPDF::NUM_DEACTIVATIONMODES))->setEnumValue(mlPDF::DEACTIVATION_MODE_EXPLICIT_DEACTIVATE);
  (_figureAnimationAutostartFld         = addBool("figureAnimationAutostart"))->setBoolValue(false);
  (_figureToolbarEnabledFld             = addBool("figureToolbarEnabled"))->setBoolValue(true);
  (_figureNavigationInterfaceEnabledFld = addBool("figureNavigationInterfaceEnabled"))->setBoolValue(false);

  (_viewBackgroundColorFld = addColor("viewBackgroundColor"))->setVector3Value(Vector3(0.8,0.8,0.8));

  (_captionFld = addString("caption"))->setStringValue("");
  (_descriptionFld = addString("description"))->setStringValue("");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

Save3DFigurePDF::~Save3DFigurePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::handleNotification(Field* field)
{
  // Call super class functionality
  PDFCreatorBase::handleNotification(field);
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::assemblePDFDocument()
{
  //HPDF_Page pdfPage;
  float yPos = 0;
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
  pdfDoc_SetGlobalPageMarginsMM(20, 20, 20, 20);

  pdfDoc_SetCurrentFont(buildInFonts.Times, 11);
  pdfDoc_WriteTextAt(0, yPos, _pageHeaderCitationTextFld->getStringValue()/*, mlPDF::IGNORE_MARGINS*/);

  yPos += 25;
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 12);
  pdfDoc_WriteTextAt(0, yPos, _pageHeaderHeadlineTextFld->getStringValue());

  if (_includeUsageHintsFld->getBoolValue())
  {
    yPos += 35;
    pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
    pdfDoc_WriteTextAt(0, yPos, "Figure is best viewed with Adobe Reader 9 or later");

    pdfDoc_SetCurrentFont(buildInFonts.Times, 9);
    yPos += 15;
    pdfDoc_WriteTextAt(0, yPos, "Click image to enable interactive mode.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "Left-click & move mouse to rotate scene.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "Right-click & move mouse to zoom.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "Both-click and move mouse to pan.");
  }

  yPos += 20;
  float sceneWidth  = pdfDoc_GetPageMaxWidth();
  float sceneHeight = (float)(pdfDoc_GetPageMaxHeight()/2.0);
  _add3DFigure(0, yPos, sceneWidth, sceneHeight);

  yPos += sceneHeight + 5;
  pdfDoc_SetCurrentFont(buildInFonts.TimesBold, 10);
  pdfDoc_WriteTextAt(0, yPos, _captionFld->getStringValue());
  
  yPos += 12;
  float descriptionBoxHeight = pdfDoc_GetPageRemainingHeightFromTop(yPos);
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), descriptionBoxHeight, _descriptionFld->getStringValue());
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::_add3DFigure(float x, float y, float width, float height)
{
  HPDF_U3D u3dModel = pdfDoc_Load3DModelDataFromFile(_u3dFilenameFld->getStringValue());

  mlPDF::VIEW3D defaultView = pdfDoc_3DModel_CreateView("Default View", mlPDF::LIGHTING_SCHEME_HEADLAMP, 0.8f, 0.8f, 0.8f);
  //pdfDoc_3DView_AddVisibleNode(defaultView, "Cow Mesh red", 0.5, true);
  //pdfDoc_3DView_AddVisibleNode(defaultView, "Point Set Cow", 0.5, true);
  pdfDoc_3DView_SetPerspectiveCamera(defaultView, cameraCenterOfOrbitFld->getVectorValue(), cameraRadiusOfOrbitFld->getFloatValue(), cameraCenterToCameraFld->getVectorValue(), cameraFOVAngleFld->getFloatValue(), cameraRollAngleFld->getFloatValue());
  pdfDoc_3DModel_AddView(u3dModel, defaultView);

  mlPDF::VIEW3D altView = pdfDoc_3DModel_CreateView("Alternative View", "AltView", mlPDF::LIGHTING_SCHEME_CAD, 0.1f, 0.1f, 0.1f);
  pdfDoc_3DView_SetPerspectiveCamera(altView, cameraCenterOfOrbitFld->getVectorValue(), cameraRadiusOfOrbitFld->getFloatValue(), cameraCenterToCameraFld->getVectorValue(), cameraFOVAngleFld->getFloatValue(), cameraRollAngleFld->getFloatValue());
  pdfDoc_3DModel_AddView(u3dModel, altView);

  pdfDoc_3DModel_SetDefaultView(u3dModel, altView);

  mlPDF::IMAGE posterImage = pdfDoc_LoadImageFromFile(_posterImageFilenameFld->getStringValue());
  mlPDF::SCENE3D u3dScene = pdfDoc_Add3DScene(x, y, width, height, u3dModel, posterImage);
  pdfDoc_3DScene_SetActivationProperties(u3dScene, _figureActivationModeFld->getStringValue(), _figureDeactivationModeFld->getStringValue(), _figureToolbarEnabledFld->getBoolValue(),_figureNavigationInterfaceEnabledFld->getBoolValue(), _figureAnimationAutostartFld->getBoolValue());
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE