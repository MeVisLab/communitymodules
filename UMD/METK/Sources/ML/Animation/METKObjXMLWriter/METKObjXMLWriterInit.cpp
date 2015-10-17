//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKObjXMLWriterInit.cpp
// \author  Konrad Mühler
// \date    2006-12-12
//
*/
//----------------------------------------------------------------------------------


// Local includes
#include "METKObjXMLWriterSystem.h"
#include <XVBasics.h>
#include "mlMETKObjXMLWriter.h"

// Called on initialization time of this dll. Register all new stuff.
void METKObjXMLWriterInit (void)
{
	ML_TRACE_IN("METKObjXMLWriterInit ()")
	
	ml::METKObjXMLWriter::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(METKObjXMLWriterInit)

