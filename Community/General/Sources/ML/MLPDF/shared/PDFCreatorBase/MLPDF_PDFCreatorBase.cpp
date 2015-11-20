//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase.cpp
// \author  Axel Newe (axel.newe@fau.de)
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

  (pdfFilenameFld = addString("pdfFilename"))->setStringValue("");
  (resourcesPathFld = addString("resourcesPath"))->setStringValue("");

  saveFld = addNotify("save");

  (statusFld     = addString("status"))    ->setStringValue("Idle.");
  (successFld    = addBool("success"))     ->setBoolValue(false);
  (progressFld   = addProgress("progress"))->setFloatValue(0.0f);

  (pdfAttrTitleFld     = addString("pdfAttrTitle"))    ->setStringValue("");
  (pdfAttrAuthorFld    = addString("pdfAttrAuthor"))   ->setStringValue("");
  (pdfAttrSubjectFld   = addString("pdfAttrSubject"))  ->setStringValue("");
  (pdfAttrKeywordsFld  = addString("pdfAttrKeywords")) ->setStringValue("");

  (viewSpecificationsFld = addString("viewSpecifications"))->setStringValue("");

  assemblyErrorMessage == "";

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
  if (field == saveFld) 
  {
    // Call the save routine.
    saveButtonClicked();
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
  pdfDocImages.clear();
  pdfDoc3DScenes.clear();
  pdfDocCurrentPage = NULL;
  _currentYAxisReferenceIsFromTop  = mlPDF::YAXIS_REFERENCE_DEFAULT;
  _defaultYAxisReferenceIsFromTop  = mlPDF::YAXIS_REFERENCE_DEFAULT;
  _previousYAxisReferenceIsFromTop.clear();

  pdfDocument = HPDF_New(NULL, NULL);

  if (pdfDocument)
  {
    // Set PDF version
    pdfDocument->pdf_version = HPDF_VER_16;

    _initFonts();
    pdfDoc_SetGlobalPageMarginsPixels(0,0,0,0);
  }
  else
  {
    statusFld->setStringValue("Critical error: Internal PDF docuemnt could not be initilaized!");
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
  progressFld->setFloatValue(0.0f);
  successFld->setBoolValue(false);

  _initPDFDocument();

  if (pdfDocument)
  {
    // Check if filename exists
    std::string filename = pdfFilenameFld->getStringValue();
    if (filename == "") 
    {
      statusFld->setStringValue("No filename specified.");
      return;
    }

    // Append ".pdf" to filename if necessary
    const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

    std::string last4 = "";

    if (filenameLength > 4) 
    { 
      last4 = filename.substr(filenameLength-4, 4); 

      mlPDF::PDFTools::stringLower(last4);

      if (last4 != ".pdf") 
      { 
        filename.append(".pdf"); 
        pdfFilenameFld->setStringValue(filename);
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
    std::string VersionString = "MeVisLab " + mlPDF::PDFTools::getMeVisLabVersionNumberString() + " (" + moduleTypeName + " module)";
    std::string ProducerString = "MeVisLab MLPDF library (v" + mlPDF::PDFTools::getModuleVersionNumberString() + ") by Axel Newe (axel.newe@fau.de)";

    // Set PDF info attributes
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_TITLE, pdfAttrTitleFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_AUTHOR, pdfAttrAuthorFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_SUBJECT, pdfAttrSubjectFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_KEYWORDS, pdfAttrKeywordsFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_CREATOR, VersionString.c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_PRODUCER, ProducerString.c_str());

    bool docAssembled = false;

    try
    {
      docAssembled = assemblePDFDocument();
    }
    catch(...)
    {
      // Do not handle errors.
    }

    if (docAssembled)
    {
      // Save the document to a file
      HPDF_STATUS saveStatus = HPDF_SaveToFile(pdfDocument, filename.c_str());

      if (saveStatus == HPDF_OK)
      {
        int numPages = (int)pdfDocPages.size();
        std::string pagesString = mlPDF::intToString(numPages) + " page";
        if (numPages != 1)
        {
          pagesString += "s";
        }

        statusFld->setStringValue("PDF file sucessfully written (" + pagesString + ").");
        progressFld->setFloatValue(1.0f);
        successFld->setBoolValue(true);
      }
      else // Possible values: HPDF_INVALID_DOCUMENT, HPDF_FAILD_TO_ALLOC_MEM, HPDF_FILE_IO_ERROR
      {
        //HPDF_FILE_IO_ERROR;
        statusFld->setStringValue("Unable to write PDF document '" + filename + "'. (Target file might be write protected.)");
      }
    }
    else
    {
      if (assemblyErrorMessage == "")
      {
        statusFld->setStringValue("PDF document could not be assembled.");
      }
      else
      {
        statusFld->setStringValue("PDF document could not be assembled: " + assemblyErrorMessage);
      }
    }

    HPDF_Free(pdfDocument);
  }
  else
  {
    statusFld->setStringValue("Critical error: Internal PDF document could not be initialized!");
  }

  return;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::_checkCoordinate(float& smaller, float& larger)
{
  if (smaller > larger)
  {
    const float tempX = larger;
    larger = smaller;
    smaller = tempX;
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::_checkAngle(float& startAngle, float& endAngle)
{
  // Check if angles are > 360 degrees
  while (startAngle > 360)
  {
    startAngle -= 360;
  }

  while (endAngle > 360)
  {
    endAngle -= 360;
  }

  // Check if angles are < -360 degrees
  while (startAngle < -360)
  {
    startAngle += 360;
  }

  while (endAngle < -360)
  {
    endAngle += 360;
  }

  // Check if angles are negative
  if (startAngle < 0)
  {
    startAngle += 360;
  }

  if (endAngle < 0)
  {
    endAngle += 360;
  }

  // Finally check if angles must be swapped (netagive angles are accepted in this case!)
  if (startAngle > endAngle)
  {
      startAngle -= 360;
  }

}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::_getYPosFromTop(float y, bool ignoreMargins)
{
  float result = 0;

  if (pdfDocCurrentPage)
  {
    result = pdfDoc_GetPageMaxY(ignoreMargins) - y;
  }

  return result;
}

//----------------------------------------------------------------------------------

const HPDF_Rect PDFCreatorBase::_getPageRect(float x, float y, float width, float height, bool ignoreMargins)
{
  float startXPos = x;
  float startYPos = y;
  float endXPos   = startXPos + width;
  float endYPos   = startYPos + height;

  if (_currentYAxisReferenceIsFromTop)
  {
    startYPos = _getYPosFromTop(y, ignoreMargins) - height;
  }

  if (!ignoreMargins)
  {
    startXPos   += _globalPageMarginLeft;

    if (!_currentYAxisReferenceIsFromTop)
    {
      startYPos   += _globalPageMarginBottom;
    }
  }

  endXPos = startXPos + width;
  endYPos = startYPos + height;

  HPDF_Rect result = { startXPos, startYPos, endXPos, endYPos };

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::_getPageX(float x, bool ignoreMargins)
{
  float result = x;

  if (!ignoreMargins)
  {
    result += _globalPageMarginLeft;
  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::_getPageY(float y, bool ignoreMargins)
{
  float result = y;

  if (_currentYAxisReferenceIsFromTop)
  {
    result = _getYPosFromTop(y, ignoreMargins);
  }
  else if (!ignoreMargins)
  {
    result += _globalPageMarginBottom;
  }

  return result;
}

//----------------------------------------------------------------------------------

const HPDF_REAL PDFCreatorBase::_getFontHeight(HPDF_Font& font, HPDF_REAL size)
{
  return (HPDF_REAL)((HPDF_Font_GetCapHeight(font)/*+HPDF_Font_GetDescent(font)*/) * size / 1000.0);
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------
/*
void PDFCreatorBase::calculateCameraPropertiesFromInventorCamera()
{
  Vector3 inventorCameraPosition      = inventorCameraPositionFld->getVectorValue();
  Vector4 inventorCameraOrientation   = inventorCameraOrientationFld->getVectorValue();
  float   inventorCameraFocalDistance = inventorCameraFocalDistanceFld->getFloatValue();
  float   inventorCameraHeight        = inventorCameraHeightFld->getFloatValue();
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateCameraPropertiesFromInventorCamera(
    inventorCameraPosition, inventorCameraOrientation, inventorCameraFocalDistance, inventorCameraHeight,
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  cameraCenterOfOrbitFld ->setVector3Value(camCenterOfOrbit);
  cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  cameraRadiusOfOrbitFld ->setFloatValue(camRadiusOfOrbit);
  cameraRollAngleFld     ->setFloatValue(camRollAngle);
  cameraFOVAngleFld      ->setFloatValue(camFOVAngle);
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
  cameraCenterOfOrbitFld ->setVector3Value(camCenterOfOrbit);
  cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  cameraRadiusOfOrbitFld ->setFloatValue(camRadiusOfOrbit);
  cameraRollAngleFld     ->setFloatValue(camRollAngle);
  cameraFOVAngleFld      ->setFloatValue(camFOVAngle);
}
*/
//----------------------------------------------------------------------------------


ML_END_NAMESPACE