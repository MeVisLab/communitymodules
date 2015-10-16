//----------------------------------------------------------------------------------
//! The ML module class Save3DFigurePDF.
/*!
// \file    Save3DFigurePDF.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// Creates PDF file with one interactive 3D scene for scientific publications.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "Save3DFigurePDF.h"
#include "../shared/MLPDF_Tools.h"
#include "../shared/PDFDocumentTools/MLPDF_PDFDocumentTools.h"

// ThirdParty includes
#include "hpdf.h"
#include "hpdf_u3d.h"

// ML includes
#include "mlUnicode.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(Save3DFigurePDF, Module);

//----------------------------------------------------------------------------------

Save3DFigurePDF::Save3DFigurePDF() : PDFCreatorBase()
{

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_u3dFilenameFld         = addString("u3dFilename"))->setStringValue("");
  (_posterImageFilenameFld = addString("posterImageFilename"))->setStringValue("");

  (_pageHeaderCitationTextFld = addString("pageHeaderCitationText"))->setStringValue("");
  (_pageHeaderHeadlineTextFld = addString("pageHeaderHeadlineText"))->setStringValue("");
  (_includeUsageHintsFld      = addBool("includeUsageHints"))->setBoolValue(true);

  (_figureActivationModeFld             = addEnum("figureActivationMode", mlPDF::ACTIVATION_MODE_STRINGS, mlPDF::NUM_ACTIVATIONMODES))->setEnumValue(mlPDF::ACTIVATION_MODE_EXPLICIT_ACTIVATE);
  (_figureDeactivationModeFld           = addEnum("figureDeactivationMode", mlPDF::DEACTIVATION_MODE_STRINGS, mlPDF::NUM_DEACTIVATIONMODES))->setEnumValue(mlPDF::DEACTIVATION_MODE_EXPLICIT_DEACTIVATE);
  (_figureAnimationAutostartFld         = addBool("figureAnimationAutostart"))->setBoolValue(false);
  (_figureToolbarEnabledFld             = addBool("figureToolbarEnabled"))->setBoolValue(true);
  (_figureNavigationInterfaceEnabledFld = addBool("figureNavigationInterfaceEnabled"))->setBoolValue(false);

  (_viewBackgroundColorFld = addColor("viewBackgroundColor"))->setVector3Value(Vector3(0.8,0.8,0.8));

  (_captionFld = addString("caption"))->setStringValue("");
  (_descriptionFld = addString("description"))->setStringValue("");

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------

Save3DFigurePDF::~Save3DFigurePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::handleNotification(Field* field)
{
  // Call super class functionality
  PDFCreatorBase::handleNotification(field);
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::assemblePDFDocument()
{
  //HPDF_Page pdfPage;
  docAddPage(mlPDF::PAGESIZE_A4, mlPDF::PAGEDIRECTION_PORTRAIT);
  docSetCurrentFont(buildInFonts.Times, 16);

  docWriteTextAt(130,450, "3D Annotation Test");

  // **********************************************
  // Idee: Noch ein Poster-Image hinzufügen
  // **********************************************


  HPDF_U3D u3dScene;

  std::string u3dFilename = _u3dFilenameFld->getStringValue();
  u3dScene = HPDF_LoadU3DFromFile(pdfDocument, u3dFilename.c_str());

  mlPDF::PDF3DVIEW defaultView = HPDF_Create3DView(pdfDocCurrentPage->mmgr, "Default View");
  //HPDF_3DView_AddNode(defaultView, "Cow Mesh red", 0.5, true);   // funktioniert! :-)
  //HPDF_3DView_AddNode(defaultView, "Point Set Cow", 0.5, true);  // funktioniert! :-)
  HPDF_3DView_SetLighting(defaultView, "HeadLamp");  // "None", "White", "Day", "Night", "Hard", "Primary", "Blue", "Red", "Cube", "CAD", "HeadLamp"
  HPDF_3DView_SetBackgroundColor(defaultView, 0.8f, 0.8f, 0.8f);
  //HPDF_3DView_SetPerspectiveProjection(defaultView);
  //HPDF_3DView_SetOrthogonalProjection(defaultView);

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
  HPDF_3DView_SetCamera(defaultView, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

  HPDF_REAL fov = _cameraFOVAngleFld->getFloatValue();
  HPDF_3DView_SetPerspectiveProjection(defaultView, fov);
  //HPDF_3DView_SetOrthogonalProjection(defaultView);    

  doc3DAddView(u3dScene, defaultView);
  //HPDF_U3D_SetDefault3DView(u3dModel, "Default View");

  mlPDF::PDF3DANNOTATION u3dAnnotation = doc3DCreateAnnotation(50, 250, 350, 400, u3dScene);
  doc3DSetActivationProperties(u3dAnnotation, _figureActivationModeFld->getStringValue(), _figureDeactivationModeFld->getStringValue(), _figureToolbarEnabledFld->getBoolValue(),_figureNavigationInterfaceEnabledFld->getBoolValue(), _figureAnimationAutostartFld->getBoolValue());
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE