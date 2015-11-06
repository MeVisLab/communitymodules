//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    MLPDF_PDFCreatorBase_3DModels.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Base class for PDF creators.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "MLPDF_PDFCreatorBase.h"
//#include "../MLPDF_DataTypes.h"
#include "../MLPDF_Tools.h"

ML_START_NAMESPACE

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

mlPDF::VIEW3D PDFCreatorBase::pdfDoc_3DModel_CreateViewFromSpecificationString(std::string specificationString)
{
  mlPDF::VIEW3D newView = NULL;

  std::string displayName   = getSpecificParameterFromString(specificationString, "<DisplayName>", "Default View");
  Vector3 backgroundColor   = getColorVec3(getSpecificParameterFromString(specificationString, "<BackgroundColor>"), Vector3(0,0,0));
  int lightingScheme        = stringToInt(getSpecificParameterFromString(specificationString, "<LightingScheme>", "9"));
  Vector3 camCenterOfOrbit  = getColorVec3(getSpecificParameterFromString(specificationString, "<CamCenterOfOrbit>"), Vector3(0, 0, 0));
  Vector3 camCenterToCamera = getColorVec3(getSpecificParameterFromString(specificationString, "<CamCenterToCamera>"), Vector3(0, 0, 0));
  float camRadiusOfOrbit    = (float)stringToDouble(getSpecificParameterFromString(specificationString, "<CamRadiusOfOrbit>", "0"));
  float camRollAngle        = (float)stringToDouble(getSpecificParameterFromString(specificationString, "<CamRollAngle>", "0"));
  float camFOVAngle         = (float)stringToDouble(getSpecificParameterFromString(specificationString, "<CamFOVAngle>", "0"));
  std::string nodes         = getSpecificParameterFromString(specificationString, "<Nodes>", "");

  newView = pdfDoc_3DModel_CreateView(displayName, (mlPDF::LIGHTING_SCHEMES)lightingScheme, backgroundColor);

  pdfDoc_3DView_SetPerspectiveCamera(newView, camCenterOfOrbit, camRadiusOfOrbit, camCenterToCamera, camFOVAngle, camRollAngle);

  pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(newView, nodes);

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

void PDFCreatorBase::pdfDoc_3DView_AddVisibleNodeFromSpecificationString(mlPDF::VIEW3D view, std::string specificationString)
{
  if (view && specificationString != "")
  {
    std::string nodeName = getSpecificParameterFromString(specificationString, "<NodeName>", "");

    if (nodeName != "")
    {
      float opacity = (float)stringToDouble(getSpecificParameterFromString(specificationString, "<NodeOpacity>", "0"));
      bool visibility = stringToBool(getSpecificParameterFromString(specificationString, "<NodeVisibility>", "true"));

      pdfDoc_3DView_AddVisibleNode(view, nodeName, opacity, visibility);
    }
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(mlPDF::VIEW3D view, std::string specificationString)
{
  if (view && specificationString != "")
  {
    StringVector specificationsVector = stringSplit(specificationString, "<ViewNode>", false);

    for (size_t i = 0; i < specificationsVector.size(); i++)
    {
      pdfDoc_3DView_AddVisibleNodeFromSpecificationString(view, specificationsVector[i]);
    }
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

void PDFCreatorBase::pdfDoc_3DModel_AddViewFromSpecificationString(mlPDF::MODEL3D model, std::string specificationString)
{
  if (model && specificationString != "")
  {
    mlPDF::VIEW3D newView = pdfDoc_3DModel_CreateViewFromSpecificationString(specificationString);

    pdfDoc_3DView_AddAllVisibleNodesFromSpecificationString(newView, specificationString);

    pdfDoc_3DModel_AddView(model, newView);
  }
}

//----------------------------------------------------------------------------------

void PDFCreatorBase::pdfDoc_3DModel_AddAllViewsFromSpecificationString(mlPDF::MODEL3D model, std::string specificationString)
{
  if (model && specificationString != "")
  {
    StringVector specificationsVector = stringSplit(specificationString, "<View>", false);

    for (size_t i = 0; i < specificationsVector.size(); i++)
    {
      pdfDoc_3DModel_AddViewFromSpecificationString(model, specificationsVector[i]);
    }
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


ML_END_NAMESPACE