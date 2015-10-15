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

Save3DFigurePDF::Save3DFigurePDF() : Module(0, 0)
{
  //-------------------------------------------------------------------
  //! Strings for enum field: specification type 
  //-------------------------------------------------------------------
  const char* const ACTIVATION_MODE_STRINGS[NUM_ACTIVATIONMODES] = {
    "ExplicitActivate",
    "PageOpen",
    "PageVisible"
  };

  //-------------------------------------------------------------------
  //! Strings for enum field: specification type 
  //-------------------------------------------------------------------
  const char* const DEACTIVATION_MODE_STRINGS[NUM_DEACTIVATIONMODES] = {
    "ExplicitDeactivate",
    "PageClosed",
    "PageInvisible"
  };

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_u3dFilenameFld         = addString("u3dFilename"))->setStringValue("");
  (_posterImageFilenameFld = addString("posterImageFilename"))->setStringValue("");
  (_pdfFilenameFld         = addString("pdfFilename"))->setStringValue("");

  (_pageHeaderCitationTextFld = addString("pageHeaderCitationText"))->setStringValue("");
  (_pageHeaderHeadlineTextFld = addString("pageHeaderHeadlineText"))->setStringValue("");
  (_includeUsageHintsFld      = addBool("includeUsageHints"))->setBoolValue(true);

  (_figureActivationModeFld             = addEnum("figureActivationMode", ACTIVATION_MODE_STRINGS, NUM_ACTIVATIONMODES))->setEnumValue(ACTIVATION_MODE_EXPLICIT_ACTIVATE);
  (_figureDeactivationModeFld           = addEnum("figureDeactivationMode", DEACTIVATION_MODE_STRINGS, NUM_DEACTIVATIONMODES))->setEnumValue(DEACTIVATION_MODE_EXPLICIT_DEACTIVATE);
  (_figureAnimationAutostartFld         = addBool("figureAnimationAutostart"))->setBoolValue(false);
  (_figureToolbarEnabledFld             = addBool("figureToolbarEnabled"))->setBoolValue(true);
  (_figureNavigationInterfaceEnabledFld = addBool("figureNavigationInterfaceEnabled"))->setBoolValue(false);

  (_viewBackgroundColorFld = addColor("viewBackgroundColor"))->setVector3Value(Vector3(0.8,0.8,0.8));

  _calculateCameraFromInventorSceneFld   = addNotify("calculateCameraFromInventorScene");
  (_autoCalculateCameraFromInventorScene = addBool("autoCalculateCameraFromInventorScene"))->setBoolValue(false);
  (_inventorCameraPositionFld            = addVector3("inventorCameraPosition")) ->setVector3Value(Vector3(0,0,0));
  (_inventorCameraOrientationFld         = addVector4("inventorCameraOrientation")) ->setVector4Value(Vector4(0,0,1,0));
  (_inventorCameraFocalDistanceFld       = addFloat("inventorCameraFocalDistance"))->setFloatValue(0);
  (_inventorCameraHeightFld              = addFloat("inventorCameraHeight"))       ->setFloatValue(0);

  (_cameraCenterOfOrbitFld  = addVector3("cameraCenterOfOrbit")) ->setVector3Value(Vector3(0));
  (_cameraCenterToCameraFld = addVector3("cameraCenterToCamera"))->setVector3Value(Vector3(0));
  (_cameraRadiusOfOrbitFld  = addFloat("cameraRadiusOfOrbit"))   ->setFloatValue(0);
  (_cameraFOVAngleFld       = addFloat("cameraFOVAngle"))        ->setFloatValue(90.0f);
  (_cameraRollAngleFld      = addFloat("cameraRollAngle"))            ->setFloatValue(0);

  (_captionFld = addString("caption"))->setStringValue("");
  (_descriptionFld = addString("description"))->setStringValue("");

  _saveFld = addNotify("save");

  (_statusFld     = addString("status"))    ->setStringValue("Idle.");
  (_progressFld   = addProgress("progress"))->setFloatValue(0.0f);

  (_mevislabVersionFld = addString("mevislabVersion"))->setStringValue("");

  (_pdfAttrTitleFld     = addString("pdfAttrTitle"))    ->setStringValue("");
  (_pdfAttrAuthorFld    = addString("pdfAttrAuthor"))   ->setStringValue("");
  (_pdfAttrSubjectFld   = addString("pdfAttrSubject"))  ->setStringValue("");
  (_pdfAttrKeywordsFld  = addString("pdfAttrKeywords")) ->setStringValue("");



  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate pdfPage data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

}

//----------------------------------------------------------------------------------

Save3DFigurePDF::~Save3DFigurePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::handleNotification(Field* field)
{
  if (field == _saveFld) 
  {
    // Call the save routine.
    saveButtonClicked();
  } 

  if (field == _calculateCameraFromInventorSceneFld) 
  {
    _calculateCameraPropertiesFromInventorCamera();
  } 

  if (
      (
        (field == _autoCalculateCameraFromInventorScene) ||
        (field == _inventorCameraPositionFld) ||
        (field == _inventorCameraOrientationFld) ||
        (field == _autoCalculateCameraFromInventorScene) ||
        (field == _inventorCameraFocalDistanceFld) ||
        (field == _inventorCameraHeightFld)
      ) 
      && 
      (_autoCalculateCameraFromInventorScene->getBoolValue())
     )
  {
    _calculateCameraPropertiesFromInventorCamera();
  }

}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::saveButtonClicked()
{
  unsigned int filenameLength = 0;
  std::string last4 = "";

  //
  // Check PDF filename
  //
  std::string pdfFilename = _pdfFilenameFld->getStringValue();
  if (pdfFilename == "") 
  {
    _statusFld->setStringValue("No PDF filename specified.");
    return;
  }

  filenameLength = static_cast<unsigned int>(pdfFilename.length());

  if (filenameLength > 4) 
  { 
    last4 = pdfFilename.substr(filenameLength-4, 4); 

    if (last4 != ".pdf") 
    { 
      pdfFilename.append(".pdf"); 
      _pdfFilenameFld->setStringValue(pdfFilename);
    }
  }

  //
  // Check U3D filename
  //
  std::string u3dFilename = _u3dFilenameFld->getStringValue();
  if (u3dFilename == "") 
  {
    _statusFld->setStringValue("No U3D filename specified.");
    return;
  }

  filenameLength = static_cast<unsigned int>(u3dFilename.length());

  if (filenameLength > 4) 
  { 
    last4 = u3dFilename.substr(filenameLength-4, 4); 

    if (last4 != ".u3d") 
    { 
      u3dFilename.append(".pdf"); 
      _u3dFilenameFld->setStringValue(u3dFilename);
    }
  }


  Save3DFigurePDFFile(pdfFilename);
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::Save3DFigurePDFFile(std::string filename)
{
  _progressFld->setFloatValue(0.0f);

///////////////////////////////////////////////////////////////////////////////////////////////////////

  HPDF_Doc  pdfDocument;
  pdfDocument = HPDF_New (NULL, NULL);

  if (pdfDocument)
  {
    HPDF_Page pdfPage;
    HPDF_Font pdfFont;

    pdfDocument->pdf_version = HPDF_VER_16;  // 3D models are allowed from v1.6 on

    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_TITLE, _pdfAttrTitleFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_AUTHOR, _pdfAttrAuthorFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_SUBJECT, _pdfAttrSubjectFld->getStringValue().c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_KEYWORDS, _pdfAttrKeywordsFld->getStringValue().c_str());

    std::string VersionString = "MeVisLab " + getMeVisLabVersionNumberString();
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_CREATOR, VersionString.c_str());
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_PRODUCER, "MeVisLab Save3DFigurePDF module by Axel Newe (axel.newe@fau.de)");

    /* use Times-Roman pdfFont. */
    pdfFont = HPDF_GetFont (pdfDocument, "Times-Roman", "WinAnsiEncoding");

    pdfPage = HPDF_AddPage (pdfDocument);

    HPDF_Page_SetWidth (pdfPage, 400);
    HPDF_Page_SetHeight (pdfPage, 500);

    HPDF_Page_BeginText (pdfPage);
    HPDF_Page_SetFontAndSize (pdfPage, pdfFont, 16);
    HPDF_Page_MoveTextPos (pdfPage, 130, 450);
    HPDF_Page_ShowText (pdfPage, "3D Annotation Test");
    HPDF_Page_EndText (pdfPage);


    // **********************************************
    // Idee: Noch ein Poster-Image hinzufügen
    // **********************************************


    HPDF_U3D u3dScene;

    HPDF_Rect annotationRect = { 50, 250, 350, 400 };

    std::string u3dFilename = _u3dFilenameFld->getStringValue();
    u3dScene = HPDF_LoadU3DFromFile(pdfDocument, u3dFilename.c_str());

    HPDF_Dict defaultView = HPDF_Create3DView(pdfPage->mmgr, "Default View");
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
    HPDF_U3D_Add3DView(u3dScene, defaultView);
    //HPDF_U3D_SetDefault3DView(u3dModel, "Default View");

    HPDF_Annotation u3dAnnotation = HPDF_Page_Create3DAnnot(pdfPage, annotationRect, u3dScene); 

    if (u3dAnnotation)
    {
      HPDF_Dict activation = HPDF_Create3DActivation(u3dAnnotation);
      HPDF_3DActivation_SetActivationMode(activation, _figureActivationModeFld->getStringValue().c_str());
      HPDF_3DActivation_SetDeactivationMode(activation, _figureDeactivationModeFld->getStringValue().c_str());
      HPDF_3DActivation_SetAnimationAutoStart(activation, _figureAnimationAutostartFld->getBoolValue());
      HPDF_3DActivation_SetToolbarEnabled(activation, _figureToolbarEnabledFld->getBoolValue());
      HPDF_3DActivation_SetNavigationInterfaceOpened(activation, _figureNavigationInterfaceEnabledFld->getBoolValue());

      HPDF_U3D_Set3DActivation(u3dAnnotation, activation);
    }

    /* save the document to a file */
    HPDF_SaveToFile (pdfDocument, filename.c_str());

    /* clean up */
    HPDF_Free (pdfDocument);

    _progressFld->setFloatValue(1.0f);
  }
  else
  {
    _statusFld->setStringValue("Unable to create PDF document '" + filename + "'.");
  }


///////////////////////////////////////////////////////////////////////////////////////////////////////

  return;
}

//----------------------------------------------------------------------------------

void Save3DFigurePDF::_calculateCameraPropertiesFromInventorCamera()
{
  Vector3 inventorCameraPosition      = _inventorCameraPositionFld->getVectorValue();
  Vector4 inventorCameraOrientation   = _inventorCameraOrientationFld->getVectorValue();
  float   inventorCameraFocalDistance = _inventorCameraFocalDistanceFld->getFloatValue();
  float   inventorCameraHeight        = _inventorCameraHeightFld->getFloatValue();
  Vector3 camCenterOfOrbit;
  Vector3 camCenterToCamera;
  float   camRadiusOfOrbit;
  float   camRollAngle;
  float   camFOVAngle;

  mlPDF::PDFDocumentTools::CalculateCameraPropertiesFromInventorCamera(
    inventorCameraPosition, inventorCameraOrientation, inventorCameraFocalDistance, inventorCameraHeight,
    camCenterOfOrbit, camCenterToCamera, camRadiusOfOrbit, camRollAngle, camFOVAngle);

  // Set field values
  _cameraCenterOfOrbitFld ->setVector3Value(camCenterOfOrbit);
  _cameraCenterToCameraFld->setVector3Value(camCenterToCamera);
  _cameraRadiusOfOrbitFld ->setFloatValue(inventorCameraFocalDistance);
  _cameraRollAngleFld     ->setFloatValue(camRollAngle);
  _cameraFOVAngleFld      ->setFloatValue(camFOVAngle);
}


//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE