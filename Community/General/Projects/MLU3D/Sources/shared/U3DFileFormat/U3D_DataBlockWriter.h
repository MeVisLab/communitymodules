//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data 
// block as specified in Standard ECMA-363 (Universal 3D File Format), 4th edition 
// (June 2007) (referred below as "the Standard")
//
// \file    U3D_DataBlockWriter.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_DataBlockWriter_H
#define _U3D_DataBlockWriter_H

// Std includes
#include<vector>

// Local includes
#include "U3D_BitStream.h"
#include "U3D_DataBlock.h"


ML_START_NAMESPACE

class U3DDataBlockWriter : public U3DDataBlock
{
public:

  //! Standard constructor
  U3DDataBlockWriter();

  //! Standard destructor
  virtual ~U3DDataBlockWriter();

  MLuint32 getDataSize()                       { return (_closed ? _dataWriter.getDataPosition() : 0); }
  MLuint32 getMetaDataSize()                   { return (/*_closed ? _metaDataWriter.getDataPosition() : 0)*/_metaDataWriter.getDataPosition()); }
  MLuint32 getDataSizeWithoutPadding()         { return (_closed ? _dataWriter.getDataPositionWithoutPadding() : 0); }
  MLuint32 getMetaDataSizeWithoutPadding()     { return (_closed ? _metaDataWriter.getDataPositionWithoutPadding() : 0); }

  void addChildDataBytes(MLuint32 uValue)      { _childDataBytes += (mlU3D::BLOCKDECLARATIONSIZE + uValue + getNumPaddingBytes(uValue)); }

  MLuint32 getNumDataBytes()                   { return mlU3D::BLOCKFUNDAMENTALSIZE * getDataSize(); }
  MLuint32 getNumDataBytesWithoutPadding()     { return mlU3D::BLOCKFUNDAMENTALSIZE * getDataSizeWithoutPadding(); }
  MLuint32 getDataSizeWithChildDataBytes();

  MLuint32 getNumMetaDataBytes()               { return mlU3D::BLOCKFUNDAMENTALSIZE * getMetaDataSize(); }
  MLuint32 getNumMetaDataBytesWithoutPadding() { return mlU3D::BLOCKFUNDAMENTALSIZE * getMetaDataSizeWithoutPadding(); }
  
  MLuint32 getNumTotalBytes()                  { return ( _closed ? mlU3D::BLOCKDECLARATIONSIZE + getNumDataBytes() + getNumMetaDataBytes() : 0 ); }
  
  mlU3D::DataVector getData() const            { return _data; }
  mlU3D::DataVector getMetaData() const        { return _metaData; }

  void writeU8(const MLuint8 uValue)           { if (!_closed) _dataWriter.writeU8(uValue); }
  void writeU16(const MLuint16 uValue)         { if (!_closed) _dataWriter.writeU16(uValue); }
  void writeU32(const MLuint32 uValue)         { if (!_closed) _dataWriter.writeU32(uValue); }
  void writeU64(const MLuint64 uValue)         { if (!_closed) _dataWriter.writeU64(uValue); }
  void writeI32(const MLint32 iValue)          { if (!_closed) _dataWriter.writeI32(iValue); }
  void writeI16(const MLint16 iValue)          { if (!_closed) _dataWriter.writeI16(iValue); }
  void writeF32(const MLfloat fValue)          { if (!_closed) _dataWriter.writeF32(fValue); }
  void writeF32(const MLdouble fValue)         { writeF32(static_cast<MLfloat>(fValue)); }
  void writeF64(const MLdouble fValue)         { if (!_closed) _dataWriter.writeF64(fValue); }

  void writeCompressedU32(const MLuint32 context, const MLuint32 uValue) { if (!_closed) _dataWriter.writeCompressedU32(context, uValue); }
  void writeCompressedU16(const MLuint32 context, const MLuint16 uValue) { if (!_closed) _dataWriter.writeCompressedU16(context, uValue); }
  void writeCompressedU8(const MLuint32 context, const MLuint8 uValue)   { if (!_closed) _dataWriter.writeCompressedU8(context, uValue); }

  void writeString(const std::string& sValue)  { if (!_closed) _dataWriter.writeString(sValue); }

  void writeF32Color(const MLfloat red, const MLfloat green, const MLfloat blue, const MLfloat alpha);
  void writeF32Color(const Vector4 colors);
  void writeF32Color(const Vector3 colors, const MLfloat alpha);
  void writeF32Color(const Vector3 colors);
  void writeF32Matrix(const MLfloat a, const MLfloat b, const MLfloat c, const MLfloat d, 
                      const MLfloat e, const MLfloat f, const MLfloat g, const MLfloat h, 
                      const MLfloat i, const MLfloat j, const MLfloat k, const MLfloat l, 
                      const MLfloat m, const MLfloat n, const MLfloat o, const MLfloat p);
  void writeIdentityMatrix();
  void writePaddingBytes(const MLuint32 numPaddingBytes);

  void writeMetaU32(const MLuint32 uValue)     { if (!_closed) _metaDataWriter.writeU32(uValue); }
  void writeMetaKeyValuePair(const MLuint32 attributes, const std::string keyString, const std::string valueString);
  void writeMetaPaddingBytes(const size_t numPaddingBytes);

  void close();

  static MLuint32 getNumPaddingBytes(const MLuint32 previousBytes) { return ( (previousBytes % 4 != 0) ? 4 - previousBytes % 4 : 0); }


private:

  U3DBitStreamWriter  _dataWriter;
  U3DBitStreamWriter  _metaDataWriter;

  mlU3D::DataVector   _data;
  mlU3D::DataVector   _metaData;

  MLuint32            _childDataBytes;

  bool                _closed;
};


ML_END_NAMESPACE

#endif // _U3D_DataBlockWriter_H


