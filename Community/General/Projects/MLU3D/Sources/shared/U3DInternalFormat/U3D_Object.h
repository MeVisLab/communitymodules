//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_H
#define _U3D_Object_H


// Local includes
#include "../../MLU3DSystem.h"
#include "U3D_Object_Nodes.h"
#include "U3D_Object_DataTypes.h"
#include "U3D_Object_Resources.h"
#include "U3D_Object_GeometryGenerators.h"


// Global includes
#include <mlBaseInit.h>
#include "mlUtilsSystem.h"
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  class MLU3D_EXPORT U3DObject : public RefCountedBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    U3DObject();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~U3DObject();

    mlU3D::MetaDataVector  metaData;
    mlU3D::ViewNodeVector  viewNodes;
    mlU3D::GroupNodeVector groupNodes;
    mlU3D::ModelNodeVector modelNodes;
    mlU3D::LightNodeVector lightNodes;

    mlU3D::PointSetGeneratorVector pointSets;
    mlU3D::LineSetGeneratorVector  lineSets;
    mlU3D::CLODMeshGeneratorVector meshes;

    mlU3D::LightResourceVector    lightResources;
    mlU3D::ViewResourceVector     viewResources;
    mlU3D::LitTextureShaderVector litTextureShaders;
    mlU3D::MaterialResourceVector materialResources;
    mlU3D::TextureResourceVector  textureResources;
    mlU3D::MotionResourceVector   motionResources;

    mlU3D::ModelBoundingBoxStruct modelBoundingBox;

    mlU3D::DefaultValuesStruct defaultValues;

    //! Clears all internal data
    void clear();

    //! Clones the given U3D
    void cloneFrom(const U3DObject& sourceU3D);

    //! Returns the total count of all geometra generators
    int getNumGeometryGenerators(std::string prefix = "");

    //! Returns the number of model nodes
    int getNumModels(std::string prefix = "");

    //! Returns the default shader name;
    std::string getDefaultShaderName();

    //! Get stats info for network panel
    std::string getNetworkPanelInfo();

    //! Reset bounding box
    void resetBoundingBox();

    //! Update all group nodes based on the model nodes
    void updateGroupNodes();


  protected:


  private:

    //! Group node tools
    GroupNodeStructVector _assembleU3DGroupNodeInfo();
    void                  _updateGroupNodesVector(GroupNodeStructVector &groupNodes, std::string thisGroupPath);
    std::string           _getFullGroupPathFromU3DGroupNodeStruct(mlU3D::GroupNodeStruct thisNode);
    void                  _makeGroupPathNamesUnique(mlU3D::GroupNodeStructVector& groupNodes);

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(U3DObject);

  };

  //! Declares the smart pointer U3DObjectPtr from U3DObject.
  ML_REFCOUNTED_PTR(U3DObject);

} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_H
