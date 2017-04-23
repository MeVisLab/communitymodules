//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_GeometryGenerators.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Object_GeometryGenerators.h"
#include "MLU3D_Constants.h"


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class GeometryGeneratorBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::GeometryGeneratorBase, RefCountedBase);

  //----------------------------------------------------------------------------------

  GeometryGeneratorBase::GeometryGeneratorBase() : RefCountedBase()
  {
    resourceName = "";

    normalCount = 0;

    shadingAttributes = mlU3D::SHADINGATTRIBUTES_NONE;
    diffuseColorCount  = 0;
    specularColorCount = 0;
    textureCoordCount  = 0;
    textureLayerCount  = 0;
  }

  //----------------------------------------------------------------------------------

  GeometryGeneratorBase::~GeometryGeneratorBase()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class PointSetGenerator: Point set geometry generator
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::PointSetGenerator, mlU3D::GeometryGeneratorBase);

  //----------------------------------------------------------------------------------

  PointSetGenerator::PointSetGenerator() : GeometryGeneratorBase()
  {
    pointCount = 0;
  }

  //----------------------------------------------------------------------------------

  PointSetGenerator::~PointSetGenerator()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class LineSetGenerator: Line set geometry generator
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::LineSetGenerator, mlU3D::GeometryGeneratorBase);

  //----------------------------------------------------------------------------------

  LineSetGenerator::LineSetGenerator() : GeometryGeneratorBase()
  {
    lineCount  = 0;
    pointCount = 0;
  }

  //----------------------------------------------------------------------------------

  LineSetGenerator::~LineSetGenerator()
  {
  }

  //----------------------------------------------------------------------------------


  // ****************************************************************************************************
  //
  // Class CLODMeshGenerator: Mesh geometry generator
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::CLODMeshGenerator, mlU3D::GeometryGeneratorBase);

  //----------------------------------------------------------------------------------

  CLODMeshGenerator::CLODMeshGenerator() : GeometryGeneratorBase()
  {
    vertexCount       = 0;
    faceCount         = 0;
    patchID           = 0;
    meshNumber        = 0; // Needed for progress calculation
    meshAttributes    = mlU3D::MESH_ATTRIBUTES_DEFAULT;

    _wem = NULL;
    ML_CHECK_NEW(_wem, WEM());
  }

  //----------------------------------------------------------------------------------

  CLODMeshGenerator::~CLODMeshGenerator()
  {
    _wem = NULL;
  }

  //----------------------------------------------------------------------------------

  void CLODMeshGenerator::setMeshData(WEMTrianglePatch* wemPatch)
  {
    _wem->removeAll();
    WEMTrianglePatch* newTrianglePatch = _wem->addWEMPatchCopy(wemPatch);
    newTrianglePatch->computeBoundingBox();
    newTrianglePatch->setId(0);
  }

  //----------------------------------------------------------------------------------

  WEMTrianglePatch* CLODMeshGenerator::getMeshData()
  {
    return reinterpret_cast<WEMTrianglePatch*>(_wem->getWEMPatchAt(0));
  }

  //----------------------------------------------------------------------------------






  //----------------------------------------------------------------------------------

} // end namespace mlU3D

ML_END_NAMESPACE
