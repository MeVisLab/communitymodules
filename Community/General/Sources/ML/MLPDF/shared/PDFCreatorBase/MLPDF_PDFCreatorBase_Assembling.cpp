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
  _previousYAxisReferenceIsFromTop = _currentYAxisReferenceIsFromTop;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_RestoreYAxisReference()
{
  _currentYAxisReferenceIsFromTop = _previousYAxisReferenceIsFromTop;
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
    }
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddPage(mlPDF::PAGESIZES pageSize, mlPDF::PAGEDIRECTIONS pageDirection)
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
    result = pdfDoc_GetPageMaxHeight(ignoreMargins) - yPos;
  }

  if (result < 0)
  {
    result = 0;
  }

  return result;
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetPageRemainingHeightFromTop(float yPos, bool ignoreMargins)
{
  float result = 0;

  if (pdfDocument && pdfDocCurrentPage)
  {
    result = _getYPosFromTop(yPos, ignoreMargins) - pdfDoc_GetPageMinY(ignoreMargins);
  }

  if (result < 0)
  {
    result = 0;
  }

  return result;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetCurrentFont(HPDF_Font font)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, _currrentFontSize);
    _currentFont = font;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetCurrentFont(HPDF_Font font, float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, font, (HPDF_REAL)fontSize);
    _currentFont      = font;
    _currrentFontSize = fontSize;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_SetCurrentFontSize(float fontSize)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Page_SetFontAndSize(pdfDocCurrentPage, _currentFont, (HPDF_REAL)fontSize);
    _currrentFontSize = fontSize;
    _currentFontHeight = _getFontHeight(_currentFont, _currrentFontSize);
  }
}

//----------------------------------------------------------------------------------

const float PDFCreatorBase::pdfDoc_GetCurrentFontHeight()
{
  return _currentFontHeight;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_WriteTextAt(float x, float y, std::string text, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    if (_currentYAxisReferenceIsFromTop)
    {
      y += _currentFontHeight;
    }

    HPDF_Page_BeginText(pdfDocCurrentPage);
    HPDF_Page_MoveTextPos(pdfDocCurrentPage, _getPageX(x), _getPageY(y));
    HPDF_Page_ShowText(pdfDocCurrentPage, text.c_str());
    HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_WriteTextAreaAt(float x, float y, float width, float height, std::string text, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect TextRect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_BeginText(pdfDocCurrentPage);
    HPDF_Page_TextRect(pdfDocCurrentPage, TextRect.left, TextRect.top, TextRect.right, TextRect.bottom, text.c_str(), HPDF_TALIGN_LEFT, NULL);
    HPDF_Page_EndText(pdfDocCurrentPage);
  }
}

//----------------------------------------------------------------------------------

mlPDF::IMAGE PDFCreatorBase::pdfDoc_LoadImageFromFile(std::string filename)
{
  mlPDF::IMAGE newImage = NULL;

  if (pdfDocument)
  {
    const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

    std::string last4 = "";

    if ( (filenameLength > 4) && (fileExists(filename)) )
    { 
      last4 = filename.substr(filenameLength-4, 4); 
      stringLower(last4);

      if (last4 == ".png") 
      { 
        newImage = HPDF_LoadPngImageFromFile(pdfDocument, filename.c_str());
      }
      else if ( (last4 == ".jpg") || (last4 == ".jpeg") )
      {
        newImage = HPDF_LoadJpegImageFromFile(pdfDocument, filename.c_str());
      }

      if (newImage)
      {
        pdfDocImages.push_back(newImage);
      }

    }  // if ( (filenameLength > 4) && (fileExists(filename)) )

  } // if (pdfDocument)

  return newImage;
}

//----------------------------------------------------------------------------------

mlPDF::MODEL3D PDFCreatorBase::pdfDoc_Load3DModelDataFromFile(std::string fileName)
{
  mlPDF::MODEL3D newScene = NULL;

  if (pdfDocument)
  {
    newScene = HPDF_LoadU3DFromFile(pdfDocument, fileName.c_str());

    if (newScene)
    {
      pdfDoc3DScenes.push_back(newScene);
    }
  }

  return newScene;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddImage(float x, float y, float width, float height, mlPDF::IMAGE image, bool ignoreMargins)
{
  if ( (pdfDocCurrentPage) && (image) )
  {
    HPDF_Rect imageRect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Page_DrawImage(pdfDocCurrentPage, image, imageRect.left, imageRect.bottom, width, height);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_AddImage(float x, float y, float width, float height, std::string imageFilename, bool ignoreMargins)
{
  if (pdfDocCurrentPage)
  {
    HPDF_Rect imageRect = _getPageRect(x, y, width, height, ignoreMargins);

    HPDF_Image image = pdfDoc_LoadImageFromFile(imageFilename);

    if (image)
    {
      HPDF_Page_DrawImage(pdfDocCurrentPage, image, imageRect.left, imageRect.bottom, width, height);
    }
  }
}

//----------------------------------------------------------------------------------

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal)
{
  mlPDF::VIEW3D newView = NULL;

  if ( (pdfDocCurrentPage) && (viewName != "") )
  {
    if (viewNameInternal != "")
    {
      newView = HPDF_Create3DView2(pdfDocCurrentPage->mmgr, viewName.c_str(), viewNameInternal.c_str());
    }
    else
    {
      newView = HPDF_Create3DView(pdfDocCurrentPage->mmgr, viewName.c_str());
    }
  }

  return newView;
}

//----------------------------------------------------------------------------------

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b)
{
  mlPDF::VIEW3D newView = NULL;

  if ( (pdfDocCurrentPage) && (viewName != "") )
  {
    newView = pdfDoc_3DModel_CreateView(viewName);

    if (newView)
    {
      pdfDoc_3DView_SetLightingScheme(newView, lightingScheme);
      pdfDoc_3DView_SetBackgroundColor(newView, r, g, b);
    }
  }

  return newView;
}

//----------------------------------------------------------------------------------

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, float r, float g, float b)
{
  mlPDF::VIEW3D newView = NULL;

  if ( (pdfDocCurrentPage) && (viewName != "") )
  {
    newView = pdfDoc_3DModel_CreateView(viewName, viewNameInternal);

    if (newView)
    {
      pdfDoc_3DView_SetLightingScheme(newView, lightingScheme);
      pdfDoc_3DView_SetBackgroundColor(newView, r, g, b);
    }
  }

  return newView;
}

//----------------------------------------------------------------------------------

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateView(std::string viewName, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor)
{
  mlPDF::VIEW3D newView = NULL;

  if ( (pdfDocCurrentPage) && (viewName != "") )
  {
    newView = pdfDoc_3DModel_CreateView(viewName);

    if (newView)
    {
      pdfDoc_3DView_SetLightingScheme(newView, lightingScheme);
      pdfDoc_3DView_SetBackgroundColor(newView, backgroundColor);
    }
  }

  return newView;
}

//----------------------------------------------------------------------------------

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, mlPDF::LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor)
{
  mlPDF::VIEW3D newView = NULL;

  if ( (pdfDocCurrentPage) && (viewName != "") )
  {
    newView = pdfDoc_3DModel_CreateView(viewName, viewNameInternal);

    if (newView)
    {
      pdfDoc_3DView_SetLightingScheme(newView, lightingScheme);
      pdfDoc_3DView_SetBackgroundColor(newView, backgroundColor);
    }
  }

  return newView;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_SetLightingScheme(mlPDF::VIEW3D view, mlPDF::LIGHTING_SCHEMES lightingScheme)
{
  if (view)
  {
    HPDF_3DView_SetLighting(view, mlPDF::LightingSchemeStrings[(int)lightingScheme]);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, float r, float g, float b)
{
  if (view)
  {
    HPDF_3DView_SetBackgroundColor(view, r, g, b);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_SetBackgroundColor(mlPDF::VIEW3D view, Vector3 color)
{
  if (view)
  {
    pdfDoc_3DView_SetBackgroundColor(view, (float)color.x, (float)color.y, (float)color.z);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_SetPerspectiveCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float fieldOfView, float cameraRollDegrees)
{
  if (view)
  {
    HPDF_REAL coox = (HPDF_REAL)centerOfOrbit.x;   // Center of Orbit, X
    HPDF_REAL cooy = (HPDF_REAL)centerOfOrbit.y;   // Center of Orbit, Y
    HPDF_REAL cooz = (HPDF_REAL)centerOfOrbit.z;   // Center of Orbit, Z
    HPDF_REAL c2cx = (HPDF_REAL)centerToCamera.x;  // Center to Camera, X
    HPDF_REAL c2cy = (HPDF_REAL)centerToCamera.y;  // Center to Camera, Y
    HPDF_REAL c2cz = (HPDF_REAL)centerToCamera.z;  // Center to Camera, Z
    HPDF_REAL roo  = (HPDF_REAL)radiusOfOrbit;     // Radius of Orbit
    HPDF_REAL roll = (HPDF_REAL)cameraRollDegrees; // Camera Roll in degrees

    HPDF_3DView_SetCamera(view, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

    HPDF_3DView_SetPerspectiveProjection(view, fieldOfView);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_SetOrthogonalCamera(mlPDF::VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float scaleFactor, float cameraRollDegrees)
{
  if (view)
  {
    HPDF_REAL coox = (HPDF_REAL)centerOfOrbit.x;   // Center of Orbit, X
    HPDF_REAL cooy = (HPDF_REAL)centerOfOrbit.y;   // Center of Orbit, Y
    HPDF_REAL cooz = (HPDF_REAL)centerOfOrbit.z;   // Center of Orbit, Z
    HPDF_REAL c2cx = (HPDF_REAL)centerToCamera.x;  // Center to Camera, X
    HPDF_REAL c2cy = (HPDF_REAL)centerToCamera.y;  // Center to Camera, Y
    HPDF_REAL c2cz = (HPDF_REAL)centerToCamera.z;  // Center to Camera, Z
    HPDF_REAL roo  = (HPDF_REAL)radiusOfOrbit;     // Radius of Orbit
    HPDF_REAL roll = (HPDF_REAL)cameraRollDegrees; // Camera Roll in degrees

    HPDF_3DView_SetCamera(view, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

    if (scaleFactor < 0)
    {
      scaleFactor = 1;
    }

    HPDF_3DView_SetOrthogonalProjection(view, scaleFactor);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_AddVisibleNode(mlPDF::VIEW3D view, std::string nodeName, float opacity, bool visibility)
{
  if (view)
  {
    HPDF_3DView_AddNode(view, nodeName.c_str(), opacity, visibility);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DModel_AddView(mlPDF::MODEL3D model, mlPDF::VIEW3D view)
{
  if (model && view)
  {
    HPDF_U3D_Add3DView(model, view);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, std::string viewName)
{
  if ((model) && (viewName.size() > 0))
  {
    HPDF_U3D_SetDefault3DView(model, viewName.c_str());
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DModel_SetDefaultView(mlPDF::MODEL3D model, mlPDF::VIEW3D view)
{
  if (model && view)
  {
    HPDF_U3D_SetDefault3DView2(model, view);
  }
}

//----------------------------------------------------------------------------------

mlPDF::SCENE3D PDFCreatorBase::pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, bool ignoreMargins)
{
  HPDF_Annotation newScene = NULL;

  if (pdfDocCurrentPage && model)
  {
    HPDF_Rect annotationRect = _getPageRect(x, y, width, height, ignoreMargins);
    newScene = HPDF_Page_Create3DAnnot(pdfDocCurrentPage, annotationRect, model); 
  }

  return newScene;
}

//----------------------------------------------------------------------------------

mlPDF::SCENE3D PDFCreatorBase::pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, mlPDF::IMAGE posterImage, bool ignoreMargins)
{
  HPDF_Annotation newScene = NULL;

  if (pdfDocCurrentPage)
  {
    HPDF_Rect annotationRect = _getPageRect(x, y, width, height, ignoreMargins);

    if (model)
    {
      newScene = HPDF_Page_Create3DAnnot(pdfDocCurrentPage, annotationRect, model); 
    }

    if ( (newScene) && (posterImage) )
    {
      HPDF_Page_DrawImage(pdfDocCurrentPage, posterImage, annotationRect.left, annotationRect.bottom, width, height);
    }

  }

  return newScene;
}

//----------------------------------------------------------------------------------

mlPDF::SCENE3D PDFCreatorBase::pdfDoc_Add3DScene(float x, float y, float width, float height, mlPDF::MODEL3D model, std::string posterFilename, bool ignoreMargins)
{
  HPDF_Annotation newScene = NULL;

  if (pdfDocCurrentPage)
  {
    HPDF_Rect annotationRect = _getPageRect(x, y, width, height, ignoreMargins);

    if (model)
    {
      newScene = HPDF_Page_Create3DAnnot(pdfDocCurrentPage, annotationRect, model); 
    }

    if (newScene)
    {
      HPDF_Image posterImage = pdfDoc_LoadImageFromFile(posterFilename);

      if (posterImage)
      {
        HPDF_Page_DrawImage(pdfDocCurrentPage, posterImage, annotationRect.left, annotationRect.bottom, width, height);
      }
    }

  }

  return newScene;
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DScene_SetActivationProperties(mlPDF::SCENE3D scene, std::string activationCode, std::string deactivationCode, bool toolbarEnabled, bool navigationInterfaceOpened, bool animationAutoStart)
{
  if (scene)
  {
    HPDF_Dict activationDict = HPDF_Create3DActivation(scene);

    if (activationDict)
    {
      HPDF_3DActivation_SetActivationMode(activationDict, activationCode.c_str());
      HPDF_3DActivation_SetDeactivationMode(activationDict, deactivationCode.c_str());
      HPDF_3DActivation_SetAnimationAutoStart(activationDict, animationAutoStart);
      HPDF_3DActivation_SetToolbarEnabled(activationDict, toolbarEnabled);
      HPDF_3DActivation_SetNavigationInterfaceOpened(activationDict, navigationInterfaceOpened);

      HPDF_U3D_Set3DActivation(scene, activationDict);
    }
  }

}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE