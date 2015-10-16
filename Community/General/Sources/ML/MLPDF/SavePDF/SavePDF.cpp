//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    SavePDF.cpp
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PDF file.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SavePDF.h"
#include "../shared/MLPDF_Tools.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"

// ThirdParty includes
#include "hpdf.h"
#include "hpdf_u3d.h"
#include "hpdf_objects.h"

// ML includes
//#include "mlUnicode.h"

#include <Inventor/nodes/SoNodes.h>


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(SavePDF, Module);

//----------------------------------------------------------------------------------

SavePDF::SavePDF() : PDFCreatorBase()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_mlU3DFileNameFld = addString("u3dFileName"))->setStringValue("");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

SavePDF::~SavePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void SavePDF::handleNotification(Field* field)
{
  // Call super class functionality
  PDFCreatorBase::handleNotification(field);
}

//----------------------------------------------------------------------------------

void SavePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  PDFCreatorBase::activateAttachments();
}

//----------------------------------------------------------------------------------

void SavePDF::assemblePDFDocument()
{
  HPDF_Page pdfPage;
  HPDF_Font pdfFont;

  std::string producerInfoString = "MeVisLab SavePDF module (v" + getModuleVersionNumberString() + ") by Axel Newe (axel.newe@fau.de)";

  /* use Times-Roman pdfFont. */
  pdfFont = HPDF_GetFont(pdfDocument, "Times-Roman", "WinAnsiEncoding");

  pdfPage = HPDF_AddPage(pdfDocument);

  HPDF_Page_SetWidth(pdfPage, 400);
  HPDF_Page_SetHeight(pdfPage, 500);

  HPDF_Page_BeginText(pdfPage);
  HPDF_Page_SetFontAndSize(pdfPage, pdfFont, 16);
  HPDF_Page_MoveTextPos(pdfPage, 130, 450);
  HPDF_Page_ShowText(pdfPage, "3D Annotation Test");
  HPDF_Page_EndText(pdfPage);

  // **********************************************
  // Idee: Noch ein Poster-Image hinzufügen
  // **********************************************

  HPDF_U3D u3dScene;

  HPDF_Rect rect1 = { 50, 100, 350, 400 };
  
  std::string u3dFilename = _mlU3DFileNameFld->getStringValue();
  u3dScene = HPDF_LoadU3DFromFile(pdfDocument, u3dFilename.c_str());

  HPDF_Dict myView = HPDF_Create3DView(pdfPage->mmgr, "Default View");
  HPDF_3DView_SetLighting(myView, "HeadLamp");  // "None", "White", "Day", "Night", "Hard", "Primary", "Blue", "Red", "Cube", "CAD", "HeadLamp"
  HPDF_3DView_SetBackgroundColor(myView, 0.8f, 0.8f, 0.8f);
    
  Vector3 coo = _cameraCenterOfOrbitFld->getVectorValue();
  Vector3 c2c = _cameraCenterToCameraFld->getVectorValue();
  HPDF_REAL coox = (HPDF_REAL)coo.x;  // Center of Orbit, X
  HPDF_REAL cooy = (HPDF_REAL)coo.y;  // Center of Orbit, Y
  HPDF_REAL cooz = (HPDF_REAL)coo.z;  // Center of Orbit, Z
  HPDF_REAL c2cx = (HPDF_REAL)c2c.x;  // Center to Camera, X
  HPDF_REAL c2cy = (HPDF_REAL)c2c.y;  // Center to Camera, Y
  HPDF_REAL c2cz = (HPDF_REAL)c2c.z;  // Center to Camera, Z
  HPDF_REAL roo  = _cameraRadiusOfOrbitFld->getFloatValue(); // Radius of Orbit
  HPDF_REAL roll = _cameraRollAngleFld->getFloatValue();     // Camera Roll in degrees
  HPDF_3DView_SetCamera(myView, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

  HPDF_REAL fov = _cameraFOVAngleFld->getFloatValue();
  HPDF_3DView_SetPerspectiveProjection(myView, fov);
  HPDF_U3D_Add3DView(u3dScene, myView);

  HPDF_Annotation u3dAnnotation = HPDF_Page_Create3DAnnot(pdfPage, rect1, u3dScene); 

  HPDF_Dict activation = HPDF_Create3DActivation(u3dAnnotation);
  HPDF_3DActivation_SetActivationMode(activation, "ExplicitActivate");
  HPDF_3DActivation_SetDeactivationMode(activation, "ExplicitDeactivate");
  HPDF_3DActivation_SetAnimationAutoStart(activation, false);
  HPDF_3DActivation_SetToolbarEnabled(activation, false);
  HPDF_3DActivation_SetNavigationInterfaceOpened(activation, false);

  HPDF_U3D_Set3DActivation(u3dAnnotation, activation);
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE