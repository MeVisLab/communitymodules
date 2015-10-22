//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase_Assembling.cpp
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


ML_START_NAMESPACE

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetYAxisReference(bool reference)
{
  _currentYAxisReferenceIsFromTop = reference;
}

//----------------------------------------------------------------------------------

const bool PDFCreatorBase::pdfDoc_GetYAxisReference()
{
  return _currentYAxisReferenceIsFromTop;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_StoreYAxisReference()
{
  _previousYAxisReferenceIsFromTop.push_back(_currentYAxisReferenceIsFromTop);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_RestoreYAxisReference()
{
  if (_previousYAxisReferenceIsFromTop.size() > 0)
  {
    _currentYAxisReferenceIsFromTop = _previousYAxisReferenceIsFromTop.back();
	_previousYAxisReferenceIsFromTop.pop_back();	
  }
  else
  {
    _currentYAxisReferenceIsFromTop = mlPDF::YAXIS_REFERENCE_DEFAULT;
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetDefaultYAxisReference(bool reference)
{
  _defaultYAxisReferenceIsFromTop = reference;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_RestoreDefaultYAxisReference()
{
  _currentYAxisReferenceIsFromTop = _defaultYAxisReferenceIsFromTop;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddPage()
{
  pdfDoc_AddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddPage(float width, float height)
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

      HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, _currrentFontSize);

      HPDF_Page_SetLineWidth(pdfDocCurrentPage, 1);
      pdfDoc_SetStrokeColor(0, 0, 0);
      pdfDoc_SetFillColor(0, 0, 0);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddPage(mlPDF::PAGE_SIZES pageSize, mlPDF::PAGE_DIRECTIONS pageDirection)
{
  if (pdfDocument)
  {
    HPDF_Page newPage = HPDF_AddPage(pdfDocument);

    if (newPage)
    {
      pdfDocCurrentPage = newPage;
      pdfDocPages.push_back(newPage);

      HPDF_Page_SetSize(newPage, (HPDF_PageSizes)pageSize, (HPDF_PageDirection)pageDirection);

      HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, _currrentFontSize);

      HPDF_Page_SetLineWidth(pdfDocCurrentPage, 1);
      pdfDoc_SetStrokeColor(0, 0, 0);
      pdfDoc_SetFillColor(0, 0, 0);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetGlobalPageMarginsMM(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = (float)(leftMargin * 2.8346465);
  _globalPageMarginRight  = (float)(rightMargin * 2.8346465);
  _globalPageMarginTop    = (float)(topMargin * 2.8346465);
  _globalPageMarginBottom = (float)(bottomMargin * 2.8346465);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetGlobalPageMarginsInch(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = leftMargin * 72;
  _globalPageMarginRight  = rightMargin * 72;
  _globalPageMarginTop    = topMargin * 72;
  _globalPageMarginBottom = bottomMargin * 72;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetGlobalPageMarginsPixels(float leftMargin, float topMargin, float rightMargin, float bottomMargin)
{
  _globalPageMarginLeft   = leftMargin;
  _globalPageMarginRight  = rightMargin;
  _globalPageMarginTop    = topMargin;
  _globalPageMarginBottom = bottomMargin;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMinX(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (ignoreMargins)
    {
      result = 0;
    }
    else
    {
      result = _globalPageMarginLeft;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMinY(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (ignoreMargins)
    {
      result = 0;
    }
    else
    {
      result = _globalPageMarginBottom;
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMaxX(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (ignoreMargins)
    {
      result = (float)HPDF_Page_GetWidth(pdfDocCurrentPage);
    }
    else
    {
      result = (float)HPDF_Page_GetWidth(pdfDocCurrentPage) - _globalPageMarginRight;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMaxY(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (pdfDocument && pdfDocCurrentPage)
    {
      if (ignoreMargins)
      {
        result = (float)HPDF_Page_GetHeight(pdfDocCurrentPage);
      }
      else
      {
        result = (float)HPDF_Page_GetHeight(pdfDocCurrentPage) - _globalPageMarginTop;
      }
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageCenterX(bool ignoreMargins)
{
  return pdfDoc_GetPageMaxX(ignoreMargins) / 2.0f;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageCenterY(bool ignoreMargins)
{
  return pdfDoc_GetPageMaxY(ignoreMargins) / 2.0f;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMaxWidth(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (ignoreMargins)
    {
      result = pdfDoc_GetPageMaxX(true);
    }
    else
    {
      result = (float)HPDF_Page_GetWidth(pdfDocCurrentPage) - _globalPageMarginLeft - _globalPageMarginRight;
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageMaxHeight(bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    if (ignoreMargins)
    {
      result = pdfDoc_GetPageMaxY(true);
    }
    else
    {
      result = (float)HPDF_Page_GetHeight(pdfDocCurrentPage) - _globalPageMarginBottom - _globalPageMarginTop;
    }

  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageRemainingHeight(float yPos, bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {

    if (!_currentYAxisReferenceIsFromTop)
    {
      result = pdfDoc_GetPageMaxHeight(ignoreMargins) - yPos;

      if (result < 0)
      {
        result = 0;
      }
    }
    else
    {
      result = _getYPosFromTop(yPos, ignoreMargins) - pdfDoc_GetPageMinY(ignoreMargins);

      if (result < 0)
      {
        result = 0;
      }

    }

  }  // if (pdfDocument && pdfDocCurrentPage)

  return result;
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE