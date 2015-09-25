//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLPDFInit.cpp
// \author  Axel Newe
// \date    2014-02-06
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLPDFSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "SavePDF/SavePDF.h"
#include "Save3DFigurePDF/Save3DFigurePDF.h"
#include "SaveU3D/SaveU3D.h"
#include "SavePRC/SavePRC.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLPDFInit()
{
  SavePDF::initClass();
  Save3DFigurePDF::initClass();
  SaveU3D::initClass();
  SavePRC::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLPDFInit)