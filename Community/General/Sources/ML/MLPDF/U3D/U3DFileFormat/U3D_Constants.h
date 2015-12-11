//----------------------------------------------------------------------------------
//! This header files contains some U3D constants as specified in 
//! Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
//! plus some other struct and type definitions
/*!
// \file    U3D_Constants.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


#ifndef _U3D_Constants_H
#define _U3D_Constants_H

// ML includes
#include "mlModuleIncludes.h"

ML_START_NAMESPACE

typedef MLuint32 U3DDataBlockFundamental;
typedef std::vector< U3DDataBlockFundamental > U3DDataVector;


//////////////////////////////////////////////////////////////////////////
// 
// Predefined values
//

// Padding value (section 9.2.5 of the Standard)
const char U3D_PADDINGVALUE                                  = 0x00;        

//////////////////////////////////////////////////////////////////////////
// 
// U3D file block types
//

// File structure blocks (section 9.4 of the Standard)
#define U3D_BLOCKTYPE_FILEHEADER                       0x00443355  // 9.4.1
#define U3D_BLOCKTYPE_FILEREFERENCE                    0xFFFFFF12  // 9.4.2
#define U3D_BLOCKTYPE_MODIFIERCHAIN                    0xFFFFFF14  // 9.4.3
#define U3D_BLOCKTYPE_PRIORITYUPDATE                   0xFFFFFF15  // 9.4.4
#define U3D_BLOCKTYPE_NEWOBJECTTYPE                    0xFFFFFF16  // 9.4.5

// Node blocks (section 9.5 of the Standard)
#define U3D_BLOCKTYPE_GROUPNODE                        0xFFFFFF21  // 9.5.1
#define U3D_BLOCKTYPE_MODELNODE                        0xFFFFFF22  // 9.5.2
#define U3D_BLOCKTYPE_LIGHTNODE                        0xFFFFFF23  // 9.5.3
#define U3D_BLOCKTYPE_VIEWNODE                         0xFFFFFF24  // 9.5.4

// Geometry generator blocks (section 9.6 of the Standard)
#define U3D_BLOCKTYPE_CLODMESHDECLARATION              0xFFFFFF31  // 9.6.1.1
#define U3D_BLOCKTYPE_CLODBASEMESHCONTINUATION         0xFFFFFF3B  // 9.6.1.2
#define U3D_BLOCKTYPE_CLODPROGRESSIVEMESHCONTINUATION  0xFFFFFF3C  // 9.6.1.3
#define U3D_BLOCKTYPE_POINTSETDECLARATION              0xFFFFFF36  // 9.6.2.1
#define U3D_BLOCKTYPE_POINTSETCONTINUATION             0xFFFFFF3E  // 9.6.2.2
#define U3D_BLOCKTYPE_LINESETDECLARATION               0xFFFFFF37  // 9.6.3.1
#define U3D_BLOCKTYPE_LINESETCONTINUATION              0xFFFFFF3F  // 9.6.3.2

// Modifier blocks (section 9.7 of the Standard)
#define U3D_BLOCKTYPE_2DGLYPHMODIFIER                  0xFFFFFF41  // 9.7.1
#define U3D_BLOCKTYPE_SUBDIVISIONMODIFIER              0xFFFFFF42  // 9.7.2
#define U3D_BLOCKTYPE_ANIMATIONMODIFIER                0xFFFFFF43  // 9.7.3
#define U3D_BLOCKTYPE_BONEWEIGHTMODIFIER               0xFFFFFF44  // 9.7.4
#define U3D_BLOCKTYPE_SHADINGMODIFIER                  0xFFFFFF45  // 9.7.5
#define U3D_BLOCKTYPE_CLODMODIFIER                     0xFFFFFF46  // 9.7.6

// Ressource blocks (section 9.8 of the Standard)
#define U3D_BLOCKTYPE_LIGHTRESOURCE                    0xFFFFFF51  // 9.8.1
#define U3D_BLOCKTYPE_VIEWRESOURCE                     0xFFFFFF52  // 9.8.2
#define U3D_BLOCKTYPE_LITTEXTURESHADER                 0xFFFFFF53  // 9.8.3
#define U3D_BLOCKTYPE_MATERIALRESOURCE                 0xFFFFFF54  // 9.8.4
#define U3D_BLOCKTYPE_TEXTUREDECLARATION               0xFFFFFF55  // 9.8.5.1
#define U3D_BLOCKTYPE_TEXTURECONTINUATIOM              0xFFFFFF5C  // 9.8.5.2
#define U3D_BLOCKTYPE_MOTIONRESOURCE                   0xFFFFFF56  // 9.8.6

//////////////////////////////////////////////////////////////////////////
// 
// U3D file header values
//

// Profile identifier flags (section 9.4.1.2 of the Standard)
#define U3D_PROFILEIDENTIFIER_BASEPROFILE         0x00000000  
#define U3D_PROFILEIDENTIFIER_EXTENSIBLEPROFILE   0x00000002  
#define U3D_PROFILEIDENTIFIER_NOCOMPRESSIONMODE   0x00000004  
#define U3D_PROFILEIDENTIFIER_DEFINEDUNITS        0x00000008  

#define U3D_PROFILEIDENTIFIER_ALL                 0x0000000E  


//////////////////////////////////////////////////////////////////////////
// 
// U3D modifier chain values
//

// Modifier chain type (section 9.4.3.2 of the Standard)
#define U3D_MODIFIERCHAINTYPE_NODEMODIFIERCHAIN               0x00000000  
#define U3D_MODIFIERCHAINTYPE_MODELRESOURCEMODIFIERCHAIN      0x00000001  
#define U3D_MODIFIERCHAINTYPE_TEXTURERESOURCEMODIFIERCHAIN    0x00000002  

// Modifier chain attributes flags (section 9.4.3.3 of the Standard)
#define U3D_MODIFIERCHAINATTRIBUTE_BOUNDINGSPHEREINFORMATIONPRESENT   0x00000001
#define U3D_MODIFIERCHAINATTRIBUTE_AXISALIGNEDBOUNDIGBOXPRESENT       0x00000002

//////////////////////////////////////////////////////////////////////////
// 
// U3D view node values
//

// View Node attributes (section 9.5.4.4 of the Standard)
#define U3D_VIEWNODEATTRIBUTE_DEFAULT                            0x00000000  
#define U3D_VIEWNODEATTRIBUTE_SCREENPOSITIONUNITS                0x00000001  
#define U3D_VIEWNODEATTRIBUTE_PROJECTIONMODE_ORTHOGRAPHIC        0x00000002  
#define U3D_VIEWNODEATTRIBUTE_PROJECTIONMODE_2POINTPERSPECTIVE   0x00000004  
#define U3D_VIEWNODEATTRIBUTE_PROJECTIONMODE_1POINTPERSPECTIVE   (U3D_VIEWNODEATTRIBUTE_PROJECTIONMODE_ORTHOGRAPHIC + U3D_VIEWNODEATTRIBUTE_PROJECTIONMODE_2POINTPERSPECTIVE)

//////////////////////////////////////////////////////////////////////////
// 
// U3D model node values
//

// Model visibility (section 9.5.2.4 of the Standard)
#define U3D_MODELVISIBILITY_NOTVISIBLE            0x00000000  
#define U3D_MODELVISIBILITY_FRONTVISIBLE          0x00000001  
#define U3D_MODELVISIBILITY_BACKVISIBLE           0x00000002  
#define U3D_MODELVISIBILITY_FRONTANDBACKVISIBLE   0x00000003  

//////////////////////////////////////////////////////////////////////////
// 
// U3D shading modifier values
//

// Shading attributes flags (section 9.7.5.3 of the Standard)
#define U3D_SHADINGMODIFIER_SHADINGATTRIBUTES_MESH       0x00000001  
#define U3D_SHADINGMODIFIER_SHADINGATTRIBUTES_LINE       0x00000002  
#define U3D_SHADINGMODIFIER_SHADINGATTRIBUTES_POINT      0x00000004  
#define U3D_SHADINGMODIFIER_SHADINGATTRIBUTES_GLYPH      0x00000008  

#define U3D_SHADINGMODIFIER_SHADINGATTRIBUTES_ALL        0x0000000F  // Just for convenience: all attributes ORed together

//////////////////////////////////////////////////////////////////////////
// 
// U3D CLOD Mesh Declaration values
//

// Mesh Attributes flags (section 9.6.1.1.3.1 of the Standard)
#define U3D_MESH_ATTRIBUTES_DEFAULT            0x00000000  
#define U3D_MESH_ATTRIBUTES_EXCLUDENORMALS     0x00000001  

// Shading Attributes flags (section 9.6.1.1.3.9.1 of the Standard)
#define U3D_SHADINGATTRIBUTES_NONE                    0x00000000  
#define U3D_SHADINGATTRIBUTES_DIFFUSECOLORS           0x00000001  
#define U3D_SHADINGATTRIBUTES_SPECULARCOLORS          0x00000002  
#define U3D_SHADINGATTRIBUTES_DIFFUSESPECULARCOLORS   0x00000003 // Just for convenience: all attributes ORed together 


//////////////////////////////////////////////////////////////////////////
// 
// U3D Lit Texture Shader values
//

// Lit Texture Shader Attributes flags (section 9.8.3.2 of the Standard)
#define U3D_LITTEXTURESHADERATTRIBUTES_NONE               0x00000000  
#define U3D_LITTEXTURESHADERATTRIBUTES_LIGHTINGENABLED    0x00000001  
#define U3D_LITTEXTURESHADERATTRIBUTES_ALPHATESTENABLED   0x00000002  
#define U3D_LITTEXTURESHADERATTRIBUTES_USEVERTEXCOLOR     0x00000004  
#define U3D_LITTEXTURESHADERATTRIBUTES_ALL                0x00000007  

// Alpha Test Function values (section 9.8.3.4 of the Standard)
#define U3D_ALPHATESTFUCTION_NEVER                        0x00000610  
#define U3D_ALPHATESTFUCTION_LESS                         0x00000611  
#define U3D_ALPHATESTFUCTION_GREATER                      0x00000612  
#define U3D_ALPHATESTFUCTION_EQUAL                        0x00000613  
#define U3D_ALPHATESTFUCTION_NOT_EQUAL                    0x00000614  
#define U3D_ALPHATESTFUCTION_LEQUAL                       0x00000615  
#define U3D_ALPHATESTFUCTION_GEQUAL                       0x00000616  
#define U3D_ALPHATESTFUCTION_ALWAYS                       0x00000617  

// Color Blend Function values (section 9.8.3.5 of the Standard)
#define U3D_COLORBLEND_ADD                                0x00000604  
#define U3D_COLORBLEND_MULTIPLY                           0x00000605  
#define U3D_COLORBLEND_ALPHABLEND                         0x00000606  
#define U3D_COLORBLEND_INVALPHABLEND                      0x00000607  


//////////////////////////////////////////////////////////////////////////
// 
// U3D Material Resource values
//

// Material Resource Attributes flags (section 9.8.4.2 of the Standard)
#define U3D_MATERIALRESOURCEATTRIBUTES_NONE           0x00000000  
#define U3D_MATERIALRESOURCEATTRIBUTES_AMBIENT        0x00000001  
#define U3D_MATERIALRESOURCEATTRIBUTES_DIFFUSE        0x00000002  
#define U3D_MATERIALRESOURCEATTRIBUTES_SPECULAR       0x00000004  
#define U3D_MATERIALRESOURCEATTRIBUTES_EMISSIVE       0x00000008  
#define U3D_MATERIALRESOURCEATTRIBUTES_REFLECTIVITY   0x00000010  
#define U3D_MATERIALRESOURCEATTRIBUTES_OPACITY        0x00000020  
#define U3D_MATERIALRESOURCEATTRIBUTES_ALL            0x0000003F  


//////////////////////////////////////////////////////////////////////////
// 
// U3D Light Resource values
//

// Light Resource Attributes flags (section 9.8.1.2 of the Standard)
#define U3D_LIGHTRESOURCEATTRIBUTES_NONE          0x00000000  
#define U3D_LIGHTRESOURCEATTRIBUTES_LIGHTENABLED  0x00000001  
#define U3D_LIGHTRESOURCEATTRIBUTES_SPECULAR      0x00000002  
#define U3D_LIGHTRESOURCEATTRIBUTES_SPOTDECAY     0x00000004  
#define U3D_LIGHTRESOURCEATTRIBUTES_ALL           0x00000007  

// Light Type values (section 9.8.2.2 of the Standard)
#define U3D_LIGHTTYPE_AMBIENT          0x00  
#define U3D_LIGHTTYPE_DIRECTIONAL      0x01  
#define U3D_LIGHTTYPE_POINT            0x02  
#define U3D_LIGHTTYPE_SPOT             0x03  




//////////////////////////////////////////////////////////////////////////



//! Enumeration of the palette types (section 8.2 of the Standard)
enum U3D_PaletteType {
    U3D_MODEL_RESOURCE_PALETTE    = 0, // 
    U3D_LIGHT_RESOURCE_PALETTE    = 1, // 
    U3D_VIEW_RESOURCE_PALETTE     = 2, // 
    U3D_TEXTURE_RESOURCE_PALETTE  = 3, // 
    U3D_SHADER_RESOURCE_PALETTE   = 4, // 
    U3D_MATERIAL_RESOURCE_PALETTE = 5, // 
    U3D_MOTION_RESOURCE_PALETTE   = 6, // 
    U3D_NODE_PALETTE              = 7  // 
};

//////////////////////////////////////////////////////////////////////////

//! Enumeration of the node types (section 8.3 of the Standard) 
enum U3D_NodeType {
    U3D_GROUP_NODE    = 0, // Group Node
    U3D_MODEL_NODE    = 1, // Model Node
    U3D_VIEW_NODE     = 2, // View node
    U3D_LIGHT_NODE    = 3  // Light node
};

//////////////////////////////////////////////////////////////////////////


const MLuint32 U3D_BLOCKDECLARATIONSIZE = 12;  
const MLuint32 U3D_BLOCKFUNDAMENTALSIZE = sizeof(U3DDataBlockFundamental);  


//////////////////////////////////////////////////////////////////////////

/// <summary>
/// A default buffer size for U3D
/// </summary>
const MLuint32 U3D_SizeBuff = 1024;

/// <summary>
/// The initial size allocated for buffers
/// </summary>
const MLuint32 U3D_DataSizeInitial = 0x00000010;

//////////////////////////////////////////////////////////////////////////


//
// Predefined contexts (based on http://www3.math.tu-berlin.de/jreality/api/constant-values.html)
// Names used here are the names from the ECMA specification document (with "U3DContext_" prefix)
// Names in [] are the names used by website
//

/// <summary>
/// The context for uncompressed U8
/// </summary>
const MLuint32 U3DContext_Uncompressed8 = 0;

/// <summary>
/// Contexts >= StaticFull are static contexts.
/// </summary>
const MLuint32 U3DContext_StaticFull = 0x00000400;  // [uACStaticFull]

/// <summary>
/// The largest allowable static context. values written to contexts > MaxRange are written as uncompressed.
/// </summary>
const MLuint32 U3D_MaxRange = U3DContext_StaticFull + 0x00003FFF;

/// <summary>
/// The context cZero
/// </summary>
const MLuint32 U3DContext_cZero = 0x000003FF;

/// <summary>
/// Bit compression context for Number Of New Points/Lines/Faces
/// </summary>
const MLuint32 U3DContext_cPointCnt_cLineCnt_cFaceCnt = 0x00000001;  // [uACContextNumNewFaces]

/// <summary>
/// Bit compression context for Position Difference Signs (9.6.1.3.4.10.1)
/// </summary>
const MLuint32 U3DContext_cPosDiffSign = 0x00000014;  // [uACContextPositionDiffSigns]

/// <summary>
/// Bit compression context for Position Difference X (9.6.1.3.4.10.2)
/// </summary>
const MLuint32 U3DContext_cPosDiffX = 0x00000015;  // [uACContextPositionDiffMagX]

/// <summary>
/// Bit compression context for Position Difference Y (9.6.1.3.4.10.3)
/// </summary>
const MLuint32 U3DContext_cPosDiffY = 0x00000016;  // [uACContextPositionDiffMagY]

/// <summary>
/// Bit compression context for Position Difference Z (9.6.1.3.4.10.4)
/// </summary>
const MLuint32 U3DContext_cPosDiffZ = 0x00000017;  // [uACContextPositionDiffMagZ]

/// <summary>
/// Bit compression context for Number Of Normals (9.6.1.3.4.11.1 / 9.6.2.2.4.3 / 9.6.3.2.4.3)  
/// </summary>
const MLuint32 U3DContext_cNormlCnt = 0x00000028;  // [uACContextNumLocalNormals]
 
/// <summary>
/// Bit compression context for Position Difference Signs (9.6.1.3.4.10.1)
/// </summary>
const MLuint32 U3DContext_cDiffNormalSign = 0x00000029;  // [uACContextNormalDiffSigns]

/// <summary>
/// Bit compression context for Position Difference X (9.6.1.3.4.10.2)
/// </summary>
const MLuint32 U3DContext_cDiffNormalX = 0x0000002A;  // [uACContextNormalDiffMagX]

/// <summary>
/// Bit compression context for Position Difference Y (9.6.1.3.4.10.3)
/// </summary>
const MLuint32 U3DContext_cDiffNormalY = 0x0000002B;  // [uACContextNormalDiffMagY]

/// <summary>
/// Bit compression context for Position Difference Z (9.6.1.3.4.10.4)
/// </summary>
const MLuint32 U3DContext_cDiffNormalZ = 0x0000002C;  // [uACContextNormalDiffMagZ]

/// <summary>
/// Bit compression context for Normal Local Index (9.6.3.2.4.6.3)
/// </summary>
const MLuint32 U3DContext_cNormlIdx = 0x0000002D;  // [uACContextNormalLocalIndex]

/// <summary>
/// Bit compression context for Shading ID (9.6.1.2.4.6.1)
/// </summary>
const MLuint32 U3DContext_cShading = 0x00000001;

/// <summary>
/// Bit compression context for New Diffuse Color Count (9.6.1.3.4.2.1)
/// </summary>
const MLuint32 U3DContext_cDiffuseCount = 0x00000063;

/// <summary>
/// Bit compression context for Diffuse Color Difference Signs (9.6.1.3.4.2.2)
/// </summary>
const MLuint32 U3DContext_cDiffuseColorSign = 0x00000064;




//
//  Inverse Quantization constants for quality level 1000 (always used in this version)
//

/// <summary>
/// Inverse Quantization - Position Quant
/// </summary>
const MLfloat U3DQuant_Position = 262144.0f;      // 2^18

/// <summary>
/// Inverse Quantization - Normal Quant
/// </summary>
const MLfloat U3DQuant_Normal = 16384.0f;         // 2^14

/// <summary>
/// Inverse Quantization - Texture Coord Quant
/// </summary>
const MLfloat U3DQuant_TextureCoord = 16384.0f;   // 2^14

/// <summary>
/// Inverse Quantization - Diffuse Color Quant
/// </summary>
const MLfloat U3DQuant_DiffuseColor = 16384.0f;   // 2^14

/// <summary>
/// Inverse Quantization - Specular Color Quant
/// </summary>
const MLfloat U3DQuant_SpecularColor = 16384.0f;  // 2^14


//////////////////////////////////////////////////////////////////////////

//
//Bit masks for reading and writing symbols.
//

/// <summary>
/// Masks all but the most significant bit
/// </summary>
const MLuint32 U3D_HalfMask = 0x00008000;

/// <summary>
/// Masks the most significant bit
/// </summary>
const MLuint32 U3D_NotHalfMask = 0x00007FFF;

/// <summary>
/// Masks all but the 2nd most significan bit
/// </summary>
const MLuint32 U3D_QuarterMask = 0x00004000;

/// <summary>
/// Masks the 2 most significant bits
/// </summary>
const MLuint32 U3D_NotThreeQuarterMask = 0x00003FFF;

/// <summary>
/// Used to swap 8 bits in place
/// </summary>
const MLuint32 U3D_Swap8[16]        = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};

const MLuint32 U3D_ReadCount[16]    = {4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
const MLuint32 U3D_FastNotMask[5]   = {0xFFFFFFFF, 0x7FFF7FFF, 0x3FFF3FFF, 0x1FFF1FFF, 0x0FFF0FFF};


//////////////////////////////////////////////////////////////////////////

//
// Other constants.
//

/// <summary>
/// Constant for reserved / zero values
/// </summary>
const MLuint32 U3D_ReservedZero = 0x00000000;

//////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

#endif // _U3D_Constants_H
