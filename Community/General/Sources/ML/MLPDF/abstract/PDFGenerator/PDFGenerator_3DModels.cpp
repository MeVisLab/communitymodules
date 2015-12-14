//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    PDFGenerator_3DModels.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "PDFGenerator.h"
#include "../../shared/MLPDF_Tools.h"


ML_START_NAMESPACE

using namespace mlPDF;

//----------------------------------------------------------------------------------

MODEL3D PDFGenerator::pdfDoc_Load3DModelDataFromFile(std::string fileName)
{
  MODEL3D newScene = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal)
{
  VIEW3D newView = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateView(std::string viewName, LIGHTING_SCHEMES lightingScheme, float r, float g, float b)
{
  VIEW3D newView = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, LIGHTING_SCHEMES lightingScheme, float r, float g, float b)
{
  VIEW3D newView = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateView(std::string viewName, LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor)
{
  VIEW3D newView = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateView(std::string viewName, std::string viewNameInternal, LIGHTING_SCHEMES lightingScheme, Vector3 backgroundColor)
{
  VIEW3D newView = NULL;

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

VIEW3D PDFGenerator::pdfDoc_3DModel_CreateViewFromSpecificationString(std::string specificationString)
{
  VIEW3D newView = NULL;

  std::string displayName   = PDFTools::getSpecificParameterFromString(specificationString, "<DisplayName>", "Default View");
  Vector3 backgroundColor   = PDFTools::getColorVec3FromString(PDFTools::getSpecificParameterFromString(specificationString, "<BackgroundColor>"), Vector3(0, 0, 0));
  int lightingScheme        = stringToInt(PDFTools::getSpecificParameterFromString(specificationString, "<LightingScheme>", "9"));
  int renderMode            = stringToInt(PDFTools::getSpecificParameterFromString(specificationString, "<RenderMode>", "0"));
  Vector3 camCenterOfOrbit  = PDFTools::getVec3FromString(PDFTools::getSpecificParameterFromString(specificationString, "<CamCenterOfOrbit>"), Vector3(0, 0, 0));
  Vector3 camCenterToCamera = PDFTools::getVec3FromString(PDFTools::getSpecificParameterFromString(specificationString, "<CamCenterToCamera>"), Vector3(0, 0, 0));
  float camRadiusOfOrbit    = (float)stringToDouble(PDFTools::getSpecificParameterFromString(specificationString, "<CamRadiusOfOrbit>", "0"));
  float camRollAngle        = (float)stringToDouble(PDFTools::getSpecificParameterFromString(specificationString, "<CamRollAngle>", "0"));
  float camFOVAngle         = (float)stringToDouble(PDFTools::getSpecificParameterFromString(specificationString, "<CamFOVAngle>", "0"));
  std::string nodes         = PDFTools::getSpecificParameterFromString(specificationString, "<Nodes>", "");

  newView = pdfDoc_3DModel_CreateView(displayName, (LIGHTING_SCHEMES)lightingScheme, backgroundColor);

  pdfDoc_3DView_SetPerspectiveCamera(newView, camCenterOfOrbit, camRadiusOfOrbit, camCenterToCamera, camFOVAngle, camRollAngle);
  pdfDoc_3DView_SetRenderMode(newView, (MODEL_RENDERMODES)renderMode);

  pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(newView, nodes);

  return newView;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_SetLightingScheme(VIEW3D view, LIGHTING_SCHEMES lightingScheme)
{
  if (view)
  {
    HPDF_3DView_SetLighting(view, LightingSchemeStrings[(int)lightingScheme]);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_SetBackgroundColor(VIEW3D view, float r, float g, float b)
{
  if (view)
  {
    HPDF_3DView_SetBackgroundColor(view, r, g, b);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_SetBackgroundColor(VIEW3D view, Vector3 color)
{
  if (view)
  {
    pdfDoc_3DView_SetBackgroundColor(view, (float)color.x, (float)color.y, (float)color.z);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_SetRenderMode(mlPDF::VIEW3D view, mlPDF::MODEL_RENDERMODES renderMode)
{
  if (view)
  {
    HPDF_3DView_SetRenderMode(view, ModelRenderModeStrings[(int)renderMode]);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_SetPerspectiveCamera(VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float fieldOfView, float cameraRollDegrees)
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

void PDFGenerator::pdfDoc_3DView_SetOrthogonalCamera(VIEW3D view, Vector3 centerOfOrbit, float radiusOfOrbit, Vector3 centerToCamera, float scaleFactor, float cameraRollDegrees)
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

void PDFGenerator::pdfDoc_3DView_AddVisibleNode(VIEW3D view, std::string nodeName, float opacity, bool visibility)
{
  if (view)
  {
    HPDF_3DView_AddNode(view, nodeName.c_str(), opacity, visibility);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_AddVisibleNodeFromSpecificationString(VIEW3D view, std::string specificationString)
{
  if (view && specificationString != "")
  {
    std::string nodeName = PDFTools::getSpecificParameterFromString(specificationString, "<NodeName>", "");

    if (nodeName != "")
    {
      float opacity = (float)stringToDouble(PDFTools::getSpecificParameterFromString(specificationString, "<NodeOpacity>", "0"));
      bool visibility = PDFTools::stringToBool(PDFTools::getSpecificParameterFromString(specificationString, "<NodeVisibility>", "true"));

      pdfDoc_3DView_AddVisibleNode(view, nodeName, opacity, visibility);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(VIEW3D view, std::string specificationString)
{
  if (view && specificationString != "")
  {
    StringVector specificationsVector = PDFTools::stringSplit(specificationString, "<ViewNode>", false);

    for (size_t i = 0; i < specificationsVector.size(); i++)
    {
      pdfDoc_3DView_AddVisibleNodeFromSpecificationString(view, specificationsVector[i]);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DModel_AddView(MODEL3D model, VIEW3D view)
{
  if (model && view)
  {
    HPDF_U3D_Add3DView(model, view);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DModel_AddViewFromSpecificationString(MODEL3D model, std::string specificationString)
{
  if (model && specificationString != "")
  {
    VIEW3D newView = pdfDoc_3DModel_CreateViewFromSpecificationString(specificationString);

    pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(newView, specificationString);

    pdfDoc_3DModel_AddView(model, newView);
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DModel_AddAllViewsFromSpecificationString(MODEL3D model, std::string specificationString)
{
  if (model && specificationString != "")
  {
    StringVector specificationsVector = PDFTools::stringSplit(specificationString, "<View>", false);

    for (size_t i = 0; i < specificationsVector.size(); i++)
    {
      const std::string thisSpecification = PDFTools::stringTrimWhitespace(specificationsVector[i]);
      pdfDoc_3DModel_AddViewFromSpecificationString(model, thisSpecification);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DModel_SetDefaultView(MODEL3D model, std::string viewName)
{
  if ((model) && (viewName.size() > 0))
  {
    HPDF_U3D_SetDefault3DView(model, viewName.c_str());
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DModel_SetDefaultView(MODEL3D model, VIEW3D view)
{
  if (model && view)
  {
    HPDF_U3D_SetDefault3DView2(model, view);
  }
}

//----------------------------------------------------------------------------------

SCENE3D PDFGenerator::pdfDoc_Add3DScene(float x, float y, float width, float height, MODEL3D model, bool ignoreMargins)
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

SCENE3D PDFGenerator::pdfDoc_Add3DScene(float x, float y, float width, float height, MODEL3D model, IMAGE posterImage, bool ignoreMargins)
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
      // The rect must be smaller than the actual 3D scene, otherwise ugly borders might be visible
      annotationRect.left   += 1;
      annotationRect.bottom += 1;
      width  -= 2;
      height -= 2;
      HPDF_Page_DrawImage(pdfDocCurrentPage, posterImage, annotationRect.left, annotationRect.bottom, width, height);
    }

  }

  return newScene;
}

//----------------------------------------------------------------------------------

SCENE3D PDFGenerator::pdfDoc_Add3DScene(float x, float y, float width, float height, MODEL3D model, std::string posterFilename, bool ignoreMargins)
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
        // The rect must be smaller than the actual 3D scene, otherwise ugly borders might be visible
        annotationRect.left += 1;
        annotationRect.bottom += 1;
        width -= 2;
        height -= 2;
        HPDF_Page_DrawImage(pdfDocCurrentPage, posterImage, annotationRect.left, annotationRect.bottom, width, height);
      }
    }

  }

  return newScene;
}

//----------------------------------------------------------------------------------

void PDFGenerator::pdfDoc_3DScene_SetActivationProperties(SCENE3D scene, std::string activationCode, std::string deactivationCode, bool toolbarEnabled, bool navigationInterfaceOpened, bool animationAutoStart)
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


ML_END_NAMESPACE