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
    _checkAngle(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);

    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_Stroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledArcDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkAngle(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

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
    _checkAngle(startAngle, endAngle);

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
  pdfDoc_AddOutlineArcDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), lineWidth, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins)
{
  pdfDoc_AddFilledArcDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlineArcRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddFilledOutlineArcDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), lineWidth, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlinePieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkAngle(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);
    HPDF_Page_SetLineCap(pdfDocCurrentPage, HPDF_ROUND_END);
    HPDF_Page_SetLineJoin(pdfDocCurrentPage, HPDF_ROUND_JOIN);

    float endLine1X = (float)(centerX + sin(mlPDF::degToRad(startAngle)) * radius);
    float endLine1Y = (float)(centerY - cos(mlPDF::degToRad(startAngle)) * radius);
    float startLine2X = (float)(centerX + sin(mlPDF::degToRad(endAngle)) * radius);
    float startLine2Y = (float)(centerY - cos(mlPDF::degToRad(endAngle)) * radius);

    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(endLine1X, ignoreMargins), _getPageY(endLine1Y, ignoreMargins));
    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(startLine2X, ignoreMargins), _getPageY(startLine2Y, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_Stroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledPieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkAngle(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineCap(pdfDocCurrentPage, HPDF_ROUND_END);
    HPDF_Page_SetLineJoin(pdfDocCurrentPage, HPDF_ROUND_JOIN);

    float endLine1X = (float)(centerX + sin(mlPDF::degToRad(startAngle)) * radius);
    float endLine1Y = (float)(centerY - cos(mlPDF::degToRad(startAngle)) * radius);
    float startLine2X = (float)(centerX + sin(mlPDF::degToRad(endAngle)) * radius);
    float startLine2Y = (float)(centerY - cos(mlPDF::degToRad(endAngle)) * radius);

    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(endLine1X, ignoreMargins), _getPageY(endLine1Y, ignoreMargins));
    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(startLine2X, ignoreMargins), _getPageY(startLine2Y, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_Fill(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlinePieDegrees(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    _checkAngle(startAngle, endAngle);

    HPDF_Page_GSave(pdfDocCurrentPage);

    HPDF_Page_SetLineWidth(pdfDocCurrentPage, lineWidth);
    HPDF_Page_SetLineCap(pdfDocCurrentPage, HPDF_ROUND_END);
    HPDF_Page_SetLineJoin(pdfDocCurrentPage, HPDF_ROUND_JOIN);

    float endLine1X = (float)(centerX + sin(mlPDF::degToRad(startAngle)) * radius);
    float endLine1Y = (float)(centerY - cos(mlPDF::degToRad(startAngle)) * radius);
    float startLine2X = (float)(centerX + sin(mlPDF::degToRad(endAngle)) * radius);
    float startLine2Y = (float)(centerY - cos(mlPDF::degToRad(endAngle)) * radius);

    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(endLine1X, ignoreMargins), _getPageY(endLine1Y, ignoreMargins));
    HPDF_Page_Arc(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins), radius, startAngle, endAngle);
    HPDF_Page_MoveTo(pdfDocCurrentPage, _getPageX(startLine2X, ignoreMargins), _getPageY(startLine2Y, ignoreMargins));
    HPDF_Page_LineTo(pdfDocCurrentPage, _getPageX(centerX, ignoreMargins), _getPageY(centerY, ignoreMargins));
    HPDF_Page_FillStroke(pdfDocCurrentPage);

    HPDF_Page_GRestore(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddOutlinePieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddOutlinePieDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), lineWidth, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledPieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, bool ignoreMargins)
{
  pdfDoc_AddFilledPieDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddFilledOutlinePieRadians(float centerX, float centerY, float radius, float startAngle, float endAngle, float lineWidth, bool ignoreMargins)
{
  pdfDoc_AddFilledOutlinePieDegrees(centerX, centerY, radius, mlPDF::radToDeg(startAngle), mlPDF::radToDeg(endAngle), lineWidth, ignoreMargins);
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

void PDFCreatorBase::pdfDoc_AddFilledCircle(float centerX, float centerY, float radius, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

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

void PDFCreatorBase::pdfDoc_AddFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_GSave(pdfDocCurrentPage);

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

void PDFCreatorBase::pdfDoc_AddFilledRectangle(float x, float y, float width, float height, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect rect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_GSave(pdfDocCurrentPage);

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