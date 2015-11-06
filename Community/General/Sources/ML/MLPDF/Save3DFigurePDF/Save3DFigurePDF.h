//----------------------------------------------------------------------------------
//! The ML module class Save3DFigurePDF.
/*!
// \file    SavePRC.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-09-25
//
// Creates PDF file with one interactive 3D scene for scientific publications.
*/
//----------------------------------------------------------------------------------


#ifndef _Save3DFigurePDF_H
#define _Save3DFigurePDF_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/PDFCreatorBase/MLPDF_PDFCreatorBase.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! 
class MLPDF_EXPORT Save3DFigurePDF : public PDFCreatorBase
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
  ML_MODULE_CLASS_HEADER(Save3DFigurePDF)
};


ML_END_NAMESPACE

#endif // _Save3DFigurePDF_H