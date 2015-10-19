//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    UMDAnimation2Init.cpp
// \author  Konrad Mühler
// \date    2005-05-04
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "AnimationSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

#include "UMDAnimation2/mlUMDAnimation2.h"
#include "METKObjXMLWriter/mlMETKObjXMLWriter.h"

ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type  system of the ML.
int AnimationInit (void)
{
	ML_TRACE_IN("AnimationInit ()")

	UMDAnimation2::initClass();
	METKObjXMLWriter::initClass();

	return 1;
};

ML_END_NAMESPACE

//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(AnimationInit)


