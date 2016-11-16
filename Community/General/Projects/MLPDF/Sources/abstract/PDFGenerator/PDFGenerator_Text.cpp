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

// Global includes
#include <boost/algorithm/string.hpp>

// Local includes
#include "PDFGenerator.h"
#include "MLPDF_Tools.h"


using namespace xercesc;


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFont(HPDF_Font& font)
{
  pdfDoc_SetCurrentFont(font, _currentFontSettings.fontSize);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFont(HPDF_Font& font, float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_STATUS result = HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, (HPDF_REAL)fontSize);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_SetCurrentFont(HPDF_Font& font, float fontSize)");
    }

    _currentFontSettings.font = font;
    _currentFontSettings.fontSize = fontSize;
    _currentFontSettings.fontHeight = _calcCurrentFontHeight();
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentFontSize(float fontSize)
{
  pdfDoc_SetCurrentFont(_currentFontSettings.font, fontSize);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetCurrentFontSize() const
{
  return _currentFontSettings.fontSize;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetCurrentFontHeight() const
{
  return _currentFontSettings.fontHeight;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetCurrentFontTextWidth(std::string text) const
{
  return (float)HPDF_Page_TextWidth(pdfDocCurrentPage, text.c_str());
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetCurrentFontTextHeight() const
{
  return (float)(HPDF_Font_GetCapHeight(_currentFontSettings.font) * _currentFontSettings.fontSize / 1000.0);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetTextHeight(HPDF_Font& font, float size) const
{
  return (float)(HPDF_Font_GetCapHeight(font) * size / 1000.0);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentTextLeading(float leading)
{
  HPDF_STATUS result = HPDF_Page_SetTextLeading(pdfDocCurrentPage, (HPDF_REAL)leading);

  if (result != HPDF_OK)
  {
    _handleError("pdfDoc_SetCurrentTextLeading(float leading)");
  }
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_GetCurrentTextLeading() const
{
  return (float)HPDF_Page_GetTextLeading(pdfDocCurrentPage);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_StoreFontSettings()
{
  _fontSettingsBackupStack.push_back(_currentFontSettings);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_RestoreFontSettings()
{
  if (_fontSettingsBackupStack.size() > 0)
  {
    mlPDF::FontSettingsStruct _fontSettings = _fontSettingsBackupStack.back();
    pdfDoc_SetCurrentFont(_fontSettings.font, _fontSettings.fontSize);

    _fontSettingsBackupStack.pop_back();
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentTextRotationDegrees(float degrees)
{
  _currentTextRotation = (double)mlPDF::degToRad(degrees);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_SetCurrentTextRotationRadians(double radians)
{
  _currentTextRotation = radians;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_ResetCurrentTextRotation()
{
  _currentTextRotation = 0;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAt(float x, float y, std::string text, bool ignoreMargins)
{
  pdfDoc_WriteTextAt(x, y, text, mlPDF::TEXTRENDERMODE_FILL, ignoreMargins);
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_WriteTextAt(float x, float y, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)
{
  if ( (pdfDocCurrentPage) && (text != "") )
  {
    HPDF_STATUS result = HPDF_OK;

    if (_currentYAxisReferenceIsFromTop)
    {
      y += _currentFontSettings.fontHeight;
    }

    HPDF_Page_BeginText(pdfDocCurrentPage);

    if (_currentTextRotation != 0)
    {
      // Need some rounding here since matrix is very sensitive for rounding/precision errors
      HPDF_REAL cosPhi = (HPDF_REAL)mlPDF::roundWithPrecision(cos(_currentTextRotation), 4);
      HPDF_REAL sinPhi = (HPDF_REAL)mlPDF::roundWithPrecision(sin(_currentTextRotation), 4);
      result = HPDF_Page_SetTextMatrix(pdfDocCurrentPage, cosPhi, sinPhi, -sinPhi, cosPhi, 0, 0);

      if (result != HPDF_OK)
      {
        _handleError("pdfDoc_WriteTextAt(float x, float y, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)");
      }

    }

    result = HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)renderMode);
    result = HPDF_Page_TextOut(pdfDocCurrentPage, _getPageX(x, ignoreMargins), _getPageY(y, ignoreMargins), text.c_str());

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_WriteTextAt(float x, float y, std::string text, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)");
    }

    result = HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)mlPDF::TEXTRENDERMODE_FILL);
    result = HPDF_Page_EndText(pdfDocCurrentPage);
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
  if ( (pdfDocCurrentPage) && (text != "") )
  {
    HPDF_STATUS result = HPDF_OK;

    HPDF_Rect TextRect = _getPageRect(x, y, width, height, ignoreMargins);

    result = HPDF_Page_BeginText(pdfDocCurrentPage);
    result = HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)renderMode);
    result = HPDF_Page_TextRect(pdfDocCurrentPage, TextRect.left, TextRect.top, TextRect.right, TextRect.bottom, text.c_str(), (HPDF_TextAlignment)alignment, NULL);

    if (result != HPDF_OK)
    {
      _handleError("pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, mlPDF::TEXT_RENDERMODES renderMode, mlPDF::TEXT_ALIGNMENTS alignment, bool ignoreMargins)");
    }

    result = HPDF_Page_SetTextRenderingMode(pdfDocCurrentPage, (HPDF_TextRenderingMode)mlPDF::TEXTRENDERMODE_FILL);
    result = HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

unsigned int PDFGenerator::_getMaxNumberOfLinesInCell(std::vector<std::string>& rowStrings)
{
  unsigned int result = 1;

  for (std::vector<std::string>::const_iterator it = rowStrings.begin(); it != rowStrings.end(); it++)
  {
    std::string thisCellString = *it;
    std::vector<std::string> splitLines;
    boost::split(splitLines, thisCellString, boost::is_any_of("\n"));

    size_t numLines = splitLines.size();

    if (numLines > result)
    {
      result = (unsigned int)numLines;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLTableAt(float x, float y, std::string html, bool ignoreMargins)
{
  return pdfDoc_WriteHTMLTableAt(x, y, html, mlPDF::TEXTRENDERMODE_FILL, false, false, ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLTableAt(float x, float y, std::string html, bool firstColumBold, bool firstRowBold, bool ignoreMargins)
{
  return pdfDoc_WriteHTMLTableAt(x, y, html, mlPDF::TEXTRENDERMODE_FILL, firstColumBold, firstRowBold, ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLTableAt(float x, float y, std::string html, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)
{
  return pdfDoc_WriteHTMLTableAt(x, y, html, renderMode, false, false, ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLTableAt(float x, float y, std::string html, mlPDF::TEXT_RENDERMODES renderMode, bool firstColumBold, bool firstRowBold, bool ignoreMargins)
{
  float tableHeight = 0;

  DOMElement* htmlRootElement = _parseXMLString(html);
  
  if (htmlRootElement)
  {
    DOMElement* tableElement = _getNodeElement(htmlRootElement, "table");

    if (tableElement)
    {

      HPDF_TextWidth cellWidthStruct;
      float currentfontSize = pdfDoc_GetCurrentFontSize();
      float currentPageLeading = pdfDoc_GetCurrentTextLeading();

      //
      // 1. Step: Write all cell text strings into vector and calc column widths
      //
      std::vector<std::vector<std::string> > cellContentStrings;
      std::vector<float>                     columnWidths;
      std::vector<int>                       rowSpans;
      std::vector<std::vector<int> >         columnSpans;

      std::vector<DOMElement*> rowNodes = _getAllNodeElements(tableElement, "tr");

      for (size_t r = 0; r < rowNodes.size(); r++)
      {
        std::vector<std::string> thisRowCellStrings;
        std::vector<int> thisRowColumnSpans;
        int numfillCells = 0;

        DOMElement* thisRowElement = rowNodes[r];

        std::vector<DOMElement*> cellNodes = _getAllNodeElements(thisRowElement, "td");

        if ((0 == r) && (firstRowBold))
        {
          pdfDoc_SetCurrentFont(buildInFonts.TimesBold, currentfontSize);
        }

        for (size_t c = 0; c < cellNodes.size(); c++)
        {
          DOMElement* thisCellElement = cellNodes[c];

          if (rowSpans.size() > c)
          {
            bool goon = true;

            for (size_t inc = 0; goon; inc++)
            {
              if (rowSpans[c + inc] > 0)
              {
                rowSpans[c + inc] -= 1;
                thisRowCellStrings.push_back(""); // Insert empty cell
                numfillCells++;
                if (columnWidths.size() <= c)
                {
                  columnWidths.push_back(1);
                }
              }
              else
              {
                goon = false;
              }

            }

          }

          int cellRowspan = 1;
          int cellColspan = 1;

          std::string cellRowspanString = _getAttributeValue(thisCellElement, "rowspan");
          std::string cellColspanString = _getAttributeValue(thisCellElement, "colspan");

          // Handle rowspan attributes
          if (cellRowspanString != "")
          {
            cellRowspan = mlPDF::stringToInt(cellRowspanString);
          }

          if (rowSpans.size() < c + 1)
          {
            rowSpans.push_back(0);
          }

          rowSpans[c] += cellRowspan;

          // Handle colspan attributes
          if (cellColspanString != "")
          {
            cellColspan = mlPDF::stringToInt(cellColspanString);
            thisRowColumnSpans.push_back(cellColspan);
          }
          else
          {
            thisRowColumnSpans.push_back(1);
          }

          // Handle cell text
          std::string cellText = _getInnerText(thisCellElement);
          thisRowCellStrings.push_back(cellText);

          if (cellColspan > 1)
          {
            for (int cs = 0; cs < cellColspan - 1; cs++)
            {
              thisRowCellStrings.push_back(""); // Insert empty cell
              if (columnWidths.size() <= c)
              {
                columnWidths.push_back(1);
              }
            }
          }

          if ((0 == c + numfillCells) && (firstColumBold))
          {
            pdfDoc_SetCurrentFont(buildInFonts.TimesBold, currentfontSize);
          }

          cellWidthStruct = HPDF_Font_TextWidth(buildInFonts.TimesBold, (const HPDF_BYTE*)cellText.c_str(), (HPDF_UINT)cellText.size());
          float cellWidth = (float)(cellWidthStruct.width * currentfontSize / (1000.0*cellColspan)) + 1; // Added +1 to compensate rounding errors...

          if ((0 == c + numfillCells) && (firstColumBold))
          {
            pdfDoc_SetCurrentFont(buildInFonts.Times, currentfontSize);
          }

          if (columnWidths.size() <= c + numfillCells)
          {
            columnWidths.push_back(cellWidth);
          }
          else
          {
            if (columnWidths[c + numfillCells] < cellWidth)
            {
              columnWidths[c + numfillCells] = cellWidth;
            }
          }
        }

        if ((0 == r) && (firstRowBold))
        {
          pdfDoc_SetCurrentFont(buildInFonts.Times, currentfontSize);
        }

        cellContentStrings.push_back(thisRowCellStrings);

        for (size_t j = 0; j < rowSpans.size(); j++)
        {
          rowSpans[j] -= 1;
        }

        columnSpans.push_back(thisRowColumnSpans);
      }

      //
      // 2. Step: Write table text to document
      //
      float yPos = y;
      float cellSpacing = 10;
      float cellLineLeadingFactor = 0.6;

      for (int thisRow = 0; thisRow < cellContentStrings.size(); thisRow++)
      {
        float cellHeightPerLine = (pdfDoc_GetCurrentFontTextHeight() + currentPageLeading*(1 - cellLineLeadingFactor));

        std::vector<std::string> thisRowStrings = cellContentStrings[thisRow];
        std::vector<int> thisRowColumnSpans = columnSpans[thisRow];

        unsigned int maxNumberOfLinesInCell = _getMaxNumberOfLinesInCell(thisRowStrings);
        float cellHeight = cellHeightPerLine * maxNumberOfLinesInCell;

        float left = x;

        if ((0 == thisRow) && (firstRowBold))
        {
          pdfDoc_SetCurrentFont(buildInFonts.TimesBold, currentfontSize);
        }

        for (int thisCell = 0; thisCell < thisRowStrings.size(); thisCell++)
        {
          std::string thisCellString = thisRowStrings[thisCell];

          if ((0 == thisCell) && (firstColumBold))
          {
            pdfDoc_SetCurrentFont(buildInFonts.TimesBold, currentfontSize);
          }

          pdfDoc_SetCurrentTextLeading(currentPageLeading*cellLineLeadingFactor);

          float cellWidth = columnWidths[thisCell];

          if (thisCell < thisRowColumnSpans.size())
          {
            if (thisRowColumnSpans[thisCell] > 1)
            {
              for (int cc = thisCell + 1; cc < columnWidths.size(); cc++)
              {
                cellWidth += columnWidths[cc];
              }
            }
          }

          pdfDoc_WriteTextAreaAt(left, yPos, cellWidth, cellHeight, thisCellString, renderMode, mlPDF::TEXTALIGNMENT_LEFT, ignoreMargins);
          //pdfDoc_AddOutlineRectangle(left, yPos, cellWidth, cellHeight, 1, ignoreMargins);
          pdfDoc_SetCurrentTextLeading(currentPageLeading);
          left += columnWidths[thisCell] + cellSpacing;

          if ((0 == thisCell) && (firstColumBold))
          {
            pdfDoc_SetCurrentFont(buildInFonts.Times, currentfontSize);
          }

        }

        if ((0 == thisRow) && (firstRowBold))
        {
          pdfDoc_SetCurrentFont(buildInFonts.Times, currentfontSize);
        }

        yPos += cellHeight;
        tableHeight += cellHeight;

      }

    } // if (tableElement)

  } // if (htmlRootElement)
  else
  {
    // assemblyErrorMessage has been set in _parseXMLString()
  }
  
  return tableHeight;
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLUnorderedListAt(float x, float y, std::string html, bool ignoreMargins)
{
  return pdfDoc_WriteHTMLUnorderedListAt(x, y, html, mlPDF::TEXTRENDERMODE_FILL, ignoreMargins);
}

//----------------------------------------------------------------------------------

float PDFGenerator::pdfDoc_WriteHTMLUnorderedListAt(float x, float y, std::string html, mlPDF::TEXT_RENDERMODES renderMode, bool ignoreMargins)
{
  float listHeight = 0;
  float yPos = y;
  float textHeight = pdfDoc_GetCurrentFontHeight() + pdfDoc_GetCurrentTextLeading() / 4;

  DOMElement* htmlRootElement = _parseXMLString(html);

  if (htmlRootElement)
  {
    DOMElement* listElement = _getNodeElement(htmlRootElement, "ul");

    if (listElement)
    {
      std::vector<DOMElement*> listItemElements = _getAllNodeElements(listElement, "li");

      for (size_t i = 0; i < listItemElements.size(); i++)
      {
        DOMElement* thisListItemElement = listItemElements[i];
        std::string thisListItemText = "•  " + _getInnerText(thisListItemElement);

        pdfDoc_WriteTextAt(x + 20, yPos, thisListItemText, renderMode, ignoreMargins);

        yPos += textHeight;
        listHeight += textHeight;
      }

    } // if (listElement)

  } // if (htmlRootElement)

  return listHeight;
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE
