//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_SavePDF.
/*!
// \file    PDF3DFigurePage_SavePDF.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDF3DFigurePage_SavePDF.h"
#include "../shared/MLPDF_Tools.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"

// ThirdParty includes
#include "hpdf.h"
#include "hpdf_u3d.h"

// ML includes
#include "mlUnicode.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(PDF3DFigurePage_SavePDF, Module);

//----------------------------------------------------------------------------------

PDF3DFigurePage_SavePDF::PDF3DFigurePage_SavePDF() : PDFCreatorBase()
{

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_u3dFilenameFld         = addString("u3dFilename"))->setStringValue("");
  (_posterImageFilenameFld = addString("posterImageFilename"))->setStringValue("");

  (_pageHeaderCitationTextFld = addString("pageHeaderCitationText"))->setStringValue("");
  (_pageHeaderHeadlineTextFld = addString("pageHeaderHeadlineText"))->setStringValue("");
  (_includeUsageHintsFld      = addBool("includeUsageHints"))->setBoolValue(true);

  (_figureActivationModeFld             = addEnum("figureActivationMode", mlPDF::ActivationModeStrings, mlPDF::NUM_ACTIVATIONMODES))->setEnumValue(mlPDF::ACTIVATION_MODE_EXPLICIT_ACTIVATE);
  (_figureDeactivationModeFld           = addEnum("figureDeactivationMode", mlPDF::DeactivationModeStrings, mlPDF::NUM_DEACTIVATIONMODES))->setEnumValue(mlPDF::DEACTIVATION_MODE_EXPLICIT_DEACTIVATE);
  (_figureAnimationAutostartFld         = addBool("figureAnimationAutostart"))->setBoolValue(false);
  (_figureToolbarEnabledFld             = addBool("figureToolbarEnabled"))->setBoolValue(true);
  (_figureNavigationInterfaceEnabledFld = addBool("figureNavigationInterfaceEnabled"))->setBoolValue(false);

  (_captionFld = addString("caption"))->setStringValue("");
  (_descriptionFld = addString("description"))->setStringValue("");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDF3DFigurePage_SavePDF::~PDF3DFigurePage_SavePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_SavePDF::handleNotification(Field* field)
{
  // Call super class functionality
  PDFCreatorBase::handleNotification(field);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_SavePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_SavePDF::assemblePDFDocument()
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
  float descriptionBoxHeight = pdfDoc_GetPageRemainingHeight(yPos);
  pdfDoc_SetCurrentFont(buildInFonts.Times, 10);
  pdfDoc_WriteTextAreaAt(0, yPos, pdfDoc_GetPageMaxWidth(), descriptionBoxHeight, _descriptionFld->getStringValue(), mlPDF::TEXTALIGNMENT_LEFT);
}

//----------------------------------------------------------------------------------

void PDF3DFigurePage_SavePDF::_add3DFigure(float x, float y, float width, float height)
{
  HPDF_U3D u3dModel = pdfDoc_Load3DModelDataFromFile(_u3dFilenameFld->getStringValue());

  pdfDoc_3DModel_AddAllViewsFromSpecificationString(u3dModel, viewSpecificationsFld->getStringValue());

  mlPDF::IMAGE posterImage = pdfDoc_LoadImageFromFile(_posterImageFilenameFld->getStringValue());
  mlPDF::SCENE3D u3dScene = pdfDoc_Add3DScene(x, y, width, height, u3dModel, posterImage);
  pdfDoc_3DScene_SetActivationProperties(u3dScene, _figureActivationModeFld->getStringValue(), _figureDeactivationModeFld->getStringValue(), _figureToolbarEnabledFld->getBoolValue(),_figureNavigationInterfaceEnabledFld->getBoolValue(), _figureAnimationAutostartFld->getBoolValue());
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE