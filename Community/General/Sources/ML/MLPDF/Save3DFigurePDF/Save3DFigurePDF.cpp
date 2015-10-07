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
    "ACTIVATION_MODE_EXPLICIT_ACIVATE",
    "ACTIVATION_MODE_PAGE_OPEN",
    "ACTIVATION_MODE_PAGE_VISIBLE"
  };

  //-------------------------------------------------------------------
  //! Strings for enum field: specification type 
  //-------------------------------------------------------------------
  const char* const DEACTIVATION_MODE_STRINGS[NUM_DEACTIVATIONMODES] = {
    "DEACTIVATION_MODE_EXPLICIT_DEACTIVATE",
    "DEACTIVATION_MODE_PAGE_CLOSED",
    "DEACTIVATION_MODE_PAGE_INVISIBLE"
  };

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_u3dFilenameFld = addString("u3dFilename"))->setStringValue("");
  (_pdfFilenameFld = addString("pdfFilename"))->setStringValue("");

  (_pageHeaderReferenceFld = addString("pageHeaderReference"))->setStringValue("");
  (_pageHeaderHeadlineFld = addString("pageHeaderHeadline"))->setStringValue("");

  (_figureActivationModeFld             = addEnum("figureActivationMode", ACTIVATION_MODE_STRINGS, NUM_ACTIVATIONMODES))->setEnumValue(ACTIVATION_MODE_EXPLICIT_ACTIVATE);
  (_figureDeactivationModeFld           = addEnum("figureDeactivationMode", DEACTIVATION_MODE_STRINGS, NUM_DEACTIVATIONMODES))->setEnumValue(DEACTIVATION_MODE_EXPLICIT_DEACTIVATE);
  (_figureAnimationAutostartFld         = addBool("figureAnimationAutostart"))->setBoolValue(false);
  (_figureToolbarEnabledFld             = addBool("figureToolbarEnabled"))->setBoolValue(true);
  (_figureNavigationInterfaceEnabledFld = addBool("figureNavigationInterfaceEnabled"))->setBoolValue(false);

  (_viewBackgroundColorFld = addColor("viewBackgroundColor"))->setVector3Value(Vector3(0.8,0.8,0.8));
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


    HPDF_U3D u3dModel;

    HPDF_Rect rect1 = { 50, 250, 350, 400 };

    u3dModel = HPDF_LoadU3DFromFile(pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\Save3DFigurePDFExample.u3d");

    HPDF_Dict defaultView = HPDF_Create3DView(pdfPage->mmgr, "Default View");
    //HPDF_3DView_AddNode(defaultView, "Cow Mesh red", 0.5, true);   // funktioniert! :-)
    //HPDF_3DView_AddNode(defaultView, "Point Set Cow", 0.5, true);  // funktioniert! :-)
    HPDF_3DView_SetLighting(defaultView, "HeadLamp");  // "None", "White", "Day", "Night", "Hard", "Primary", "Blue", "Red", "Cube", "CAD", "HeadLamp"
    HPDF_3DView_SetBackgroundColor(defaultView, 0.8f, 0.8f, 0.8f);
    //HPDF_3DView_SetPerspectiveProjection(defaultView);
    //HPDF_3DView_SetOrthogonalProjection(defaultView);

    // Kamera-Berechnung:
    // http://rodomontano.altervista.org/GuidePDF3D.htm
    // http://www3.math.tu-berlin.de/jreality/mediawiki/index.php/Export_U3D_files
    // http://www.pdflib.com/pdflib-cookbook/multimedia/javascript-for-3d-camera/

    HPDF_REAL coox = 0;  // Center of Orbit, X
    HPDF_REAL cooy = 0;  // Center of Orbit, Y
    HPDF_REAL cooz = 0;  // Center of Orbit, Z
    HPDF_REAL c2cx = 0;  // Center to Camera, X
    HPDF_REAL c2cy = 0;  // Center to Camera, Y
    HPDF_REAL c2cz = 0;  // Center to Camera, Z
    HPDF_REAL roo  = 0;  // Radius of Orbit
    HPDF_REAL roll = 0;  // Camera Roll
    HPDF_3DView_SetCamera(defaultView, coox, cooy, cooz, c2cx, c2cy, c2cz, roo, roll);

    HPDF_U3D_Add3DView(u3dModel, defaultView);
    //HPDF_U3D_SetDefault3DView(u3dModel, "Default View");

    HPDF_Annotation u3dAnnotation = HPDF_Page_Create3DAnnot(pdfPage, rect1, u3dModel); 

    HPDF_Dict activation = HPDF_Create3DActivation(u3dAnnotation);
    HPDF_3DActivation_SetActivationMode(activation, "ExplicitActivate");
    HPDF_3DActivation_SetDeactivationMode(activation, "ExplicitDeactivate");
    HPDF_3DActivation_SetAnimationAutoStart(activation, false);
    HPDF_3DActivation_SetToolbarEnabled(activation, true);
    HPDF_3DActivation_SetNavigationInterfaceOpened(activation, true);

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



//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------


ML_END_NAMESPACE