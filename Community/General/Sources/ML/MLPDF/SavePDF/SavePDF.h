//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    SavePDF.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-08-01
//
// Creates PDF file.
*/
//----------------------------------------------------------------------------------


#ifndef _SavePDF_H
#define _SavePDF_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/PDFCreatorBase/MLPDF_PDFCreatorBase.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT SavePDF : public PDFCreatorBase
{
public:

  //! Constructor.
  SavePDF();

protected:

  //! Destructor.
  virtual ~SavePDF();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! Assemble PDF document
  //! This is the main method for all derived modules.
  //! It is called by the saveButtonClicked() method.
  //! Add all code that assembles the actual PDF document here!
  virtual void assemblePDFDocument();

private:

  /* FIELDS */

  //! Field - Filename
  StringField   *_mlU3DFileNameFld;


  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePDF)
};


ML_END_NAMESPACE

#endif // _SavePDF_H