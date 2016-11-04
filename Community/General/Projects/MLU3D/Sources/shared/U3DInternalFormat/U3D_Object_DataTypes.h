//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_DataTypes.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_DataType_H
#define _U3D_Object_DataType_H


// Local includes
#include "MLU3DSystem.h"

// Global includes
#include "mlUtilsSystem.h"



ML_START_NAMESPACE

namespace mlU3D {

  struct MetaDataStruct
  {
    std::string key;
    std::string value;
  };

  typedef std::vector<MetaDataStruct> MetaDataVector;

  struct ModelBoundingBoxStruct
  {
    Vector3  start;
    Vector3  end;
    Vector3  center;
    MLdouble radius;
  };

  typedef std::map<Vector4, MLuint> ColorMap;


} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_DataType_H
