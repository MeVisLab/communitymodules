//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase.cpp
// \author  Axel Newe
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "MLPDF_PDFCreatorBase.h"
#include "../MLPDF_Tools.h"
#include "../PDFDocumentTools/MLPDF_PDFDocumentTools.h"

// ML includes
//#include "mlUnicode.h"

#include <Inventor/nodes/SoNodes.h>


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_ABSTRACT_MODULE_CLASS_SOURCE(PDFCreatorBase, Module);

//----------------------------------------------------------------------------------

PDFCreatorBase::PDFCreatorBase() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_mlPDFFileNameFld = addString("pdfFilename"))->setStringValue("");

  _saveFld = addNotify("save");

  (_statusFld     = addString("status"))    ->setStringValue("Idle.");
  (_successFld    = addBool("success"))     ->setBoolValue(false);
  (_progressFld   = addProgress("progress"))->setFloatValue(0.0f);

  (_pdfAttrTitleFld     = addString("pdfAttrTitle"))    ->setStringValue("");
  (_pdfAttrAuthorFld    = addString("pdfAttrAuthor"))   ->setStringValue("");
  (_pdfAttrSubjectFld   = addString("pdfAttrSubject"))  ->setStringValue("");
  (_pdfAttrKeywordsFld  = addString("pdfAttrKeywords")) ->setStringValue("");

  (_cameraCenterOfOrbitFld  = addVector3("cameraCenterOfOrbit")) ->setVector3Value(Vector3(0));
  (_cameraCenterToCameraFld = addVector3("cameraCenterToCamera"))->setVector3Value(Vector3(0));
  (_cameraRadiusOfOrbitFld  = addFloat("cameraRadiusOfOrbit"))   ->setFloatValue(0);
  (_cameraFOVAngleFld       = addFloat("cameraFOVAngle"))        ->setFloatValue(90.0f);
  (_cameraRollAngleFld      = addFloat("cameraRollAngle"))            ->setFloatValue(0);

  _calculateCameraFromInventorSceneFld   = addNotify("calculateCameraFromInventorScene");
  (_autoCalculateCameraFromInventorScene = addBool("autoCalculateCameraFromInventorScene"))->setBoolValue(false);
  (_inventorCameraPositionFld            = addVector3("inventorCameraPosition")) ->setVector3Value(Vector3(0,0,0));
  (_inventorCameraOrientationFld         = addVector4("inventorCameraOrientation")) ->setVector4Value(Vector4(0,0,1,0));
  (_inventorCameraFocalDistanceFld       = addFloat("inventorCameraFocalDistance"))->setFloatValue(0);
  (_inventorCameraHeightFld              = addFloat("inventorCameraHeight"))       ->setFloatValue(0);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

PDFCreatorBase::~PDFCreatorBase()
{
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::handleNotification(Field* field)
{
  if (field == _saveFld) 
  {
    // Call the save routine.
    saveButtonClicked();
  } 

  if (field == _calculateCameraFromInventorSceneFld) 
  {
    calculateCameraPropertiesFromInventorCamera();
  } 

  if (
      (
        (field == _autoCalculateCameraFromInventorScene) ||
        (field == _inventorCameraPositionFld) ||
        (field == _inventorCameraOrientationFld) ||
        (field == _autoCalculateCameraFromInventorScene) ||
        (field == _inventorCameraFocalDistanceFld) ||
        (field == _inventorCameraHeightFld)
      ) 
      && 
      (_autoCalculateCameraFromInventorScene->getBoolValue())
     )
  {
    calculateCameraPropertiesFromInventorCamera();
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::_initPDFDocument()
{
  pdfDocPages.clear();
  pdfDocCurrentPage = NULL;
  pdfDocument = HPDF_New(NULL, NULL);

  if (pdfDocument)
  {
    // Set PDF version
    pdfDocument->pdf_version = HPDF_VER_16;

    _initFonts();
  }
  else
  {
    _statusFld->setStringValue("Critical error: Internal PDF docuemnt could not be initilaized!");
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::_initFonts()
{
  buildInFonts.Times                = HPDF_GetFont(pdfDocument, "Times-Roman", "WinAnsiEncoding");
  buildInFonts.TimesBold            = HPDF_GetFont(pdfDocument, "Times-Bold", "WinAnsiEncoding");
  buildInFonts.TimesItalic          = HPDF_GetFont(pdfDocument, "Times-Italic", "WinAnsiEncoding");
  buildInFonts.TimesBoldItalic      = HPDF_GetFont(pdfDocument, "Times-BoldItalic", "WinAnsiEncoding");
  buildInFonts.Courier              = HPDF_GetFont(pdfDocument, "Courier", "WinAnsiEncoding");
  buildInFonts.CourierBold          = HPDF_GetFont(pdfDocument, "Courier-Bold", "WinAnsiEncoding");
  buildInFonts.CourierOblique       = HPDF_GetFont(pdfDocument, "Courier-Oblique", "WinAnsiEncoding");
  buildInFonts.CourierBoldOblique   = HPDF_GetFont(pdfDocument, "Courier-BoldOblique", "WinAnsiEncoding");
  buildInFonts.Helvetica            = HPDF_GetFont(pdfDocument, "Helvetica", "WinAnsiEncoding");
  buildInFonts.HelveticaBold        = HPDF_GetFont(pdfDocument, "Helvetica-Bold", "WinAnsiEncoding");
  buildInFonts.HelveticaOblique     = HPDF_GetFont(pdfDocument, "Helvetica-Oblique", "WinAnsiEncoding");
  buildInFonts.HelveticaBoldOblique = HPDF_GetFont(pdfDocument, "Helvetica-BoldOblique", "WinAnsiEncoding");
  buildInFonts.Symbol               = HPDF_GetFont(pdfDocument, "Symbol", "WinAnsiEncoding");
  buildInFonts.ZapfDingbats         = HPDF_GetFont(pdfDocument, "ZapfDingbats", "WinAnsiEncoding");

  _currentFont      = buildInFonts.Times;
  _currrentFontSize = 10;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::saveButtonClicked()
{
  _progressFld->setFloatValue(0.0f);
  _successFld->setBoolValue(false);

  _initPDFDocument();

  if (pdfDocument)
  {
    // Check if filename exists
    std::string filename = _mlPDFFileNameFld->getStringValue();
    if (filename == "") 
    {
      _statusFld->setStringValue("No filename specified.");
      return;
    }

    // Append ".pdf" to filename if necessary
    const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

    std::string last4 = "";

    if (filenameLength > 4) 
    { 
      last4 = filename.substr(filenameLength-4, 4); 

      if (last4 != ".pdf") 
      { 
        filename.append(".pdf"); 
        _mlPDFFileNameFld->setStringValue(filename);
      }
    }

    // Assemble PDF info properties
    std::string moduleTypeName = this->whoAmI(false);

    if (moduleTypeName.size() > 0)
    {
      moduleTypeName = "'" + moduleTypeName + "'";
    }
    else
    {
      moduleTypeName = "unknown";
    }

    // Please do not remove or modify these credits
    std::string VersionString = "MeVisLab " + getMeVisLabVersionNumberString() + " (" + moduleTypeName + " module)";
    std::string ProducerString = "MeVisLab MLPDF library (v" + getModuleVersionNumberString() + ") by Axel Newe (axel.newe@fau.de)";

    // Set PDF info attributes
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_TITLE, _pdfAttrTitleFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_AUTHOR, _pdfAttrAuthorFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_SUBJECT, _pdfAttrSubjectFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_KEYWORDS, _pdfAttrKeywordsFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_CREATOR, VersionString.c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_PRODUCER, ProducerString.c_str());

    assemblePDFDocument();

    // Save the document to a file
    HPDF_STATUS saveStatus = HPDF_SaveToFile(pdfDocument, filename.c_str());

    if (saveStatus == HPDF_OK)
    {
      _statusFld->setStringValue("PDF file sucessfully written.");
      _progressFld->setFloatValue(1.0f);
      _successFld->setBoolValue(true);
    }
    else // Possible values: HPDF_INVALID_DOCUMENT, HPDF_FAILD_TO_ALLOC_MEM, HPDF_FILE_IO_ERROR
    {
      //HPDF_FILE_IO_ERROR;
      _statusFld->setStringValue("Unable to write PDF document '" + filename + "'.");
    }

    HPDF_Free(pdfDocument);
  }
  else
  {
    _statusFld->setStringValue("Critical error: Internal PDF docuemnt could not be initialized!");
  }

  return;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docAddPage()
{
  docAddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docAddPage(float width, float height)
{
  HPDF_Page newPage = HPDF_AddPage(pdfDocument);

  if (newPage)
  {
    pdfDocCurrentPage = newPage;
    pdfDocPages.push_back(newPage);

    HPDF_Page_SetWidth(newPage, width);
    HPDF_Page_SetHeight(newPage, height);

    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docAddPage(mlPDF::PAGESIZES pageSize, mlPDF::PAGEDIRECTIONS pageDirection)
{
  HPDF_Page newPage = HPDF_AddPage(pdfDocument);

  if (newPage)
  {
    pdfDocCurrentPage = newPage;
    pdfDocPages.push_back(newPage);

    HPDF_Page_SetSize(newPage, (HPDF_PageSizes)pageSize, (HPDF_PageDirection)pageDirection);

    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docSetCurrentFont(HPDF_Font font)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, _currrentFontSize);
    _currentFont = font;
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docSetCurrentFont(HPDF_Font font, float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, (HPDF_REAL)fontSize);
    _currentFont      = font;
    _currrentFontSize = fontSize;
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docSetCurrentFontSize(float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, (HPDF_REAL)fontSize);
    _currrentFontSize = fontSize;
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::docWriteTextAt(float x, float y, std::string text)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_BeginText(pdfDocCurrentPage);
    HPDF_Page_MoveTextPos(pdfDocCurrentPage, (HPDF_REAL)x, (HPDF_REAL)y);
    HPDF_Page_ShowText(pdfDocCurrentPage, text.c_str());
    HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::doc3DLoadModelDatasFromFfile()
{
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::doc3DCreateView()
{
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::doc3DAddView(mlPDF::PDF3DSCENE scene, mlPDF::PDF3DVIEW view)
{
  if (scene && view)
  {
    HPDF_U3D_Add3DView(scene, view);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::doc3DSetDefaultView(mlPDF::PDF3DSCENE scene, std::string viewName)
{
  if ((scene) && (viewName.size() > 0))
  {
    HPDF_U3D_SetDefault3DView(scene, viewName.c_str());
  }
}

//----------------------------------------------------------------------------------

mlPDF::PDF3DANNOTATION PDFCreatorBase::doc3DCreateAnnotation(float left, float bottom, float right, float top, mlPDF::PDF3DSCENE scene)
{
  HPDF_Annotation newAnnotation = NULL;

  if (pdfDocCurrentPage)
  {
    HPDF_Rect annotationRect = { left, bottom, right, top };
    newAnnotation = HPDF_Page_Create3DAnnot(pdfDocCurrentPage, annotationRect, scene); 
  }

  return newAnnotation;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::doc3DSetActivationProperties(mlPDF::PDF3DANNOTATION annotation, std::string activationCode, std::string deactivationCode, bool toolbarEnabled, bool navigationInterfaceOpened, bool animationAutoStart)
{
  if (annotation)
  {
    HPDF_Dict activationDict = HPDF_Create3DActivation(annotation);

    if (activationDict)
    {
      HPDF_3DActivation_SetActivationMode(activationDict, activationCode.c_str());
      HPDF_3DActivation_SetDeactivationMode(activationDict, deactivationCode.c_str());
      HPDF_3DActivation_SetAnimationAutoStart(activationDict, animationAutoStart);
      HPDF_3DActivation_SetToolbarEnabled(activationDict, toolbarEnabled);
      HPDF_3DActivation_SetNavigationInterfaceOpened(activationDict, navigationInterfaceOpened);

      HPDF_U3D_Set3DActivation(annotation, activationDict);
    }
  }

}

//----------------------------------------------------------------------------------




//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------

void PDFCreatorBase::calculateCameraPropertiesFromInventorCamera()
{
  Vector3 inventorCameraPosition      = _inventorCameraPositionFld->getVectorValue();
  Vector4 inventorCameraOrientation   = _inventorCameraOrientationFld->getVectorValue();
  float   inventorCameraFocalDistance = _inventorCameraFocalDistanceFld->getFloatValue();
  float   inventorCameraHeight        = _inventorCameraHeightFld->getFloatValue();
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateCameraPropertiesFromInventorCamera(
    inventorCameraPosition, inventorCameraOrientation, inventorCameraFocalDistance, inventorCameraHeight,
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  _cameraCenterOfOrbitFld ->setVector3Value(camCenterOfOrbit);
  _cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  _cameraRadiusOfOrbitFld ->setFloatValue(camRadiusOfOrbit);
  _cameraRollAngleFld     ->setFloatValue(camRollAngle);
  _cameraFOVAngleFld      ->setFloatValue(camFOVAngle);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::calculateDefaultCameraProperties()
{
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateDefaultCameraProperties(
    // Inputs
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  _cameraCenterOfOrbitFld ->setVector3Value(camCenterOfOrbit);
  _cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  _cameraRadiusOfOrbitFld ->setFloatValue(camRadiusOfOrbit);
  _cameraRollAngleFld     ->setFloatValue(camRollAngle);
  _cameraFOVAngleFld      ->setFloatValue(camFOVAngle);
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE