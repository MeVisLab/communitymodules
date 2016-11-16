//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLPDFInit.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-02-06
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLPDFSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers ...
#include "abstract/PDFGenerator/PDFGenerator.h"
#include "PDFUtils/PDFCalcCamera.h"
#include "PDFUtils/PDFCreateFibersFromLists.h"
#include "PDFUtils/PDFCreateViewSpecifications.h"
#include "LoadPointLineGeometry/LoadPointLineGeometry.h"
#include "U3D/SaveU3D/SaveU3D.h"
#include "U3D/LoadU3D/LoadU3D.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLPDFInit()
{
  PDFGenerator::initClass();

  PDFCalcCamera::initClass();
  PDFCreateFibersFromLists::initClass();
  PDFCreateViewSpecifications::initClass();

  SaveU3D::initClass();
  LoadU3D::initClass();

  LoadPointLineGeometry::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLPDFInit)