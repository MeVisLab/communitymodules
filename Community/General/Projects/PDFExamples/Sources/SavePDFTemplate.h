//----------------------------------------------------------------------------------
//! The ML module class SavePDFTemplate.
/*!
// \file    SavePDFTemplate.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-20
*/
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "SavePDFTemplateSystem.h"
#include "../shared/PDFCreatorBase/MLPDF_PDFCreatorBase.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class SAVEPDFTEMPLATE_EXPORT SavePDFTemplate : public PDFCreatorBase
{
public:

  //! Constructor.
  SavePDFTemplate();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

protected:

  //! Assemble PDF document
  //! This is the main method for all modules derived from PDFCreatorBase.
  //! Add all code that assembles the actual PDF document here!
  virtual void assemblePDFDocument();

private:

  StringField*  _modelFilenameFld;
  StringField*  _posterImageFilenameFld;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePDFTemplate)
};


ML_END_NAMESPACE