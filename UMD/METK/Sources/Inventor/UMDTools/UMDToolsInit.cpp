//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization
/*!
// \file    UMDToolsInit.cpp
// \author  Christian Tietjen
// \date    2004-12-23
//
*/
//----------------------------------------------------------------------------------

// Local includes
#include "UMDToolsSystem.h"
#include <XVBasics.h>
#include "SoClearGLBuffers.h"
#include "SoFileToSceneGraph.h"
#include "SoFramesPerSecond.h"
#include "SoMinimalEnclosingCircle.h"
#include "SoMousePosition.h"
#include "SoShapeInfo.h"
#include "SoShapePartition.h"


// Called on initialization time of this dll. Register all new stuff.
void UMDToolsInit()
{
   // Register the class in the inventor runtime type system.
   SoClearGLBuffers::initClass();
   SoFileToSceneGraph::initClass();
   SoFramesPerSecond::initClass();
   SoMinimalEnclosingCircle::initClass();
   SoMousePosition::initClass();
   SoShapeInfo::initClass();
   SoShapePartition::initClass();
};

// Macro implementing the platform independent dll initialization
XV_INIT_LIBRARY(UMDToolsInit)


