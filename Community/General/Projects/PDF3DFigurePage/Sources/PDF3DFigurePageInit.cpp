//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    PDF3DFigurePageInit.cpp
// \author  Axel Newe
// \date    2015-10-01
*/
//----------------------------------------------------------------------------------


// Local includes
#include "PDF3DFigurePageSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "PDF3DFigurePage_Utils/PDF3DFigurePage_Utils.h"
#include "PDF3DFigurePage_SavePDF/PDF3DFigurePage_SavePDF.h"
#include "PDF3DFigurePage_LoadPointLineGeometry/PDF3DFigurePage_LoadPointLineGeometry.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int PDF3DFigurePageInit()
{
  // Add initClass calls from modules here.
  PDF3DFigurePage_Utils::initClass();
  PDF3DFigurePage_LoadPointLineGeometry::initClass();
  PDF3DFigurePage_SavePDF::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(PDF3DFigurePageInit)