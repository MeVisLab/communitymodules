//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_Nodes.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_Nodes_H
#define _U3D_Object_Nodes_H


// Local includes
#include "../../MLU3DSystem.h"

// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class NodeBase: base class for all view nodes, group nodes, model nodes & light nodes
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT NodeBase : public RefCountedBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    NodeBase();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~NodeBase();

    std::string nodeName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(NodeBase);


  };

  //! Declares the smart pointer NodeBasePtr from NodeBase.
  ML_REFCOUNTED_PTR(NodeBase);


  // ****************************************************************************************************
  //
  // Class ViewNode: Represents a View Node
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT ViewNode : public NodeBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    ViewNode();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~ViewNode();

    std::string displayName;
    std::string resourceName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(ViewNode);


  };

  //! Declares the smart pointer ViewNodePtr from ViewNode.
  ML_REFCOUNTED_PTR(ViewNode);

  typedef std::vector<ViewNode> ViewNodeVector;


  // ****************************************************************************************************
  //
  // Class GroupNode: Represents a Group Node
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT GroupNode : public NodeBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    GroupNode();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~GroupNode();

    std::string displayName;
    std::string parentName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(GroupNode);


  };

  //! Declares the smart pointer GroupNodePtr from GroupNode.
  ML_REFCOUNTED_PTR(GroupNode);

  typedef std::vector<GroupNode> GroupNodeVector;


  // ****************************************************************************************************
  //
  // Class ModelNode: Represents a Model Node
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT ModelNode : public NodeBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    ModelNode();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~ModelNode();

    std::string displayName;
    std::string internalName;
    std::string groupPath;
    std::string shaderName;
    std::string geometryGeneratorName;
    MLuint32    visibility;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(ModelNode);


  };

  //! Declares the smart pointer ModelNodePtr from ViewNode.
  ML_REFCOUNTED_PTR(ModelNode);

  typedef std::vector<ModelNode> ModelNodeVector;


  // ****************************************************************************************************
  //
  // Class LightNode: Represents a Light Node
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT LightNode : public NodeBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    LightNode();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~LightNode();

    std::string displayName;
    std::string resourceName;

  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(LightNode);


  };

  //! Declares the smart pointer LightNodePtr from LightNode.
  ML_REFCOUNTED_PTR(LightNode);

  typedef std::vector<LightNode> LightNodeVector;






} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_Nodes_H
