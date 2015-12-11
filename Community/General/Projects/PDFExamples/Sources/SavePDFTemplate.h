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

// Project includes
#include <PDFGenerator.h>

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class SAVEPDFTEMPLATE_EXPORT SavePDFTemplate : public PDFGenerator
{
public:

  //! Constructor.
  SavePDFTemplate();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

protected:

  //! Assemble PDF document
  //! This is the main method for all derived modules.
  //! Add all code that assembles the actual PDF document here!
  //! Return true if assembly was successful, false otherwise.
  virtual bool assemblePDFDocument();

private:

  //! Adding a 3D figure requires a little more than just one command.
  //! Therefore it has been outsourced to a separate method...
  void _add3DFigure(float x, float y, float width, float height);

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(SavePDFTemplate)
};


ML_END_NAMESPACE