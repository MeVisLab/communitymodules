//----------------------------------------------------------------------------------
// Dynamic library and runtime type system initialization.
//
// \file    MLU3DInit.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "MLU3DSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all data  structure headers ...
#include "shared/U3DInternalFormat/U3D_Object.h"
#include "shared/U3DInternalFormat/U3D_Object_Nodes.h"
#include "shared/U3DInternalFormat/U3D_Object_Modifiers.h"
#include "shared/U3DInternalFormat/U3D_Object_Resources.h"
#include "shared/U3DInternalFormat/U3D_Object_GeometryGenerators.h"

// Include all module headers ...
#include "abstract/BaseModules/U3DModule.h"
#include "abstract/BaseModules/U3DGenerator.h"
#include "abstract/BaseModules/U3DProcessor.h"
#include "abstract/BaseModules/U3DInspector.h"
#include "modules/U3DInit/U3DInit.h"
#include "modules/U3DInfo/U3DInfo.h"
#include "modules/U3DLoad/U3DLoad.h"
#include "modules/U3DSave/U3DSave.h"
#include "modules/U3DManager/U3DManager.h"
#include "modules/U3DAssemble/U3DAssemble.h"
#include "modules/U3DAddModel/U3DAddModelFromWEM.h"
#include "modules/U3DAddModel/U3DAddModelFromFiberSet.h"
#include "modules/U3DAddModel/U3DAddModelFromMarkerList.h"
#include "modules/U3DGeometryTo/U3DGeometryToXMarkerList.h"
#include "modules/U3DGeometryTo/U3DGeometryToFiberSet.h"
#include "modules/U3DGeometryTo/U3DGeometryToWEM.h"
#include "modules/U3DSpecificationGenerator/U3DSpecificationGenerator.h"
#include "modules/auxiliary/ListsToFibers.h"
#include "modules/auxiliary/LoadPointLineGeometry.h"

ML_START_NAMESPACE

using namespace mlU3D;

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLU3DInit()
{
  // Init data structures
  U3DObject     ::initClass();

  NodeBase      ::initClass();
  ViewNode      ::initClass();
  GroupNode     ::initClass();
  ModelNode     ::initClass();
  LightNode     ::initClass();

  ModifierBase  ::initClass();

  GeometryGeneratorBase ::initClass();
  PointSetGenerator     ::initClass();
  LineSetGenerator      ::initClass();
  CLODMeshGenerator     ::initClass();

  ResourceBase     ::initClass();
  LightResource    ::initClass();
  ViewResource     ::initClass();
  LitTextureShader ::initClass();
  MaterialResource ::initClass();
  TextureResource  ::initClass();
  MotionResource   ::initClass();

  // Init base modules
  U3DModule     ::initClass();
  U3DGenerator  ::initClass();
  U3DProcessor  ::initClass();
  U3DInspector  ::initClass();

  // Init modules
  U3DInit   ::initClass();
  U3DInfo   ::initClass();
  U3DLoad   ::initClass();
  U3DSave   ::initClass();

  U3DManager::initClass();
  
  U3DAssemble ::initClass();  

  U3DGeometryToXMarkerList::initClass();
  U3DGeometryToFiberSet   ::initClass();
  U3DGeometryToWEM        ::initClass();

  U3DAddModelFromWEM       ::initClass();
  U3DAddModelFromFiberSet  ::initClass();
  U3DAddModelFromMarkerList::initClass();

  U3DSpecificationGenerator::initClass();

  ListsToFibers        ::initClass();
  LoadPointLineGeometry::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLU3DInit)