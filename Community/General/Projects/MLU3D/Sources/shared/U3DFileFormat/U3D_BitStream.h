//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data block as specified in 
// Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
// Chapter 10 and Annex A
//
// \file    U3D_Bitstream.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Bitstream_H
#define _U3D_Bitstream_H

// Std includes
#include <vector>

// Local includes
#include "MLU3D_Constants.h"
#include "U3D_ContextManager.h"


ML_START_NAMESPACE


class U3DBitStreamWriter
{
public:

  //! Standard constructor
  U3DBitStreamWriter();

  //! Standard desctructor
  ~U3DBitStreamWriter() {};

  void Init();

  void writeU8(const MLuint8 uValue)
  {
    mlU3D::DataBlockFundamental symbol = (mlU3D::DataBlockFundamental) uValue;
    swapBits8(symbol);
    bool escape = false;
    writeSymbol(mlU3D::Context_Uncompressed8, symbol, escape);
  }

  void writeU16(const MLuint16 uValue)
  {
    writeU8((MLuint8)(0x00FF & uValue));
    writeU8((MLuint8)(0x00FF & (uValue >> 8)));
  }

  void writeU32(const MLuint32 uValue)
  {
    writeU16((MLuint16)(0x0000FFFF & uValue));
    writeU16((MLuint16)(0x0000FFFF & (uValue >> 16)));
  }

  void writeU64(const MLuint64 uValue)
  {
    writeU32((MLuint32)(0x00000000FFFFFFFF & uValue));
    writeU32((MLuint32)(0x00000000FFFFFFFF & (uValue >> 32)));
  }

  void writeI32(const MLint32 iValue)
  {
    writeU32((MLuint32)iValue);
  }

  void writeI16(const MLint16 iValue)
  {
    writeU16((MLuint16)iValue);
  }   
  
  void writeF32(MLfloat fValue)
  {
    MLuint32* MLuint32Pointer = reinterpret_cast<MLuint32*>(&fValue);
    writeU32(*MLuint32Pointer);
  }

  void writeF64(MLdouble fValue)
  {
    MLuint64* MLuint64Pointer = reinterpret_cast<MLuint64*>(&fValue);
    writeU64(*MLuint64Pointer);
  }

  void writeString(std::string sValue)
  {
    if (!sValue.empty())
    {
      writeU16((MLuint16)sValue.length());

      std::string::iterator CurrentChar;
      for (CurrentChar = sValue.begin() ; CurrentChar < sValue.end(); CurrentChar++ )
      {
        writeU8((MLuint8)*CurrentChar);
      }
    }
    else
    {
      writeU16(0x0000);   // Write empty string code
    }
  }

  void writeCompressedU32(const MLuint32 context, const MLuint32 uValue);

  void writeCompressedU16(const MLuint32 context, const MLuint16 uValue);

  void writeCompressedU8(const MLuint32 context, const MLuint8 uValue);

  mlU3D::DataVector closeAndGet(); 

  MLint32 getDataPosition() { return ( (0 == _dataBitOffset) ? _dataPosition : _dataPosition + 1); }

  MLint32 getDataPositionWithoutPadding(); 

  void alignToByte();

  void alignTo4Byte();

private:

  /*
  * _writeSymbol
  * Write the given symbol to the datablock in the specified context.
  * rEscape returns as false if the symbol was written successfully.
  * rEscape will return true when writing in dynamically compressed
  * contexts when the symbol to write has not appeared yet in the
  * context's histogram. In this case, the escape symbol, 0, is
  * written.
  */
  void writeSymbol(const MLuint32 context, const MLuint32 symbol, bool &rEscape);

  /*
  * _swapBits8
  * reverses the order of the bits of an 8 bit value.
  * E.g. abcdefgh -> hgfedcba
  */
  void swapBits8(MLuint32 &rValue) { rValue = (mlU3D::Swap8[(rValue) & 0xf] << 4) | (mlU3D::Swap8[(rValue) >> 4]); }

  /*
  * _writeBit
  * Write the given bit to the datablock.
  */
  void writeBit(const MLuint32 bit);

  /*
  * _incrementPosition
  * Updates the values of the datablock stored in dataLocal and
  dataLocalNext
  * to the next values in the datablock.
  */
  void incrementPosition();

  /*
  * _getLocal
  * store the initial 64 bits of the datablock in dataLocal and
  * dataLocalNext
  */
  void getLocal();

  /*
  * _putLocal
  * stores the local values of the data to the data array
  *
  */
  void putLocal();

  /*
  * _checkPosition
  * checks that the array allocated for writing is large
  * enough. Reallocates if necessary.
  */
  void checkPosition();

  /*
  * _allocateDataBuffer
  * Creates and new array for storing the data written. Copies
  * values of the old array to the new arry.
  */
  void allocateDataBuffer(const MLint32 size);

  /*
  * _getBitCount
  * returns the number of bits written
  */
  MLuint32 getBitCount() 
  { 
    return (_dataPosition << 5) + _dataBitOffset; 
  }

  ContextManager       _contextManager; // the context manager handles the updates to the histograms for the compression contexts.
  
  MLuint32                    _high;           // the upper limit on the probability
  MLuint32                    _low;            // the lower limit on the probability
  MLuint32                    _underflow;      // stores the number of bits of underflow caused by the limited range of high and low
  bool                        _compressed;     // this is true if a compressed value was written. when the datablock is retrieved, a 32 bit 0 is written to reset the values of high, low, and underflow.
  mlU3D::DataVector               _data;           // the data section of the datablock to write.
  MLint32                     _dataPosition;   // the position currently to write in the datablock specified in 32 bit increments.
  mlU3D::DataBlockFundamental _dataLocal;      // the local value of the data corresponding to dataposition
  mlU3D::DataBlockFundamental _dataLocalNext;  // the 32 bits in data after dataLocal
  MLint32                     _dataBitOffset;  // the offset into dataLocal that the next write will occur
   
};


ML_END_NAMESPACE

#endif // _U3D_Bitstream_H
