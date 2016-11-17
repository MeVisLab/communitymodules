//----------------------------------------------------------------------------------
// Specification generator for U3D file handling.
//
// \file    MLU3D_SpecificationGenerator.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_SpecificationGenerator_H
#define _U3D_SpecificationGenerator_H 

// Local includes
#include "../MLU3DSystem.h"

// ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  //-----------------------------------------------------------------------------------------------//
  //! Types of specifications.
  //-----------------------------------------------------------------------------------------------//
  enum SPECIFICATION_OBJECTTYPES {
    SPECIFICATION_OBJECTTYPE_POINTCLOUD           = 0,
    SPECIFICATION_OBJECTTYPE_LINESET_FROM_MARKERS = 1,
    SPECIFICATION_OBJECTTYPE_LINESET_FROM_FIBERS  = 2,
    SPECIFICATION_OBJECTTYPE_MESH                 = 3,
    SPECIFICATION_OBJECTTYPE_METADATA             = 4,
    NUM_SPECIFICATION_OBJECTTYPES                 = 5
  };

  const char* const SpecificationObjectTypeStrings[mlU3D::NUM_SPECIFICATION_OBJECTTYPES] = {
    "OBJECTTYPE_POINTCLOUD",
    "OBJECTTYPE_LINESET_FROM_MARKERS",
    "OBJECTTYPE_LINESET_FROM_FIBERS",
    "OBJECTTYPE_MESH",
    "OBJECTTYPE_METADATA"
  };

  //-----------------------------------------------------------------------------------------------//
  //! Model visibility codes.
  //-----------------------------------------------------------------------------------------------//
  enum SPECIFICATION_MODELVISIBILITY {
    SPECIFICATION_MODELVISIBILITY_NOTVISIBLE          = 0,
    SPECIFICATION_MODELVISIBILITY_FRONTVISIBLE        = 1,
    SPECIFICATION_MODELVISIBILITY_BACKVISIBLE         = 2,
    SPECIFICATION_MODELVISIBILITY_FRONTANDBACKVISIBLE = 3,
    NUM_SPECIFICATION_MODELVISIBILITY                 = 4
  };

  const char* const SpecificationModelVisibilityStrings[NUM_SPECIFICATION_MODELVISIBILITY] = {
    "MODELVISIBILITY_NOTVISIBLE",
    "MODELVISIBILITY_FRONTVISIBLE",
    "MODELVISIBILITY_BACKVISIBLE",
    "MODELVISIBILITY_FRONTANDBACKVISIBLE"
  };


class MLU3D_EXPORT SpecificationGenerator
{

public:

  static std::string getMetaDataKey(std::string fieldValue);
  static std::string getMetaDataValue(std::string fieldValue);
  static std::string getWEMLabel(std::string fieldValue);
  static std::string getFiberSetIndices(std::string fieldValue);
  static std::string getPositionTypes(std::string fieldValue);
  static std::string getConnectionTypes(std::string fieldValue);
  static std::string getObjectName(std::string fieldValue);
  static std::string getGroupPath(std::string fieldValue);
  static std::string getColor(Vector3 colorValue, float alphaValue, bool useDefaultColor, bool useVertexColor);
  static std::string getSpecularColor(Vector3 colorValue, bool useDefaultColor);
  static std::string getOpacity(float alphaValue);
  static std::string getModelVisibility(int fieldValue);
};

} // end namespace mlU3D

ML_END_NAMESPACE

#endif // _U3D_SpecificationGenerator_H