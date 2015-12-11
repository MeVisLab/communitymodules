//----------------------------------------------------------------------------------
//! This header file contains data format specifications and functions from Adobe's documentation
//! http://help.adobe.com/livedocs/acrobat_sdk/9/Acrobat9_HTMLHelp/API_References/PRCReference/PRC_Format_Specification/prc_overview.html
//! plus some additional types
/*
// \file    PRC_DataFormats.h
// \author  Axel Newe  (axel.newe@fau.de)
// \date    2015-08-01
*/
//----------------------------------------------------------------------------------

#ifndef _PRC_DataFormats_H
#define _PRC_DataFormats_H

#include "PRC_BitStream.h"
#include "../../shared/MLPDF_DataTypes.h"

#include "mlModuleIncludes.h"

ML_START_NAMESPACE
  
typedef MLuint8   PRC_Byte;
typedef MLuint16  PRC_Word;
typedef MLuint32  PRC_DWord;

union PRC_IEEE754_Double  // This is the IEEE 754 double-precision format.
{
 double d; 
 struct
 {
   unsigned int mantissa1:32;
   unsigned int mantissa0:20;
   unsigned int exponent:11;
   unsigned int negative:1;
 } ieee;
};

union PRC_IEEE754_Float  // This is the IEEE 754 float-precision format. 
{
 float f;  
 struct {
   unsigned int mantissa:23;
   unsigned int exponent:8;
   unsigned int negative:1;
 } ieee;
};

enum PRC_ValueType 
{
  PRC_ValueType_Double,
  PRC_ValueType_Exponent
};

struct PRC_CodingOfFrequent_DoubleOrExponent
{
  short    Type;
  short    NumberOfBits;
  unsigned Bits;
  union {
    unsigned ul[2];
    double   Value;
  } UnsignedOrDouble;
};

#define DOUBLE_WITH_TWO_DWORD(upper,lower)  lower,upper
#define UPPERPOWER                          (1)
#define LOWERPOWER                          (!UPPERPOWER)

#define NEXTBYTE(pbd)              ((pbd)--)
#define PREVIOUSBYTE(pbd)          ((pbd)++)
#define MOREBYTE(pbd,pbend)        ((pbd)>=(pbend))
#define OFFSETBYTE(pbd,offset)     ((pbd)-=offset)
#define BEFOREBYTE(pbd)            ((pbd)+1)
#define DIFFPOINTERS(p1,p2)        ((unsigned)((p2)-(p1)))
#define SEARCHBYTE(pbstart,b,nb)   (unsigned char *)memchr((pbstart),(b),(nb))
#define BYTEAT(pb,i)              *((pb)+(i))

#define NEGATIVE(d)     (((union PRC_IEEE754_Double *)&(d))->ieee.negative)
#define EXPONENT(d)     (((union PRC_IEEE754_Double *)&(d))->ieee.exponent)
#define MANTISSA0(d)    (((union PRC_IEEE754_Double *)&(d))->ieee.mantissa0)
#define MANTISSA1(d)    (((union PRC_IEEE754_Double *)&(d))->ieee.mantissa1)

#define NUMBER_OF_ELEMENT_IN_CODINGOFFREQUENT_DOUBLEOREXPONENT (2077)

#define DOUBLE_WITH_TWO_DWORD_IN_TREE(upper,lower)    {lower,upper}

struct PRC_CodingOfFrequent_DoubleOrExponent* getCodingOfFrequent(unsigned Bits, short NumberOfBits);
int compareCodingOfFrequent(const void* CodingOfFrequentPointer1,const void* CodingOfFrequentPointer2);

extern PRC_DWord stadwZero[2],stadwNegativeZero[2];

extern PRC_CodingOfFrequent_DoubleOrExponent codingOfFrequentArray[NUMBER_OF_ELEMENT_IN_CODINGOFFREQUENT_DOUBLEOREXPONENT];

struct PRC_Vector2D
{
  double x;
  double y;

  PRC_Vector2D() : x(0.0), y(0.0) {}
  PRC_Vector2D(double X, double Y) : x(X), y(Y) {}
  PRC_Vector2D(const double c[], double fx=0, double fy=0) : x(c?c[0]:fx), y(c?c[1]:fy) {}
  PRC_Vector2D(const PRC_Vector2D& sVector2d) : x(sVector2d.x), y(sVector2d.y) {}

  void serializeVector2d(PRC_BitStream&);
  
  void Set(double fx, double fy) { x = fx; y = fy; }
  double Dot(const PRC_Vector2D & sPt) const { return(x*sPt.x)+(y*sPt.y); }
  double LengthSquared() { return(x*x+y*y); }
  
  friend PRC_Vector2D operator + (const PRC_Vector2D& a, const PRC_Vector2D& b) { return PRC_Vector2D(a.x+b.x,a.y+b.y); }
  friend PRC_Vector2D operator - (const PRC_Vector2D& a) { return PRC_Vector2D(-a.x,-a.y); }
  friend PRC_Vector2D operator - (const PRC_Vector2D& a, const PRC_Vector2D& b) { return PRC_Vector2D(a.x-b.x,a.y-b.y); }
  friend PRC_Vector2D operator * (const PRC_Vector2D& a, const double d) { return PRC_Vector2D(a.x*d,a.y*d); }
  friend PRC_Vector2D operator * (const double d, const PRC_Vector2D& a) { return PRC_Vector2D(a.x*d,a.y*d); }
  friend PRC_Vector2D operator / (const PRC_Vector2D& a, const double d) { return PRC_Vector2D(a.x/d,a.y/d); }
    
  double Length();
  bool Normalize();
  
  bool operator==(const PRC_Vector2D &v) const
  {
    return x==v.x && y==v.y;
  }

  bool operator!=(const PRC_Vector2D &v) const
  {
    return !(x==v.x && y==v.y);
  }

  bool operator<(const PRC_Vector2D &v) const
  {
    if(x!=v.x)
      return (x<v.x);
    return (y<v.y);
  }

  friend std::ostream& operator << (std::ostream& out, const PRC_Vector2D& v)
  {
    out << "(" << v.x << "," << v.y << ")";
    return out;
  }
};

class PRC_Vector3D
{
public :

  double x;
  double y;
  double z;

  PRC_Vector3D() : x(0), y(0), z(0) {}
  PRC_Vector3D(double fx, double fy, double fz) : x(fx), y(fy), z(fz) {}
  PRC_Vector3D(const double c[], double fx=0, double fy=0, double fz=0) : x(c?c[0]:fx), y(c?c[1]:fy), z(c?c[2]:fz) {}
  PRC_Vector3D(const PRC_Vector3D& sVector3d) : x(sVector3d.x), y(sVector3d.y), z(sVector3d.z) {}

  void Set(double fx, double fy, double fz) { x = fx; y = fy; z = fz; }
  double Dot(const PRC_Vector3D & sPt) const { return(x*sPt.x)+(y*sPt.y)+(z*sPt.z); }
  double LengthSquared() { return(x*x+y*y+z*z); }

  friend PRC_Vector3D operator + (const PRC_Vector3D& a, const PRC_Vector3D& b) { return PRC_Vector3D(a.x+b.x,a.y+b.y,a.z+b.z); }
  friend PRC_Vector3D operator - (const PRC_Vector3D& a) { return PRC_Vector3D(-a.x,-a.y,-a.z); }
  friend PRC_Vector3D operator - (const PRC_Vector3D& a, const PRC_Vector3D& b) { return PRC_Vector3D(a.x-b.x,a.y-b.y,a.z-b.z); }
  friend PRC_Vector3D operator * (const PRC_Vector3D& a, const double d) { return PRC_Vector3D(a.x*d,a.y*d,a.z*d); }
  friend PRC_Vector3D operator * (const double d, const PRC_Vector3D& a) { return PRC_Vector3D(a.x*d,a.y*d,a.z*d); }
  friend PRC_Vector3D operator / (const PRC_Vector3D& a, const double d) { return PRC_Vector3D(a.x/d,a.y/d,a.z/d); }
  friend PRC_Vector3D operator * (const PRC_Vector3D& a, const PRC_Vector3D& b) { return PRC_Vector3D((a.y*b.z)-(a.z*b.y), (a.z*b.x)-(a.x*b.z), (a.x*b.y)-(a.y*b.x)); }

  void write(PRC_BitStream &out) { out << x << y << z; }
  void serializeVector3d(PRC_BitStream &pbs) const { pbs << x << y << z; }
  void serializeVector2d(PRC_BitStream &pbs) const { pbs << x << y; }

  double Length();
  bool Normalize();

  bool operator==(const PRC_Vector3D &v) const  { return x==v.x && y==v.y && z==v.z; }
  bool operator!=(const PRC_Vector3D &v) const  { return !(x==v.x && y==v.y && z==v.z); }
 
  bool operator<(const PRC_Vector3D &v) const
  {
    if(x != v.x)
    {
      return (x < v.x);
    }

    if(y != v.y)
    {
      return (y < v.y);
    }

    return (z < v.z);
  }

  friend std::ostream& operator << (std::ostream& out, const PRC_Vector3D& v)
  {
    out << "(" << v.x << "," << v.y << "," << v.z << ")";
    return out;
  }

};

//***********************************************************************************

enum  PRCObjectType { PRCOBJECTTYPE_POINTSET, PRCOBJECTTYPE_LINESET, PRCOBJECTTYPE_MESH };

struct PRCObjectInfoStruct
{
  PRCObjectType Type; 
  std::string   DisplayName;
  std::string   InternalName;
  std::string   GroupPath;
  MLint         ParentTreeNodeID;
  Vector4       RGBAColor;
};

typedef std::vector< PRCObjectInfoStruct > PRCObjectInfoVector;

struct PRCDefaultValuesStruct
{
  std::string defaultModelName;
  std::string defaultLightName;
  std::string defaultViewName;
  Vector3     defaultMaterialAmbientColor;
  Vector3     defaultMaterialDiffuseColor;
  Vector3     defaultMaterialSpecularColor;
  Vector3     defaultMaterialEmissiveColor;
  MLfloat     defaultMaterialTransparency;
  Vector4     defaultMaterialDiffuseColorWithTransparency;
};


struct PRCMeshInfoStruct
{
  std::string DisplayName;
  std::string InternalName;
  std::string ResourceName;

  MLuint32    MeshAttributes;
  MLuint32    FaceCount;
  MLuint32    VertexCount;
  MLuint32    NormalCount;            
  MLuint32    DiffuseColorCount;
  MLuint32    SpecularColorCount;
  MLuint32    TextureCoordCount;
  MLuint32    ShadingAttributes;
  MLuint32    PatchID;
  MLuint32    MeshNumber; // Needed for progress calculation
  
  Vector4     DefaultDiffuseColor;
  Vector3     DefaultSpecularColor;
  Vector3     DefaultAmbientColor;
  Vector3     DefaultEmissiveColor;
};

typedef std::vector<PRCMeshInfoStruct> PRCMeshInfoVector;





struct PRCModelTreeNode
{
  MLint        ID;           // negative = invalid node
  std::string  Name;
  MLintVector  ChildrenIDs;
};

typedef std::vector<PRCModelTreeNode> PRCModelTreeNodeVector;




ML_END_NAMESPACE

#endif // _PRC_DataFormats_H
