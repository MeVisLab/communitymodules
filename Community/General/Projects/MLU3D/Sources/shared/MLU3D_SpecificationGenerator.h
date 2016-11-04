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
#include "MLU3DSystem.h"

// ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  static std::string USEVERTEXCOLORS = "{VertexColors}";

  //-----------------------------------------------------------------------------------------------//
  //! Types of specifications.
  //-----------------------------------------------------------------------------------------------//
  enum NEW_SPECIFICATION_OBJECTTYPES {
    OBJECTTYPE_POINTCLOUD = 0,
    OBJECTTYPE_LINESET = 1,
    OBJECTTYPE_MESH = 2,
    OBJECTTYPE_METADATA = 3,
    NUM_OBJECTTYPES = 4
  };

  //-----------------------------------------------------------------------------------------------//
  //! Model visibility codes.
  //-----------------------------------------------------------------------------------------------//
  enum NEW_SPECIFICATION_MODELVISIBILITY {
    NEW_SPECIFICATION_MODELVISIBILITY_NOTVISIBLE = 0,
    NEW_SPECIFICATION_MODELVISIBILITY_FRONTVISIBLE = 1,
    NEW_SPECIFICATION_MODELVISIBILITY_BACKVISIBLE = 2,
    NEW_SPECIFICATION_MODELVISIBILITY_FRONTANDBACKVISIBLE = 3,
    NUM_NEW_SPECIFICATION_MODELVISIBILITY = 4
  };

  const char* const NewSpecificationModelVisibilityStrings[NUM_NEW_SPECIFICATION_MODELVISIBILITY] = {
    "MODELVISIBILITY_NOTVISIBLE",
    "MODELVISIBILITY_FRONTVISIBLE",
    "MODELVISIBILITY_BACKVISIBLE",
    "MODELVISIBILITY_FRONTANDBACKVISIBLE"
  };


class MLU3D_EXPORT SpecificationGenerator
{

public:

  static std::string GetMetaDataKey(std::string fieldValue);
  static std::string GetMetaDataValue(std::string fieldValue);
  static std::string GetWEMLabel(std::string fieldValue);
  static std::string GetPositionTypes(std::string fieldValue);
  static std::string GetConnectionTypes(std::string fieldValue);
  static std::string GetObjectName(std::string fieldValue);
  static std::string GetGroupPath(std::string fieldValue);
  static std::string GetColor(Vector3 colorValue, float alphaValue, bool useDefaultColor, bool useVertexColor);
  static std::string GetSpecularColor(Vector3 colorValue, bool useDefaultColor);
  static std::string GetOpacity(float alphaValue);
  static std::string GetModelVisibility(int fieldValue);
};

} // end namespace mlU3D

ML_END_NAMESPACE

#endif // _U3D_SpecificationGenerator_H