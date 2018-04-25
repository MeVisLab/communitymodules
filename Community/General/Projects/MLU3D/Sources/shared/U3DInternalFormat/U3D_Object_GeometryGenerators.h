//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_GeometryGenerators.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_GeometryGenerators_H
#define _U3D_Object_GeometryGenerators_H


// Local includes
#include "../../MLU3DSystem.h"
#include "../MLU3D_DataTypes.h"

// WEM includes
#include <WEMBase/WEM.h>

// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class GeometryGeneratorBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT GeometryGeneratorBase : public RefCountedBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    GeometryGeneratorBase();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~GeometryGeneratorBase();

    std::string resourceName;

    MLuint32 normalCount;

    MLuint32 shadingAttributes;
    MLuint32 diffuseColorCount;
    MLuint32 specularColorCount;
    MLuint32 textureCoordCount;
    MLuint32 textureLayerCount;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(GeometryGeneratorBase);


  };

  //! Declares the smart pointer GeometryGeneratorBasePtr from GeometryGeneratorBase.
  ML_REFCOUNTED_PTR(GeometryGeneratorBase);


  // ****************************************************************************************************
  //
  // Class PointSetGenerator: Point set geometry generator
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT PointSetGenerator : public GeometryGeneratorBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    PointSetGenerator();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~PointSetGenerator();

    MLuint32               pointCount;
    mlU3D::PositionsVector positions;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(PointSetGenerator);


  };

  //! Declares the smart pointer PointSetGeneratorPtr from PointSetGenerator.
  ML_REFCOUNTED_PTR(PointSetGenerator);

  typedef std::vector<PointSetGenerator> PointSetGeneratorVector;


  // ****************************************************************************************************
  //
  // Class LineSetGenerator: Line set geometry generator
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT LineSetGenerator : public GeometryGeneratorBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    LineSetGenerator();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~LineSetGenerator();

    MLuint32 lineCount;
    MLuint32 pointCount;
    mlU3D::PositionsVector positions;
    mlU3D::LinesVector     lines;


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(LineSetGenerator);


  };

  //! Declares the smart pointer LineSetGeneratorPtr from LineSetGenerator.
  ML_REFCOUNTED_PTR(LineSetGenerator);

  typedef std::vector<LineSetGenerator> LineSetGeneratorVector;


  // ****************************************************************************************************
  //
  // Class CLODMeshGenerator: Mesh geometry generator
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT CLODMeshGenerator : public GeometryGeneratorBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    CLODMeshGenerator();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~CLODMeshGenerator();

    void              setMeshData(WEMTrianglePatch* wemPatch);
    WEMTrianglePatch* getMeshData();

    MLuint32 vertexCount;
    MLuint32 faceCount;
    MLuint32 patchID;
    MLuint32 meshNumber;     // Needed for progress calculation
    MLuint32 meshAttributes;
	std::vector<std::pair<MLfloat,MLfloat>> textureCoordinates;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(CLODMeshGenerator);

    WEMPtr _wem;

  };

  //! Declares the smart pointer CLODMeshGeneratorPtr from CLODMeshGenerator.
  ML_REFCOUNTED_PTR(CLODMeshGenerator);

  typedef std::vector<CLODMeshGenerator> CLODMeshGeneratorVector;

} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_GeometryGenerators_H
