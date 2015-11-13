//----------------------------------------------------------------------------------
//! The ML module class PDF3DFigurePage_SavePDF.
/*!
// \file    PDF3DFigurePage_SavePDF.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-01
//
*/
//----------------------------------------------------------------------------------


#ifndef _Save3DFigurePDF_H
#define _Save3DFigurePDF_H


// Local includes
#include "PDF3DFigurePageSystem.h"
#include "../shared/PDFCreatorBase/MLPDF_PDFCreatorBase.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class PDF3DFIGUREPAGE_EXPORT PDF3DFigurePage_SavePDF : public PDFCreatorBase
{
public:

  //! Constructor.
  PDF3DFigurePage_SavePDF();

protected:

  //! Destructor.
  virtual ~PDF3DFigurePage_SavePDF();

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

  //! Filename fields
  StringField*  _u3dFilenameFld;
  StringField*  _posterImageFilenameFld;

  //! Page property fields
  StringField*  _pageHeaderCitationTextFld;
  StringField*  _pageHeaderHeadlineTextFld;
  BoolField*    _includeUsageHintsFld;

  //! Figure property fields
  EnumField*    _figureActivationModeFld;
  EnumField*    _figureDeactivationModeFld;
  BoolField*    _figureAnimationAutostartFld;
  BoolField*    _figureToolbarEnabledFld;
  BoolField*    _figureNavigationInterfaceEnabledFld;

  //! Description fields
  StringField*  _captionFld;
  StringField*  _descriptionFld;

  void _add3DFigure(float x, float y, float width, float height);

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDF3DFigurePage_SavePDF)
};


ML_END_NAMESPACE

#endif // _Save3DFigurePDF_H