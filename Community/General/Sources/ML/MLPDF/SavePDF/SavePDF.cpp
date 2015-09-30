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

// ThirdParty includes
#include "hpdf.h"
#include "hpdf_u3d.h"

// ML includes
#include "mlUnicode.h"


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


  (_centerOfOrbitFld  = addVector3("centerOfOrbit"))->setVector3Value(Vector3(0));
  (_centerToCameraFld = addVector3("centerToCamera"))->setVector3Value(Vector3(0));
  (_radiusOfOrbitFld  = addFloat("radiusOfOrbit"))->setFloatValue(0);
  (_cameraFOVAngleFld = addFloat("cameraFOVAngle"))->setFloatValue(90.0f);
  (_cameraRollFld     = addFloat("cameraRoll"))->setFloatValue(0);

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


    HPDF_U3D u3dModel;
    HPDF_Annotation u3dAnnot;

    HPDF_Rect rect1 = { 50, 100, 350, 400 };

//    u3dModel = HPDF_LoadU3DFromFile(pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\SavePDFExample.u3d");
//    u3dModel = HPDF_LoadU3DFromFile(pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\knee.u3d");
    std::string u3dFilename = _mlU3DFileNameFld->getStringValue();
    u3dModel = HPDF_LoadU3DFromFile(pdfDocument, u3dFilename.c_str());

    HPDF_Dict myView = HPDF_Page_Create3DView(pdfPage, u3dModel, u3dAnnot, "My View");    
    //HPDF_3DView_AddNode(defaultView, "Cow Mesh red", 0.5f, true);   // funktioniert! :-)
    //HPDF_3DView_AddNode(defaultView, "Point Set Cow", 0.5f, true);  // funktioniert! :-)
    HPDF_3DView_SetLighting(myView, "HeadLamp");  // "None", "White", "Day", "Night", "Hard", "Primary", "Blue", "Red", "Cube", "CAD", "HeadLamp"
    HPDF_3DView_SetBackgroundColor(myView, 0.8f, 0.8f, 0.8f);
    
    // Kamera-Berechnung:
    // http://rodomontano.altervista.org/GuidePDF3D.htm
    // http://www3.math.tu-berlin.de/jreality/mediawiki/index.php/Export_U3D_files
    // http://www.pdflib.com/pdflib-cookbook/multimedia/javascript-for-3d-camera/

    HPDF_REAL coox =   75.5665f;  // Center of Orbit, X
    HPDF_REAL cooy =  -25.4742f;  // Center of Orbit, Y
    HPDF_REAL cooz =   26.7935f;  // Center of Orbit, Z
    HPDF_REAL c2cx = -154.7669f;  // Center to Camera, X
    HPDF_REAL c2cy =   -0.0742f;  // Center to Camera, Y
    HPDF_REAL c2cz = -116.9477f;  // Center to Camera, Z
    HPDF_REAL roo  = sqrt(pow(c2cx, 2) + pow(c2cy, 2) + pow(c2cz, 2));  // Radius of Orbit        -> SoCustomExaminerViewer.focalDistance
    HPDF_REAL roll = 90.0f;  // Camera Roll in degrees

    Vector3 coo = _centerOfOrbitFld->getVector3Value();
    Vector3 c2c = _centerToCameraFld->getVector3Value();
    roo         = _radiusOfOrbitFld->getFloatValue();
    roll        = _cameraRollFld->getFloatValue();
    coox = (HPDF_REAL)coo.x;
    cooy = (HPDF_REAL)coo.y;
    cooz = (HPDF_REAL)coo.z;
    c2cx = (HPDF_REAL)c2c.x;
    c2cy = (HPDF_REAL)c2c.y;
    c2cz = (HPDF_REAL)c2c.z;

    HPDF_3DView_SetCamera(myView, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

    HPDF_REAL fov = _cameraFOVAngleFld->getFloatValue();
    HPDF_3DView_SetPerspectiveProjection(myView, fov);
    //HPDF_3DView_SetOrthogonalProjection(defaultView);


//    HPDF_U3D_Add3DView(u3dModel, myView);
    //HPDF_U3D_SetDefault3DView(u3dModel, "Default View");

    //HPDF_Create3DView();
    //HPDF_3DView_Add3DC3DMeasure();

    //HPDF_U3D_AddOnInstanciate();
    //HPDF_STATUS stat = HPDF_Annot_Set3DView(mmgr, annot, u3dAnnot, defaultView);

    //HPDF_Page_Create3DAnnotExData();
    //HPDF_3DAnnotExData_Set3DMeasurement();




    u3dAnnot = HPDF_Page_Create3DAnnot(pdfPage, rect1, u3dModel); 

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



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE