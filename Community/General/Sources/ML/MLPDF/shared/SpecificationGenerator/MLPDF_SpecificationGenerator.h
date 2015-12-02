//----------------------------------------------------------------------------------
//! Tools for MLPDF specification generation.
/*!
// \file    MLPDF_SpecificationGenerator.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-08-26
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_SpecificationGenerator_H
#define _PDF_SpecificationGenerator_H 

#include "mlModuleIncludes.h"

#include "SaveU3D/SaveU3D.h"

ML_START_NAMESPACE

namespace mlPDF {

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
    MODELVISIBILITY_NOTVISIBLE = 0,
    MODELVISIBILITY_FRONTVISIBLE = 1,
    MODELVISIBILITY_BACKVISIBLE = 2,
    MODELVISIBILITY_FRONTANDBACKVISIBLE = 3,
    NUM_MODELVISIBILITY = 4
  };

  const char* const NewSpecificationModelVisibilityStrings[NUM_MODELVISIBILITY] = {
    "MODELVISIBILITY_NOTVISIBLE",
    "MODELVISIBILITY_FRONTVISIBLE",
    "MODELVISIBILITY_BACKVISIBLE",
    "MODELVISIBILITY_FRONTANDBACKVISIBLE"
  };


class MLPDF_EXPORT SpecificationGenerator
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

} // end namespace mlPDF

ML_END_NAMESPACE

#endif // _PDF_SpecificationGenerator_H