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

SavePDF::SavePDF() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_mlPDFFileNameFld = addString("pdfFilename"))->setStringValue("");
  (_mlU3DFileNameFld = addString("u3dFileName"))->setStringValue("");

  _saveFld = addNotify("save");

  (_statusFld     = addString("status"))    ->setStringValue("Idle.");
  (_progressFld   = addProgress("progress"))->setFloatValue(0.0f);

  (_mevislabVersionFld = addString("mevislabVersion"))->setStringValue("");

  (_pdfAttrTitleFld     = addString("pdfAttrTitle"))    ->setStringValue("");
  (_pdfAttrAuthorFld    = addString("pdfAttrAuthor"))   ->setStringValue("");
  (_pdfAttrSubjectFld   = addString("pdfAttrSubject"))  ->setStringValue("");
  (_pdfAttrKeywordsFld  = addString("pdfAttrKeywords")) ->setStringValue("");

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

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate pdfPage data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

}

//----------------------------------------------------------------------------------

SavePDF::~SavePDF()
{
  // Destroy own dynamic data structures here
}

//----------------------------------------------------------------------------------

void SavePDF::handleNotification(Field* field)
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

void SavePDF::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void SavePDF::saveButtonClicked()
{
  std::string filename = _mlPDFFileNameFld->getStringValue();
  if (filename == "") 
  {
    _statusFld->setStringValue("No filename specified.");
    return;
  }

  const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

  std::string last4 = "";

  if (filenameLength > 4) 
  { 
    last4 = filename.substr(filenameLength-4, 4); 

    if (last4 != ".pdf") 
    { 
      filename.append(".pdf"); 
      _mlPDFFileNameFld->setStringValue(filename);
    }
  }

  savePDFFile(filename);
}

//----------------------------------------------------------------------------------

void SavePDF::savePDFFile(std::string filename)
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
    HPDF_SetInfoAttr (pdfDocument, HPDF_INFO_PRODUCER, "MeVisLab SavePDF module by Axel Newe (axel.newe@fau.de)");

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
//    HPDF_Annotation u3dAnnotation;

    HPDF_Rect rect1 = { 50, 100, 350, 400 };

//    u3dScene = HPDF_LoadU3DFromFile(pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\SavePDFExample.u3d");
//    u3dScene = HPDF_LoadU3DFromFile(pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\knee.u3d");
    std::string u3dFilename = _mlU3DFileNameFld->getStringValue();
    u3dScene = HPDF_LoadU3DFromFile(pdfDocument, u3dFilename.c_str());


    HPDF_Dict myView = HPDF_Create3DView(pdfPage->mmgr, "Default View");
    //HPDF_3DView_AddNode(myView, "Cow Mesh red", 0.5f, true);   // funktioniert! :-)
    //HPDF_3DView_AddNode(myView, "Point Set Cow", 0.5f, true);  // funktioniert! :-)
    HPDF_3DView_SetLighting(myView, "HeadLamp");  // "None", "White", "Day", "Night", "Hard", "Primary", "Blue", "Red", "Cube", "CAD", "HeadLamp"
    HPDF_3DView_SetBackgroundColor(myView, 0.8f, 0.8f, 0.8f);
    
    // Kamera-Berechnung:
    // http://rodomontano.altervista.org/GuidePDF3D.htm
    // http://www3.math.tu-berlin.de/jreality/mediawiki/index.php/Export_U3D_files
    // http://www.pdflib.com/pdflib-cookbook/multimedia/javascript-for-3d-camera/

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
    //HPDF_3DView_SetOrthogonalProjection(defaultView);    
    HPDF_U3D_Add3DView(u3dScene, myView);

/*
    HPDF_Dict myView2 = HPDF_Create3DView(pdfPage->mmgr, "Alternative View");
    HPDF_3DView_SetLighting(myView2, "White");
    HPDF_3DView_SetBackgroundColor(myView2, 0.1f, 0.1f, 0.1f);
    HPDF_3DView_SetCamera(myView2, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll+180);
    HPDF_3DView_SetPerspectiveProjection(myView2, fov);
    HPDF_U3D_Add3DView(u3dScene, myView2);

    HPDF_U3D_SetDefault3DView(u3dScene, "Alternative View");
*/

    HPDF_Annotation u3dAnnotation = HPDF_Page_Create3DAnnot(pdfPage, rect1, u3dScene); 

//    HPDF_JavaScript script = HPDF_CreateJavaScript(pdfDocument, "var i = 0;");

    HPDF_Dict activation = HPDF_Create3DActivation(u3dAnnotation);
    HPDF_3DActivation_SetActivationMode(activation, "ExplicitActivate");
    HPDF_3DActivation_SetDeactivationMode(activation, "ExplicitDeactivate");
    HPDF_3DActivation_SetAnimationAutoStart(activation, false);
    HPDF_3DActivation_SetToolbarEnabled(activation, false);
    HPDF_3DActivation_SetNavigationInterfaceOpened(activation, false);

    HPDF_U3D_Set3DActivation(u3dAnnotation, activation);

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

void SavePDF::_calculateCameraPropertiesFromInventorCamera()
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