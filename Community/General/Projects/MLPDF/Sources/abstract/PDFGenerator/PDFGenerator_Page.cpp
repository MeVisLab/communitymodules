//----------------------------------------------------------------------------------
// The ML module class SavePDF.
//
// \file    PDFGenerator_Page.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-08-01
//
// Base class for PDF creators.
//----------------------------------------------------------------------------------

// Local includes
#include "PDFGenerator.h"


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_AddPage()
{
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_AddPage(float width, float height)
{
  if (pdfDocument)
  {
    HPDF_Page newPage = HPDF_AddPage(pdfDocument);

    if (newPage)
    {
      pdfDocCurrentPage = newPage;
      pdfDocPages.push_back(newPage);

      HPDF_Page_SetWidth(newPage, width);
      HPDF_Page_SetHeight(newPage, height);
      _currentPageSize = mlPDF::PAGESIZE_USER;

      HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFontSettings.font, _currentFontSettings.fontSize);

      HPDF_Page_SetLineWidth(pdfDocCurrentPage, 1);
      pdfDoc_SetStrokeColor(0, 0, 0);
      pdfDoc_SetFillColor(0, 0, 0);

      pdfDoc_CurrentXPos = 0;
      pdfDoc_CurrentYPos = 0;
    }
    else
    {
      _handleError("pdfDoc_AddPage(float width, float height)");
    }

  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_AddPage(mlPDF::PAGE_SIZES pageSize, mlPDF::PAGE_DIRECTIONS pageDirection)
{
  if (pdfDocument)
  {
    HPDF_Page newPage = HPDF_AddPage(pdfDocument);

    if (newPage)
    {
      pdfDocCurrentPage = newPage;
      pdfDocPages.push_back(newPage);

      if (pageSize == mlPDF::PAGESIZE_USER) // It is illegal to set this value manually...
      {
        pageSize = mlPDF::PAGESIZE_A4; // ... therefore set it to default value.
      }

      HPDF_Page_SetSize(newPage, (HPDF_PageSizes)pageSize, (HPDF_PageDirection)pageDirection);
      _currentPageSize = pageSize;

      HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFontSettings.font, _currentFontSettings.fontSize);

      HPDF_Page_SetLineWidth(pdfDocCurrentPage, 1);
      pdfDoc_SetStrokeColor(0, 0, 0);
      pdfDoc_SetFillColor(0, 0, 0);

      pdfDoc_CurrentXPos = 0;
      pdfDoc_CurrentYPos = 0;
    }
    else
    {
      _handleError("pdfDoc_AddPage(mlPDF::PAGE_SIZES pageSize, mlPDF::PAGE_DIRECTIONS pageDirection)");
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetGlobalPageMarginsMM(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = (float)(leftMargin * 2.8346465);
  _globalPageMarginRight  = (float)(rightMargin * 2.8346465);
  _globalPageMarginTop    = (float)(topMargin * 2.8346465);
  _globalPageMarginBottom = (float)(bottomMargin * 2.8346465);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetGlobalPageMarginsInch(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = leftMargin * 72;
  _globalPageMarginRight  = rightMargin * 72;
  _globalPageMarginTop    = topMargin * 72;
  _globalPageMarginBottom = bottomMargin * 72;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetGlobalPageMarginsPixels(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = leftMargin;
  _globalPageMarginRight  = rightMargin;
  _globalPageMarginTop    = topMargin;
  _globalPageMarginBottom = bottomMargin;
}

//----------------------------------------------------------------------------------

mlPDF::PagePropertiesStruct PDFGenerator::pdfDoc_GetPageProperties() const
{
  mlPDF::PagePropertiesStruct result;

  result.PageSize     = _currentPageSize;
  result.Width        = pdfDoc_GetPageMaxWidth(true);
  result.Height       = pdfDoc_GetPageMaxHeight(true);
  result.MarginLeft   = pdfDoc_GetMarginLeft();
  result.MarginRight  = pdfDoc_GetMarginRight();
  result.MarginTop    = pdfDoc_GetMarginTop();
  result.MarginBottom = pdfDoc_GetMarginBottom();

  return result;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetMarginLeft() const
{
  return _globalPageMarginLeft;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetMarginRight() const
{
  return _globalPageMarginRight;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetMarginTop() const
{
  return _globalPageMarginTop;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetMarginBottom() const
{
  return _globalPageMarginBottom;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMinX(bool /*ignoreMargins*/) const
{
  return 0;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMinY(bool /*ignoreMargins*/) const
{
  return 0;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMaxX(bool ignoreMargins) const
{
  return pdfDoc_GetPageMaxWidth(ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMaxY(bool ignoreMargins) const
{
  return pdfDoc_GetPageMaxHeight(ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageCenterX(bool ignoreMargins) const
{
  return pdfDoc_GetPageMaxX(ignoreMargins) / 2.0f;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageCenterY(bool ignoreMargins) const
{
  return pdfDoc_GetPageMaxY(ignoreMargins) / 2.0f;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMaxWidth(bool ignoreMargins) const
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    result = (float)HPDF_Page_GetWidth(pdfDocCurrentPage);

    if (!ignoreMargins)
    {
      result -= (_globalPageMarginLeft + _globalPageMarginRight);
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageMaxHeight(bool ignoreMargins) const
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    result = (float)HPDF_Page_GetHeight(pdfDocCurrentPage);

    if (!ignoreMargins)
    {
      result -= (_globalPageMarginBottom + _globalPageMarginTop);
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageRemainingHeight(bool ignoreMargins) const
{
  return pdfDoc_GetPageRemainingHeight(pdfDoc_CurrentYPos, ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetPageRemainingHeight(float yPos, bool ignoreMargins) const
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    result = pdfDoc_GetPageMaxHeight(ignoreMargins) - yPos;

    if (result < 0)
    {
      result = 0;
    }

  }  // if (pdfDocument && pdfDocCurrentPage)

  return result;
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE
