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

// Project includes
#include "../SaveU3D/U3DFileFormat/U3D_Constants.h"

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
    pdfDoc_WriteTextAt(0, yPos, "Click inside image or frame to enable interactive mode.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "- Left-click & move mouse to rotate scene.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "- Right-click & move mouse to zoom.");
    yPos += 10;
    pdfDoc_WriteTextAt(0, yPos, "- Both-click and move mouse to pan.");
  }

  yPos += 20;
  float sceneWidth  = pdfDoc_GetPageMaxWidth();
  float sceneHeight = sceneWidth * 0.75; // Use fixed heigt ratio to facilitate poster image creation!
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
  std::string u3dFilename = _u3dFilenameFld->getStringValue();
  bool sceneAdded = false;

  pdfDoc_AddOutlineRectangle(x, y, width, height, 1);

  if (mlPDF::fileExists(u3dFilename))
  {
    std::string viewsSpecification = _getViewsSpecificationFromU3DFile(u3dFilename);

    HPDF_U3D u3dModel = pdfDoc_Load3DModelDataFromFile(u3dFilename);

    if (u3dModel)
    {

      if (mlPDF::PDFTools::stringTrimWhitespace(viewsSpecification) == "")
      {
        viewsSpecification = viewSpecificationsFld->getStringValue();
      }

      pdfDoc_3DModel_AddAllViewsFromSpecificationString(u3dModel, viewsSpecification);

      // Load poster image
      std::string posterFilename = _posterImageFilenameFld->getStringValue();
      bool posterExists = false;
      mlPDF::IMAGE posterImage = NULL;

      if (mlPDF::fileExists(posterFilename))
      {
        posterImage = pdfDoc_LoadImageFromFile(posterFilename);

        if (posterImage)
        {
          posterExists = true;
        }
      }

      if (!posterExists)
      {
        pdfDoc_WriteTextAreaAt(x, y + height / 2.0, width, 50, "Click here to display interactive 3D figure.", mlPDF::TEXT_ALIGNMENTS::TEXTALIGNMENT_CENTER);
      }

      mlPDF::SCENE3D u3dScene = pdfDoc_Add3DScene(x + 1, y + 1, width - 2, height - 2, u3dModel, posterImage);

      if (u3dScene)
      {
        pdfDoc_3DScene_SetActivationProperties(u3dScene, _figureActivationModeFld->getStringValue(), _figureDeactivationModeFld->getStringValue(), _figureToolbarEnabledFld->getBoolValue(), _figureNavigationInterfaceEnabledFld->getBoolValue(), _figureAnimationAutostartFld->getBoolValue());
        sceneAdded = true;
      }

    } // if (u3dModel)

  } // if (mlPDF::fileExists(u3dFilename))

  if (!sceneAdded)
  {
    pdfDoc_WriteTextAreaAt(x, y + height / 2.0, width, 50, "FAILED TO IMPORT 3D FIGURE!", mlPDF::TEXT_ALIGNMENTS::TEXTALIGNMENT_CENTER);
  }
}

//----------------------------------------------------------------------------------

std::string PDF3DFigurePage_SavePDF::_getViewsSpecificationFromU3DFile(std::string filename)
{
  std::string result = "";

  std::ifstream u3dFile;
  u3dFile.open(filename, std::ios::out | std::ios::binary);

  if (u3dFile.is_open())
  {
    U3DDataBlockFundamental blockType = _readU32(u3dFile);

    if (blockType == U3D_BLOCKTYPE_FILEHEADER)
    {
      // Get data block size
      U3DDataBlockFundamental blockDataSize = _readU32(u3dFile);

      // Get meta data block size
      /*U3DDataBlockFundamental blockMetaDataSize = */_readU32(u3dFile);

      // Calc meta data start position
      MLuint32 dataStartPos = (MLuint32)u3dFile.tellg();
      MLuint32 metaDataStartPos = dataStartPos + blockDataSize;

      if (blockDataSize % 4 != 0)
      {
        metaDataStartPos += (4 - (blockDataSize % 4));
      }

      // Go to meta data start position
      u3dFile.seekg(metaDataStartPos, std::ios::beg);

      // Get Key/Value Pair Count (9.2.6.1)
      U3DDataBlockFundamental keyValuePairCount = _readU32(u3dFile);

      for (int thisKeyValuePair = 0; thisKeyValuePair < (int)keyValuePairCount; thisKeyValuePair++)
      {
        // Get Key/Value Pair Attributes (9.2.6.2)
        /*U3DDataBlockFundamental keyValuePairAttributes = */_readU32(u3dFile);

        std::string keyString = _readString(u3dFile);
        std::string valueString = _readString(u3dFile);

        if (keyString == "ViewSpecifications")
        {
          result = valueString;
        }

      }

    }

    u3dFile.close();

  }

  return result;
}

//----------------------------------------------------------------------------------

MLuint8 PDF3DFigurePage_SavePDF::_readU8(std::ifstream& file)
{
  char* u8Buf = new char[1];

  file.read(u8Buf, 1);

  MLuint8 result = u8Buf[0];

  delete[] u8Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint16 PDF3DFigurePage_SavePDF::_readU16(std::ifstream& file)
{
  char* u16Buf = new char[2];

  file.read(u16Buf, 2);

  MLuint16 byte1 = u16Buf[1] << 8;
  MLuint8  byte0 = u16Buf[0];

  MLuint16 result = byte1 | byte0;

  delete[] u16Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint32 PDF3DFigurePage_SavePDF::_readU32(std::ifstream& file)
{
  char* u32Buf = new char[4];

  file.read(u32Buf, 4);

  MLuint32 byte3 = u32Buf[3] << 24;
  MLuint32 byte2 = u32Buf[2] << 16;
  MLuint32 byte1 = u32Buf[1] << 8;
  MLuint8  byte0 = u32Buf[0];

  MLuint32 result = byte3 | byte2 | byte1 | byte0;

  delete[] u32Buf;

  return result;
}

//----------------------------------------------------------------------------------

std::string PDF3DFigurePage_SavePDF::_readString(std::ifstream& file)
{
  std::string result = "";

  MLuint16 stringLength = _readU16(file);

  for (int i = 0; i < stringLength; i++)
  {
    MLuint8 stringCharacter = _readU8(file);

    result += stringCharacter;
  }

  return result;

}//----------------------------------------------------------------------------------

ML_END_NAMESPACE