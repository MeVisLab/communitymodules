//----------------------------------------------------------------------------------
//! The ML module class SavePDF.
/*!
// \file    SavePRC.h
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PDF file.
*/
//----------------------------------------------------------------------------------


#ifndef _SavePDF_H
#define _SavePDF_H


// Local includes
#include "MLPDFSystem.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT SavePDF : public Module
{
public:

  //! Constructor.
  SavePDF();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

  //! Updates the internal module state after loading or cloning the module,
  //! and enables notification handling again.
  virtual void activateAttachments();

private:


  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePDF)
};


ML_END_NAMESPACE

#endif // _SavePDF_H