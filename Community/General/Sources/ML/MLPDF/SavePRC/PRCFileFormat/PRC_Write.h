//----------------------------------------------------------------------------------
//! This header files contains 
/*! 
//
// \file    PRC_Write.h
// \author  Axel Newe
// \date    2014-09-01
*/


#ifndef _PRC_Write_H
#define _PRC_Write_H

#include<math.h>

#include "PRC_BitStream.h"
#include "PRC_DataTypes.h"
#include "PRC_Defines.h"

// ML includes
#include "mlModuleIncludes.h"


ML_START_NAMESPACE



void writeUncompressedUnsignedInteger(std::ostream &out, MLuint32 data);
void writeUnit(PRC_BitStream &,bool,double);
void writeEmptyMarkups(PRC_BitStream&);
bool isTypeEligibleForReference(MLuint32 type);
MLuint32 createCADIdentifier();
MLuint32 createPRCUniqueIdentifier();
void writeName(PRC_BitStream&,const std::string&);
void resetName();
void writeGraphics(PRC_BitStream&, MLuint32=MLuint32_MAX, MLuint32=MLuint32_MAX, uint16_t=1, bool=false);
void resetGraphics();
void resetGraphicsAndName();


#pragma warning( push )
  #pragma warning( disable : 4512 )
  // Suppress warning that assignment operator could not be generated.

class PRC_UserData
{
public:
  PRC_UserData(MLuint32 s = 0, MLuint8* d = 0) : size(s),data(d) {}

  void write(PRC_BitStream&);

private:
  MLuint32 size;
  MLuint8* data;
};

class PRC_SingleAttribute : public PRC_AttributeEntry
{
public:
  PRC_SingleAttribute() : type(PRC_ModellerAttributeType_Null) {}
	PRC_SingleAttribute(int32_t integer) : PRC_AttributeEntry(), type(PRC_ModellerAttributeType_Int)  { value.integer = integer; }
	PRC_SingleAttribute(double real) : PRC_AttributeEntry(), type(PRC_ModellerAttributeType_Real) { value.real = real; }
  PRC_SingleAttribute(MLuint32 time) : PRC_AttributeEntry(), type(PRC_ModellerAttributeType_Time) { value.time = time; }  
	PRC_SingleAttribute(const std::string &text) : PRC_AttributeEntry(), type(PRC_ModellerAttributeType_String) { value_text = text; }
	PRC_SingleAttribute(MLuint32 title, int32_t integer) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Int) { value.integer = integer; }
	PRC_SingleAttribute(MLuint32 title, double real) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Real) { value.real = real; }
  PRC_SingleAttribute(MLuint32 title, MLuint32 time) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Time) { value.time = time; }  
	PRC_SingleAttribute(MLuint32 title, const std::string &text) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_String) { value_text = text; }
	PRC_SingleAttribute(const std::string title, int32_t integer) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Int) { value.integer = integer; }
	PRC_SingleAttribute(const std::string title, double real) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Real) { value.real = real; }
  PRC_SingleAttribute(const std::string title, MLuint32 time) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_Time) { value.time = time; }  
	PRC_SingleAttribute(const std::string title, const std::string &text) : PRC_AttributeEntry(title), type(PRC_ModellerAttributeType_String) { value_text = text; }

  void serializeSingleAttribute(PRC_BitStream&) const;

  PRC_ModellerAttributeType  type;
  union PRCSingleAttributeData
  {
    int32_t integer;
    double real;
    MLuint32 time;
  } value;
  std::string value_text;
};

class PRC_Attribute : public PRC_AttributeEntry
{
public:
  PRC_Attribute() : PRC_AttributeEntry() {}
	PRC_Attribute(MLuint32 title) : PRC_AttributeEntry(title) {}
	PRC_Attribute(const std::string title) : PRC_AttributeEntry(title) {}

  void serializeAttribute(PRC_BitStream &) const;
  PRC_SingleAttribute& newKey() { attribute_keys.resize(attribute_keys.size()+1); return attribute_keys.back(); }
  void addKey(const PRC_SingleAttribute &key) { attribute_keys.push_back(key); }

  std::deque<PRC_SingleAttribute> attribute_keys;
};

typedef std::list<PRC_Attribute> PRC_AttributeList;

class PRC_Attributes
{
public:
  void serializeAttributes(PRC_BitStream&) const;
  void addAttribute(const PRC_Attribute &attribute) { attributes.push_front(attribute); }
  PRC_Attribute& newAttribute() { attributes.push_front(PRC_Attribute()); return attributes.front(); }

  PRC_AttributeList attributes;
};

class PRC_ContentBase : public PRC_Attributes
{
public:
  PRC_ContentBase(MLuint32 t, std::string n="") : type(t),name(n),CAD_identifier(0), CAD_persistent_identifier(0), PRC_unique_identifier(0)
  {
    if(isTypeEligibleForReference(type))
    {
      CAD_identifier = createCADIdentifier();
      PRC_unique_identifier = createPRCUniqueIdentifier();
    }
  }

  void serializeContentPRCBase(PRC_BitStream&) const;

  MLuint32 getPRCID() const { return PRC_unique_identifier; }
  MLuint32 getType() const  { return type; }
  MLuint32 type;
  std::string name;
  MLuint32 CAD_identifier, CAD_persistent_identifier, PRC_unique_identifier;
};

class PRC_ReferenceUniqueIdentifier
{
public:
  PRC_ReferenceUniqueIdentifier() : type(0), unique_identifier(MLuint32_MAX) {}

  void serializeReferenceUniqueIdentifier(PRC_BitStream&);

  MLuint32 type;
  MLuint32 unique_identifier;
};

class PRC_Picture : public PRC_ContentBase
{
public:
  PRC_Picture(std::string n="") : PRC_ContentBase(PRC_TYPE_GRAPH_Picture,n), format(PRC_PictureDataFormat_PNG), uncompressed_file_index(MLuint32_MAX), pixel_width(0), pixel_height(0) {}

  void serializePicture(PRC_BitStream&);

  PRC_PictureDataFormat format;
  MLuint32 uncompressed_file_index;
  MLuint32 pixel_width;
  MLuint32 pixel_height;
};

class PRC_TextureDefinition : public PRC_ContentBase
{
public:
  PRC_TextureDefinition(std::string n="") :
    PRC_ContentBase(PRC_TYPE_GRAPH_TextureDefinition,n), picture_index(MLuint32_MAX), texture_mapping_attribute(PRC_TEXTURE_MAPPING_DIFFUSE),
    texture_mapping_attribute_intensity(1.0), texture_mapping_attribute_components(PRC_TEXTURE_MAPPING_COMPONENTS_RGBA),
    texture_function(PRC_TextureFunction_Modulate), texture_applying_mode(PRC_TEXTURE_APPLYING_MODE_NONE),
    texture_wrapping_mode_S(PRC_TextureWrappingMode_Unknown), texture_wrapping_mode_T(PRC_TextureWrappingMode_Unknown) // ,
    {}

  void serializeTextureDefinition(PRC_BitStream&);
  
  MLuint32 picture_index;
  MLuint32 texture_mapping_attribute;
  double texture_mapping_attribute_intensity;
  MLuint8 texture_mapping_attribute_components;
  PRC_TextureFunction texture_function;
  MLuint8 texture_applying_mode;
  PRC_TextureWrappingMode texture_wrapping_mode_S;
  PRC_TextureWrappingMode texture_wrapping_mode_T;
};

typedef std::deque <PRC_TextureDefinition*>  PRC_TextureDefinitionList;

class PRC_Material
{
public:
  virtual ~PRC_Material() {}
  virtual void serializeMaterial(PRC_BitStream&) = 0;
};

typedef std::deque <PRC_Material*>  PRC_MaterialList;

class PRC_MaterialGeneric : public PRC_ContentBase, public PRC_Material
{
public:
  PRC_MaterialGeneric(std::string n="") : PRC_ContentBase(PRC_TYPE_GRAPH_Material,n), ambient(MLuint32_MAX), diffuse(MLuint32_MAX), emissive(MLuint32_MAX), specular(MLuint32_MAX), 
    shininess(0.0), ambient_alpha(1.0), diffuse_alpha(1.0), emissive_alpha(1.0), specular_alpha(1.0) {}

  void serializeMaterialGeneric(PRC_BitStream&);
  void serializeMaterial(PRC_BitStream &pbs) { serializeMaterialGeneric(pbs); }

  MLuint32 picture_index;
  MLuint32 ambient;
  MLuint32 diffuse;
  MLuint32 emissive;
  MLuint32 specular;
  double shininess;
  double ambient_alpha;
  double diffuse_alpha;
  double emissive_alpha;
  double specular_alpha;

  bool operator==(const PRC_MaterialGeneric &m) const
  {
    return (ambient==m.ambient && diffuse==m.diffuse && emissive==m.emissive && specular==m.specular && shininess==m.shininess &&
            ambient_alpha==m.ambient_alpha && diffuse_alpha==m.diffuse_alpha && emissive_alpha==m.emissive_alpha && specular_alpha==m.specular_alpha);
  }
};

class PRC_TextureApplication : public PRC_ContentBase, public PRC_Material
{
public:
  PRC_TextureApplication(std::string n="") : PRC_ContentBase(PRC_TYPE_GRAPH_TextureApplication,n), material_generic_index(MLuint32_MAX), texture_definition_index(MLuint32_MAX),
    next_texture_index(MLuint32_MAX), UV_coordinates_index(0) {}

  void serializeTextureApplication(PRC_BitStream&);
  void serializeMaterial(PRC_BitStream &pbs) { serializeTextureApplication(pbs); }

  MLuint32 material_generic_index;
  MLuint32 texture_definition_index;
  MLuint32 next_texture_index;
  MLuint32 UV_coordinates_index;
};

class PRC_LinePattern : public PRC_ContentBase
{
public:
  PRC_LinePattern(std::string n="") : PRC_ContentBase(PRC_TYPE_GRAPH_LinePattern,n), phase(0), is_real_length(false) {}

  void serializeLinePattern(PRC_BitStream&);

  std::vector<double> lengths;
  double phase;
  bool is_real_length;
};

typedef std::deque <PRC_LinePattern*>  PRC_LinePatternList;

class PRC_Style : public PRC_ContentBase
{
public:
  PRC_Style(std::string n="") : PRC_ContentBase(PRC_TYPE_GRAPH_Style,n), line_width(0.0), is_vpicture(false), line_pattern_vpicture_index(MLuint32_MAX),
    is_material(false), color_material_index(MLuint32_MAX), is_transparency_defined(false), transparency(255), additional(0) {}

  void serializeCategory1LineStyle(PRC_BitStream&);

  double line_width;
  bool is_vpicture;
  MLuint32 line_pattern_vpicture_index;
  bool is_material;
  MLuint32 color_material_index;
  bool is_transparency_defined;
  MLuint8 transparency;
  MLuint8 additional;
};

typedef std::deque <PRC_Style*>  PRC_StyleList;

class PRC_TesselationFace
{
public:
  PRC_TesselationFace() : start_wire(0), used_entities_flag(0), start_triangulated(0), number_of_texture_coordinate_indexes(0), is_rgba(false), behaviour(PRC_GRAPHICS_Show) {}

  void serializeTessFace(PRC_BitStream&);

  std::vector<MLuint32> line_attributes;
  MLuint32 start_wire;			             // Specifing bounding wire seems not to work as of Acrobat/Reader 9.2
  std::vector<MLuint32> sizes_wire;	     // Specifing bounding wire seems not to work as of Acrobat/Reader 9.2
  MLuint32 used_entities_flag;
  MLuint32 start_triangulated;
  std::vector<MLuint32> sizes_triangulated;
  MLuint32 number_of_texture_coordinate_indexes;
  bool is_rgba;
  std::vector<MLuint8> rgba_vertices;
  MLuint32 behaviour;
};

typedef std::deque <PRC_TesselationFace*>  PRC_TesselationFaceList;

class PRC_TesselationBaseData
{
public:
  PRC_TesselationBaseData() : is_calculated(false) {}

  void serializeContentBaseTessData(PRC_BitStream&);

  bool is_calculated;
  std::vector<double> coordinates;
};

class PRC_Tesselation : public PRC_TesselationBaseData
{
public:
  virtual ~PRC_Tesselation() {}

  virtual void serializeBaseTessData(PRC_BitStream &pbs) = 0;
};

typedef std::deque <PRC_Tesselation*>  PRC_TesselationList;

class PRC_3DTesselation : public PRC_Tesselation
{
public:
  PRC_3DTesselation() : has_faces(false), has_loops(false), crease_angle(25.8419) {} // arccos(0.9), default found in Acrobat output
  ~PRC_3DTesselation() { for(PRC_TesselationFaceList::iterator it=face_tessellation.begin(); it!=face_tessellation.end(); ++it) delete *it; }

  void serialize3DTess(PRC_BitStream&);
  void serializeBaseTessData(PRC_BitStream &pbs) { serialize3DTess(pbs); }
  void addTessFace(PRC_TesselationFace*& pTessFace);

  bool has_faces;
  bool has_loops;
  double crease_angle;
  std::vector<double> normal_coordinate;
  std::vector<MLuint32> wire_index;		       // Specifing bounding wire seems not to work as of Acrobat/Reader 9.2
  std::vector<MLuint32> triangulated_index;
  PRC_TesselationFaceList face_tessellation;
  std::vector<double> texture_coordinate;
};

class PRC_3DWireTesselation : public PRC_Tesselation
{
public:
  PRC_3DWireTesselation() : is_rgba(false), is_segment_color(false) {}

  void serialize3DWireTess(PRC_BitStream&);
  void serializeBaseTessData(PRC_BitStream &pbs) { serialize3DWireTess(pbs); }

  bool is_rgba;
  bool is_segment_color;
  std::vector<MLuint32> wire_indexes;
  std::vector<MLuint8> rgba_vertices;
};

class PRC_MarkupTesselation : public PRC_Tesselation
{
public:
  PRC_MarkupTesselation() : behaviour(0) {}

  void serializeMarkupTess(PRC_BitStream&);
  void serializeBaseTessData(PRC_BitStream &pbs) { serializeMarkupTess(pbs); }

  std::vector<MLuint32> codes;
  std::vector<std::string> texts;
  std::string label;
  MLuint8 behaviour;
};

class PRC_Graphics
{
public:
  PRC_Graphics() : layer_index(MLuint32_MAX), index_of_line_style(MLuint32_MAX), behaviour_bit_field(PRC_GRAPHICS_Show) {}

  void serializeGraphics(PRC_BitStream&);
  void serializeGraphicsForced(PRC_BitStream&);
  bool has_graphics() { return (index_of_line_style!=MLuint32_MAX || layer_index!=MLuint32_MAX || behaviour_bit_field!=PRC_GRAPHICS_Show) ; }

  MLuint32 layer_index;
  MLuint32 index_of_line_style;
  uint16_t behaviour_bit_field;
};

typedef std::deque <PRC_Graphics*>  PRC_GraphicsList;

void writeGraphics(PRC_BitStream&, const PRC_Graphics&, bool=false);

class PRC_Markup: public PRC_Graphics, public PRC_ContentBase
{
public:
  PRC_Markup(std::string n="") : PRC_ContentBase(PRC_TYPE_MKP_Markup,n), type(PRC_MarkupType_Unknown), sub_type(PRC_MarkupSubType_Unknown), index_tessellation(MLuint32_MAX) {}

  void serializeMarkup(PRC_BitStream&);

  PRC_MarkupType type;
  PRC_MarkupSubType sub_type;
  MLuint32 index_tessellation;
};

typedef std::deque <PRC_Markup*>  PRC_MarkupList;

class PRC_AnnotationItem: public PRC_Graphics, public PRC_ContentBase
{
public:
  PRC_AnnotationItem(std::string n="") : PRC_ContentBase(PRC_TYPE_MKP_AnnotationItem,n) {}

  void serializeAnnotationItem(PRC_BitStream&);
  void serializeAnnotationEntity(PRC_BitStream &pbs) { serializeAnnotationItem(pbs); }

  PRC_ReferenceUniqueIdentifier markup;
};

typedef std::deque <PRC_AnnotationItem*>  PRC_AnnotationItemList;

class PRC_RepresentationItemContent: public PRC_Graphics, public PRC_ContentBase
{
public:
  PRC_RepresentationItemContent(MLuint32 t, std::string n="") : PRC_ContentBase(t,n), index_local_coordinate_system(MLuint32_MAX), index_tessellation(MLuint32_MAX) {}

  void serializeRepresentationItemContent(PRC_BitStream&);

  MLuint32 index_local_coordinate_system;
  MLuint32 index_tessellation;
};

class PRC_RepresentationItem : public PRC_RepresentationItemContent
{
public:
  PRC_RepresentationItem(MLuint32 t, std::string n="") : PRC_RepresentationItemContent(t,n) {}
  virtual ~PRC_RepresentationItem() {}

  virtual void serializeRepresentationItem(PRC_BitStream &pbs) = 0;
};

typedef std::deque <PRC_RepresentationItem*>  PRC_RepresentationItemList;

class PRC_BoundaryRepresentationModel : public PRC_RepresentationItem
{
public:
  PRC_BoundaryRepresentationModel(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_BrepModel,n), has_brep_data(true), context_id(MLuint32_MAX), body_id(MLuint32_MAX), is_closed(false) {}

  void serializeBrepModel(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializeBrepModel(pbs); }

  bool has_brep_data;
  MLuint32 context_id;
  MLuint32 body_id;
  bool is_closed;
};

class PRC_PolyBoundaryRepresentationModel : public PRC_RepresentationItem
{
public:
  PRC_PolyBoundaryRepresentationModel(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_PolyBrepModel,n), is_closed(false) {}

  void serializePolyBrepModel(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializePolyBrepModel(pbs); }

  bool is_closed;
};

class PRC_PointSet : public PRC_RepresentationItem
{
public:
  PRC_PointSet(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_PointSet,n) {}

  void serializePointSet(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializePointSet(pbs); }

  std::vector<PRC_Vector3D> point;
};

class PRC_Wire : public PRC_RepresentationItem
{
public:
  PRC_Wire(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_Curve,n), has_wire_body(true), context_id(MLuint32_MAX), body_id(MLuint32_MAX) {}

  void serializeWire(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializeWire(pbs); }

  bool has_wire_body;
  MLuint32 context_id;
  MLuint32 body_id;
};

class PRC_PolyWire : public PRC_RepresentationItem
{
public:
  PRC_PolyWire(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_PolyWire,n) {}

  void serializePolyWire(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializePolyWire(pbs); }
};

class PRC_Set : public PRC_RepresentationItem
{
public:
  PRC_Set(std::string n="") : PRC_RepresentationItem(PRC_TYPE_RI_Set,n) {}
  ~PRC_Set() { for(PRC_RepresentationItemList::iterator it=elements.begin(); it!=elements.end(); ++it) delete *it; }

  void serializeSet(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializeSet(pbs); }

  MLuint32 addBrepModel(PRC_BoundaryRepresentationModel*& pBrepModel);
  MLuint32 addPolyBrepModel(PRC_PolyBoundaryRepresentationModel*& pPolyBrepModel);
  MLuint32 addPointSet(PRC_PointSet*& pPointSet);
  MLuint32 addSet(PRC_Set*& pSet);
  MLuint32 addWire(PRC_Wire*& pWire);
  MLuint32 addPolyWire(PRC_PolyWire*& pPolyWire);
  MLuint32 addRepresentationItem(PRC_RepresentationItem*& pRepresentationItem);
  PRC_RepresentationItemList elements;
};

class PRC_Transformation3D
{
public:
  virtual ~PRC_Transformation3D() {}
  virtual void serializeTransformation3d(PRC_BitStream&) const =0;
};

typedef std::deque <PRC_Transformation3D*> PRC_Transformation3DList;

class PRC_GeneralTransformation3D : public PRC_Transformation3D
{
public:
  PRC_GeneralTransformation3D() { setidentity(); }
  PRC_GeneralTransformation3D(const double t[]) { set(t); }
  
  void serializeGeneralTransformation3d(PRC_BitStream&) const;
  void serializeTransformation3d(PRC_BitStream& pbs)  const { serializeGeneralTransformation3d(pbs); }

  double mat[4][4];

  bool operator==(const PRC_GeneralTransformation3D &t) const
  {
    for (size_t i=0;i<4;i++)
      for (size_t j=0;j<4;j++)
        if(mat[i][j]!=t.mat[i][j])
         return false;
    return true;
  }

  bool operator<(const PRC_GeneralTransformation3D &t) const
  {
    for (size_t i=0;i<4;i++)
      for (size_t j=0;j<4;j++)
        if(mat[i][j]!=t.mat[i][j])
        {
          return (mat[i][j]<t.mat[i][j]);
        }
    return false;
  }

  void set(const double t[])
  {
    if(t!=NULL) 
     for (size_t i=0;i<4;i++)
       for (size_t j=0;j<4;j++)
         mat[i][j]=t[4*i+j];
    else
      setidentity();
  }

  void setidentity()
  {
    mat[0][0]=1; mat[0][1]=0; mat[0][2]=0; mat[0][3]=0;
    mat[1][0]=0; mat[1][1]=1; mat[1][2]=0; mat[1][3]=0;
    mat[2][0]=0; mat[2][1]=0; mat[2][2]=1; mat[2][3]=0;
    mat[3][0]=0; mat[3][1]=0; mat[3][2]=0; mat[3][3]=1;
  }

  bool isidtransform() const 
  {
    return(
    mat[0][0]==1 && mat[0][1]==0 && mat[0][2]==0 && mat[0][3]==0 &&
    mat[1][0]==0 && mat[1][1]==1 && mat[1][2]==0 && mat[1][3]==0 &&
    mat[2][0]==0 && mat[2][1]==0 && mat[2][2]==1 && mat[2][3]==0 &&
    mat[3][0]==0 && mat[3][1]==0 && mat[3][2]==0 && mat[3][3]==1);
  }

  bool isnotidtransform() const 
  {
    return !isidtransform();
  }

  double M(size_t i, size_t j) const 
  {
    return mat[j][i];
  }
};

typedef std::deque <PRC_GeneralTransformation3D> PRC_GeneralTransformation3DList;

class PRC_CartesianTransformation3D : public PRC_Transformation3D
{
public:
  PRC_CartesianTransformation3D() : behaviour(PRC_TRANSFORMATION_Identity), origin(0.0,0.0,0.0), X(1.0,0.0,0.0), Y(0.0,1.0,0.0), Z(0.0,0.0,1.0),
    scale(1.0,1.0,1.0), uniform_scale(1.0), X_homogeneous_coord(0.0), Y_homogeneous_coord(0.0), Z_homogeneous_coord(0.0), origin_homogeneous_coord(1.0) {}
  PRC_CartesianTransformation3D(const double o[3], const double x[3], const double y[3], double sc) :
    behaviour(PRC_TRANSFORMATION_Identity), origin(o,0.0,0.0,0.0), X(x,1.0,0.0,0.0), Y(y,0.0,1.0,0.0), Z(0.0,0.0,1.0), scale(1.0,1.0,1.0), uniform_scale(sc),
    X_homogeneous_coord(0.0), Y_homogeneous_coord(0.0), Z_homogeneous_coord(0.0), origin_homogeneous_coord(1.0)
    {
      if(origin!=PRC_Vector3D(0.0,0.0,0.0))
        behaviour = behaviour | PRC_TRANSFORMATION_Translate;
      if(X!=PRC_Vector3D(1.0,0.0,0.0) || Y!=PRC_Vector3D(0.0,1.0,0.0))
        behaviour = behaviour | PRC_TRANSFORMATION_Rotate;
      if(uniform_scale!=1)
        behaviour = behaviour | PRC_TRANSFORMATION_Scale;
    }

  void serializeCartesianTransformation3d(PRC_BitStream& pbs) const;
  void serializeTransformation3d(PRC_BitStream& pbs) const { serializeCartesianTransformation3d(pbs); }

  MLuint8 behaviour;
  PRC_Vector3D origin;
  PRC_Vector3D X;
  PRC_Vector3D Y;
  PRC_Vector3D Z;
  PRC_Vector3D scale;
  double uniform_scale;
  double X_homogeneous_coord;
  double Y_homogeneous_coord;
  double Z_homogeneous_coord;
  double origin_homogeneous_coord;

  bool operator==(const PRC_CartesianTransformation3D &t) const
  {
    return behaviour==t.behaviour && origin==t.origin && X==t.X && Y==t.Y && Z==t.Z && scale==t.scale && uniform_scale==t.uniform_scale &&
           X_homogeneous_coord==t.X_homogeneous_coord && Y_homogeneous_coord==t.Y_homogeneous_coord &&
           Z_homogeneous_coord==t.Z_homogeneous_coord && origin_homogeneous_coord==t.origin_homogeneous_coord;
  }
};

class PRC_Transformation
{
public:
  PRC_Transformation() : has_transformation(false), geometry_is_2D(false), behaviour(PRC_TRANSFORMATION_Identity), origin(0.0,0.0,0.0), x_axis(1.0,0.0,0.0), y_axis(0.0,1.0,0.0), scale(1) {}

  void serializeTransformation(PRC_BitStream&);
  bool has_transformation;
  bool geometry_is_2D;
  MLuint8 behaviour;
  PRC_Vector3D origin;
  PRC_Vector3D x_axis;
  PRC_Vector3D y_axis;
  double scale;
};

class PRC_CoordinateSystem : public PRC_RepresentationItemContent
{
public:
  PRC_CoordinateSystem(std::string n="") : PRC_RepresentationItemContent(PRC_TYPE_RI_CoordinateSystem,n), axis_set(NULL) {}
  ~PRC_CoordinateSystem() { delete axis_set; }

  void serializeCoordinateSystem(PRC_BitStream&);
  void serializeRepresentationItem(PRC_BitStream &pbs) { serializeCoordinateSystem(pbs); }
  void setAxisSet(PRC_GeneralTransformation3D*& transform) { axis_set = transform; transform = NULL; } 
  void setAxisSet(PRC_CartesianTransformation3D*& transform) { axis_set = transform; transform = NULL; } 

  PRC_Transformation3D *axis_set;

  bool operator==(const PRC_CoordinateSystem &t) const
  {
    if(index_local_coordinate_system!=t.index_local_coordinate_system)
    {
      return false;
    }
    PRC_GeneralTransformation3D*    axis_set_general = dynamic_cast<PRC_GeneralTransformation3D*>(axis_set);
    PRC_GeneralTransformation3D*    t_axis_set_general = dynamic_cast<PRC_GeneralTransformation3D*>(t.axis_set);
    PRC_CartesianTransformation3D*  axis_set_cartesian = dynamic_cast<PRC_CartesianTransformation3D*>(axis_set);
    PRC_CartesianTransformation3D*  t_axis_set_cartesian = dynamic_cast<PRC_CartesianTransformation3D*>(t.axis_set);

    if(axis_set_general!=NULL)
    {
      return (t_axis_set_general!=NULL?(*axis_set_general==*t_axis_set_general):false); 
    }

    if(axis_set_cartesian!=NULL)
    {
      return (t_axis_set_cartesian!=NULL?(*axis_set_cartesian==*t_axis_set_cartesian):false); 
    }

    return false;
  }
};

typedef std::deque <PRC_CoordinateSystem*>  PRC_CoordinateSystemList;

class PRC_FontKeysSameFont
{
public:
  void serializeFontKeysSameFont(PRC_BitStream&);

  std::string font_name;
  MLuint32 char_set;
  std::vector<PRC_FontKey> font_keys;
};

// Topology
class PRC_BaseGeometry : public PRC_Attributes
{
public:
  PRC_BaseGeometry() : base_information(false), identifier(0) {}
  PRC_BaseGeometry(std::string n, MLuint32 id = 0) : base_information(true),name(n),identifier(id) {}

  void serializeBaseGeometry(PRC_BitStream&);

  bool base_information;
  std::string name;
  MLuint32 identifier;
};

class PRC_BoundingBox
{
public:
  PRC_BoundingBox() : min(0.0,0.0,0.0), max(0.0,0.0,0.0) {}
  PRC_BoundingBox(const PRC_Vector3D &v1, const PRC_Vector3D& v2) : min(v1),max(v2) {}

  void serializeBoundingBox(PRC_BitStream &pbs);

  PRC_Vector3D min;
  PRC_Vector3D max;
};

class PRC_Domain
{
public:
  void serializeDomain(PRC_BitStream &pbs);
  PRC_Vector2D min;
  PRC_Vector2D max;
};

class PRC_Interval
{
public:
  PRC_Interval() : min(0), max(0) {}
  PRC_Interval(double m, double M) : min(m), max(M) {}

  void serializeInterval(PRC_BitStream &pbs);

  double min;
  double max;
};

class PRC_Parameterization
{
public:
  PRC_Parameterization() : parameterization_coeff_a(1), parameterization_coeff_b(0) {}
  PRC_Parameterization(double min, double max) : interval(min, max), parameterization_coeff_a(1), parameterization_coeff_b(0) {}

  void serializeParameterization(PRC_BitStream &pbs);
  PRC_Interval interval;
  double parameterization_coeff_a;
  double parameterization_coeff_b;
};

class PRC_UVParameterization
{
public:
  PRC_UVParameterization() : swap_uv(false), parameterization_on_u_coeff_a(1), parameterization_on_v_coeff_a(1), parameterization_on_u_coeff_b(0), parameterization_on_v_coeff_b(0) {}

  void serializeUVParameterization(PRC_BitStream &pbs);

  bool swap_uv;
  PRC_Domain uv_domain;
  double parameterization_on_u_coeff_a;
  double parameterization_on_v_coeff_a;
  double parameterization_on_u_coeff_b;
  double parameterization_on_v_coeff_b;
};

class PRC_ControlPoint
{
public:
  PRC_ControlPoint() : x(0), y(0), z(0), w(1) {}
  PRC_ControlPoint(double X, double Y, double Z=0, double W=1) : x(X), y(Y), z(Z), w(W) {}
  PRC_ControlPoint(const PRC_Vector3D &v) : x(v.x), y(v.y), z(v.z), w(1) {}

  void Set(double fx, double fy, double fz, double fw=1) { x = fx; y = fy; z = fz; w = fw; }

  double x;
  double y;
  double z;
  double w;
};

class PRC_ContentSurface: public PRC_BaseGeometry
{
public:
  PRC_ContentSurface() : PRC_BaseGeometry(), extend_info(PRC_ExtendType_None) {}
  PRC_ContentSurface(std::string n) : PRC_BaseGeometry(n,createCADIdentifier()),extend_info(PRC_ExtendType_None) {} 

  void serializeContentSurface(PRC_BitStream&);

  PRC_ExtendType extend_info;
};

class PRC_Surface : public PRC_ContentSurface
{
public:
  PRC_Surface() : PRC_ContentSurface() {}
  PRC_Surface(std::string n) : PRC_ContentSurface(n) {}
  virtual ~PRC_Surface() {}

  virtual void  serializeSurface(PRC_BitStream &pbs) = 0;
};

class PRC_NURBSSurface : public PRC_Surface
{
public:
  PRC_NURBSSurface() : PRC_Surface(), knot_type(PRC_KnotType_Unspecified), surface_form(PRC_BSplineSurfaceForm_Unspecified) {}
  PRC_NURBSSurface(std::string n) : PRC_Surface(n), knot_type(PRC_KnotType_Unspecified), surface_form(PRC_BSplineSurfaceForm_Unspecified) {}

  void  serializeNURBSSurface(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeNURBSSurface(pbs); }

  bool is_rational;
  MLuint32 degree_in_u;
  MLuint32 degree_in_v;
  std::vector<PRC_ControlPoint> control_point;
  std::vector<double> knot_u;
  std::vector<double> knot_v;
  const PRC_KnotType knot_type;
  const PRC_BSplineSurfaceForm surface_form;
};

class PRC_ContentCurve: public PRC_BaseGeometry
{
public:
  PRC_ContentCurve() : PRC_BaseGeometry(), extend_info(PRC_ExtendType_None), is_3d(true) {}
  PRC_ContentCurve(std::string n) : PRC_BaseGeometry(n,createCADIdentifier()),extend_info(PRC_ExtendType_None), is_3d(true) {} 

  void serializeContentCurve(PRC_BitStream&);

  PRC_ExtendType extend_info;
  bool is_3d;
};

class PRC_Curve : public PRC_ContentCurve
{
public:
  PRC_Curve() : PRC_ContentCurve() {}
  PRC_Curve(std::string n) : PRC_ContentCurve(n) {}
  virtual ~PRC_Curve() {}

  virtual void  serializeCurve(PRC_BitStream &pbs) = 0;
};

typedef std::deque <PRC_Curve*>  PRC_CurveList;

class PRC_NURBSCurve : public PRC_Curve
{
public:
  PRC_NURBSCurve() : PRC_Curve(), knot_type(PRC_KnotType_Unspecified), curve_form(PRC_BSplineCurveForm_Unspecified) {}
  PRC_NURBSCurve(std::string n) : PRC_Curve(n), knot_type(PRC_KnotType_Unspecified), curve_form(PRC_BSplineCurveForm_Unspecified) {}

  void  serializeNURBSCurve(PRC_BitStream &pbs);
  void  serializeCurve(PRC_BitStream &pbs) { serializeNURBSCurve(pbs); }

  bool is_rational;
  MLuint32 degree;
  std::vector<PRC_ControlPoint> control_point;
  std::vector<double> knot;
  const PRC_KnotType knot_type;
  const PRC_BSplineCurveForm curve_form;
};

class PRC_PolyLine : public PRC_Curve, public PRC_Transformation, public PRC_Parameterization
{
public:
  PRC_PolyLine() : PRC_Curve() {}
  PRC_PolyLine(std::string n) : PRC_Curve(n) {}

  void  serializePolyLine(PRC_BitStream &pbs);
  void  serializeCurve(PRC_BitStream &pbs) { serializePolyLine(pbs); }

  std::vector<PRC_Vector3D> point;
};

class PRC_Circle : public PRC_Curve, public PRC_Transformation, public PRC_Parameterization
{
public:
  PRC_Circle() : PRC_Curve(), PRC_Parameterization(0,2*M_PI) {}
  PRC_Circle(std::string n) : PRC_Curve(n), PRC_Parameterization(0,2*M_PI) {}

  void  serializeCircle(PRC_BitStream &pbs);
  void  serializeCurve(PRC_BitStream &pbs) { serializeCircle(pbs); }

  double radius;
};

class PRC_Composite : public PRC_Curve, public PRC_Transformation, public PRC_Parameterization
{
public:
  PRC_Composite() : PRC_Curve() {}
  PRC_Composite(std::string n) : PRC_Curve(n) {}

  void  serializeComposite(PRC_BitStream &pbs);
  void  serializeCurve(PRC_BitStream &pbs) { serializeComposite(pbs); }

  PRC_CurveList base_curve;
  std::vector<bool> base_sense;
  bool is_closed;
};

class PRC_Blend01 : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Blend01() : PRC_Surface(), center_curve(NULL), origin_curve(NULL), tangent_curve(NULL) {}
  PRC_Blend01(std::string n) : PRC_Surface(n), center_curve(NULL), origin_curve(NULL), tangent_curve(NULL) {}
  ~PRC_Blend01() { delete center_curve; delete origin_curve; delete tangent_curve; }

  void  serializeBlend01(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeBlend01(pbs); }

  PRC_Curve* center_curve;
  PRC_Curve* origin_curve;
  PRC_Curve* tangent_curve;
};

class PRC_Ruled : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Ruled() : PRC_Surface(), first_curve(NULL), second_curve(NULL) {}
  PRC_Ruled(std::string n) : PRC_Surface(n) {}
  ~PRC_Ruled() { delete first_curve; delete second_curve; }

  void  serializeRuled(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeRuled(pbs); }

  PRC_Curve* first_curve;
  PRC_Curve* second_curve;
};

class PRC_Sphere : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Sphere() : PRC_Surface() {}
  PRC_Sphere(std::string n) : PRC_Surface(n) {}

  void  serializeSphere(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeSphere(pbs); }

  double radius;
};

class PRC_Cone : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Cone() : PRC_Surface() {}
  PRC_Cone(std::string n) : PRC_Surface(n) {}

  void  serializeCone(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeCone(pbs); }

  double bottom_radius;
  double semi_angle;
};

class PRC_Cylinder : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Cylinder() : PRC_Surface() {}
  PRC_Cylinder(std::string n) : PRC_Surface(n) {}

  void  serializeCylinder(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeCylinder(pbs); }

  double radius;
};

class PRC_Torus : public PRC_Surface, public PRC_Transformation, public PRC_UVParameterization
{
public:
  PRC_Torus() : PRC_Surface() {}
  PRC_Torus(std::string n) : PRC_Surface(n) {}

  void  serializeTorus(PRC_BitStream &pbs);
  void  serializeSurface(PRC_BitStream &pbs) { serializeTorus(pbs); }

  double major_radius;
  double minor_radius;
};

class PRC_BaseTopology : public PRC_Attributes
{
public:
  PRC_BaseTopology() : base_information(false),identifier(0) {}
  PRC_BaseTopology(std::string n, MLuint32 id = 0) : base_information(true),name(n),identifier(id) {}

  void serializeBaseTopology(PRC_BitStream&);

  bool base_information;
  std::string name;
  MLuint32 identifier;
};

class PRC_TopologyItem
{
public:
  virtual ~PRC_TopologyItem() {}
  virtual void serializeTopoItem(PRC_BitStream&)=0;
};

class PRC_BodyContent: public PRC_BaseTopology
{
public:
  PRC_BodyContent() : PRC_BaseTopology(), behavior(0) {}
  PRC_BodyContent(std::string n) : PRC_BaseTopology(n,createCADIdentifier()), behavior(0) {}

  void serializeContentBody(PRC_BitStream&);

  MLuint8 behavior;
};

class PRC_Body : public PRC_BodyContent, public PRC_TopologyItem
{
public:
  PRC_Body() : PRC_BodyContent(), topo_item_type(PRC_TYPE_ROOT) {}
  PRC_Body(MLuint32 tit) : PRC_BodyContent(), topo_item_type(tit) {}
  PRC_Body(MLuint32 tit, std::string n) : PRC_BodyContent(n), topo_item_type(tit) {}
  virtual ~PRC_Body() {}

  virtual void serializeBody(PRC_BitStream &pbs) = 0;
  void serializeTopoItem(PRC_BitStream &pbs) { serializeBody(pbs); }
  MLuint32 serialType() { return topo_item_type; }
  virtual double serialTolerance() { return 0; }

  const MLuint32 topo_item_type;
};

typedef std::deque <PRC_Body*>  PRC_BodyList;

class PRC_WireEdgeContent : public PRC_BaseTopology
{
public:
  PRC_WireEdgeContent() : PRC_BaseTopology(), curve_3d(NULL), has_curve_trim_interval(false) {}
  PRC_WireEdgeContent(std::string n) : PRC_BaseTopology(n,createCADIdentifier()), curve_3d(NULL), has_curve_trim_interval(false) {} 
  ~PRC_WireEdgeContent() { delete curve_3d; }

  void serializeContentWireEdge(PRC_BitStream &pbs);

  PRC_Curve* curve_3d;
  bool has_curve_trim_interval;
  PRC_Interval curve_trim_interval;
};

class PRC_WireEdge : public PRC_WireEdgeContent, public PRC_TopologyItem
{
public:
  void serializeWireEdge(PRC_BitStream &pbs);
  void serializeTopoItem(PRC_BitStream &pbs) { serializeWireEdge(pbs); }
};

class PRC_SingleWireBody : public PRC_Body
{
public:
  PRC_SingleWireBody() : PRC_Body(PRC_TYPE_TOPO_SingleWireBody), wire_edge(NULL) {}
  PRC_SingleWireBody(std::string n) : PRC_Body(PRC_TYPE_TOPO_SingleWireBody, n), wire_edge(NULL) {}
  ~PRC_SingleWireBody() { delete wire_edge; }

  void serializeSingleWireBody(PRC_BitStream &pbs);
  void serializeBody(PRC_BitStream &pbs) { serializeSingleWireBody(pbs); }
  void setWireEdge(PRC_WireEdge*& wireEdge) { wire_edge = wireEdge; wireEdge = NULL; }  

  PRC_WireEdge* wire_edge;
};

class PRC_Face : public PRC_BaseTopology, public PRC_TopologyItem, public PRC_Graphics
{
public:
  PRC_Face() : PRC_BaseTopology(), base_surface(NULL), have_surface_trim_domain(false), have_tolerance(false), tolerance(0), number_of_loop(0), outer_loop_index(-1) {}
  PRC_Face(std::string n) : PRC_BaseTopology(n,createCADIdentifier()), base_surface(NULL), have_surface_trim_domain(false), have_tolerance(false), tolerance(0), number_of_loop(0), outer_loop_index(-1) {} 
  ~PRC_Face() { delete base_surface; }

  void serializeFace(PRC_BitStream &pbs);
  void serializeTopoItem(PRC_BitStream &pbs) { serializeFace(pbs); }
  void setBaseSurface(PRC_Surface*& surface) { base_surface = surface; surface = NULL; }

  PRC_Surface *base_surface;
  const bool have_surface_trim_domain;
  PRC_Domain surface_trim_domain;
  const bool have_tolerance;
  const double tolerance;
  const MLuint32 number_of_loop;
  const int32_t outer_loop_index;
};

typedef std::deque <PRC_Face*>  PRC_FaceList;

class PRC_Shell : public PRC_BaseTopology, public PRC_TopologyItem
{
public:
  PRC_Shell() : PRC_BaseTopology(), shell_is_closed(false) {}
  PRC_Shell(std::string n) : PRC_BaseTopology(n,createCADIdentifier()), shell_is_closed(false) {}
  ~PRC_Shell() { for(PRC_FaceList::iterator it=face.begin(); it!=face.end(); ++it) delete *it; }

  void serializeShell(PRC_BitStream &pbs);
  void serializeTopoItem(PRC_BitStream &pbs) { serializeShell(pbs); }
  void addFace(PRC_Face*& pFace, MLuint8 orientation=2);

  bool shell_is_closed;
  PRC_FaceList face;
  std::vector<MLuint8> orientation_surface_with_shell;
};

typedef std::deque <PRC_Shell*>  PRC_ShellList;

class PRC_Connex : public PRC_BaseTopology, public PRC_TopologyItem
{
public:
  PRC_Connex() : PRC_BaseTopology() {}
  PRC_Connex(std::string n) : PRC_BaseTopology(n,createCADIdentifier()) {} 
  ~PRC_Connex() { for(PRC_ShellList::iterator it=shell.begin(); it!=shell.end(); ++it) delete *it; }

  void serializeConnex(PRC_BitStream &pbs);
  void serializeTopoItem(PRC_BitStream &pbs) { serializeConnex(pbs); }
  void addShell(PRC_Shell*& pShell);

  PRC_ShellList shell;
};

typedef std::deque <PRC_Connex*>  PRC_ConnexList;

class PRC_BoundaryRepresentationData : public PRC_Body, public PRC_BoundingBox
{
public:
  PRC_BoundaryRepresentationData() : PRC_Body(PRC_TYPE_TOPO_BrepData) {}
  PRC_BoundaryRepresentationData(std::string n) : PRC_Body(PRC_TYPE_TOPO_BrepData, n) {}
  ~PRC_BoundaryRepresentationData() { for(PRC_ConnexList::iterator it=connex.begin(); it!=connex.end(); ++it) delete *it; }

  void serializeBrepData(PRC_BitStream &pbs);
  void serializeBody(PRC_BitStream &pbs) { serializeBrepData(pbs); }
  void addConnex(PRC_Connex*& pConnex);

  PRC_ConnexList connex;
};

// For now - treat just the case of Bezier surfaces cubic 4x4 or linear 2x2
class PRC_CompressedFace : public PRC_BaseTopology, public PRC_Graphics
{
public:
  PRC_CompressedFace() : PRC_BaseTopology(), orientation_surface_with_shell(true), degree(0) {}
  PRC_CompressedFace(std::string n) : PRC_BaseTopology(n,createCADIdentifier()), orientation_surface_with_shell(true), degree(0) {} 

  void serializeCompressedFace(PRC_BitStream &pbs, double brep_data_compressed_tolerance);
  void serializeContentCompressedFace(PRC_BitStream &pbs);
  void serializeCompressedAnaNurbs(PRC_BitStream &pbs, double brep_data_compressed_tolerance);
  void serializeCompressedNurbs(PRC_BitStream &pbs, double brep_data_compressed_tolerance);

  bool orientation_surface_with_shell;
  MLuint32 degree;
  std::vector<PRC_Vector3D> control_point;
};

typedef std::deque <PRC_CompressedFace*>  PRC_CompressedFaceList;

// For now - treat just the case of one connex/one shell
class PRC_CompressedBoundaryRepresentationData : public PRC_Body
{
public:
  PRC_CompressedBoundaryRepresentationData() : PRC_Body(PRC_TYPE_TOPO_BrepDataCompress), serial_tolerance(0), brep_data_compressed_tolerance(0) {}
  PRC_CompressedBoundaryRepresentationData(std::string n) : PRC_Body(PRC_TYPE_TOPO_BrepDataCompress, n), serial_tolerance(0), brep_data_compressed_tolerance(0) {}
  ~PRC_CompressedBoundaryRepresentationData() { for(PRC_CompressedFaceList::iterator it=face.begin(); it!=face.end(); ++it) delete *it; }

  void serializeCompressedBrepData(PRC_BitStream &pbs);
  void serializeBody(PRC_BitStream &pbs) { serializeCompressedBrepData(pbs); }
  void serializeCompressedShell(PRC_BitStream &pbs);
  double serialTolerance() { return serial_tolerance; }

  double serial_tolerance;
  double brep_data_compressed_tolerance;
  PRC_CompressedFaceList face;
};

class PRC_TopologyContext : public PRC_ContentBase
{
public:
  PRC_TopologyContext(std::string n="") : PRC_ContentBase(PRC_TYPE_TOPO_Context,n), behaviour(0), granularity(1), tolerance(0), have_smallest_face_thickness(false), smallest_thickness(0), have_scale(false), scale(1) {}
  ~PRC_TopologyContext() { for(PRC_BodyList::iterator it=body.begin(); it!=body.end(); ++it) delete *it; }

  void serializeTopoContext(PRC_BitStream&);
  void serializeContextAndBodies(PRC_BitStream&);
  void serializeGeometrySummary(PRC_BitStream&);
  void serializeContextGraphics(PRC_BitStream&);
  MLuint32 addSingleWireBody(PRC_SingleWireBody*& body);
  MLuint32 addBrepData(PRC_BoundaryRepresentationData*& body);
  MLuint32 addCompressedBrepData(PRC_CompressedBoundaryRepresentationData*& body);

  MLuint8  behaviour;
  double granularity;
  double tolerance;
  bool have_smallest_face_thickness;
  double smallest_thickness;
  bool have_scale;
  double scale;
  PRC_BodyList body;
};

typedef std::deque <PRC_TopologyContext*>  PRC_TopoContextList;

class PRC_UniqueID
{
public:
  PRC_UniqueID() : id0(0), id1(0), id2(0), id3(0)  {}

  void serializeCompressedUniqueId(PRC_BitStream&) const;
  void serializeFileStructureUncompressedUniqueId(std::ostream& out) const;

  MLuint32 id0;
  MLuint32 id1;
  MLuint32 id2;
  MLuint32 id3;
};

class PRC_Unit
{
public:
  PRC_Unit() : unit_from_CAD_file(false), unit(1) {}
  PRC_Unit(double u, bool ufcf=true) : unit_from_CAD_file(ufcf), unit(u) {}

  void serializeUnit(PRC_BitStream&);

  bool unit_from_CAD_file;
  double unit;
};

class PRC_ProductOccurrence: public PRC_Graphics, public PRC_ContentBase
{
public:
  PRC_ProductOccurrence(std::string n="") : PRC_ContentBase(PRC_TYPE_ASM_ProductOccurence,n), index_part(MLuint32_MAX), index_prototype(MLuint32_MAX), prototype_in_same_file_structure(true),
    index_external_data(MLuint32_MAX), external_data_in_same_file_structure(true), product_behaviour(0), product_information_flags(0), product_load_status(PRC_ProductLoadStatus_Loaded),
    location(NULL) {}
  ~PRC_ProductOccurrence() { delete location; }

  void setLocation(PRC_GeneralTransformation3D*& transform) { location = transform; transform = NULL; }
  void serializeProductOccurrence(PRC_BitStream&);

  MLuint32 index_part;
  MLuint32 index_prototype;
  bool prototype_in_same_file_structure;
  PRC_UniqueID prototype_file_structure;
  MLuint32 index_external_data;
  bool external_data_in_same_file_structure;
  PRC_UniqueID external_data_file_structure;
  std::vector<MLuint32> index_son_occurrence;
  MLuint8 product_behaviour;
  PRC_Unit unit_information;
  MLuint8 product_information_flags;
  PRC_ProductLoadStatus product_load_status;
  PRC_GeneralTransformation3D *location;
};

typedef std::deque <PRC_ProductOccurrence*>  PRC_ProductOccurrenceList;

class PRC_PartDefinition: public PRC_Graphics, public PRC_ContentBase, public PRC_BoundingBox
{
public:
	PRC_PartDefinition(std::string n="") : PRC_ContentBase(PRC_TYPE_ASM_PartDefinition,n) {}
  ~PRC_PartDefinition() { for(PRC_RepresentationItemList::iterator it=representation_item.begin(); it!=representation_item.end(); ++it) delete *it; }

	void serializePartDefinition(PRC_BitStream&);
	MLuint32 addBrepModel(PRC_BoundaryRepresentationModel*& pBrepModel);
	MLuint32 addPolyBrepModel(PRC_PolyBoundaryRepresentationModel*& pPolyBrepModel);
	MLuint32 addPointSet(PRC_PointSet*& pPointSet);
	MLuint32 addSet(PRC_Set*& pSet);
	MLuint32 addWire(PRC_Wire*& pWire);
	MLuint32 addPolyWire(PRC_PolyWire*& pPolyWire);
	MLuint32 addRepresentationItem(PRC_RepresentationItem*& pRepresentationItem);

	PRC_RepresentationItemList representation_item;
};

typedef std::deque <PRC_PartDefinition*>  PRC_PartDefinitionList;

#pragma warning( pop ) 

extern std::string currentName;
extern MLuint32 current_layer_index;
extern MLuint32 current_index_of_line_style;
extern uint16_t current_behaviour_bit_field;


ML_END_NAMESPACE

#endif // _PRC_Write_H
