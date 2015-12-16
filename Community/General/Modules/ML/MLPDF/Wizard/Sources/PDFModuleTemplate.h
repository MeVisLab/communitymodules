//----------------------------------------------------------------------------------
//! The ML module class ${vars.moduleName}.
/*!
// \file    ${__TARGETFILE__}
// \author  ${vars.author}
// \date    ${__DATE__}
//
// ${vars.comment}
*/
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "${vars.cppProject.fullProject}System.h"

// Project includes
#include <PDFGenerator.h>

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! ${vars.comment}
class ${vars.cppProject.fullProjectUpper}_EXPORT ${vars.moduleName} : public PDFGenerator
{

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(${vars.moduleName})

public:

  //! Constructor.
  ${vars.moduleName}();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

protected:

  //! Destructor.
  virtual ~${vars.moduleName}();

  //! Assemble PDF document
  //! This is the main method for all derived modules.
  //! Add all code that assembles the actual PDF document here!
  //! Return true if assembly was successful, false otherwise.
  virtual bool assemblePDFDocument();

private:

%if fields:
  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

%for f in fields:
  //! ${f.fieldComment}
  ${f.fieldType}Field *_${f.fieldName}Fld;

%endfor
  //@}
%endif

  //! Adding a 3D figure requires a little more than just one command.
  //! Therefore it has been outsourced to a separate method...
  void _add3DFigure(float x, float y, float width, float height);

};


ML_END_NAMESPACE