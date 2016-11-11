//----------------------------------------------------------------------------------
// This header files contains some U3D constants as specified in 
// Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) 
// (referred below as "the Standard") plus some other struct and type definitions
//
// \file    MLU3D_Constants.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Constants_H
#define _U3D_Constants_H

// ML includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  const std::string USEVERTEXCOLORS = "{VertexColors}";

  const std::string GEOMETRYPREFIX_POINTSET = "PointSetGeometry";
  const std::string GEOMETRYPREFIX_LINESET  = "LineSetGeometry";
  const std::string GEOMETRYPREFIX_MESH     = "MeshGeometry";

  typedef MLuint32 DataBlockFundamental;
  typedef std::vector<DataBlockFundamental> DataVector;

  const MLint32 DATASIZEINCREMENT = 0x0000000F;

  //////////////////////////////////////////////////////////////////////////
  // 
  // Predefined values
  //

  // Padding value (section 9.2.5 of the Standard)
  const char PADDINGVALUE = 0x00;

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D file block types
  //

  // File structure blocks (section 9.4 of the Standard)
  const MLuint32 BLOCKTYPE_FILEHEADER     = 0x00443355;  // 9.4.1
  const MLuint32 BLOCKTYPE_FILEREFERENCE  = 0xFFFFFF12;  // 9.4.2
  const MLuint32 BLOCKTYPE_MODIFIERCHAIN  = 0xFFFFFF14; // 9.4.3
  const MLuint32 BLOCKTYPE_PRIORITYUPDATE = 0xFFFFFF15;  // 9.4.4
  const MLuint32 BLOCKTYPE_NEWOBJECTTYPE  = 0xFFFFFF16;  // 9.4.5

  // Node blocks (section 9.5 of the Standard)
  const MLuint32 BLOCKTYPE_GROUPNODE = 0xFFFFFF21;  // 9.5.1
  const MLuint32 BLOCKTYPE_MODELNODE = 0xFFFFFF22;  // 9.5.2
  const MLuint32 BLOCKTYPE_LIGHTNODE = 0xFFFFFF23;  // 9.5.3
  const MLuint32 BLOCKTYPE_VIEWNODE  = 0xFFFFFF24;  // 9.5.4

  // Geometry generator blocks (section 9.6 of the Standard)
  const MLuint32  BLOCKTYPE_CLODMESHDECLARATION             = 0xFFFFFF31;  // 9.6.1.1
  const MLuint32  BLOCKTYPE_CLODBASEMESHCONTINUATION        = 0xFFFFFF3B;  // 9.6.1.2
  const MLuint32  BLOCKTYPE_CLODPROGRESSIVEMESHCONTINUATION = 0xFFFFFF3C;  // 9.6.1.3
  const MLuint32  BLOCKTYPE_POINTSETDECLARATION             = 0xFFFFFF36;  // 9.6.2.1
  const MLuint32  BLOCKTYPE_POINTSETCONTINUATION            = 0xFFFFFF3E;  // 9.6.2.2
  const MLuint32  BLOCKTYPE_LINESETDECLARATION              = 0xFFFFFF37;  // 9.6.3.1
  const MLuint32  BLOCKTYPE_LINESETCONTINUATION             = 0xFFFFFF3F;  // 9.6.3.2

  // Modifier blocks (section 9.7 of the Standard)
  const MLuint32  BLOCKTYPE_2DGLYPHMODIFIER     = 0xFFFFFF41;  // 9.7.1
  const MLuint32  BLOCKTYPE_SUBDIVISIONMODIFIER = 0xFFFFFF42;  // 9.7.2
  const MLuint32  BLOCKTYPE_ANIMATIONMODIFIER   = 0xFFFFFF43;  // 9.7.3
  const MLuint32  BLOCKTYPE_BONEWEIGHTMODIFIER  = 0xFFFFFF44;  // 9.7.4
  const MLuint32  BLOCKTYPE_SHADINGMODIFIER     = 0xFFFFFF45;  // 9.7.5
  const MLuint32  BLOCKTYPE_CLODMODIFIER        = 0xFFFFFF46;  // 9.7.6

  // Ressource blocks (section 9.8 of the Standard)
  const MLuint32  BLOCKTYPE_LIGHTRESOURCE       = 0xFFFFFF51;  // 9.8.1
  const MLuint32  BLOCKTYPE_VIEWRESOURCE        = 0xFFFFFF52;  // 9.8.2
  const MLuint32  BLOCKTYPE_LITTEXTURESHADER    = 0xFFFFFF53;  // 9.8.3
  const MLuint32  BLOCKTYPE_MATERIALRESOURCE    = 0xFFFFFF54;  // 9.8.4
  const MLuint32  BLOCKTYPE_TEXTUREDECLARATION  = 0xFFFFFF55;  // 9.8.5.1
  const MLuint32  BLOCKTYPE_TEXTURECONTINUATIOM = 0xFFFFFF5C;  // 9.8.5.2
  const MLuint32  BLOCKTYPE_MOTIONRESOURCE      = 0xFFFFFF56;  // 9.8.6

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D file header values
  //

  // Profile identifier flags (section 9.4.1.2 of the Standard)
  const MLuint32  PROFILEIDENTIFIER_BASEPROFILE       = 0x00000000;
  const MLuint32  PROFILEIDENTIFIER_EXTENSIBLEPROFILE = 0x00000002;
  const MLuint32  PROFILEIDENTIFIER_NOCOMPRESSIONMODE = 0x00000004;
  const MLuint32  PROFILEIDENTIFIER_DEFINEDUNITS      = 0x00000008;

  const MLuint32  PROFILEIDENTIFIER_ALL               = 0x0000000E;


  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D modifier chain values
  //

  // Modifier chain type (section 9.4.3.2 of the Standard)
  const MLuint32  MODIFIERCHAINTYPE_NODEMODIFIERCHAIN            = 0x00000000;
  const MLuint32  MODIFIERCHAINTYPE_MODELRESOURCEMODIFIERCHAIN   = 0x00000001;
  const MLuint32  MODIFIERCHAINTYPE_TEXTURERESOURCEMODIFIERCHAIN = 0x00000002;

  // Modifier chain attributes flags (section 9.4.3.3 of the Standard)
  const MLuint32  MODIFIERCHAINATTRIBUTE_BOUNDINGSPHEREINFORMATIONPRESENT = 0x00000001;
  const MLuint32  MODIFIERCHAINATTRIBUTE_AXISALIGNEDBOUNDIGBOXPRESENT     = 0x00000002;

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D view node values
  //

  // View Node attributes (section 9.5.4.4 of the Standard)
  const MLuint32  VIEWNODEATTRIBUTE_DEFAULT                          = 0x00000000;
  const MLuint32  VIEWNODEATTRIBUTE_SCREENPOSITIONUNITS              = 0x00000001;
  const MLuint32  VIEWNODEATTRIBUTE_PROJECTIONMODE_ORTHOGRAPHIC      = 0x00000002;
  const MLuint32  VIEWNODEATTRIBUTE_PROJECTIONMODE_2POINTPERSPECTIVE = 0x00000004;
  const MLuint32  VIEWNODEATTRIBUTE_PROJECTIONMODE_1POINTPERSPECTIVE = VIEWNODEATTRIBUTE_PROJECTIONMODE_ORTHOGRAPHIC | VIEWNODEATTRIBUTE_PROJECTIONMODE_2POINTPERSPECTIVE;

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D model node values
  //

  // Model visibility (section 9.5.2.4 of the Standard)
  const MLuint32  MODELVISIBILITY_NOTVISIBLE          = 0x00000000;
  const MLuint32  MODELVISIBILITY_FRONTVISIBLE        = 0x00000001;
  const MLuint32  MODELVISIBILITY_BACKVISIBLE         = 0x00000002;
  const MLuint32  MODELVISIBILITY_FRONTANDBACKVISIBLE = 0x00000003;

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D shading modifier values
  //

  // Shading attributes flags (section 9.7.5.3 of the Standard)
  const MLuint32  SHADINGMODIFIER_SHADINGATTRIBUTES_MESH  = 0x00000001;
  const MLuint32  SHADINGMODIFIER_SHADINGATTRIBUTES_LINE  = 0x00000002;
  const MLuint32  SHADINGMODIFIER_SHADINGATTRIBUTES_POINT = 0x00000004;
  const MLuint32  SHADINGMODIFIER_SHADINGATTRIBUTES_GLYPH = 0x00000008;

  const MLuint32  SHADINGMODIFIER_SHADINGATTRIBUTES_ALL   = 0x0000000F;  // Just for convenience: all attributes ORed together

  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D CLOD Mesh Declaration values
  //

  // Mesh Attributes flags (section 9.6.1.1.3.1 of the Standard)
  const MLuint32  MESH_ATTRIBUTES_DEFAULT        = 0x00000000;
  const MLuint32  MESH_ATTRIBUTES_EXCLUDENORMALS = 0x00000001;

  // Shading Attributes flags (section 9.6.1.1.3.9.1 of the Standard)
  const MLuint32  SHADINGATTRIBUTES_NONE                  = 0x00000000;
  const MLuint32  SHADINGATTRIBUTES_DIFFUSECOLORS         = 0x00000001;
  const MLuint32  SHADINGATTRIBUTES_SPECULARCOLORS        = 0x00000002;
  const MLuint32  SHADINGATTRIBUTES_DIFFUSESPECULARCOLORS = 0x00000003; // Just for convenience: all attributes ORed together 


  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D Lit Texture Shader values
  //

  // Lit Texture Shader Attributes flags (section 9.8.3.2 of the Standard)
  const MLuint32  LITTEXTURESHADERATTRIBUTES_NONE             = 0x00000000;
  const MLuint32  LITTEXTURESHADERATTRIBUTES_LIGHTINGENABLED  = 0x00000001;
  const MLuint32  LITTEXTURESHADERATTRIBUTES_ALPHATESTENABLED = 0x00000002;
  const MLuint32  LITTEXTURESHADERATTRIBUTES_USEVERTEXCOLOR   = 0x00000004;
  const MLuint32  LITTEXTURESHADERATTRIBUTES_ALL              = 0x00000007;

  // Alpha Test Function values (section 9.8.3.4 of the Standard)
  const MLuint32  ALPHATESTFUCTION_NEVER     = 0x00000610;
  const MLuint32  ALPHATESTFUCTION_LESS      = 0x00000611;
  const MLuint32  ALPHATESTFUCTION_GREATER   = 0x00000612;
  const MLuint32  ALPHATESTFUCTION_EQUAL     = 0x00000613;
  const MLuint32  ALPHATESTFUCTION_NOT_EQUAL = 0x00000614;
  const MLuint32  ALPHATESTFUCTION_LEQUAL    = 0x00000615;
  const MLuint32  ALPHATESTFUCTION_GEQUAL    = 0x00000616;
  const MLuint32  ALPHATESTFUCTION_ALWAYS    = 0x00000617;

  // Color Blend Function values (section 9.8.3.5 of the Standard)
  const MLuint32  COLORBLEND_ADD           = 0x00000604;
  const MLuint32  COLORBLEND_MULTIPLY      = 0x00000605;
  const MLuint32  COLORBLEND_ALPHABLEND    = 0x00000606;
  const MLuint32  COLORBLEND_INVALPHABLEND = 0x00000607;


  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D Material Resource values
  //

  // Material Resource Attributes flags (section 9.8.4.2 of the Standard)
  const MLuint32  MATERIALRESOURCEATTRIBUTES_NONE         = 0x00000000;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_AMBIENT      = 0x00000001;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_DIFFUSE      = 0x00000002;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_SPECULAR     = 0x00000004;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_EMISSIVE     = 0x00000008;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_REFLECTIVITY = 0x00000010;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_OPACITY      = 0x00000020;
  const MLuint32  MATERIALRESOURCEATTRIBUTES_ALL          = 0x0000003F;


  //////////////////////////////////////////////////////////////////////////
  // 
  // U3D Light Resource values
  //

  // Light Resource Attributes flags (section 9.8.1.2 of the Standard)
  const MLuint32  LIGHTRESOURCEATTRIBUTES_NONE         = 0x00000000;
  const MLuint32  LIGHTRESOURCEATTRIBUTES_LIGHTENABLED = 0x00000001;
  const MLuint32  LIGHTRESOURCEATTRIBUTES_SPECULAR     = 0x00000002;
  const MLuint32  LIGHTRESOURCEATTRIBUTES_SPOTDECAY    = 0x00000004;
  const MLuint32  LIGHTRESOURCEATTRIBUTES_ALL          = 0x00000007;

  // Light Type values (section 9.8.2.2 of the Standard)
  const MLuint8  LIGHTTYPE_AMBIENT     = 0x00;
  const MLuint8  LIGHTTYPE_DIRECTIONAL = 0x01;
  const MLuint8  LIGHTTYPE_POINT       = 0x02;
  const MLuint8  LIGHTTYPE_SPOT        = 0x03;


  //////////////////////////////////////////////////////////////////////////



  //! Enumeration of the palette types (section 8.2 of the Standard)
  enum PaletteType {
    MODEL_RESOURCE_PALETTE    = 0, // 
    LIGHT_RESOURCE_PALETTE    = 1, // 
    VIEW_RESOURCE_PALETTE     = 2, // 
    TEXTURE_RESOURCE_PALETTE  = 3, // 
    SHADER_RESOURCE_PALETTE   = 4, // 
    MATERIAL_RESOURCE_PALETTE = 5, // 
    MOTION_RESOURCE_PALETTE   = 6, // 
    NODE_PALETTE              = 7  // 
  };

  //////////////////////////////////////////////////////////////////////////

  //! Enumeration of the node types (section 8.3 of the Standard) 
  enum NodeType {
    GROUP_NODE = 0, // Group Node
    MODEL_NODE = 1, // Model Node
    VIEW_NODE  = 2, // View node
    LIGHT_NODE = 3  // Light node
  };

  //////////////////////////////////////////////////////////////////////////


  const MLuint32 BLOCKDECLARATIONSIZE = 12;
  const MLuint32 BLOCKFUNDAMENTALSIZE = sizeof(DataBlockFundamental);


  //////////////////////////////////////////////////////////////////////////

  /// <summary>
  /// A default buffer size for U3D
  /// </summary>
  const MLuint32 SizeBuff = 1024;

  /// <summary>
  /// The initial size allocated for buffers
  /// </summary>
  const MLuint32 DataSizeInitial = 0x00000010;

  //////////////////////////////////////////////////////////////////////////


  //
  // Predefined contexts (based on http://www3.math.tu-berlin.de/jreality/api/constant-values.html)
  // Names used here are the names from the ECMA specification document (with "mlU3D::Context_" prefix)
  // Names in [] are the names used by website
  //

  /// <summary>
  /// The context for uncompressed U8
  /// </summary>
  const MLuint32 Context_Uncompressed8 = 0;

  /// <summary>
  /// Contexts >= StaticFull are static contexts.
  /// </summary>
  const MLuint32 Context_StaticFull = 0x00000400;  // [uACStaticFull]

  /// <summary>
  /// The largest allowable static context. values written to contexts > MaxRange are written as uncompressed.
  /// </summary>
  const MLuint32 MaxRange = Context_StaticFull + 0x00003FFF;

  /// <summary>
  /// The context cZero
  /// </summary>
  const MLuint32 Context_cZero = 0x000003FF;

  /// <summary>
  /// Bit compression context for Number Of New Points/Lines/Faces
  /// </summary>
  const MLuint32 Context_cPointCnt_cLineCnt_cFaceCnt = 0x00000001;  // [uACContextNumNewFaces]

  /// <summary>
  /// Bit compression context for Position Difference Signs (9.6.1.3.4.10.1)
  /// </summary>
  const MLuint32 Context_cPosDiffSign = 0x00000014;  // [uACContextPositionDiffSigns]

  /// <summary>
  /// Bit compression context for Position Difference X (9.6.1.3.4.10.2)
  /// </summary>
  const MLuint32 Context_cPosDiffX = 0x00000015;  // [uACContextPositionDiffMagX]

  /// <summary>
  /// Bit compression context for Position Difference Y (9.6.1.3.4.10.3)
  /// </summary>
  const MLuint32 Context_cPosDiffY = 0x00000016;  // [uACContextPositionDiffMagY]

  /// <summary>
  /// Bit compression context for Position Difference Z (9.6.1.3.4.10.4)
  /// </summary>
  const MLuint32 Context_cPosDiffZ = 0x00000017;  // [uACContextPositionDiffMagZ]

  /// <summary>
  /// Bit compression context for Number Of Normals (9.6.1.3.4.11.1 / 9.6.2.2.4.3 / 9.6.3.2.4.3)  
  /// </summary>
  const MLuint32 Context_cNormlCnt = 0x00000028;  // [uACContextNumLocalNormals]

  /// <summary>
  /// Bit compression context for Position Difference Signs (9.6.1.3.4.10.1)
  /// </summary>
  const MLuint32 Context_cDiffNormalSign = 0x00000029;  // [uACContextNormalDiffSigns]

  /// <summary>
  /// Bit compression context for Position Difference X (9.6.1.3.4.10.2)
  /// </summary>
  const MLuint32 Context_cDiffNormalX = 0x0000002A;  // [uACContextNormalDiffMagX]

  /// <summary>
  /// Bit compression context for Position Difference Y (9.6.1.3.4.10.3)
  /// </summary>
  const MLuint32 Context_cDiffNormalY = 0x0000002B;  // [uACContextNormalDiffMagY]

  /// <summary>
  /// Bit compression context for Position Difference Z (9.6.1.3.4.10.4)
  /// </summary>
  const MLuint32 Context_cDiffNormalZ = 0x0000002C;  // [uACContextNormalDiffMagZ]

  /// <summary>
  /// Bit compression context for Normal Local Index (9.6.3.2.4.6.3)
  /// </summary>
  const MLuint32 Context_cNormlIdx = 0x0000002D;  // [uACContextNormalLocalIndex]

  /// <summary>
  /// Bit compression context for Shading ID (9.6.1.2.4.6.1)
  /// </summary>
  const MLuint32 Context_cShading = 0x00000001;

  /// <summary>
  /// Bit compression context for New Diffuse Color Count (9.6.1.3.4.2.1)
  /// </summary>
  const MLuint32 Context_cDiffuseCount = 0x00000063;

  /// <summary>
  /// Bit compression context for Diffuse Color Difference Signs (9.6.1.3.4.2.2)
  /// </summary>
  const MLuint32 Context_cDiffuseColorSign = 0x00000064;


  //
  //  Inverse Quantization constants for quality level 1000 (always used in this version)
  //

  /// <summary>
  /// Inverse Quantization - Position Quant
  /// </summary>
  const MLfloat Quant_Position = 262144.0f;      // 2^18

  /// <summary>
  /// Inverse Quantization - Normal Quant
  /// </summary>
  const MLfloat Quant_Normal = 16384.0f;         // 2^14

  /// <summary>
  /// Inverse Quantization - Texture Coord Quant
  /// </summary>
  const MLfloat Quant_TextureCoord = 16384.0f;   // 2^14

  /// <summary>
  /// Inverse Quantization - Diffuse Color Quant
  /// </summary>
  const MLfloat Quant_DiffuseColor = 16384.0f;   // 2^14

  /// <summary>
  /// Inverse Quantization - Specular Color Quant
  /// </summary>
  const MLfloat Quant_SpecularColor = 16384.0f;  // 2^14


  //////////////////////////////////////////////////////////////////////////

  //
  //Bit masks for reading and writing symbols.
  //

  /// <summary>
  /// Masks all but the most significant bit
  /// </summary>
  const MLuint32 HalfMask = 0x00008000;

  /// <summary>
  /// Masks the most significant bit
  /// </summary>
  const MLuint32 NotHalfMask = 0x00007FFF;

  /// <summary>
  /// Masks all but the 2nd most significan bit
  /// </summary>
  const MLuint32 QuarterMask = 0x00004000;

  /// <summary>
  /// Masks the 2 most significant bits
  /// </summary>
  const MLuint32 NotThreeQuarterMask = 0x00003FFF;

  /// <summary>
  /// Used to swap 8 bits in place
  /// </summary>
  const MLuint32 Swap8[16] = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 };

  const MLuint32 ReadCount[16] = { 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
  const MLuint32 FastNotMask[5] = { 0xFFFFFFFF, 0x7FFF7FFF, 0x3FFF3FFF, 0x1FFF1FFF, 0x0FFF0FFF };


  //////////////////////////////////////////////////////////////////////////

  //
  // Other constants.
  //

  /// <summary>
  /// Constant for reserved / zero values
  /// </summary>
  const MLuint32 ReservedZero = 0x00000000;

  //////////////////////////////////////////////////////////////////////////

} // end namespace mlU3D

ML_END_NAMESPACE

#endif // _U3D_Constants_H
