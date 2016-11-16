//----------------------------------------------------------------------------------
//! This class provides an U3D data block (writing mode) as specified in 
//! Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
//! Chapter 9
/*!
// \file    U3D_DataBlockWriter.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_DataBlock.h"
#include "U3D_DataBlockWriter.h"


ML_START_NAMESPACE


U3DDataBlockWriter::U3DDataBlockWriter() : U3DDataBlock()
{
  _childDataBytes  = 0;
  _closed          = false;
}


U3DDataBlockWriter::~U3DDataBlockWriter()
{
}


MLuint32 U3DDataBlockWriter::getDataSizeWithChildDataBytes()
{ 
  MLuint32 value = 0;

  if (_closed)
  {
    if (0 == _childDataBytes)
    {
      value = getDataSizeWithoutPadding();
    }
    else
    {
      value = getNumDataBytes() + _childDataBytes;
    }
  }

  return value;
}



void U3DDataBlockWriter::close()
{
  if (!_closed)
  {
    U3DDataVector tempDataVector = _dataWriter.closeAndGet();
    U3DDataVector::iterator CopyEnd = tempDataVector.begin() + _dataWriter.getDataPosition();
    _data.assign(tempDataVector.begin(), CopyEnd);

    U3DDataVector tempMetaDataVector = _metaDataWriter.closeAndGet();
    U3DDataVector::iterator MetaCopyEnd = tempMetaDataVector.begin() + _metaDataWriter.getDataPosition();
    _metaData.assign(tempMetaDataVector.begin(), MetaCopyEnd);
    
    _closed = true;
  }   
}


void U3DDataBlockWriter::writeF32Color(const MLfloat red, const MLfloat green, const MLfloat blue, const MLfloat alpha)
{
  _dataWriter.writeF32(red);   // Write color value red
  _dataWriter.writeF32(green); // Write color value green
  _dataWriter.writeF32(blue);  // Write color value blue
  _dataWriter.writeF32(alpha); // Write color value alpha
}


void U3DDataBlockWriter::writeF32Color(const Vector4 colors)
{
  writeF32Color(static_cast<MLfloat>(colors[0]), static_cast<MLfloat>(colors[1]), static_cast<MLfloat>(colors[2]), static_cast<MLfloat>(colors[3]));  
}

void U3DDataBlockWriter::writeF32Color(const Vector3 colors, const MLfloat alpha)
{
  writeF32Color(static_cast<MLfloat>(colors[0]), static_cast<MLfloat>(colors[1]), static_cast<MLfloat>(colors[2]), alpha);
}

void U3DDataBlockWriter::writeF32Color(const Vector3 colors)
{
  writeF32Color(static_cast<MLfloat>(colors[0]), static_cast<MLfloat>(colors[1]), static_cast<MLfloat>(colors[2]), 1.0f);
}

void U3DDataBlockWriter::writeF32Matrix(MLfloat a, MLfloat b, MLfloat c, MLfloat d, MLfloat e, MLfloat f, MLfloat g, MLfloat h, MLfloat i, MLfloat j, MLfloat k, MLfloat l, MLfloat m, MLfloat n, MLfloat o, MLfloat p)
{
  _dataWriter.writeF32(a);   // Write Parent Node Transform Matrix Element A
  _dataWriter.writeF32(b);   // Write Parent Node Transform Matrix Element B
  _dataWriter.writeF32(c);   // Write Parent Node Transform Matrix Element C
  _dataWriter.writeF32(d);   // Write Parent Node Transform Matrix Element D
  _dataWriter.writeF32(e);   // Write Parent Node Transform Matrix Element E
  _dataWriter.writeF32(f);   // Write Parent Node Transform Matrix Element F
  _dataWriter.writeF32(g);   // Write Parent Node Transform Matrix Element G
  _dataWriter.writeF32(h);   // Write Parent Node Transform Matrix Element H
  _dataWriter.writeF32(i);   // Write Parent Node Transform Matrix Element I
  _dataWriter.writeF32(j);   // Write Parent Node Transform Matrix Element J
  _dataWriter.writeF32(k);   // Write Parent Node Transform Matrix Element K
  _dataWriter.writeF32(l);   // Write Parent Node Transform Matrix Element L
  _dataWriter.writeF32(m);   // Write Parent Node Transform Matrix Element M
  _dataWriter.writeF32(n);   // Write Parent Node Transform Matrix Element N
  _dataWriter.writeF32(o);   // Write Parent Node Transform Matrix Element O
  _dataWriter.writeF32(p);   // Write Parent Node Transform Matrix Element P
}


void U3DDataBlockWriter::writeIdentityMatrix()
{
  writeF32Matrix(1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0);
}


void U3DDataBlockWriter::writePaddingBytes(MLuint32 numPaddingBytes)
{
  for (MLuint32 i = 0; i < numPaddingBytes; i++)
  {
    _dataWriter.writeU8(U3D_PADDINGVALUE);
  }
}


void U3DDataBlockWriter::writeMetaKeyValuePair(MLuint32 attributes, std::string keyString, std::string valueString)
{
  if (!_closed)
  {
    _metaDataWriter.writeU32(attributes);        // Write meta data key/value pair - Pair Attributes
    _metaDataWriter.writeString(keyString);      // Write meta data key/value pair - Key String
    _metaDataWriter.writeString(valueString);    // Write meta data key/value pair - Value String
  }
}


void U3DDataBlockWriter::writeMetaPaddingBytes(size_t numPaddingBytes)
{
  for (size_t i = 0; i < numPaddingBytes; i++)
  {
    _metaDataWriter.writeU8(U3D_PADDINGVALUE);
  }
}


ML_END_NAMESPACE
