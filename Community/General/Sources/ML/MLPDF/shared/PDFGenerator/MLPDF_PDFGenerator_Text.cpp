//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    PDFGenerator_Text.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDFGenerator.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFont(HPDF_Font font)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, _currrentFontSize);
    _currentFont = font;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFont(HPDF_Font font, float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, (HPDF_REAL)fontSize);
    _currentFont = font;
    _currrentFontSize = fontSize;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFontSize(float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, (HPDF_REAL)fontSize);
    _currrentFontSize = fontSize;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

const float PDFGenerator::pdfDoc_GetCurrentFontHeight()
{
  return _currentFontHeight;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAt(float x, float y, std::string text, bool ignoreMargins)
{
  pdfDoc_WriteTextAt(x, y, text, mlPDF::TEXTRENDERMODE_FILL, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAt(float x, float y, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    if (_currentYAxisReferenceIsFromTop)
    {
      y += _currentFontHeight;
    }

    HPDF_Page_BeginText(pdfDocCurrentPage);
    HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)renderMode);
    HPDF_Page_MoveTextPos(pdfDocCurrentPage, _getPageX(x, ignoreMargins), _getPageY(y, ignoreMargins));
    HPDF_Page_ShowText(pdfDocCurrentPage, text.c_str());
    HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)mlPDF::TEXTRENDERMODE_FILL);
    HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, bool ignoreMargins)
{
  pdfDoc_WriteTextAreaAt(x, y, width, height, text, mlPDF::TEXTRENDERMODE_FILL, mlPDF::TEXTALIGNMENT_LEFT, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_ALIGNMENTS alignment, bool ignoreMargins)
{
  pdfDoc_WriteTextAreaAt(x, y, width, height, text, mlPDF::TEXTRENDERMODE_FILL, alignment, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)
{
  pdfDoc_WriteTextAreaAt(x, y, width, height, text, renderMode, mlPDF::TEXTALIGNMENT_LEFT, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_RENDERMODES renderMode, mlPDF::TEXT_ALIGNMENTS alignment, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect TextRect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_BeginText(pdfDocCurrentPage);
    HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)renderMode);
    HPDF_Page_TextRect(pdfDocCurrentPage, TextRect.left, TextRect.top, TextRect.right, TextRect.bottom, text.c_str(), (HPDF_TextAlignment)alignment, NULL);
    HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)mlPDF::TEXTRENDERMODE_FILL);
    HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE