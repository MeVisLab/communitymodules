//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase_Graphics.cpp
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

void PDFCreatorBase::pdfDoc_SetStrokeColor(float r, float g, float b)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetRGBStroke(pdfDocCurrentPage, r, g, b);
  }
}


//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetStrokeColor(Vector3 color)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetRGBStroke(pdfDocCurrentPage, (HPDF_REAL)color.x, (HPDF_REAL)color.y, (HPDF_REAL)color.z);
  }
}


//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetFillColor(float r, float g, float b)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetRGBFill(pdfDocCurrentPage, r, g, b);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetFillColor(Vector3 color)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetRGBFill(pdfDocCurrentPage, (HPDF_REAL)color.x, (HPDF_REAL)color.y, (HPDF_REAL)color.z);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddLine(float startX, float startY, float endX, float endY, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(startX, ignoreMargins), _getPageY(startY, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(endX, ignoreMargins), _getPageY(endY, ignoreMargins));
    HPDF_Page_Stroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlineArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkCoordinate(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_Stroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkCoordinate(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_Fill(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkCoordinate(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_FillStroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlineArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddOutlineArcDegrees(centerX, centerY, radius, radToDeg(startAngle), radToDeg(endAngle), lineWidth, ignoreMargins);
}


//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddFilledArcDegrees(centerX, centerY, radius, radToDeg(startAngle), radToDeg(endAngle), lineWidth, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddFilledOutlineArcDegrees(centerX, centerY, radius, radToDeg(startAngle), radToDeg(endAngle), lineWidth, ignoreMargins);
}

//----------------------------------------------------------------------------------


void PDFCreatorBase::pdfDoc_AddOutlineCircle(float centerX, float centerY, float radius, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);
    
    HPDF_Page_Circle(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius);
    HPDF_Page_Stroke(pdfDocCurrentPage);
      
    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledCircle(float centerX, float centerY, float radius, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Circle(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius);
    HPDF_Page_Fill(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineCircle(float centerX, float centerY, float radius, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Circle(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius);
    HPDF_Page_FillStroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlineEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Ellipse(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radiusX, radiusY);
    HPDF_Page_Stroke(pdfDocCurrentPage);


    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Ellipse(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radiusX, radiusY);
    HPDF_Page_Fill(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Ellipse(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radiusX, radiusY);
    HPDF_Page_FillStroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlineRectangle(float x, float y, float width, float height, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect rect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Rectangle(pdfDocCurrentPage, rect.left, rect.bottom, width, height);
    HPDF_Page_Stroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledRectangle(float x, float y, float width, float height, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect rect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Rectangle(pdfDocCurrentPage, rect.left, rect.bottom, width, height);
    HPDF_Page_Fill(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineRectangle(float x, float y, float width, float height, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect rect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Rectangle(pdfDocCurrentPage, rect.left, rect.bottom, width, height);
    HPDF_Page_FillStroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



ML_END_NAMESPACE