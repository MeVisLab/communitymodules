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


  (_mlFileNameFld = addString("filename"))->setStringValue("");

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
  std::string filename = _mlFileNameFld->getStringValue();
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
      _mlFileNameFld->setStringValue(filename);
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




    HPDF_U3D u3dModel;
    HPDF_Annotation u3dAnnot;

    HPDF_Rect rect1 = { 50, 250, 350, 400 };

    u3dModel = HPDF_LoadU3DFromFile (pdfDocument, "D:\\MeVisLab\\Packages\\Community\\General\\Modules\\ML\\MLPDF\\networks\\SavePDFExample.u3d");
    u3dAnnot = HPDF_Page_Create3DAnnot (pdfPage, rect1, u3dModel); 
    HPDF_Dict view = HPDF_Page_Create3DView(pdfPage, u3dModel, u3dAnnot, "View Name");
    HPDF_3DView_SetBackgroundColor(view, 0.5, 0.5, 0.5);
    //HPDF_STATUS stat = HPDF_Annot_Set3DView (mmgr, annot, u3dAnnot, view);



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