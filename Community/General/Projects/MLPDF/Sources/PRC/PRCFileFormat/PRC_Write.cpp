//----------------------------------------------------------------------------------
//! This source file contains 
/*! 
//
// \file    PRC_Write.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-08-01
*/
//----------------------------------------------------------------------------------

#include "PRC_Write.h"
#include "PRC_Macros.h"

// ML includes
#include "mlModuleIncludes.h"


ML_START_NAMESPACE


using namespace std;

std::string currentName;
MLuint32 current_layer_index = MLuint32_MAX;
MLuint32 current_index_of_line_style = MLuint32_MAX;
uint16_t current_behaviour_bit_field = 1;


// Count leading zeros.
MLuint32 CountLeadingZeros(MLuint32 a) 
{
  static const int MultiplyDeBruijnBitPosition[32] = {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };

  a |= a >> 1; // first round down to one less than a power of 2 
  a |= a >> 2;
  a |= a >> 4;
  a |= a >> 8;
  a |= a >> 16;

  return 31-MultiplyDeBruijnBitPosition[(MLuint32)(a * 0x07C4ACDDU) >> 27];
}

// Portable integer implementation of ceil(log2(x)).
MLuint32 Log2(MLuint32 x) 
{
  assert(x != 0);
  MLuint32 L=31-CountLeadingZeros(x);
  return ((MLuint32) 1 << L == x) ? L : L+1;
}

void writeUncompressedUnsignedInteger(ostream &out, MLuint32 data)
{
  out.write(((char*)&data)+0,1);
  out.write(((char*)&data)+1,1);
  out.write(((char*)&data)+2,1);
  out.write(((char*)&data)+3,1);
}

MLuint32 createCADIdentifier()
{
  static MLuint32 ID = 1;
  return ID++;
}

MLuint32 createPRCUniqueIdentifier()
{
  static MLuint32 ID = 1;
  return ID++;
}

bool isTypeEligibleForReference(MLuint32 type)
{
  if(type == PRC_TYPE_MISC_EntityReference ||
     type == PRC_TYPE_MISC_MarkupLinkedItem ||
     type == PRC_TYPE_RI_BrepModel ||
     type == PRC_TYPE_RI_Curve ||
     type == PRC_TYPE_RI_Direction ||
     type == PRC_TYPE_RI_Plane ||
     type == PRC_TYPE_RI_PointSet ||
     type == PRC_TYPE_RI_PolyBrepModel ||
     type == PRC_TYPE_RI_PolyWire ||
     type == PRC_TYPE_RI_Set ||
     type == PRC_TYPE_RI_CoordinateSystem ||
     type == PRC_TYPE_ASM_ProductOccurence ||
     type == PRC_TYPE_ASM_PartDefinition ||
     type == PRC_TYPE_ASM_Filter ||
     type == PRC_TYPE_MKP_View ||
     type == PRC_TYPE_MKP_Markup ||
     type == PRC_TYPE_MKP_Leader ||
     type == PRC_TYPE_MKP_AnnotationItem ||
     type == PRC_TYPE_MKP_AnnotationSet ||
     type == PRC_TYPE_MKP_AnnotationReference ||
     type == PRC_TYPE_GRAPH_Style ||
     type == PRC_TYPE_GRAPH_Material ||
     type == PRC_TYPE_GRAPH_TextureApplication ||
     type == PRC_TYPE_GRAPH_TextureDefinition ||
     type == PRC_TYPE_GRAPH_LinePattern ||
     type == PRC_TYPE_GRAPH_DottingPattern ||
     type == PRC_TYPE_GRAPH_HatchingPattern ||
     type == PRC_TYPE_GRAPH_SolidPattern ||
     type == PRC_TYPE_GRAPH_VPicturePattern ||
     type == PRC_TYPE_GRAPH_AmbientLight ||
     type == PRC_TYPE_GRAPH_PointLight ||
     type == PRC_TYPE_GRAPH_DirectionalLight ||
     type == PRC_TYPE_GRAPH_SpotLight ||
     type == PRC_TYPE_GRAPH_SceneDisplayParameters ||
     type == PRC_TYPE_GRAPH_Camera
    )
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool isCompressedType(MLuint32 type)
{
  return (type == PRC_TYPE_TOPO_BrepDataCompress || type == PRC_TYPE_TOPO_SingleWireBodyCompress || type == PRC_TYPE_TESS_3D_Compressed);
}

void writeName(PRC_BitStream &pbs,const std::string &name)
{
  pbs << (name == currentName);
  if(name != currentName)
  {
    pbs << name;
    currentName = name;
  }
}

void resetName()
{
  currentName = "";
}

void writeGraphics(PRC_BitStream &pbs,MLuint32 l,MLuint32 i,uint16_t b,bool force)
{
  if(force || current_layer_index != l || current_index_of_line_style != i || current_behaviour_bit_field != b)
  {
    pbs << false << (MLuint32)(l+1) << (MLuint32)(i+1)
        << (MLuint8)(b&0xFF) << (MLuint8)((b>>8)&0xFF);
    current_layer_index = l;
    current_index_of_line_style = i;
    current_behaviour_bit_field = b;
  }
  else
    pbs << true;
}

void writeGraphics(PRC_BitStream &pbs,const PRC_Graphics &graphics,bool force)
{
  if(force || current_layer_index != graphics.layer_index || current_index_of_line_style != graphics.index_of_line_style || current_behaviour_bit_field != graphics.behaviour_bit_field)
  {
    pbs << false
        << (MLuint32)(graphics.layer_index+1)
        << (MLuint32)(graphics.index_of_line_style+1)
        << (MLuint8)(graphics.behaviour_bit_field&0xFF)
        << (MLuint8)((graphics.behaviour_bit_field>>8)&0xFF);
    current_layer_index = graphics.layer_index;
    current_index_of_line_style = graphics.index_of_line_style;
    current_behaviour_bit_field = graphics.behaviour_bit_field;
  }
  else
    pbs << true;
}

void resetGraphics()
{
  current_layer_index = MLuint32_MAX;
  current_index_of_line_style = MLuint32_MAX;
  current_behaviour_bit_field = 1;
}

void resetGraphicsAndName()
{
  resetGraphics(); resetName();
}

void SerializeArrayRGBA (const std::vector<MLuint8> &rgba_vertices,const bool is_rgba, PRC_BitStream &pbs)
{
  MLuint32 i = 0;
  MLuint32 j = 0;

  const MLuint32 number_by_vector=is_rgba?4:3;
  const std::vector<MLuint8> &vector_color = rgba_vertices;
  const MLuint32 number_of_colors = (MLuint32)vector_color.size();
  const MLuint32 number_of_vectors = number_of_colors / number_by_vector;
  
  // first one 
  for (i=0;i<number_by_vector;i++)
     WriteCharacter (vector_color[i])
  
  for (i=1;i<number_of_vectors;i++)
  {
     bool b_same = true;
     for (j=0;j<number_by_vector;j++)
     {
        if ((vector_color[i*number_by_vector+j] - vector_color[(i-1)*number_by_vector+j]) != 0)
        {
           b_same = false;
           break;
        }
     }
     WriteBoolean (b_same)
     if (!b_same)
     {
        for (j=0;j<number_by_vector;j++)
           WriteCharacter (vector_color[i*number_by_vector+j])
     }
  }
}

void writeUnit(PRC_BitStream &out,bool fromCAD,double unit)
{
  out << fromCAD << unit;
}

void writeEmptyMarkups(PRC_BitStream &out)
{
  out << (MLuint32)0 // # of linked items
      << (MLuint32)0 // # of leaders
      << (MLuint32)0 // # of markups
      << (MLuint32)0; // # of annotation entities
}

void writeUnsignedIntegerWithVariableBitNumber(PRC_BitStream &pbs, MLuint32 value, MLuint32 bit_number)
{
   MLuint32 i;
   for(i=0; i<bit_number; i++)
   {
      if( value >= 1u<<(bit_number - 1 - i) )
      {
         WriteBoolean (true)
      
         value -= 1u<<(bit_number - 1 - i);
      }
      else
      {
         WriteBoolean (false)
      }
   }
}

void writeIntegerWithVariableBitNumber(PRC_BitStream &pbs, int32_t iValue, MLuint32 uBitNumber)
{ 
  WriteBoolean(iValue<0);
  WriteUnsignedIntegerWithVariableBitNumber(abs(iValue), uBitNumber - 1);
}

void writeDoubleWithVariableBitNumber(PRC_BitStream &pbs, double dValue,double dTolerance, unsigned uBitNumber)
{
// calling functions must ensure no overflow
  int32_t iTempValue = (int32_t) ( dValue / dTolerance );
  WriteIntegerWithVariableBitNumber(iTempValue, uBitNumber);
}

MLuint32  GetNumberOfBitsUsedToStoreUnsignedInteger(MLuint32 uValue)
{
  MLuint32 uNbBit=2;
  MLuint32 uTemp = 2;
  while(uValue >= uTemp)
  {
    uTemp*=2;
    uNbBit++;
  }
  return uNbBit-1;
}

void  writeNumberOfBitsThenUnsignedInteger(PRC_BitStream &pbs, MLuint32 unsigned_integer)
{
   MLuint32 number_of_bits = GetNumberOfBitsUsedToStoreUnsignedInteger( unsigned_integer );
   WriteUnsignedIntegerWithVariableBitNumber ( number_of_bits, 5 )
   WriteUnsignedIntegerWithVariableBitNumber ( unsigned_integer, number_of_bits )
}

MLuint32  GetNumberOfBitsUsedToStoreInteger(int32_t iValue)
{
   return GetNumberOfBitsUsedToStoreUnsignedInteger(abs(iValue))+1;
}

int32_t intdiv(double dValue, double dTolerance)
{
  double ratio=fabs(dValue)/dTolerance;
  assert(ratio <= INT_MAX);
  int32_t iTempValue=(int32_t) ratio;
  if(ratio - iTempValue >= 0.5) iTempValue++;
  if(dValue < 0) 
    return -iTempValue;
  else
    return iTempValue;
}

// round dValue to nearest multiple of dTolerance
double roundto(double dValue, double dTolerance)
{
    return intdiv(dValue, dTolerance) * dTolerance;
}

PRC_Vector3D roundto(PRC_Vector3D vec, double dTolerance)
{
    PRC_Vector3D res;
    res.x = roundto(vec.x,dTolerance);
    res.y = roundto(vec.y,dTolerance);
    res.z = roundto(vec.z,dTolerance);
    return    res;
}

MLuint32  GetNumberOfBitsUsedToStoreDouble(double dValue, double dTolerance )
{
   return GetNumberOfBitsUsedToStoreInteger(intdiv(dValue,dTolerance));
}

MLuint32  GetNumberOfBitsUsedToStoreTripleInteger(const MLint32_Triple &iTriple)
{
   const MLuint32 x_bits = GetNumberOfBitsUsedToStoreInteger(iTriple.x);
   const MLuint32 y_bits = GetNumberOfBitsUsedToStoreInteger(iTriple.y);
   const MLuint32 z_bits = GetNumberOfBitsUsedToStoreInteger(iTriple.z);
   MLuint32 bits = x_bits;
   if(y_bits > bits)
     bits = y_bits;
   if(z_bits > bits)
     bits = z_bits;
   return bits;
}

MLint32_Triple iroundto(PRC_Vector3D vec, double dTolerance)
{
    MLint32_Triple res;
    res.x = intdiv(vec.x, dTolerance);
    res.y = intdiv(vec.y, dTolerance);
    res.z = intdiv(vec.z, dTolerance);
    return    res;
}










void PRC_UserData::write(PRC_BitStream &pbs)
{
  pbs << size;
  if(size > 0) {
    MLuint32 quot=size/8;
    MLuint32 rem=size-8*quot;
    for(MLuint32 i = 0; i < quot; ++i)
      pbs << data[i];
    for(MLuint32 j = 0; j < rem; ++j) // 0-based, big endian bit counting
      pbs << (bool)((data[quot] & (0x80 >> j))!=0);
  }
}

void PRC_AttributeEntry::serializeAttributeEntry(PRC_BitStream &pbs) const
{
  WriteBoolean (title_is_integer) 
  if (title_is_integer)
    WriteUnsignedInteger (title_integer)
  else  
    WriteString (title_text)
}

void PRC_SingleAttribute::serializeSingleAttribute(PRC_BitStream &pbs) const
{
  SerializeAttributeEntry
  WriteUnsignedInteger (type)
  switch (type)
  {
    case PRC_ModellerAttributeType_Int:
      WriteInteger (value.integer)
      break;
    case PRC_ModellerAttributeType_Real:
      WriteDouble (value.real)
      break;
    case PRC_ModellerAttributeType_Time:
      WriteUnsignedInteger (value.time)
      break;
    case PRC_ModellerAttributeType_String:
      WriteString (value_text)
      break; 
    default:
      break;
  }
}

void PRC_Attribute::serializeAttribute(PRC_BitStream &pbs) const
{
  WriteUnsignedInteger (PRC_TYPE_MISC_Attribute) 
  
  SerializeAttributeEntry
  const MLuint32 size_of_attribute_keys = (MLuint32)attribute_keys.size();
  WriteUnsignedInteger (size_of_attribute_keys) 
  for(size_t i=0;i<size_of_attribute_keys;i++) 
    SerializeContentSingleAttribute (attribute_keys[i]) 
}

void PRC_Attributes::serializeAttributes(PRC_BitStream &pbs) const
{
  if (attributes.empty()) { // shortcut for most typical case
    const MLuint32 number_of_attributes = 0;
    WriteUnsignedInteger (number_of_attributes) 
    return;
  }
  const MLuint32 number_of_attributes = (MLuint32)attributes.size();
  WriteUnsignedInteger (number_of_attributes) 
  for(PRC_AttributeList::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
  {
    SerializeAttribute(*it)
  }
}

void PRC_ContentBase::serializeContentPRCBase(PRC_BitStream &pbs) const
{
  SerializeAttributeData

  SerializeName (name)
  if (isTypeEligibleForReference(type))
  {
    WriteUnsignedInteger (CAD_identifier)
    WriteUnsignedInteger (CAD_persistent_identifier)
    WriteUnsignedInteger (PRC_unique_identifier)
  }
}

void PRC_ReferenceUniqueIdentifier::serializeReferenceUniqueIdentifier(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_MISC_ReferenceOnPRCBase)
  WriteUnsignedInteger (type)
  const bool reference_in_same_file_structure = true;
  WriteBoolean (reference_in_same_file_structure)
// if (!reference_in_same_file_structure)
//    SerializeCompressedUniqueId (target_file_structure)
  WriteUnsignedInteger (unique_identifier)
}

void PRC_RGBColor::serializeRgbColor(PRC_BitStream &pbs)
{
  WriteDouble (red)
  WriteDouble (green)
  WriteDouble (blue)
}

void PRC_Picture::serializePicture(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_GRAPH_Picture)
  SerializeContentPRCBase
  WriteInteger (format) //see Types for picture files
  WriteUnsignedInteger (uncompressed_file_index+1)
  WriteUnsignedInteger (pixel_width)
  WriteUnsignedInteger (pixel_height)
}

void PRC_TextureDefinition::serializeTextureDefinition(PRC_BitStream &pbs)
{
  MLuint32 i=0; // universal index for PRC standart compatibility
  const MLuint8 texture_dimension = 2;
  const MLuint32 texture_mapping_attributes = texture_mapping_attribute;
  const MLuint32 size_texture_mapping_attributes_intensities = 1;
  const double *texture_mapping_attributes_intensities = &texture_mapping_attribute_intensity;
  const MLuint32 size_texture_mapping_attributes_components = 1;
  const MLuint8 *texture_mapping_attributes_components = &texture_mapping_attribute_components;
  const PRC_TextureMappingType eMappingType = PRC_TextureMappingType_Stored;
  
  const double red = 1.0;
  const double green = 1.0;
  const double blue = 1.0;
  const double alpha = 1.0;
  const PRC_TextureBlendParameter blend_src_rgb = PRC_TextureBlendParameter_Unknown;
  const PRC_TextureBlendParameter blend_dst_rgb = PRC_TextureBlendParameter_Unknown;
  const PRC_TextureBlendParameter blend_src_alpha = PRC_TextureBlendParameter_Unknown;
  const PRC_TextureBlendParameter blend_dst_alpha = PRC_TextureBlendParameter_Unknown;
  const PRC_TextureAlphaTest alpha_test = PRC_TextureAlphaTest_Unknown;
  const double alpha_test_reference = 1.0;
  const PRC_TextureWrappingMode texture_wrapping_mode_R = PRC_TextureWrappingMode_ClampToBorder;
  const bool texture_transformation = false;

  WriteUnsignedInteger (PRC_TYPE_GRAPH_TextureDefinition)

  SerializeContentPRCBase

  WriteUnsignedInteger (picture_index+1) 
  WriteCharacter (texture_dimension) 

  //   SerializeTextureMappingType
  WriteInteger (eMappingType) // Texture mapping type
  //  if (eMappingType == TEXTURE_MAPPING_OPERATOR)
  //  {
  //     WriteInteger (eMappingOperator) // Texture mapping operator
  //     WriteInteger (transformation)
  //     if (transformation)
  //        SerializeCartesianTransformation3d (transformation) 
  //  }

  WriteUnsignedInteger (texture_mapping_attributes) // Texture mapping attributes 
  WriteUnsignedInteger (size_texture_mapping_attributes_intensities)
  for (i=0;i<size_texture_mapping_attributes_intensities;i++)
     WriteDouble (texture_mapping_attributes_intensities[i])
  WriteUnsignedInteger (size_texture_mapping_attributes_components)
  for (i=0;i<size_texture_mapping_attributes_components;i++)
     WriteCharacter (texture_mapping_attributes_components[i]) 

  WriteInteger (texture_function)
  // reserved for future use; see Texture function 
  if (texture_function == PRC_TextureFunction_Blend)
  {
     WriteDouble (red) // blend color component in the range [0.0,1.0]
     WriteDouble (green) // blend color component in the range [0.0,1.0]
     WriteDouble (blue) // blend color component in the range [0.0,1.0]
     WriteDouble (alpha) // blend color component in the range [0.0,1.0]
  }

#pragma warning( push )
  #pragma warning( disable : 4127 )
  // Suppress warning that assignment operator could not be generated.

  WriteInteger (blend_src_rgb) // Texture blend parameter 
  // reserved for future use; see Texture blend parameter 
  if (blend_src_rgb != PRC_TextureBlendParameter_Unknown)
     WriteInteger (blend_dst_rgb) // Texture blend parameter 

  WriteInteger (blend_src_alpha) // Texture blend parameter 
  // reserved for future use; see Texture blend parameter 
  if (blend_src_alpha != PRC_TextureBlendParameter_Unknown)
  {
     WriteInteger (blend_dst_alpha) // Texture blend parameter 
  }

  WriteCharacter (texture_applying_mode) // Texture applying mode 
  if (texture_applying_mode & PRC_TEXTURE_APPLYING_MODE_ALPHATEST)
  {
     WriteInteger (alpha_test) // Texture alpha test 
     WriteDouble (alpha_test_reference)
  }

  WriteInteger (texture_wrapping_mode_S) // Texture wrapping mode

  if (texture_dimension > 1)
  {
     WriteInteger (texture_wrapping_mode_T) // Texture wrapping mode
  }

  if (texture_dimension > 2 )
  {
     WriteInteger (texture_wrapping_mode_R) // Texture wrapping mode
  }

#pragma warning( pop ) 

  WriteBit (texture_transformation)
//  if (texture_transformation)
//     SerializeTextureTransformation (texture_transformation)
}

void PRC_MaterialGeneric::serializeMaterialGeneric(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_GRAPH_Material)
  SerializeContentPRCBase
  WriteUnsignedInteger (ambient + 1)
  WriteUnsignedInteger (diffuse + 1)
  WriteUnsignedInteger (emissive + 1)
  WriteUnsignedInteger (specular + 1)
  WriteDouble (shininess)
  WriteDouble (ambient_alpha)
  WriteDouble (diffuse_alpha)
  WriteDouble (emissive_alpha)
  WriteDouble (specular_alpha)
}

void PRC_TextureApplication::serializeTextureApplication(PRC_BitStream &pbs) 
{
  WriteUnsignedInteger (PRC_TYPE_GRAPH_TextureApplication)
  SerializeContentPRCBase

  WriteUnsignedInteger (material_generic_index+1) 
  WriteUnsignedInteger (texture_definition_index+1) 
  WriteUnsignedInteger (next_texture_index+1) 
  WriteUnsignedInteger (UV_coordinates_index+1)
}

void PRC_LinePattern::serializeLinePattern(PRC_BitStream &pbs)
{
  MLuint32 i = 0;
  WriteUnsignedInteger (PRC_TYPE_GRAPH_LinePattern)
  SerializeContentPRCBase
  
  const MLuint32 size_lengths = (MLuint32)lengths.size();
  WriteUnsignedInteger (size_lengths)
  for (i=0;i<size_lengths;i++)
    WriteDouble (lengths[i])
  WriteDouble (phase)
  WriteBoolean (is_real_length)
}

void PRC_Style::serializeCategory1LineStyle(PRC_BitStream &pbs)
{
  const bool is_additional_1_defined = (additional!=0);
  const MLuint8 additional_1 = additional;
  const bool is_additional_2_defined = false;
  const MLuint8 additional_2 = 0;
  const bool is_additional_3_defined = false;
  const MLuint8 additional_3 = 0;
  WriteUnsignedInteger (PRC_TYPE_GRAPH_Style)
  SerializeContentPRCBase
  WriteDouble (line_width)
  WriteBoolean (is_vpicture)
  WriteUnsignedInteger (line_pattern_vpicture_index + 1)
  WriteBoolean (is_material)
  WriteUnsignedInteger (color_material_index + 1)
  WriteBoolean (is_transparency_defined)
  if (is_transparency_defined)
     WriteCharacter (transparency)
  WriteBoolean (is_additional_1_defined)
  if (is_additional_1_defined)
     WriteCharacter (additional_1)
  WriteBoolean (is_additional_2_defined)

#pragma warning( push )
  #pragma warning( disable : 4127 )
  // Suppress warning that assignment operator could not be generated.

  if (is_additional_2_defined)
  {
     WriteCharacter (additional_2)
  }

  WriteBoolean (is_additional_3_defined)

  if (is_additional_3_defined)
  {
     WriteCharacter (additional_3)
  }

#pragma warning( pop )
}

void PRC_Graphics::serializeGraphics(PRC_BitStream &pbs)
{
  if(current_layer_index != this->layer_index || current_index_of_line_style != this->index_of_line_style || current_behaviour_bit_field != this->behaviour_bit_field)
  {
    pbs << false
        << (MLuint32)(this->layer_index+1)
        << (MLuint32)(this->index_of_line_style+1)
        << (MLuint8)(this->behaviour_bit_field&0xFF)
        << (MLuint8)((this->behaviour_bit_field>>8)&0xFF);
    current_layer_index = this->layer_index;
    current_index_of_line_style = this->index_of_line_style;
    current_behaviour_bit_field = this->behaviour_bit_field;
  }
  else
    pbs << true;
}

void PRC_Graphics::serializeGraphicsForced(PRC_BitStream &pbs)
{
  pbs << false
      << (MLuint32)(this->layer_index+1)
      << (MLuint32)(this->index_of_line_style+1)
      << (MLuint8)(this->behaviour_bit_field&0xFF)
      << (MLuint8)((this->behaviour_bit_field>>8)&0xFF);
  current_layer_index = this->layer_index;
  current_index_of_line_style = this->index_of_line_style;
  current_behaviour_bit_field = this->behaviour_bit_field;
}

void PRC_Markup::serializeMarkup(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_MKP_Markup)
  SerializeContentPRCBase 
  SerializeGraphics
  WriteUnsignedInteger (type) 
  WriteUnsignedInteger (sub_type) 
  const MLuint32 number_of_linked_items = 0;
  WriteUnsignedInteger (number_of_linked_items) 
//  for (i=0;i<number_of_linked_items;i++) 
//     SerializeReferenceUniqueIdentifier (linked_items[i])
  const MLuint32 number_of_leaders = 0;
  WriteUnsignedInteger (number_of_leaders) 
//  for (i=0;i<number_of_leaders;i++) 
//     SerializeReferenceUniqueIdentifier (leaders[i])
  WriteUnsignedInteger (index_tessellation + 1) 
  SerializeUserData
}

void PRC_AnnotationItem::serializeAnnotationItem(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_MKP_AnnotationItem)
  SerializeContentPRCBase 
  SerializeGraphics
  SerializeReferenceUniqueIdentifier (markup)
  SerializeUserData
}

void PRC_RepresentationItemContent::serializeRepresentationItemContent(PRC_BitStream &pbs)
{
  SerializeContentPRCBase 
  SerializeGraphics
  WriteUnsignedInteger (index_local_coordinate_system + 1)
  WriteUnsignedInteger (index_tessellation + 1)
}

void  PRC_BoundaryRepresentationModel::serializeBrepModel(PRC_BitStream &pbs)
{
   WriteUnsignedInteger (PRC_TYPE_RI_BrepModel)

   SerializeRepresentationItemContent 
   WriteBit (has_brep_data)
   if (has_brep_data)
   {
      WriteUnsignedInteger (context_id+1)
      WriteUnsignedInteger (body_id+1)
   }
   WriteBoolean (is_closed)
   SerializeUserData
}

void  PRC_PolyBoundaryRepresentationModel::serializePolyBrepModel(PRC_BitStream &pbs)
{
   WriteUnsignedInteger (PRC_TYPE_RI_PolyBrepModel)

   SerializeRepresentationItemContent 
   WriteBoolean (is_closed)
   SerializeUserData
}

void  PRC_PointSet::serializePointSet(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_RI_PointSet)

  SerializeRepresentationItemContent 

  const MLuint32 number_of_points = (MLuint32)point.size();
  WriteUnsignedInteger (number_of_points)
  for (MLuint32 i=0;i<number_of_points;i++)
  {
     SerializeVector3d (point[i])
  }
  SerializeUserData
}

void  PRC_Set::serializeSet(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_RI_Set)

  SerializeRepresentationItemContent 

  const MLuint32 number_of_elements = (MLuint32)elements.size();
  WriteUnsignedInteger (number_of_elements)
  for (MLuint32 i=0;i<number_of_elements;i++)
  {
    SerializeRepresentationItem (elements[i])
  }
  SerializeUserData
}

MLuint32 PRC_Set::addBrepModel(PRC_BoundaryRepresentationModel*& pBrepModel)
{
  elements.push_back(pBrepModel);
  pBrepModel = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addPolyBrepModel(PRC_PolyBoundaryRepresentationModel*& pPolyBrepModel)
{
  elements.push_back(pPolyBrepModel);
  pPolyBrepModel = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addPointSet(PRC_PointSet*& pPointSet)
{
  elements.push_back(pPointSet);
  pPointSet  = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addSet(PRC_Set*& pSet)
{
  elements.push_back(pSet);
  pSet = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addWire(PRC_Wire*& pWire)
{
  elements.push_back(pWire);
  pWire = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addPolyWire(PRC_PolyWire*& pPolyWire)
{
  elements.push_back(pPolyWire);
  pPolyWire = NULL;
  return (MLuint32)(elements.size()-1);
}

MLuint32 PRC_Set::addRepresentationItem(PRC_RepresentationItem*& pRepresentationItem)
{
  elements.push_back(pRepresentationItem);
  pRepresentationItem = NULL;
  return (MLuint32)(elements.size()-1);
}

void PRC_Wire::serializeWire(PRC_BitStream &pbs)
{
   WriteUnsignedInteger (PRC_TYPE_RI_Curve)

   SerializeRepresentationItemContent 
   WriteBit (has_wire_body)
   if (has_wire_body)
   {
      WriteUnsignedInteger (context_id+1)
      WriteUnsignedInteger (body_id+1)
   }

   SerializeUserData
}

void PRC_PolyWire::serializePolyWire(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_RI_PolyWire)

  SerializeRepresentationItemContent 
  SerializeUserData
}

void PRC_GeneralTransformation3D::serializeGeneralTransformation3d(PRC_BitStream &pbs) const
{
  WriteUnsignedInteger (PRC_TYPE_MISC_GeneralTransformation)

  for (int j=0;j<4;j++)
  {
    for (int i=0;i<4;i++)
    {
     WriteDouble(mat[i][j]); 
    }
  }
}

void PRC_CartesianTransformation3D::serializeCartesianTransformation3d(PRC_BitStream &pbs) const
{
  WriteUnsignedInteger (PRC_TYPE_MISC_CartesianTransformation)
  WriteCharacter ( behaviour )
  if (behaviour & PRC_TRANSFORMATION_Translate)
  {
     SerializeVector3d ( origin )
  }

  if (behaviour & PRC_TRANSFORMATION_NonOrtho)
  {
     SerializeVector3d ( X )
     SerializeVector3d ( Y )
     SerializeVector3d ( Z )
  }
  else if (behaviour & PRC_TRANSFORMATION_Rotate)
  {
     SerializeVector3d ( X )
     SerializeVector3d ( Y )
  }

  if (behaviour & PRC_TRANSFORMATION_NonUniformScale)
  {
        SerializeVector3d ( scale )
  }
  else if (behaviour & PRC_TRANSFORMATION_Scale)
  {
        WriteDouble ( uniform_scale )
  }

  if (behaviour & PRC_TRANSFORMATION_Homogeneous)
  {
     WriteDouble ( X_homogeneous_coord )
     WriteDouble ( Y_homogeneous_coord )
     WriteDouble ( Z_homogeneous_coord )
     WriteDouble ( origin_homogeneous_coord )
  }
}

void PRC_Transformation::serializeTransformation(PRC_BitStream &pbs)
{ 
   WriteBit ( has_transformation )
   if (has_transformation)
   {
      WriteCharacter ( behaviour )
      if ( geometry_is_2D )
      {
         if (behaviour & PRC_TRANSFORMATION_Translate)
            SerializeVector2d ( origin )
         if (behaviour & PRC_TRANSFORMATION_Rotate)
         {
            SerializeVector2d ( x_axis )
            SerializeVector2d ( y_axis )
         }
         if (behaviour & PRC_TRANSFORMATION_Scale)
            WriteDouble ( scale )
      }
      else
      {
         if (behaviour & PRC_TRANSFORMATION_Translate)
            SerializeVector3d ( origin )
         if (behaviour & PRC_TRANSFORMATION_Rotate)
         {
            SerializeVector3d ( x_axis )
            SerializeVector3d ( y_axis )
         }
         if (behaviour & PRC_TRANSFORMATION_Scale)
            WriteDouble ( scale )
      }
   }
}

void PRC_CoordinateSystem::serializeCoordinateSystem(PRC_BitStream &pbs)
{
  WriteUnsignedInteger (PRC_TYPE_RI_CoordinateSystem)

  SerializeRepresentationItemContent 
  axis_set->serializeTransformation3d(pbs);
  SerializeUserData
}

void PRC_FontKeysSameFont::serializeFontKeysSameFont(PRC_BitStream &pbs)
{
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteString (font_name)
  WriteUnsignedInteger (char_set)
  const MLuint32 number_of_font_keys = (MLuint32)font_keys.size();
  WriteUnsignedInteger (number_of_font_keys)
  for (i=0;i<number_of_font_keys;i++)
  {
     WriteUnsignedInteger (font_keys[i].font_size + 1)
     WriteCharacter (font_keys[i].attributes)
  }
}

void PRC_TesselationFace::serializeTessFace(PRC_BitStream &pbs)
{
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteUnsignedInteger (PRC_TYPE_TESS_Face)

  const MLuint32 size_of_line_attributes = (MLuint32)line_attributes.size();
  WriteUnsignedInteger (size_of_line_attributes) 
  for (i=0;i<size_of_line_attributes;i++) 
     SerializeLineAttr (line_attributes[i])

  WriteUnsignedInteger (start_wire) 
  const MLuint32 size_of_sizes_wire = (MLuint32)sizes_wire.size();
  WriteUnsignedInteger (size_of_sizes_wire) 
  for (i=0;i<size_of_sizes_wire;i++) 
     WriteUnsignedInteger (sizes_wire[i])

  WriteUnsignedInteger (used_entities_flag) 

  WriteUnsignedInteger (start_triangulated) 
  const MLuint32 size_of_sizes_triangulated = (MLuint32)sizes_triangulated.size();
  WriteUnsignedInteger (size_of_sizes_triangulated) 
  for (i=0;i<size_of_sizes_triangulated;i++) 
     WriteUnsignedInteger (sizes_triangulated[i])

  if(number_of_texture_coordinate_indexes==0 &&
     used_entities_flag &
     (
      PRC_FACETESSDATA_PolyfaceTextured|
      PRC_FACETESSDATA_TriangleTextured|
      PRC_FACETESSDATA_TriangleFanTextured|
      PRC_FACETESSDATA_TriangleStripeTextured|
      PRC_FACETESSDATA_PolyfaceOneNormalTextured|
      PRC_FACETESSDATA_TriangleOneNormalTextured|
      PRC_FACETESSDATA_TriangleFanOneNormalTextured|
      PRC_FACETESSDATA_TriangleStripeOneNormalTextured
     ))
    WriteUnsignedInteger (1)  // workaround for error of not setting number_of_texture_coordinate_indexes
  else 
    WriteUnsignedInteger (number_of_texture_coordinate_indexes)

#pragma warning( push )
  #pragma warning( disable : 4127 )
  // Suppress warning that assignment operator could not be generated.

  const bool has_vertex_colors = !rgba_vertices.empty();
  WriteBoolean (has_vertex_colors)
  if (has_vertex_colors)
  {
     WriteBoolean (is_rgba)
     const bool b_optimised=false;
     WriteBoolean (b_optimised)
     if (!b_optimised)
     {
       SerializeArrayRGBA (rgba_vertices, is_rgba, pbs);
     }
     else
     {
     // not described
     }
  }

  if (size_of_line_attributes) 
  {
     WriteUnsignedInteger (behaviour)
  }

#pragma warning( push )
}

void PRC_TesselationBaseData::serializeContentBaseTessData(PRC_BitStream &pbs)
{
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteBoolean (is_calculated)
  const MLuint32 number_of_coordinates = (MLuint32)coordinates.size();
  WriteUnsignedInteger (number_of_coordinates)
  for (i=0;i<number_of_coordinates;i++)
     WriteDouble (coordinates[i])
}

void PRC_3DTesselation::serialize3DTess(PRC_BitStream &pbs)
{
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteUnsignedInteger (PRC_TYPE_TESS_3D)
  SerializeContentBaseTessData 
  WriteBoolean (has_faces)
  WriteBoolean (has_loops)
  const bool must_recalculate_normals=normal_coordinate.empty();
  WriteBoolean (must_recalculate_normals)
  if (must_recalculate_normals)
  {
     const MLuint8 normals_recalculation_flags=0;
     // not used; should be zero
     WriteCharacter (normals_recalculation_flags)
     // definition similar to VRML
     WriteDouble (crease_angle)
  }
  
  const MLuint32 number_of_normal_coordinates = (MLuint32)normal_coordinate.size();
  WriteUnsignedInteger (number_of_normal_coordinates)
  for (i=0;i<number_of_normal_coordinates;i++)
     WriteDouble (normal_coordinate[i])
  
  const MLuint32 number_of_wire_indices = (MLuint32)wire_index.size();
  WriteUnsignedInteger (number_of_wire_indices)
  for (i=0;i<number_of_wire_indices;i++)
     WriteUnsignedInteger (wire_index[i])
  
  // note : those can be single triangles, triangle fans or stripes
  const MLuint32 number_of_triangulated_indices = (MLuint32)triangulated_index.size();
  WriteUnsignedInteger (number_of_triangulated_indices)
  for (i=0;i<number_of_triangulated_indices;i++)
     WriteUnsignedInteger (triangulated_index[i])
  
  const MLuint32 number_of_face_tessellation = (MLuint32)face_tessellation.size();
  WriteUnsignedInteger (number_of_face_tessellation)
  for (i=0;i<number_of_face_tessellation;i++)
     SerializeTessFace (face_tessellation[i])
  
  const MLuint32 number_of_texture_coordinates = (MLuint32)texture_coordinate.size();
  WriteUnsignedInteger (number_of_texture_coordinates)
  for (i=0;i<number_of_texture_coordinates;i++)
     WriteDouble (texture_coordinate[i])
}

void PRC_3DTesselation::addTessFace(PRC_TesselationFace*& pTessFace)
{
  face_tessellation.push_back(pTessFace);
  pTessFace = NULL;
}

void PRC_3DWireTesselation::serialize3DWireTess(PRC_BitStream &pbs)
{
// group___tf3_d_wire_tess_data_____serialize2.html
// group___tf3_d_wire_tess_data_____serialize_content2.html
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteUnsignedInteger (PRC_TYPE_TESS_3D_Wire)
  SerializeContentBaseTessData 
  const MLuint32 number_of_wire_indexes = (MLuint32)wire_indexes.size();
  WriteUnsignedInteger (number_of_wire_indexes)
  for (i=0;i<number_of_wire_indexes;i++)
     WriteUnsignedInteger (wire_indexes[i])
  
  const bool has_vertex_colors = !rgba_vertices.empty();
  WriteBoolean (has_vertex_colors)
  
#pragma warning( push )
  #pragma warning( disable : 4127 )
  // Suppress warning that assignment operator could not be generated.
    
  if (has_vertex_colors)
  {
     WriteBoolean (is_rgba)
     WriteBoolean (is_segment_color)
     const bool b_optimised=false;
     WriteBoolean (b_optimised)
     if (!b_optimised)
     {
       SerializeArrayRGBA (rgba_vertices, is_rgba, pbs);
     }
     else
     {
     // not described
     }
  }

#pragma warning( push )

}

void PRC_MarkupTesselation::serializeMarkupTess(PRC_BitStream &pbs)
{
// group___tf_markup_tess_data_____serialize2.html
// group___tf_markup_tess_data_____serialize_content2.html
  MLuint32 i=0; // universal index for PRC standart compatibility
  WriteUnsignedInteger (PRC_TYPE_TESS_Markup)
  SerializeContentBaseTessData 

  const MLuint32 number_of_codes = (MLuint32)codes.size();
  WriteUnsignedInteger (number_of_codes)
  for (i=0;i<number_of_codes;i++)
     WriteUnsignedInteger (codes[i])
  const MLuint32 number_of_texts = (MLuint32)texts.size();
  WriteUnsignedInteger (number_of_texts)
  for (i=0;i<number_of_texts;i++)
     WriteString (texts[i])
  WriteString (label) // label of tessellation
  WriteCharacter (behaviour)
}

void PRC_BaseTopology::serializeBaseTopology(PRC_BitStream &pbs)
{
   WriteBoolean (base_information)
   if (base_information)
   {
      SerializeAttributeData
      SerializeName (name)
      WriteUnsignedInteger (identifier)
   }
}

void PRC_BaseGeometry::serializeBaseGeometry(PRC_BitStream &pbs)
{
   WriteBoolean (base_information)
   if (base_information)
   {
      SerializeAttributeData
      SerializeName (name)
      WriteUnsignedInteger (identifier)
   }
}

void PRC_BodyContent::serializeContentBody(PRC_BitStream &pbs)
{
   SerializeBaseTopology
   WriteCharacter ( behavior )
}

void PRC_BoundingBox::serializeBoundingBox(PRC_BitStream &pbs)
{
   SerializeVector3d ( min )
   SerializeVector3d ( max )
}

void PRC_Domain::serializeDomain(PRC_BitStream &pbs)
{
   SerializeVector2d ( min )
   SerializeVector2d ( max )
}

void PRC_Interval::serializeInterval(PRC_BitStream &pbs)
{
   WriteDouble ( min )
   WriteDouble ( max )
}

void PRC_Parameterization::serializeParameterization(PRC_BitStream &pbs)
{
   SerializeInterval ( interval )
   WriteDouble ( parameterization_coeff_a )
   WriteDouble ( parameterization_coeff_b )
}

void PRC_UVParameterization::serializeUVParameterization(PRC_BitStream &pbs)
{
   WriteBoolean ( swap_uv )
   SerializeDomain ( uv_domain )
   WriteDouble ( parameterization_on_u_coeff_a )
   WriteDouble ( parameterization_on_v_coeff_a )
   WriteDouble ( parameterization_on_u_coeff_b )
   WriteDouble ( parameterization_on_v_coeff_b )
}

void PRC_ContentSurface::serializeContentSurface(PRC_BitStream &pbs)
{
   SerializeBaseGeometry
   WriteUnsignedInteger ( extend_info )
}

void  PRC_NURBSSurface::serializeNURBSSurface(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
//  MLuint32 i=0, j=0;
   WriteUnsignedInteger (PRC_TYPE_SURF_NURBS) 

   SerializeContentSurface 
   WriteBoolean ( is_rational )
   WriteUnsignedInteger ( degree_in_u )
   WriteUnsignedInteger ( degree_in_v )
   const MLuint32 highest_index_of_knots_in_u = (MLuint32)knot_u.size()-1;
   const MLuint32 highest_index_of_knots_in_v = (MLuint32)knot_v.size()-1;
   const MLuint32 highest_index_of_control_point_in_u = highest_index_of_knots_in_u - degree_in_u - 1;
   const MLuint32 highest_index_of_control_point_in_v = highest_index_of_knots_in_v - degree_in_v - 1;
   WriteUnsignedInteger ( highest_index_of_control_point_in_u )
   WriteUnsignedInteger ( highest_index_of_control_point_in_v )
   WriteUnsignedInteger ( highest_index_of_knots_in_u )
   WriteUnsignedInteger ( highest_index_of_knots_in_v )
   for (i=0; i < (highest_index_of_control_point_in_u+1)*(highest_index_of_control_point_in_v+1); i++)
   {
      WriteDouble ( control_point[i].x )
      WriteDouble ( control_point[i].y )
      WriteDouble ( control_point[i].z )
      if (is_rational)
         WriteDouble ( control_point[i].w )
   }
//  for (i=0; i<=highest_index_of_control_point_in_u; i++)
//  {
//     for (j=0; j<=highest_index_of_control_point_in_v; j++)
//     {
//        WriteDouble ( control_point[i*(highest_index_of_control_point_in_u+1)+j].x )
//        WriteDouble ( control_point[i*(highest_index_of_control_point_in_u+1)+j].y )
//        WriteDouble ( control_point[i*(highest_index_of_control_point_in_u+1)+j].z )
//        if (is_rational)
//           WriteDouble ( control_point[i*(highest_index_of_control_point_in_u+1)+j].w )
//     }
//  }
   for (i=0; i<=highest_index_of_knots_in_u; i++)
      WriteDouble ( knot_u[i] )
   for (i=0; i<=highest_index_of_knots_in_v; i++)
      WriteDouble ( knot_v[i] )
   WriteUnsignedInteger ( knot_type )
   WriteUnsignedInteger ( surface_form )
}

void  PRC_CompressedFace::serializeCompressedFace(PRC_BitStream &pbs, double brep_data_compressed_tolerance)
{
   serializeCompressedAnaNurbs( pbs, brep_data_compressed_tolerance );
}

void  PRC_CompressedFace::serializeContentCompressedFace(PRC_BitStream &pbs)
{
   WriteBoolean ( orientation_surface_with_shell )
   const bool surface_is_trimmed = false;
   WriteBoolean ( surface_is_trimmed )
}

void  PRC_CompressedFace::serializeCompressedAnaNurbs(PRC_BitStream &pbs, double brep_data_compressed_tolerance)
{
   // WriteCompressedEntityType ( PRC_HCG_AnaNurbs )
   const bool is_a_curve = false;
   WriteBoolean ( is_a_curve ) 
   WriteUnsignedIntegerWithVariableBitNumber (13 , 4)
   serializeContentCompressedFace( pbs );
   serializeCompressedNurbs( pbs, brep_data_compressed_tolerance );
}

void  PRC_CompressedFace::serializeCompressedNurbs(PRC_BitStream &pbs, double brep_data_compressed_tolerance)
{
   const double nurbs_tolerance = 0.2*brep_data_compressed_tolerance;
   const MLuint32 degree_in_u = degree;
   const MLuint32 degree_in_v = degree;
   
   WriteUnsignedIntegerWithVariableBitNumber ( degree_in_u, 5)
   WriteUnsignedIntegerWithVariableBitNumber ( degree_in_v, 5)

   const MLuint32 number_of_knots_in_u = 4; // 0011 or 00001111 knot vector - just 2 spans
   WriteUnsignedIntegerWithVariableBitNumber (number_of_knots_in_u - 2, 16)
   MLuint32 number_bit = degree_in_u ? Log2( degree_in_u + 2 ) : 2;
   WriteBoolean (false) // Multiplicity_is_already_stored - no
   WriteUnsignedIntegerWithVariableBitNumber( degree_in_u+1,number_bit)
   WriteBoolean (true) // Multiplicity_is_already_stored - yes
   const MLuint32 number_of_knots_in_v = 4; // 0011 or 00001111 knot vector - just 2 spans
   WriteUnsignedIntegerWithVariableBitNumber (number_of_knots_in_v - 2, 16)
   number_bit = degree_in_v ? Log2( degree_in_v + 2 ) : 2;
   WriteBoolean (false) // Multiplicity_is_already_stored - no
   WriteUnsignedIntegerWithVariableBitNumber( degree_in_v+1,number_bit)
   WriteBoolean (true) // Multiplicity_is_already_stored - yes
  
   const bool is_closed_u = false; 
   WriteBoolean ( is_closed_u )
   const bool is_closed_v = false; 
   WriteBoolean ( is_closed_v )

   const MLuint32 number_of_control_point_in_u = degree_in_u + 1;
   const MLuint32 number_of_control_point_in_v = degree_in_v + 1;

#if defined(__GNUC__) && !defined(__clang__)
   PRC_Vector3D P[number_of_control_point_in_u][number_of_control_point_in_v];
#else
   vector<vector<PRC_Vector3D> > P(number_of_control_point_in_u, vector<PRC_Vector3D>(number_of_control_point_in_v));
#endif
   for(MLuint32 i=0;i<number_of_control_point_in_u;i++)
   for(MLuint32 j=0;j<number_of_control_point_in_v;j++)
      P[i][j] = control_point[i*number_of_control_point_in_v+j];
#ifdef __GNUC__
   MLint32_Triple compressed_control_point[number_of_control_point_in_u][number_of_control_point_in_v];
   MLuint32 control_point_type[number_of_control_point_in_u][number_of_control_point_in_v];
#else
   vector<vector<MLint32_Triple> > compressed_control_point(number_of_control_point_in_u, vector<MLint32_Triple>(number_of_control_point_in_v));
   vector<vector<MLuint32> > control_point_type(number_of_control_point_in_u, vector<MLuint32>(number_of_control_point_in_v));
#endif

   MLuint32 number_of_bits_for_isomin = 1;
   MLuint32 number_of_bits_for_rest = 1;
   
   for(MLuint32 j = 1; j < number_of_control_point_in_v; j++)
   {
      compressed_control_point[0][j] = iroundto(P[0][j]-P[0][j-1], nurbs_tolerance );
      P[0][j] = P[0][j-1] + roundto(P[0][j]-P[0][j-1], nurbs_tolerance);
      MLuint32 bit_size = GetNumberOfBitsUsedToStoreTripleInteger(compressed_control_point[0][j]);
      if (bit_size > number_of_bits_for_isomin)
        number_of_bits_for_isomin = bit_size;
   }

   for(MLuint32 i = 1; i < number_of_control_point_in_u; i++)
   {
      compressed_control_point[i][0] = iroundto(P[i][0]-P[i-1][0], nurbs_tolerance );
      P[i][0] = P[i-1][0] + roundto(P[i][0]-P[i-1][0], nurbs_tolerance);
      MLuint32 bit_size = GetNumberOfBitsUsedToStoreTripleInteger(compressed_control_point[i][0]);
      if (bit_size > number_of_bits_for_isomin)
        number_of_bits_for_isomin = bit_size;
   }

   for(MLuint32 i=1;i<number_of_control_point_in_u;i++)
   for(MLuint32 j=1;j<number_of_control_point_in_v;j++)
   {
     compressed_control_point[i][j].x = 0;
     compressed_control_point[i][j].y = 0;
     compressed_control_point[i][j].z = 0;
     
     PRC_Vector3D V = P[i-1][j] - P[i-1][j-1];
     PRC_Vector3D U = P[i][j-1] - P[i-1][j-1];
     PRC_Vector3D Pc = P[i][j] - (P[i-1][j-1] + U + V);

     if(Pc.Length() < nurbs_tolerance)
     {
       control_point_type[i][j] = 0;
       P[i][j] = P[i-1][j-1] + U + V;
     }
     else 
     {
       PRC_Vector3D N = U*V;
       PRC_Vector3D Ue = U;
       PRC_Vector3D Ne = N;
       if( V.Length() < FLT_EPSILON || !Ue.Normalize() || !Ne.Normalize())
       {
          control_point_type[i][j] = 3;
       // Pc = roundto(Pc, nurbs_tolerance); // not sure if this rounding really happens, need to experiment, docs imply but do not state
          compressed_control_point[i][j] = iroundto(Pc, nurbs_tolerance);
          P[i][j] = P[i-1][j-1] + U + V + roundto(Pc, nurbs_tolerance); // see above
       }
       else
       {
         PRC_Vector3D NUe = Ne*Ue;
         double x = Pc.Dot(Ue);
         double y = Pc.Dot(NUe);
         double z = Pc.Dot(Ne);

         if(x*x+y*y<nurbs_tolerance*nurbs_tolerance)
         {
           control_point_type[i][j] = 1;
           compressed_control_point[i][j] = iroundto(PRC_Vector3D(0.0,0.0,z), nurbs_tolerance);
           P[i][j] = P[i-1][j-1] + U + V + roundto(z, nurbs_tolerance)*Ne; // see above
         }
         else
         {
           if(fabs(z)<nurbs_tolerance/2)
           {
             control_point_type[i][j] = 2;
             compressed_control_point[i][j] = iroundto(PRC_Vector3D(x,y,0), nurbs_tolerance);
             P[i][j] = P[i-1][j-1] + U + V + roundto(x, nurbs_tolerance)*Ue + roundto(y, nurbs_tolerance)*NUe; // see above
           }
           else
           {
             control_point_type[i][j] = 3;
             compressed_control_point[i][j] = iroundto(Pc, nurbs_tolerance);
             P[i][j] = P[i-1][j-1] + U + V + roundto(Pc, nurbs_tolerance); // see above
           }
         }
       }
     }
     MLuint32 bit_size = GetNumberOfBitsUsedToStoreTripleInteger(compressed_control_point[i][j]);
     if (bit_size > number_of_bits_for_rest)
       number_of_bits_for_rest = bit_size;
   }

   if( number_of_bits_for_rest == 2 ) number_of_bits_for_rest--; // really I think it must be unconditional, but so it seems to be done in Adobe Acrobat (9.3)
   WriteUnsignedIntegerWithVariableBitNumber ( number_of_bits_for_isomin, 20 )
   WriteUnsignedIntegerWithVariableBitNumber ( number_of_bits_for_rest,   20 )
   WriteDouble ( P[0][0].x )
   WriteDouble ( P[0][0].y )
   WriteDouble ( P[0][0].z )
   
   for(MLuint32 j = 1; j < number_of_control_point_in_v; j++)
   {
      WriteIntegerWithVariableBitNumber(compressed_control_point[0][j].x, number_of_bits_for_isomin+1)
      WriteIntegerWithVariableBitNumber(compressed_control_point[0][j].y, number_of_bits_for_isomin+1)
      WriteIntegerWithVariableBitNumber(compressed_control_point[0][j].z, number_of_bits_for_isomin+1)
   }
   
   for(MLuint32 i = 1; i < number_of_control_point_in_u; i++)
   {
      WriteIntegerWithVariableBitNumber(compressed_control_point[i][0].x, number_of_bits_for_isomin+1)
      WriteIntegerWithVariableBitNumber(compressed_control_point[i][0].y, number_of_bits_for_isomin+1)
      WriteIntegerWithVariableBitNumber(compressed_control_point[i][0].z, number_of_bits_for_isomin+1)
   }
   
   for(MLuint32 i = 1; i < number_of_control_point_in_u; i++)
   {
      for(MLuint32 j = 1; j < number_of_control_point_in_v; j++)
      {
         WriteUnsignedIntegerWithVariableBitNumber ( control_point_type[i][j], 2 )
         
         if(control_point_type[i][j] == 1)
         {
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].z, number_of_bits_for_rest+1 )
         }
         else if(control_point_type[i][j] == 2)
         {
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].x, number_of_bits_for_rest+1 )
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].y, number_of_bits_for_rest+1 )
         }
         else if(control_point_type[i][j] == 3)
         {
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].x, number_of_bits_for_rest+1 )
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].y, number_of_bits_for_rest+1 )
            WriteIntegerWithVariableBitNumber ( compressed_control_point[i][j].z, number_of_bits_for_rest+1 )
         }
      }
   }
   
   const MLuint32 type_param_u = 0;
   WriteBoolean( type_param_u == 0 )
   const MLuint32 type_param_v = 0;
   WriteBoolean( type_param_v == 0 )
   const bool is_rational = false;
   WriteBoolean( is_rational )
}

void PRC_CompressedBoundaryRepresentationData::serializeCompressedShell(PRC_BitStream &pbs)
{
   MLuint32 i;
   const MLuint32 number_of_face = (MLuint32)face.size();
   WriteBoolean ( number_of_face == 1 )

   if( number_of_face != 1 )
      WriteNumberOfBitsThenUnsignedInteger (number_of_face)
   
   for( i=0; i < number_of_face; i++)
         SerializeCompressedFace ( face[i] )
   
   const bool is_an_iso_face = false;
   for( i=0; i < number_of_face; i++)
      WriteBoolean ( is_an_iso_face )
}

void PRC_CompressedBoundaryRepresentationData::serializeCompressedBrepData(PRC_BitStream &pbs)
{
   WriteUnsignedInteger ( PRC_TYPE_TOPO_BrepDataCompress )
   SerializeContentBody
   
   WriteDouble ( brep_data_compressed_tolerance )
   const MLuint32 number_of_bits_to_store_reference = 1;
   WriteNumberOfBitsThenUnsignedInteger ( number_of_bits_to_store_reference )
   const MLuint32 number_vertex_iso = 0;
   WriteUnsignedIntegerWithVariableBitNumber ( number_vertex_iso, number_of_bits_to_store_reference )
   const MLuint32 number_edge_iso = 0;
   WriteUnsignedIntegerWithVariableBitNumber ( number_edge_iso, number_of_bits_to_store_reference )
   
   const MLuint32 number_of_shell = 1;
   const MLuint32 number_of_connex = 1;
   WriteBoolean ( number_of_shell == 1 && number_of_connex == 1 )
   serializeCompressedShell( pbs );

   MLuint32 i;
   const MLuint32 number_of_faces = (MLuint32)face.size();
   for(i=0; i< number_of_faces; i++)
      face[i]->serializeBaseTopology( pbs );
}

void  PRC_Blend01::serializeBlend01(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Blend01) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   SerializePtrCurve ( center_curve ) 
   SerializePtrCurve ( origin_curve ) 
   SerializePtrCurve ( tangent_curve ) 
}

void  PRC_Ruled::serializeRuled(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Ruled) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   SerializePtrCurve ( first_curve ) 
   SerializePtrCurve ( second_curve ) 
}

void  PRC_Sphere::serializeSphere(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Sphere) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   WriteDouble ( radius )
}

void  PRC_Cone::serializeCone(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Cone) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   WriteDouble ( bottom_radius )
   WriteDouble ( semi_angle )
}

void  PRC_Cylinder::serializeCylinder(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Cylinder) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   WriteDouble ( radius )
}

void  PRC_Torus::serializeTorus(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_SURF_Torus) 

   SerializeContentSurface
   SerializeTransformation
   SerializeUVParameterization
   WriteDouble ( major_radius )
   WriteDouble ( minor_radius )
}

void PRC_Face::serializeFace(PRC_BitStream &pbs)
{ 
   MLuint32 i = 0;
   WriteUnsignedInteger (PRC_TYPE_TOPO_Face) 

   SerializeBaseTopology
   SerializePtrSurface ( base_surface )
   WriteBit ( have_surface_trim_domain )
   if ( have_surface_trim_domain )
      SerializeDomain ( surface_trim_domain ) 
   WriteBit ( have_tolerance )
   if ( have_tolerance )
      WriteDouble ( tolerance ) 
   WriteUnsignedInteger ( number_of_loop )
   WriteInteger ( outer_loop_index )
   for (i=0;i<number_of_loop;i++)
   {
//    SerializePtrTopology ( loop[i] )
   }
}

void PRC_Shell::serializeShell(PRC_BitStream &pbs)
{ 
   MLuint32 i = 0;
   WriteUnsignedInteger (PRC_TYPE_TOPO_Shell) 

   SerializeBaseTopology
   WriteBoolean ( shell_is_closed )
   MLuint32 number_of_face = (MLuint32)face.size();
   WriteUnsignedInteger ( number_of_face ) 
   for (i=0;i<number_of_face;i++)
   {
      SerializePtrTopology ( face[i] )
      WriteCharacter ( orientation_surface_with_shell[i] )
   }
}

void PRC_Shell::addFace(PRC_Face*& pFace, MLuint8 orientation)
{
  face.push_back(pFace);
  pFace = NULL;
  orientation_surface_with_shell.push_back(orientation);
}

void PRC_Connex::serializeConnex(PRC_BitStream &pbs)
{ 
   MLuint32 i = 0;
   WriteUnsignedInteger (PRC_TYPE_TOPO_Connex) 

   SerializeBaseTopology
   MLuint32 number_of_shell = (MLuint32)shell.size();
   WriteUnsignedInteger ( number_of_shell ) 
   for (i=0;i<number_of_shell;i++)
   {
      SerializePtrTopology ( shell[i] )
   }
}

void PRC_Connex::addShell(PRC_Shell*& pShell)
{
  shell.push_back(pShell);
  pShell = NULL;
}

#define have_bbox( behavior ) (behavior!=0)
void PRC_BoundaryRepresentationData::serializeBrepData(PRC_BitStream &pbs)
{
   MLuint32 i = 0;
   WriteUnsignedInteger ( PRC_TYPE_TOPO_BrepData) 

   SerializeContentBody 
   MLuint32 number_of_connex = (MLuint32)connex.size();
   WriteUnsignedInteger ( number_of_connex ) 
   for ( i=0; i<number_of_connex; i++)
   {
      SerializePtrTopology ( connex[i] )
   }
   if ( have_bbox(behavior) )
      SerializeBoundingBox
}
#undef have_bbox

void PRC_BoundaryRepresentationData::addConnex(PRC_Connex*& pConnex)
{
  connex.push_back(pConnex);
  pConnex = NULL;
}

void PRC_WireEdgeContent::serializeContentWireEdge(PRC_BitStream &pbs)
{
   SerializeBaseTopology
   SerializePtrCurve ( curve_3d )
   WriteBit ( has_curve_trim_interval )
   if ( has_curve_trim_interval )
      SerializeInterval ( curve_trim_interval )
}

void PRC_WireEdge::serializeWireEdge(PRC_BitStream &pbs)
{
   WriteUnsignedInteger (PRC_TYPE_TOPO_WireEdge) 
   SerializeContentWireEdge 
}

void PRC_ContentCurve::serializeContentCurve(PRC_BitStream &pbs)
{
   SerializeBaseGeometry
   WriteUnsignedInteger ( extend_info )
   WriteBoolean ( is_3d )
}

void  PRC_NURBSCurve::serializeNURBSCurve(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
   WriteUnsignedInteger (PRC_TYPE_CRV_NURBS) 

   SerializeContentCurve 
   WriteBoolean ( is_rational )
   WriteUnsignedInteger ( degree )
   MLuint32 highest_index_of_control_point = (MLuint32)(control_point.size()-1);
   MLuint32 highest_index_of_knots = (MLuint32)(knot.size()-1);
   WriteUnsignedInteger ( highest_index_of_control_point )
   WriteUnsignedInteger ( highest_index_of_knots )
   for (i=0; i<=highest_index_of_control_point; i++)
   {
      WriteDouble ( control_point[i].x )
      WriteDouble ( control_point[i].y )
      if (is_3d)
         WriteDouble ( control_point[i].z )
      if (is_rational)
         WriteDouble ( control_point[i].w )
   }
   for (i=0; i<=highest_index_of_knots; i++)
      WriteDouble ( knot[i] )
   WriteUnsignedInteger ( knot_type )
   WriteUnsignedInteger ( curve_form )
}

void  PRC_PolyLine::serializePolyLine(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
   WriteUnsignedInteger (PRC_TYPE_CRV_PolyLine) 

   SerializeContentCurve 
   SerializeTransformation
   SerializeParameterization
   MLuint32 number_of_point = (MLuint32)point.size();
   WriteUnsignedInteger ( number_of_point ) 
   for (i=0; i<number_of_point; i++) 
   {
      if (is_3d)
         SerializeVector3d ( point[i] )
      else
         SerializeVector2d ( point[i] )
   }
}

void  PRC_Circle::serializeCircle(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_CRV_Circle) 

   SerializeContentCurve 
   SerializeTransformation
   SerializeParameterization
   WriteDouble ( radius )
}

void  PRC_Composite::serializeComposite(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
   WriteUnsignedInteger (PRC_TYPE_CRV_Composite) 

   SerializeContentCurve 
   SerializeTransformation
   SerializeParameterization
   MLuint32 number_of_curves = (MLuint32)base_curve.size();
   WriteUnsignedInteger ( number_of_curves ) 
   for (i=0; i<number_of_curves; i++) 
   {
      SerializePtrCurve ( base_curve[i] )
      WriteBoolean ( base_sense[i] )
   }
   WriteBoolean ( is_closed )
}

void PRC_TopologyContext::serializeTopoContext(PRC_BitStream &pbs)
{ 
   WriteUnsignedInteger (PRC_TYPE_TOPO_Context) 

   SerializeContentPRCBase
   WriteCharacter ( behaviour )
   WriteDouble ( granularity )
   WriteDouble ( tolerance )
   WriteBoolean ( have_smallest_face_thickness )
   if ( have_smallest_face_thickness )
      WriteDouble ( smallest_thickness )
   WriteBoolean ( have_scale )
   if ( have_scale )
      WriteDouble ( scale )
}

void PRC_TopologyContext::serializeContextAndBodies(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
   SerializeTopoContext 
   MLuint32 number_of_bodies = (MLuint32)body.size();
   WriteUnsignedInteger (number_of_bodies) 
   for (i=0;i<number_of_bodies;i++) 
      SerializeBody (body[i]) 
}

void PRC_TopologyContext::serializeGeometrySummary(PRC_BitStream &pbs)
{ 
   MLuint32 i=0;
   MLuint32 number_of_bodies = (MLuint32)body.size();
   WriteUnsignedInteger (number_of_bodies) 
   for (i=0;i<number_of_bodies;i++) 
   {
      WriteUnsignedInteger ( body[i]->serialType() ) 
      if ( isCompressedType(body[i]->serialType()) )
      {
         WriteDouble ( body[i]->serialTolerance() ) 
      }
   }
}

void PRC_TopologyContext::serializeContextGraphics(PRC_BitStream &pbs)
{ 
   MLuint32 i=0, j=0, k=0, l=0;
   ResetCurrentGraphics
   MLuint32 number_of_body = (MLuint32)body.size();
   PRC_GraphicsList element;
   bool has_graphics = false;
   for (i=0;i<number_of_body;i++)
   {
        if ( body[i]->topo_item_type == PRC_TYPE_TOPO_BrepData && dynamic_cast<PRC_BoundaryRepresentationData*>(body[i]))
        {
                PRC_BoundaryRepresentationData *body_i = dynamic_cast<PRC_BoundaryRepresentationData*>(body[i]);
                for (j=0;j<body_i->connex.size();j++)
                {
                        for(k=0;k<body_i->connex[j]->shell.size();k++)
                        {
                                for( l=0;l<body_i->connex[j]->shell[k]->face.size();l++)
                                {
                                        element.push_back( body_i->connex[j]->shell[k]->face[l] );
                                        has_graphics = has_graphics || body_i->connex[j]->shell[k]->face[l]->has_graphics();
                                }
                        }
                }
        }
        else if ( body[i]->topo_item_type == PRC_TYPE_TOPO_BrepDataCompress && dynamic_cast<PRC_CompressedBoundaryRepresentationData*>(body[i]))
        {
                PRC_CompressedBoundaryRepresentationData *body_i = dynamic_cast<PRC_CompressedBoundaryRepresentationData*>(body[i]);
               for( l=0;l<body_i->face.size();l++)
               {
                 element.push_back( body_i->face[l] );
                 has_graphics = has_graphics || body_i->face[l]->has_graphics();
               }
        }
   }
   MLuint32 number_of_treat_type = 0;
   if (has_graphics && !element.empty())
     number_of_treat_type = 1;
   WriteUnsignedInteger (number_of_treat_type) 
   for (i=0;i<number_of_treat_type;i++) 
   {
      const MLuint32 element_type = PRC_TYPE_TOPO_Face;
      WriteUnsignedInteger (element_type) 
      const MLuint32 number_of_element = (MLuint32)element.size();
      WriteUnsignedInteger (number_of_element) 
      for (j=0;j<number_of_element;j++) 
      {
         WriteBoolean ( element[j]->has_graphics() ) 
         if (element[j]->has_graphics()) 
         {
            element[j]->serializeGraphics(pbs);
         }
      }
   }
}

MLuint32 PRC_TopologyContext::addSingleWireBody(PRC_SingleWireBody*& pSingleWireBody)
{
  body.push_back(pSingleWireBody);
  pSingleWireBody = NULL;
  return (MLuint32)(body.size()-1);
}

MLuint32 PRC_TopologyContext::addBrepData(PRC_BoundaryRepresentationData*& pBrepData)
{
  body.push_back(pBrepData);
  pBrepData = NULL;
  return (MLuint32)(body.size()-1);
}

MLuint32 PRC_TopologyContext::addCompressedBrepData(PRC_CompressedBoundaryRepresentationData*& pCompressedBrepData)
{
  body.push_back(pCompressedBrepData);
  pCompressedBrepData = NULL;
  return (MLuint32)(body.size()-1);
}

void PRC_SingleWireBody::serializeSingleWireBody(PRC_BitStream &pbs)
{
  WriteUnsignedInteger ( PRC_TYPE_TOPO_SingleWireBody) 

  SerializeContentBody 
  SerializePtrTopology ( wire_edge )
}

void PRC_UniqueID::serializeCompressedUniqueId(PRC_BitStream &pbs) const
{
   WriteUnsignedInteger (id0) 
   WriteUnsignedInteger (id1) 
   WriteUnsignedInteger (id2) 
   WriteUnsignedInteger (id3)   
}

void PRC_UniqueID::serializeFileStructureUncompressedUniqueId(std::ostream& out) const
{
   WriteUncompressedUnsignedInteger (id0) 
   WriteUncompressedUnsignedInteger (id1) 
   WriteUncompressedUnsignedInteger (id2) 
   WriteUncompressedUnsignedInteger (id3) 
}

void PRC_Unit::serializeUnit(PRC_BitStream &pbs)
{
   WriteBoolean (unit_from_CAD_file)
   WriteDouble (unit)
}

void PRC_ProductOccurrence::serializeProductOccurrence(PRC_BitStream &pbs)
{
   WriteUnsignedInteger ( PRC_TYPE_ASM_ProductOccurence ) 

   SerializePRCBaseWithGraphics 

   WriteUnsignedInteger (index_part+1)
   WriteUnsignedInteger (index_prototype+1)
   if (index_prototype != MLuint32_MAX)
   {
      WriteBoolean (prototype_in_same_file_structure)
      if (!prototype_in_same_file_structure)
         SerializeCompressedUniqueId (prototype_file_structure)
   }
   WriteUnsignedInteger(index_external_data+1)
   if (index_external_data != MLuint32_MAX)
   {
      WriteBoolean (external_data_in_same_file_structure)
      if (!external_data_in_same_file_structure)
         SerializeCompressedUniqueId (external_data_file_structure)
   }
   const MLuint32 number_of_son_product_occurrences = (MLuint32)index_son_occurrence.size();
   WriteUnsignedInteger (number_of_son_product_occurrences)
   for (MLuint32 i=0;i<number_of_son_product_occurrences;i++)
      WriteUnsignedInteger (index_son_occurrence[i])

   WriteCharacter (product_behaviour)

// SerializeProductInformation (product_information)
   SerializeUnit (unit_information)
   WriteCharacter (product_information_flags)
   WriteInteger (product_load_status)

   const bool has_location = location != NULL;
   WriteBit (has_location)
   if (has_location)
     location->serializeTransformation3d (pbs);
   
   WriteUnsignedInteger (0) // number_of_references
   
// SerializeMarkups (markups)
   WriteUnsignedInteger (0) // number_of_linked_items 
   WriteUnsignedInteger (0) // number_of_leaders 
   WriteUnsignedInteger (0) // number_of_markups 
   WriteUnsignedInteger (0) // number_of_annotation_entities 

   
   WriteUnsignedInteger (0) // number_of_views
   WriteBit (false) // has_entity_filter
   WriteUnsignedInteger (0) // number_of_display_filters
   WriteUnsignedInteger (0) // number_of_scene_display_parameters

   SerializeUserData
}

MLuint32 PRC_PartDefinition::addBrepModel(PRC_BoundaryRepresentationModel*& pBrepModel)
{
  representation_item.push_back(pBrepModel);
  pBrepModel = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addPolyBrepModel(PRC_PolyBoundaryRepresentationModel*& pPolyBrepModel)
{
  representation_item.push_back(pPolyBrepModel);
  pPolyBrepModel = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addPointSet(PRC_PointSet*& pPointSet)
{
  representation_item.push_back(pPointSet);
  pPointSet = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addSet(PRC_Set*& pSet)
{
  representation_item.push_back(pSet);
  pSet = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addWire(PRC_Wire*& pWire)
{
  representation_item.push_back(pWire);
  pWire = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addPolyWire(PRC_PolyWire*& pPolyWire)
{
  representation_item.push_back(pPolyWire);
  pPolyWire = NULL;
  return (MLuint32)(representation_item.size()-1);
}

MLuint32 PRC_PartDefinition::addRepresentationItem(PRC_RepresentationItem*& pRepresentationItem)
{
  representation_item.push_back(pRepresentationItem);
  pRepresentationItem = NULL;
  return (MLuint32)(representation_item.size()-1);
}

void PRC_PartDefinition::serializePartDefinition(PRC_BitStream &pbs)
{
  WriteUnsignedInteger ( PRC_TYPE_ASM_PartDefinition ) 
  
  SerializePRCBaseWithGraphics
  SerializeBoundingBox

  MLuint32 number_of_representation_items = (MLuint32)representation_item.size();
  WriteUnsignedInteger (number_of_representation_items)
  for (MLuint32 i=0;i<number_of_representation_items;i++)
    SerializeRepresentationItem (representation_item[i])
  
  // SerializeMarkups (markups)
  WriteUnsignedInteger (0) // number_of_linked_items 
  WriteUnsignedInteger (0) // number_of_leaders 
  WriteUnsignedInteger (0) // number_of_markups 
  WriteUnsignedInteger (0) // number_of_annotation_entities 

  WriteUnsignedInteger (0) // number_of_views
  SerializeUserData
}  


ML_END_NAMESPACE

