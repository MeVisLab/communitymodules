//----------------------------------------------------------------------------------
//! Tools for PDF file creation.
/*!
// \file    MLPDF_DataTypes.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
*/
//----------------------------------------------------------------------------------


#ifndef _PDF_DataTypes_H
#define _PDF_DataTypes_H 

#include "mlModuleIncludes.h"

ML_START_NAMESPACE

  

typedef std::vector<std::string> StringVector;
typedef std::vector<MLint>       MLintVector;

struct ModelBoundingBoxStruct
{
  Vector3  start;
  Vector3  end;
  Vector3  center;
  MLdouble radius;
};

struct GroupNodeStruct
{
  MLint        id;
  std::string  name;
  StringVector parents;
};

typedef std::vector<GroupNodeStruct> GroupNodeVector;

struct MetaDataStruct
{
  std::string key;
  std::string value;
};

typedef std::vector<MetaDataStruct> MetaDataVector;

struct PositionStruct
{
  Vector3 position;
  Vector3 color;
  double  alpha;
  double  size;
};

typedef std::vector<PositionStruct> PositionsVector;

struct LineStruct
{
  MLuint32 startIndex;
  MLuint32 endIndex;
  double   width;
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

struct SpecificationParametersStruct
{
  // For U3D objects
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
  double      PointSize;
  double      LineWidth;
};

ML_END_NAMESPACE

#endif // _PDF_DataTypes_H