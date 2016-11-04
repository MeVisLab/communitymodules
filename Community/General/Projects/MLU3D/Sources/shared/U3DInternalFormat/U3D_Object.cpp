//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Object.h"


ML_START_NAMESPACE

namespace mlU3D {

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::U3DObject, RefCountedBase);

  //----------------------------------------------------------------------------------

  U3DObject::U3DObject() : RefCountedBase()
  {
    clear();
    resetBoundingBox();
  }

  //----------------------------------------------------------------------------------

  U3DObject::~U3DObject()
  {
  }

  //----------------------------------------------------------------------------------

  void U3DObject::clear()
  {
    metaData.clear();

    viewNodes.clear();
    groupNodes.clear();
    modelNodes.clear();
    lightNodes.clear();

    pointSets.clear();
    lineSets.clear();
    meshes.clear();

    lightResources.clear();
    viewResources.clear();
    litTextureShaders.clear();
    materialResources.clear();
    textureResources.clear();
    motionResources.clear();
  }

  //----------------------------------------------------------------------------------

  int U3DObject::getNumGeometryGenerators()
  {
    int numGeometryGenerators = 0;

    numGeometryGenerators += (int)pointSets.size();
    numGeometryGenerators += (int)lineSets.size();
    numGeometryGenerators += (int)meshes.size();

    return numGeometryGenerators;
  }

  //----------------------------------------------------------------------------------

  void U3DObject::resetBoundingBox()
  {
    modelBoundingBox.start.x = ML_DOUBLE_MAX;
    modelBoundingBox.start.y = ML_DOUBLE_MAX;
    modelBoundingBox.start.z = ML_DOUBLE_MAX;
    modelBoundingBox.end.x = ML_DOUBLE_MAX * -1;
    modelBoundingBox.end.y = ML_DOUBLE_MAX * -1;
    modelBoundingBox.end.z = ML_DOUBLE_MAX * -1;
  }

  //----------------------------------------------------------------------------------

} // end namespace mlU3D

ML_END_NAMESPACE
