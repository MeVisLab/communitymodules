//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    PDFGenerator_Document.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-08-01
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDFGenerator.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCompressionMode(const unsigned int compressionFlags)
{
  if (pdfDocument)
  {
    HPDF_STATUS result = HPDF_SetCompressionMode(pdfDocument, (HPDF_UINT)compressionFlags);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetCompressionMode(const unsigned int compressionFlags)");
    }

  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetPasswords(std::string editPassword, std::string openPassword)
{
  if ((pdfDocument) && (editPassword != ""))
  {
    HPDF_STATUS result = HPDF_OK;

    result = HPDF_SetPassword(pdfDocument, editPassword.c_str(), openPassword.c_str());

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetPasswords(std::string editPassword, std::string openPassword)");
    }

    result = HPDF_SetEncryptionMode(pdfDocument, HPDF_ENCRYPT_R3, 16);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetPasswords(std::string editPassword, std::string openPassword)");
    }

  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetPermissions(const unsigned int  permissionFlags)
{
  if (pdfDocument)
  {
    HPDF_UINT permissions = permissionFlags | 0xFFFFF0C0; // Bit 7,8,13-32: must be 1

    HPDF_STATUS result = HPDF_SetPermission(pdfDocument, permissions);

    if (result == HPDF_DOC_ENCRYPTDICT_NOT_FOUND)
    {
      HPDF_ResetError(pdfDocument);
      std::string randomPassword = _getRandomPassword(64);
      pdfDoc_SetPasswords(randomPassword);
      HPDF_SetPermission(pdfDocument, permissions);
    }
    else if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetPermissions(const unsigned int  permissionFlags)");
    }

  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetDocumentProperty(mlPDF::DOCUMENT_PROPERTY_KEYS propertyKey, std::string value)
{
  if (pdfDocument)
  {
    HPDF_STATUS result = HPDF_SetInfoAttr(pdfDocument, (_HPDF_InfoType)propertyKey, value.c_str());

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetDocumentProperty(mlPDF::DOCUMENT_PROPERTY_KEYS propertyKey, std::string value)");
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetInitialPageDisplayLayout(mlPDF::PAGE_LAYOUTS layout)
{
  if (pdfDocument)
  {
    HPDF_STATUS result = HPDF_SetPageLayout(pdfDocument, (HPDF_PageLayout)layout);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetInitialPageDisplayLayout(mlPDF::PAGE_LAYOUTS layout)");
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetDocumentOpenMode(mlPDF::DOCUMENT_OPEN_MODES mode)
{
  if (pdfDocument)
  {
    HPDF_STATUS result = HPDF_SetPageMode(pdfDocument, (HPDF_PageMode)mode);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetDocumentOpenMode(mlPDF::DOCUMENT_OPEN_MODES mode)");
    }

  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetYAxisReference(bool reference)
{
  _currentYAxisReferenceIsFromTop = reference;
}

//----------------------------------------------------------------------------------

bool PDFGenerator::pdfDoc_GetYAxisReference() const
{
  return _currentYAxisReferenceIsFromTop;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_StoreYAxisReference()
{
  _yAxisReferenceIsFromTopBackupStack.push_back(_currentYAxisReferenceIsFromTop);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_RestoreYAxisReference()
{
  if (_yAxisReferenceIsFromTopBackupStack.size() > 0)
  {
    _currentYAxisReferenceIsFromTop = _yAxisReferenceIsFromTopBackupStack.back();
    _yAxisReferenceIsFromTopBackupStack.pop_back();
  }
  else
  {
    _currentYAxisReferenceIsFromTop = mlPDF::YAXIS_REFERENCE_DEFAULT;
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetDefaultYAxisReference(bool reference)
{
  _defaultYAxisReferenceIsFromTop = reference;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_RestoreDefaultYAxisReference()
{
  _currentYAxisReferenceIsFromTop = _defaultYAxisReferenceIsFromTop;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetDocumentCreationDateTime()
{
  time_t now = time(0);
  struct tm  datetime;
  datetime = *localtime(&now);

  pdfDoc_SetDocumentCreationDateTime(datetime);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetDocumentCreationDateTime(std::tm datetime)
{
  _documentCreationDate.year        = datetime.tm_year + 1900;
  _documentCreationDate.month       = datetime.tm_mon + 1;
  _documentCreationDate.day         = datetime.tm_mday;
  _documentCreationDate.hour        = datetime.tm_hour;
  _documentCreationDate.minutes     = datetime.tm_min;
  _documentCreationDate.seconds     = datetime.tm_sec;
  _documentCreationDate.ind         = ' ';
  _documentCreationDate.off_hour    = 0;
  _documentCreationDate.off_minutes = 0;
}

//----------------------------------------------------------------------------------

std::tm PDFGenerator::pdfDoc_GetDocumentCreationDateTime() const
{
  std::tm result;

  result.tm_year = _documentCreationDate.year - 1900;
  result.tm_mon  = _documentCreationDate.month - 1;
  result.tm_mday = _documentCreationDate.day;
  result.tm_hour = _documentCreationDate.hour;
  result.tm_min  = _documentCreationDate.minutes;
  result.tm_sec  = _documentCreationDate.seconds;

  return result;
}


//----------------------------------------------------------------------------------


ML_END_NAMESPACE
