//----------------------------------------------------------------------------------
//! The ML module class Save3DFigurePDF.
/*!
// \file    SavePRC.h
// \author  Axel Newe
// \date    2015-09-25
//
// Creates PDF file with one interactive 3D scene for scientific publications.
*/
//----------------------------------------------------------------------------------


#ifndef _Save3DFigurePDF_H
#define _Save3DFigurePDF_H


// Local includes
#include "MLPDFSystem.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT Save3DFigurePDF : public Module
{
public:

  //! Constructor.
  Save3DFigurePDF();

protected:

  //! Destructor.
  virtual ~Save3DFigurePDF();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  //! Field - Filename
  StringField   *_mlFileNameFld;

  //! Field - Save notification
  NotifyField   *_saveFld;

  //! Status message
  StringField   *_statusFld;

  //! Progress bar.
  ProgressField *_progressFld;

  //! MeVisLab Program version (must be set via Python scripting, because the full version number is not available in C++)
  StringField * _mevislabVersionFld;


  //! PDF file property fields
  StringField   *_pdfAttrTitleFld;
  StringField   *_pdfAttrAuthorFld;
  StringField   *_pdfAttrSubjectFld;
  StringField   *_pdfAttrKeywordsFld;


  // Methods ============================================================

  void saveButtonClicked();
  void Save3DFigurePDFFile(std::string filename);




  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(Save3DFigurePDF)
};


ML_END_NAMESPACE

#endif // _Save3DFigurePDF_H