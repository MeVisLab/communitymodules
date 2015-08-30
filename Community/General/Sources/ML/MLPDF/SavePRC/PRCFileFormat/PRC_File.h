//----------------------------------------------------------------------------------
//! This header files contains 
/*! 
//
// \file    PRC_File.h
// \author  Axel Newe
// \date    2015-08-01
*/


#ifndef _PRC_File_H
#define _PRC_File_H

#include <stack>


#include "PRC_Defines.h"
#include "PRC_BitStream.h"
#include "PRC_Write.h"

#include "mlModuleIncludes.h"

ML_START_NAMESPACE


class PRCFile;
class PRC_FileStructure;





typedef std::map<PRC_RGBColor,MLuint32> PRCcolorMap;

struct PRCmaterial
{
  PRCmaterial() : alpha(1.0),shininess(1.0),
      picture_data(NULL), picture_format(PRC_PictureDataFormat_Bitmap_RGB), picture_width(0), picture_height(0), picture_size(0),
      picture_replace(false), picture_repeat(false) {}
  PRCmaterial(const PRC_RGBAColor &a, const PRC_RGBAColor &d, const PRC_RGBAColor &e,
              const PRC_RGBAColor &s, double p, double h,
              const MLuint8* pic=NULL, PRC_PictureDataFormat picf=PRC_PictureDataFormat_Bitmap_RGB,
              MLuint32 picw=0, MLuint32 pich=0, MLuint32 pics=0, bool picreplace=false, bool picrepeat=false) :
      ambient(a), diffuse(d), emissive(e), specular(s), alpha(p), shininess(h),
      picture_data(pic), picture_format(picf), picture_width(picw), picture_height(pich), picture_size(pics),
      picture_replace(picreplace), picture_repeat(picrepeat) {
        if(picture_size==0)
        {
          if (picture_format==PRC_PictureDataFormat_Bitmap_RGB)
             picture_size = picture_width*picture_height*3;
          if (picture_format==PRC_PictureDataFormat_Bitmap_RGBAlpha)
             picture_size = picture_width*picture_height*4;
          if (picture_format==PRC_PictureDataFormat_Bitmap_Grey)
             picture_size = picture_width*picture_height*1;
          if (picture_format==PRC_PictureDataFormat_Bitmap_GreyAlpha)
             picture_size = picture_width*picture_height*2;
        }
      }
  PRC_RGBAColor ambient,diffuse,emissive,specular;
  double alpha,shininess;
  const MLuint8* picture_data;
  PRC_PictureDataFormat picture_format;
  MLuint32 picture_width;
  MLuint32 picture_height;
  MLuint32 picture_size;
  bool picture_replace; // replace material color with texture color? if false - just modify
  bool picture_repeat;  // repeat texture? if false - clamp to edge

  bool operator==(const PRCmaterial &m) const
  {
    return (ambient==m.ambient && diffuse==m.diffuse && emissive==m.emissive
        && specular==m.specular && alpha==m.alpha && shininess==m.shininess
        && picture_replace==m.picture_replace && picture_repeat==m.picture_repeat
        && picture_format==m.picture_format
        && picture_width==m.picture_width && picture_height==m.picture_height && picture_size==m.picture_size
        && (picture_data==m.picture_data || memcmp(picture_data,m.picture_data,picture_size)==0) );
  }
  bool operator<(const PRCmaterial &m) const
  {
    if(ambient!=m.ambient)
      return (ambient<m.ambient);
    if(diffuse!=m.diffuse)
      return (diffuse<m.diffuse);
    if(emissive!=m.emissive)
      return (emissive<m.emissive);
    if(specular!=m.specular)
      return (specular<m.specular);
    if(alpha!=m.alpha)
      return (alpha<m.alpha);
    if(shininess!=m.shininess)
      return (shininess<m.shininess);
    if(picture_replace!=m.picture_replace)
      return (picture_replace<m.picture_replace);
    if(picture_repeat!=m.picture_repeat)
      return (picture_repeat<m.picture_repeat);
    if(picture_format!=m.picture_format)
      return (picture_format<m.picture_format);
    if(picture_width!=m.picture_width)
      return (picture_width<m.picture_width);
    if(picture_height!=m.picture_height)
      return (picture_height<m.picture_height);
    if(picture_size!=m.picture_size)
      return (picture_size<m.picture_size);
    if(picture_data!=m.picture_data)
      return (memcmp(picture_data,m.picture_data,picture_size)<0);
    return false;
  }
};
typedef std::map<PRCmaterial,MLuint32> PRCmaterialMap;

struct PRCpicture
{
  PRCpicture() : 
      data(NULL), format(PRC_PictureDataFormat_Bitmap_RGB),
      width(0), height(0), size(0) {}
  PRCpicture(const MLuint8* pic, PRC_PictureDataFormat picf,
             MLuint32 picw, MLuint32 pich, MLuint32 pics=0) :
      data(pic), format(picf),
      width(picw), height(pich), size(pics)
  {
    if(size==0)
    {
      if (format==PRC_PictureDataFormat_Bitmap_RGB)
         size = width*height*3;
      if (format==PRC_PictureDataFormat_Bitmap_RGBAlpha)
         size = width*height*4;
      if (format==PRC_PictureDataFormat_Bitmap_Grey)
         size = width*height*1;
      if (format==PRC_PictureDataFormat_Bitmap_GreyAlpha)
         size = width*height*2;
    }
  }
  PRCpicture(const PRCmaterial& m) :
      data(m.picture_data), format(m.picture_format),
      width(m.picture_width), height(m.picture_height), size(m.picture_size) {}

  const MLuint8* data;
  PRC_PictureDataFormat format;
  MLuint32 width;
  MLuint32 height;
  MLuint32 size;
  bool operator==(const PRCpicture& p) const
  {
    return ( format==p.format
        && width==p.width && height==p.height && size==p.size
        && (data==p.data || memcmp(data,p.data,size)==0) );
  }
  bool operator<(const PRCpicture& p) const
  {
    if(format!=p.format)
      return (format<p.format);
    if(width!=p.width)
      return (width<p.width);
    if(height!=p.height)
      return (height<p.height);
    if(size!=p.size)
      return (size<p.size);
    if(data!=p.data)
      return (memcmp(data,p.data,size)<0);
    return false;
  }
};

typedef std::map<PRCpicture,MLuint32> PRCpictureMap;

struct PRCmaterialgeneric
{
  PRCmaterialgeneric() : alpha(1.0),shininess(1.0) {}
  PRCmaterialgeneric(const PRC_RGBAColor& a, const PRC_RGBAColor& d, const PRC_RGBAColor& e,
              const PRC_RGBAColor& s, double p, double h) :
      ambient(a), diffuse(d), emissive(e), specular(s), alpha(p), shininess(h) {}
  PRCmaterialgeneric(const PRCmaterial& m) :
      ambient(m.ambient), diffuse(m.diffuse), emissive(m.emissive), specular(m.specular), alpha(m.alpha), shininess(m.shininess) {}
  PRC_RGBAColor ambient,diffuse,emissive,specular;
  double alpha,shininess;

  bool operator==(const PRCmaterialgeneric& m) const
  {
    return (ambient==m.ambient && diffuse==m.diffuse && emissive==m.emissive
        && specular==m.specular && alpha==m.alpha && shininess==m.shininess);
  }
  bool operator<(const PRCmaterialgeneric& m) const
  {
    if(ambient!=m.ambient)
      return (ambient<m.ambient);
    if(diffuse!=m.diffuse)
      return (diffuse<m.diffuse);
    if(emissive!=m.emissive)
      return (emissive<m.emissive);
    if(specular!=m.specular)
      return (specular<m.specular);
    if(alpha!=m.alpha)
      return (alpha<m.alpha);
    if(shininess!=m.shininess)
      return (shininess<m.shininess);
    return false;
  }
};
typedef std::map<PRCmaterialgeneric,MLuint32> PRCmaterialgenericMap;

struct PRCtexturedefinition
{
  PRCtexturedefinition() : 
      picture_index(MLuint32_MAX), picture_replace(false), picture_repeat(false) {}
  PRCtexturedefinition(MLuint32 picindex, bool picreplace=false, bool picrepeat=false) :
      picture_index(picindex), picture_replace(picreplace), picture_repeat(picrepeat) {}
  PRCtexturedefinition(MLuint32 picindex, const PRCmaterial& m) :
      picture_index(picindex), picture_replace(m.picture_replace), picture_repeat(m.picture_repeat) {}
  MLuint32 picture_index;
  bool picture_replace; // replace material color with texture color? if false - just modify
  bool picture_repeat;  // repeat texture? if false - clamp to edge

  bool operator==(const PRCtexturedefinition& t) const
  {
    return (picture_index==t.picture_index
        && picture_replace==t.picture_replace && picture_repeat==t.picture_repeat);
  }
  bool operator<(const PRCtexturedefinition& t) const
  {
    if(picture_index!=t.picture_index)
      return (picture_index<t.picture_index);
    if(picture_replace!=t.picture_replace)
      return (picture_replace<t.picture_replace);
    if(picture_repeat!=t.picture_repeat)
      return (picture_repeat<t.picture_repeat);
    return false;
  }
};
typedef std::map<PRCtexturedefinition,MLuint32> PRCtexturedefinitionMap;

struct PRCtextureapplication
{
  PRCtextureapplication() : 
      material_generic_index(MLuint32_MAX), texture_definition_index(MLuint32_MAX) {}
  PRCtextureapplication(MLuint32 matindex, MLuint32 texindex) :
      material_generic_index(matindex), texture_definition_index(texindex) {}
  MLuint32 material_generic_index;
  MLuint32 texture_definition_index;

  bool operator==(const PRCtextureapplication& t) const
  {
    return (material_generic_index==t.material_generic_index
        && texture_definition_index==t.texture_definition_index);
  }
  bool operator<(const PRCtextureapplication& t) const
  {
    if(material_generic_index!=t.material_generic_index)
      return (material_generic_index<t.material_generic_index);
    if(texture_definition_index!=t.texture_definition_index)
      return (texture_definition_index<t.texture_definition_index);
    return false;
  }
};
typedef std::map<PRCtextureapplication,MLuint32> PRCtextureapplicationMap;

struct PRCstyle
{
  PRCstyle() : 
      line_width(0), alpha(1), is_material(false), color_material_index(MLuint32_MAX) {}
  PRCstyle(double linewidth, double alph, bool ismat, MLuint32 colindex=MLuint32_MAX) :
      line_width(linewidth), alpha(alph), is_material(ismat), color_material_index(colindex) {}
  double line_width;
  double alpha;
  bool is_material;
  MLuint32 color_material_index;

  bool operator==(const PRCstyle& s) const
  {
    return (line_width==s.line_width && alpha==s.alpha && is_material==s.is_material
        && color_material_index==s.color_material_index);
  }
  bool operator<(const PRCstyle& s) const
  {
    if(line_width!=s.line_width)
      return (line_width<s.line_width);
    if(alpha!=s.alpha)
      return (alpha<s.alpha);
    if(is_material!=s.is_material)
      return (is_material<s.is_material);
    if(color_material_index!=s.color_material_index)
      return (color_material_index<s.color_material_index);
    return false;
  }
};
typedef std::map<PRCstyle,MLuint32> PRCstyleMap;

struct PRCtessrectangle // rectangle
{
  PRC_Vector3D vertices[4];
  MLuint32 style;
};
typedef std::vector<PRCtessrectangle> PRCtessrectangleList;

struct PRCtessquad // rectangle
{
  PRC_Vector3D vertices[4];
  PRC_RGBAColor  colours[4];
};
typedef std::vector<PRCtessquad> PRCtessquadList;
/*
struct PRCtesstriangle // textured triangle
{
  PRCtesstriangle() : 
  style(MLuint32_MAX) {}
  PRC_Vector3D vertices[3];
// PRC_Vector3D normals[3];
// PRC_RGBAColor  colors[3];
  PRC_Vector2D texcoords[3];
  MLuint32 style;
};
typedef std::vector<PRCtesstriangle> PRCtesstriangleList;
*/
struct PRCtessline // polyline
{
  std::vector<PRC_Vector3D> point;
  PRC_RGBColor color;
};
typedef std::list<PRCtessline> PRCtesslineList;
typedef std::map<double, PRCtesslineList> PRCtesslineMap;

struct PRCface
{
  PRCface() : transform(NULL), face(NULL) {}
  MLuint32 style;
  bool transparent;
  PRC_GeneralTransformation3D*  transform;
  PRC_Face* face;
};
typedef std::vector <PRCface>  PRCfaceList;

struct PRCcompface
{
  PRCcompface() : face(NULL) {}
  MLuint32 style;
  bool transparent;
  PRC_CompressedFace* face;
};
typedef std::vector <PRCcompface>  PRCcompfaceList;

struct PRCwire
{
  PRCwire() : style(MLuint32_MAX), transform(NULL), curve(NULL) {}
  MLuint32 style;
  PRC_GeneralTransformation3D*  transform;
  PRC_Curve* curve;
};
typedef std::vector <PRCwire>  PRCwireList;

typedef std::map <MLuint32,std::vector<PRC_Vector3D> >  PRCpointsetMap;

class PRCoptions
{
public:
  double compression;
  double granularity;

  bool closed;   // render the surface as one-sided; may yield faster rendering
  bool tess;     // use tessellated mesh to store straight patches
  bool do_break; //
  bool no_break; // do not render transparent patches as one-faced nodes
  double crease_angle; // crease angle for meshes

  PRCoptions(double compression=0.0, double granularity=0.0, bool closed=false,
             bool tess=false, bool do_break=true, bool no_break=false, double crease_angle=25.8419)
    : compression(compression), granularity(granularity), closed(closed),
      tess(tess), do_break(do_break), no_break(no_break), crease_angle(crease_angle) {}
};

class PRCgroup
{
 public:
  PRCgroup() : 
    product_occurrence(NULL), parent_product_occurrence(NULL), part_definition(NULL), parent_part_definition(NULL), transform(NULL) {}
  PRCgroup(const std::string& name) : 
    product_occurrence(NULL), parent_product_occurrence(NULL), part_definition(NULL), parent_part_definition(NULL), transform(NULL), name(name) {}
  PRC_ProductOccurrence *product_occurrence, *parent_product_occurrence;
  PRC_PartDefinition *part_definition, *parent_part_definition;
  PRCfaceList       faces;
  PRCcompfaceList   compfaces;
  PRCtessrectangleList  rectangles;
// PRCtesstriangleList   triangles;
  PRCtessquadList       quads;
  PRCtesslineMap        lines;
  PRCwireList           wires;
  PRCpointsetMap        points;
  std::vector<PRC_PointSet*>      pointsets;
  std::vector<PRC_PolyBoundaryRepresentationModel*> polymodels;
  std::vector<PRC_PolyWire*>      polywires;
  PRC_GeneralTransformation3D*  transform;
  std::string name;
  PRCoptions options;
};

void makeFileUUID(PRC_UniqueID&);
void makeAppUUID(PRC_UniqueID&);

class PRCUncompressedFile
{
  public:
    PRCUncompressedFile() : file_size(0), data(NULL) {}
    PRCUncompressedFile(MLuint32 fs, MLuint8 *d) : file_size(fs), data(d) {}
    ~PRCUncompressedFile() { if(data != NULL) delete[] data; }
    MLuint32 file_size;
    MLuint8 *data;

    void write(std::ostream&) const;

    MLuint32 getSize() const;
};
typedef std::deque <PRCUncompressedFile*>  PRCUncompressedFileList;

class PRCStartHeader
{
  public:
    MLuint32 minimal_version_for_read; // PRCVersion
    MLuint32 authoring_version; // PRCVersion
    PRC_UniqueID file_structure_uuid;
    PRC_UniqueID application_uuid; // should be 0

    PRCStartHeader() :
      minimal_version_for_read(PRC_VERSION), authoring_version(PRC_VERSION) {}
    void serializeStartHeader(std::ostream&) const;

    MLuint32 getStartHeaderSize() const;
};

#pragma warning( push )
  #pragma warning( disable : 4512 )
  // Suppress warning that assignment operator could not be generated.

class PRC_FileStructure : public PRCStartHeader
{
  public:
    MLuint32 number_of_referenced_file_structures;
    double tessellation_chord_height_ratio;
    double tessellation_angle_degree;
    std::string default_font_family_name;
    std::vector<PRC_RGBColor> colors;
    std::vector<PRC_Picture> pictures;
    PRCUncompressedFileList uncompressed_files;
    PRC_TextureDefinitionList texture_definitions;
    PRC_MaterialList materials;
    PRC_StyleList styles;
    PRC_CoordinateSystemList reference_coordinate_systems;
    std::vector<PRC_FontKeysSameFont> font_keys_of_font;
    PRC_PartDefinitionList part_definitions;
    PRC_ProductOccurrenceList product_occurrences;
//  PRC_MarkupList markups;
//  PRC_AnnotationItemList annotation_entities;
    double unit;
    PRC_TopoContextList contexts;
    PRC_TesselationList tessellations;

    MLuint32 sizes[6];
    MLuint8 *globals_data;
    PRC_BitStream globals_out; // order matters: PRC_BitStream must be initialized last
    MLuint8 *tree_data;
    PRC_BitStream tree_out;
    MLuint8 *tessellations_data;
    PRC_BitStream tessellations_out;
    MLuint8 *geometry_data;
    PRC_BitStream geometry_out;
    MLuint8 *extraGeometry_data;
    PRC_BitStream extraGeometry_out;

    ~PRC_FileStructure () {
      for(PRCUncompressedFileList::iterator   it=uncompressed_files.begin();  it!=uncompressed_files.end();  ++it) delete *it;
      for(PRC_TextureDefinitionList::iterator it=texture_definitions.begin(); it!=texture_definitions.end(); ++it) delete *it;
      for(PRC_MaterialList::iterator          it=materials.begin();           it!=materials.end();           ++it) delete *it;
      for(PRC_StyleList::iterator             it=styles.begin();              it!=styles.end();              ++it) delete *it;
      for(PRC_TopoContextList::iterator       it=contexts.begin();            it!=contexts.end();            ++it) delete *it;
      for(PRC_TesselationList::iterator              it=tessellations.begin();       it!=tessellations.end();       ++it) delete *it;
      for(PRC_PartDefinitionList::iterator    it=part_definitions.begin();    it!=part_definitions.end();    ++it) delete *it;
      for(PRC_ProductOccurrenceList::iterator it=product_occurrences.begin(); it!=product_occurrences.end(); ++it) delete *it;
      for(PRC_CoordinateSystemList::iterator  it=reference_coordinate_systems.begin(); it!=reference_coordinate_systems.end(); it++)
        delete *it;

      free(globals_data);
      free(tree_data);
      free(tessellations_data);
      free(geometry_data);
      free(extraGeometry_data);
    }

    PRC_FileStructure() :
      number_of_referenced_file_structures(0),
      tessellation_chord_height_ratio(2000.0),tessellation_angle_degree(40.0),
      default_font_family_name(""),
      unit(1),
      globals_data(NULL),globals_out(globals_data,0),
      tree_data(NULL),tree_out(tree_data,0),
      tessellations_data(NULL),tessellations_out(tessellations_data,0),
      geometry_data(NULL),geometry_out(geometry_data,0),
      extraGeometry_data(NULL),extraGeometry_out(extraGeometry_data,0) {}
    void write(std::ostream&);
    void prepare();
    MLuint32 getSize();
    void serializeFileStructureGlobals(PRC_BitStream&);
    void serializeFileStructureTree(PRC_BitStream&);
    void serializeFileStructureTessellation(PRC_BitStream&);
    void serializeFileStructureGeometry(PRC_BitStream&);
    void serializeFileStructureExtraGeometry(PRC_BitStream&);
    MLuint32 addPicture(PRC_PictureDataFormat format, MLuint32 size, const MLuint8 *picture, MLuint32 width=0, MLuint32 height=0, std::string name="");
    MLuint32 addTextureDefinition(PRC_TextureDefinition*& pTextureDefinition);
    MLuint32 addRgbColor(const PRC_RGBColor &color);
    MLuint32 addRgbColorUnique(const PRC_RGBColor &color);
    MLuint32 addMaterialGeneric(PRC_MaterialGeneric*& pMaterialGeneric);
    MLuint32 addTextureApplication(PRC_TextureApplication*& pTextureApplication);
    MLuint32 addStyle(PRC_Style*& pStyle);
    MLuint32 addPartDefinition(PRC_PartDefinition*& pPartDefinition);
    MLuint32 addProductOccurrence(PRC_ProductOccurrence*& pProductOccurrence);
    MLuint32 addTopoContext(PRC_TopologyContext*& pTopoContext);
    MLuint32 getTopoContext(PRC_TopologyContext*& pTopoContext);
    MLuint32 add3DTess(PRC_3DTesselation*& p3DTess);
    MLuint32 add3DWireTess(PRC_3DWireTesselation*& p3DWireTess);
/*
    MLuint32 addMarkupTess(PRC_MarkupTesselation*& pMarkupTess);
    MLuint32 addMarkup(PRC_Markup*& pMarkup);
    MLuint32 addAnnotationItem(PRC_AnnotationItem*& pAnnotationItem);
 */
    MLuint32 addCoordinateSystem(PRC_CoordinateSystem*& pCoordinateSystem);
    MLuint32 addCoordinateSystemUnique(PRC_CoordinateSystem*& pCoordinateSystem);
};

#pragma warning( pop ) 

class PRCFileStructureInformation
{
  public:
    PRC_UniqueID UUID;
    MLuint32 reserved; // 0
    MLuint32 number_of_offsets;
    MLuint32 *offsets;

    void write(std::ostream&);

    MLuint32 getSize();
};

class PRCHeader : public PRCStartHeader
{
  public :
    MLuint32 number_of_file_structures;
    PRCFileStructureInformation *fileStructureInformation;
    MLuint32 model_file_offset;
    MLuint32 file_size; // not documented
    PRCUncompressedFileList uncompressed_files;

    void write(std::ostream&);
    MLuint32 getSize();
};

typedef std::map <PRC_GeneralTransformation3D,MLuint32> PRCtransformMap;

#pragma warning( push )
  #pragma warning( disable : 4512 )
  // Suppress warning that assignment operator could not be generated.

class PRCFile
{
  public:
    PRCFile(std::ostream &os, double u=1, MLuint32 n=1) :
      number_of_file_structures(n),
      fileStructures(new PRC_FileStructure*[n]),
      unit(u),
      modelFile_data(NULL),modelFile_out(modelFile_data,0),
      fout(NULL),output(os)
      {
        for(MLuint32 i = 0; i < number_of_file_structures; ++i)
        {
          fileStructures[i] = new PRC_FileStructure();
          fileStructures[i]->minimal_version_for_read = PRC_VERSION;
          fileStructures[i]->authoring_version = PRC_VERSION;
          makeFileUUID(fileStructures[i]->file_structure_uuid);
          makeAppUUID(fileStructures[i]->application_uuid);
          fileStructures[i]->unit = u;
        }

        groups.push(PRCgroup());
        PRCgroup &group = groups.top();
        group.name="root";
        group.transform = NULL;
        group.product_occurrence = new PRC_ProductOccurrence(group.name);
        group.parent_product_occurrence = NULL;
        group.part_definition = new PRC_PartDefinition;
        group.parent_part_definition = NULL;
      }

    PRCFile(const std::string &name, double u=1, MLuint32 n=1) :
      number_of_file_structures(n),
      fileStructures(new PRC_FileStructure*[n]),
      unit(u),
      modelFile_data(NULL),modelFile_out(modelFile_data,0),
      fout(new std::ofstream(name.c_str(),
                             std::ios::out|std::ios::binary|std::ios::trunc)),
      output(*fout)
      {
        for(MLuint32 i = 0; i < number_of_file_structures; ++i)
        {
          fileStructures[i] = new PRC_FileStructure();
          fileStructures[i]->minimal_version_for_read = PRC_VERSION;
          fileStructures[i]->authoring_version = PRC_VERSION;
          makeFileUUID(fileStructures[i]->file_structure_uuid);
          makeAppUUID(fileStructures[i]->application_uuid);
          fileStructures[i]->unit = u;
        }

        groups.push(PRCgroup());
        PRCgroup &group = groups.top();
        group.name="root";
        group.transform = NULL;
        group.product_occurrence = new PRC_ProductOccurrence(group.name);
        group.parent_product_occurrence = NULL;
        group.part_definition = new PRC_PartDefinition;
        group.parent_part_definition = NULL;
      }

    ~PRCFile()
    {
      for(MLuint32 i = 0; i < number_of_file_structures; ++i)
        delete fileStructures[i];
      delete[] fileStructures;
      if(fout != NULL)
        delete fout;
      free(modelFile_data);
      for(PRCpictureMap::iterator it=pictureMap.begin(); it!=pictureMap.end(); ++it) delete it->first.data;
    }

    void begingroup(const char *name, PRCoptions *options=NULL,
                    const double* t=NULL);
    void endgroup();

    std::string lastgroupname;
    std::vector<std::string> lastgroupnames;
    std::string calculate_unique_name(const PRC_ContentBase *prc_entity,const PRC_ContentBase *prc_occurence);
    
    bool finish();
    MLuint32 getSize();

    const MLuint32 number_of_file_structures;
    PRC_FileStructure **fileStructures;
    PRCHeader header;
    PRC_Unit unit;
    MLuint8 *modelFile_data;
    PRC_BitStream modelFile_out; // order matters: PRC_BitStream must be initialized last
    PRCcolorMap colorMap;
    PRC_ColorMap colourMap;
    PRC_ColorWidthMap colourwidthMap;
    PRCmaterialgenericMap materialgenericMap;
    PRCtexturedefinitionMap texturedefinitionMap;
    PRCtextureapplicationMap textureapplicationMap;
    PRCstyleMap styleMap;
    PRCpictureMap pictureMap;
    PRCgroup rootGroup;
    PRCtransformMap transformMap;
    std::stack<PRCgroup> groups;
    PRCgroup& findGroup();
    void doGroup(PRCgroup& group);
    MLuint32 addColor(const PRC_RGBColor &color);
    MLuint32 addColour(const PRC_RGBAColor &colour);
    MLuint32 addColourWidth(const PRC_RGBAColor &colour, double width);
    MLuint32 addLineMaterial(const PRC_RGBAColor& c, double width)
               { return addColourWidth(c,width); }
    MLuint32 addMaterial(const PRCmaterial &material);
    MLuint32 addTransform(PRC_GeneralTransformation3D*& transform);
    MLuint32 addTransform(const double* t);
    MLuint32 addTransform(const double origin[3], const double x_axis[3], const double y_axis[3], double scale);
    void addPoint(const double P[3], const PRC_RGBAColor &c, double w=1.0);
    void addPoints(MLuint32 n, const double P[][3], const PRC_RGBAColor &c, double w=1.0);
    void addLines(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[],
                      const PRC_RGBAColor& c, double w,
                      bool segment_color, MLuint32 nC, const PRC_RGBAColor C[], MLuint32 nCI, const MLuint32 CI[]);
    MLuint32 createLines(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[],
                      bool segment_color, MLuint32 nC, const PRC_RGBAColor C[], MLuint32 nCI, const MLuint32 CI[]);
    void addTriangles(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][3], const PRCmaterial &m,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][3],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][3],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][3],
                      MLuint32 nM, const PRCmaterial M[], const MLuint32 MI[], double ca);
    MLuint32 createTriangleMesh(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][3], MLuint32 style_index,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][3],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][3],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][3],
                      MLuint32 nS, const MLuint32 S[], const MLuint32 SI[], double ca);
    MLuint32 createTriangleMesh(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][3], const PRCmaterial& m,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][3],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][3],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][3],
                      MLuint32 nM, const PRCmaterial M[], const MLuint32 MI[], double ca)
            {
               const MLuint32 style = addMaterial(m);
               if(M!=NULL && nM>0)
               {
                 MLuint32* const styles = new MLuint32[nM];
                 for(MLuint32 i=0; i<nM; i++)
                   styles[i]=addMaterial(M[i]);
                 const MLuint32 meshid =  createTriangleMesh(nP, P, nI, PI, style, nN, N, NI, nT, T, TI, nC, C, CI, nM, styles, MI, ca);
                 delete[] styles;
                 return meshid;
               }
               else
                 return createTriangleMesh(nP, P, nI, PI, style, nN, N, NI, nT, T, TI, nC, C, CI, 0, NULL, NULL, ca);
            }
    void addQuads(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][4], const PRCmaterial &m,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][4],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][4],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][4],
                      MLuint32 nM, const PRCmaterial M[], const MLuint32 MI[], double ca);
    MLuint32 createQuadMesh(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][4], MLuint32 style_index,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][4],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][4],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][4],
                      MLuint32 nS, const MLuint32 S[],    const MLuint32 SI[], double ca);
    MLuint32 createQuadMesh(MLuint32 nP, const double P[][3], MLuint32 nI, const MLuint32 PI[][4], const PRCmaterial& m,
                      MLuint32 nN, const double N[][3],   const MLuint32 NI[][4],
                      MLuint32 nT, const double T[][2],   const MLuint32 TI[][4],
                      MLuint32 nC, const PRC_RGBAColor C[],  const MLuint32 CI[][4],
                      MLuint32 nM, const PRCmaterial M[], const MLuint32 MI[], double ca)
            {
               const MLuint32 style = addMaterial(m);
               if(M!=NULL && nM>0)
               {
                 MLuint32* const styles = new MLuint32[nM];
                 for(MLuint32 i=0; i<nM; i++)
                   styles[i]=addMaterial(M[i]);
                 const MLuint32 meshid =  createQuadMesh(nP, P, nI, PI, style, nN, N, NI, nT, T, TI, nC, C, CI, nM, styles, MI, ca);
                 delete[] styles;
                 return meshid;
               }
               else
                 return createQuadMesh(nP, P, nI, PI, style, nN, N, NI, nT, T, TI, nC, C, CI, 0, NULL, NULL, ca);
            }
#define PRCTRANSFORM const double origin[3]=NULL, const double x_axis[3]=NULL, const double y_axis[3]=NULL, double scale=1, const double* t=NULL
#define PRCCARTRANSFORM const double origin[3], const double x_axis[3], const double y_axis[3], double scale
#define PRCGENTRANSFORM const double* t=NULL
#define PRCNOMATERIALINDEX MLuint32_MAX
    void useMesh(MLuint32 tess_index, MLuint32 style_index,            PRCGENTRANSFORM);
    void useMesh(MLuint32 tess_index, const PRCmaterial& m,            PRCGENTRANSFORM)
           { useMesh(tess_index,addMaterial(m),t); }
    void useMesh(MLuint32 tess_index, MLuint32 style_index,            PRCCARTRANSFORM);
    void useMesh(MLuint32 tess_index, const PRCmaterial& m,            PRCCARTRANSFORM)
           { useMesh(tess_index,addMaterial(m),origin, x_axis, y_axis, scale); }

    void useLines(MLuint32 tess_index, MLuint32 style_index,           PRCGENTRANSFORM);
    void useLines(MLuint32 tess_index, const PRC_RGBAColor& c,  double w, PRCGENTRANSFORM)
           { useLines(tess_index, addLineMaterial(c,w), t); }
    void useLines(MLuint32 tess_index, MLuint32 style_index,           PRCCARTRANSFORM);
    void useLines(MLuint32 tess_index, const PRC_RGBAColor& c,  double w, PRCCARTRANSFORM)
           { useLines(tess_index,addLineMaterial(c,w),origin, x_axis, y_axis, scale); }

//  void addTriangle(const double P[][3], const double T[][2], MLuint32 style_index);
  
    void addLine(MLuint32 n, const double P[][3], const PRC_RGBAColor &c, double w=1.0);
    void addBezierCurve(MLuint32 n, const double cP[][3], const PRC_RGBAColor &c);
    void addCurve(MLuint32 d, MLuint32 n, const double cP[][3], const double *k, const PRC_RGBAColor &c, const double w[]);
    void addQuad(const double P[][3], const PRC_RGBAColor C[]);

    void addRectangle(const double P[][3], const PRCmaterial &m);
    void addPatch(const double cP[][3], const PRCmaterial &m);
    void addSurface(MLuint32 dU, MLuint32 dV, MLuint32 nU, MLuint32 nV,
     const double cP[][3], const double *kU, const double *kV, const PRCmaterial &m,
     const double w[]);
    void addTube(MLuint32 n, const double cP[][3], const double oP[][3], bool straight, const PRCmaterial& m, PRCTRANSFORM);
    void addHemisphere(double radius, const PRCmaterial& m, PRCTRANSFORM);
    void addSphere(double radius, const PRCmaterial& m, PRCTRANSFORM);
    void addDisk(double radius, const PRCmaterial& m, PRCTRANSFORM);
    void addCylinder(double radius, double height, const PRCmaterial& m, PRCTRANSFORM);
    void addCone(double radius, double height, const PRCmaterial& m, PRCTRANSFORM);
    void addTorus(double major_radius, double minor_radius, double angle1, double angle2, const PRCmaterial& m, PRCTRANSFORM);
#undef PRCTRANSFORM
#undef PRCCARTRANSFORM
#undef PRCGENTRANSFORM


    MLuint32 addPicture(PRC_PictureDataFormat format, MLuint32 size, const MLuint8 *picture, MLuint32 width=0, MLuint32 height=0,
      std::string name="", MLuint32 fileStructure=0)
      { return fileStructures[fileStructure]->addPicture(format, size, picture, width, height, name); }
    MLuint32 addPicture(const PRCpicture& pic,
      std::string name="", MLuint32 fileStructure=0)
      { return fileStructures[fileStructure]->addPicture(pic.format, pic.size, pic.data, pic.width, pic.height, name); }
    MLuint32 addTextureDefinition(PRC_TextureDefinition*& pTextureDefinition, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addTextureDefinition(pTextureDefinition);
      }
    MLuint32 addTextureApplication(PRC_TextureApplication*& pTextureApplication, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addTextureApplication(pTextureApplication);
      }
    MLuint32 addRgbColor(const PRC_RGBColor &color,
       MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addRgbColor(color);
      }
    MLuint32 addRgbColorUnique(const PRC_RGBColor &color,
       MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addRgbColorUnique(color);
      }
    MLuint32 addMaterialGeneric(PRC_MaterialGeneric*& pMaterialGeneric,
       MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addMaterialGeneric(pMaterialGeneric);
      }
    MLuint32 addStyle(PRC_Style*& pStyle, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addStyle(pStyle);
      }
    MLuint32 addPartDefinition(PRC_PartDefinition*& pPartDefinition, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addPartDefinition(pPartDefinition);
      }
    MLuint32 addProductOccurrence(PRC_ProductOccurrence*& pProductOccurrence, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addProductOccurrence(pProductOccurrence);
      }
    MLuint32 addTopoContext(PRC_TopologyContext*& pTopoContext, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addTopoContext(pTopoContext);
      }
    MLuint32 getTopoContext(PRC_TopologyContext*& pTopoContext, MLuint32 fileStructure=0)
    {
      return fileStructures[fileStructure]->getTopoContext(pTopoContext);
    }
    MLuint32 add3DTess(PRC_3DTesselation*& p3DTess, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->add3DTess(p3DTess);
      }
    MLuint32 add3DWireTess(PRC_3DWireTesselation*& p3DWireTess, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->add3DWireTess(p3DWireTess);
      }
/*
    MLuint32 addMarkupTess(PRC_MarkupTesselation*& pMarkupTess, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addMarkupTess(pMarkupTess);
      }
    MLuint32 addMarkup(PRC_Markup*& pMarkup, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addMarkup(pMarkup);
      }
    MLuint32 addAnnotationItem(PRC_AnnotationItem*& pAnnotationItem, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addAnnotationItem(pAnnotationItem);
      }
 */
    MLuint32 addCoordinateSystem(PRC_CoordinateSystem*& pCoordinateSystem, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addCoordinateSystem(pCoordinateSystem);
      }
    MLuint32 addCoordinateSystemUnique(PRC_CoordinateSystem*& pCoordinateSystem, MLuint32 fileStructure=0)
      {
        return fileStructures[fileStructure]->addCoordinateSystemUnique(pCoordinateSystem);
      }
  private:
    void serializeModelFileData(PRC_BitStream&);
    std::ofstream *fout;
    std::ostream &output;
};

#pragma warning( pop ) 

ML_END_NAMESPACE


#endif // _PRC_File_H
