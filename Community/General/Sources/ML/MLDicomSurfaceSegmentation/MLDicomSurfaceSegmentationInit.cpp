//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLDicomSurfaceSegmentationInit.cpp
// \author  Axel Newe
// \date    2015-09-25
*/
//----------------------------------------------------------------------------------


// Local includes
#include "MLDicomSurfaceSegmentationSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Module-header includes
#include "DicomSurfaceSegmentationLoad/DicomSurfaceSegmentationLoad.h"
#include "DicomSurfaceSegmentationSave/DicomSurfaceSegmentationSave.h"

ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int DicomSurfaceSegmentationInit()
{
  // Add initClass calls from modules here.
  DicomSurfaceSegmentationLoad::initClass();
  DicomSurfaceSegmentationSave::initClass();

  return 1;
}

ML_END_NAMESPACE

//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(DicomSurfaceSegmentationInit)