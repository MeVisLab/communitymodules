//----------------------------------------------------------------------------------
// Data types for U3D file handling.
//
// \file    MLU3D_DataTypes.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _MLU3D_DataTypes_H
#define _MLU3D_DataTypes_H 

// ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  typedef std::vector<std::string> StringVector;
  typedef std::vector<MLint>       MLintVector;

  struct PositionStruct
  {
    Vector3 position;
    Vector3 color;
    double  alpha;
  };

  typedef std::vector<PositionStruct> PositionsVector;

  struct LineStruct
  {
    MLuint32 startIndex;
    MLuint32 endIndex;
  };

  typedef std::vector<LineStruct> LinesVector;

  struct PointSetSpecificationStruct
  {
    std::string     internalName;
    PositionsVector positions;
  };

  typedef std::vector<PointSetSpecificationStruct> PointSetsVector;

  struct LineSetSpecificationStruct
  {
    std::string     internalName;
    PositionsVector positions;
    LinesVector     lines;
  };

  typedef std::vector<LineSetSpecificationStruct> LineSetsVector;

  enum ModelType { MODELTYPE_POINTSET, MODELTYPE_LINESET, MODELTYPE_MESH /*, MODELTYPE_GLYPH*/ };

  struct SpecificationParametersStruct
  {
    // For U3D objects

    ModelType   ObjectType;
    std::string ObjectName;
    std::string GroupPath;
    std::string Color;
    std::string SpecularColor;
    std::string Opacity;
    std::string GlyphText;       // U3D Glyphs are not parsed by Acrobat
    std::string ModelVisibility;
    std::string MetaDataKey;
    std::string MetaDataValue;
    std::string WEMLabel;
    std::string PositionTypes;
    std::string ConnectionTypes;
  };

  struct GeometryGeneratorInfoStruct
  {
    std::string GeometryName;
    bool        UseVertexColors;
  };

  typedef std::map<std::string, GeometryGeneratorInfoStruct> GeometryGeneratorMap;

} // end namespace mlU3D

ML_END_NAMESPACE

#endif // _MLU3D_DataTypes_H