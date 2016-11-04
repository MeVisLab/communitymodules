//----------------------------------------------------------------------------------
// This class provides a bit stream encoder for an U3D data block as specified in 
// Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
// Chapter 10 and Annex A
//
// \file    U3D_BitStream.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_DataBlock.h"
#include "U3D_ContextManager.h"
#include "U3D_BitStream.h"


ML_START_NAMESPACE


U3DBitStreamWriter::U3DBitStreamWriter()
{
  Init();
}

void U3DBitStreamWriter::Init()
{
  _compressed      = false;
  _high            = 0x0000FFFF;
  _low             = 0x00000000;
  
  _underflow       = 0;  // Stores the number of bits of underflow caused by the limited range of high and low
  _dataPosition    = 0;  // The position currently to write in the datablock specified in 32 bit increments.
  _dataLocal       = 0;  // The local value of the data corresponding to dataposition
  _dataLocalNext   = 0;  // The 32 bits in data after dataLocal
  _dataBitOffset   = 0;  // The offset into dataLocal that the next write will occur

  _data.clear();
  _data.resize(mlU3D::DATASIZEINCREMENT);
}

void U3DBitStreamWriter::writeCompressedU32(MLuint32 context, MLuint32 uValue)
{
  if (context == mlU3D::Context_StaticFull)   // In this case a division by zero might occur in _writeSymbol() so we capture it here 
  {
    writeU32(uValue);
  }
  else
  {
    _compressed = true;
    bool escape = false;

    if((context != 0) && (context < mlU3D::MaxRange))
    {
      writeSymbol(context, uValue, escape);

      if(escape == true)
      {
        writeU32(uValue);
        _contextManager.addSymbol(context, uValue + 1U);
      }
    }
    else
    {
      writeU32(uValue);
    }
  }
}

void U3DBitStreamWriter::writeCompressedU16(MLuint32 context, MLuint16 uValue)
{
  if (context == mlU3D::Context_StaticFull)   // In this case a division by zero might occur in _writeSymbol() so we capture it here 
  {
    writeU16(uValue);
  }
  else
  {
    _compressed = true;
    bool escape = false; 

    if ((context != 0) && (context < mlU3D::MaxRange))
    {
      writeSymbol(context, uValue, escape);

      if(escape == true)
      {
        writeU16(uValue);
        _contextManager.addSymbol(context, uValue + 1U);
      }
    }
    else
    {
      writeU16(uValue);
    }
  }
}

void U3DBitStreamWriter::writeCompressedU8(MLuint32 context, MLuint8 uValue)
{
  if (context == mlU3D::Context_StaticFull)   // In this case a division by zero might occur in _writeSymbol() so we capture it here 
  {
    writeU8(uValue);
  }
  else
  {
    _compressed = true;
    bool escape = false;

    if ((context != 0) && (context < mlU3D::MaxRange))
    {
      writeSymbol(context, uValue, escape);

      if(escape == true)
      {
        writeU8(uValue);
        _contextManager.addSymbol(context, uValue + 1U);
      }
    }
    else
    {
      writeU8(uValue);
    }
  }
}

mlU3D::DataVector U3DBitStreamWriter::closeAndGet()
{
  if (_compressed) // Flush the arithmetic coder
  {
    writeU32(0);
  }

  alignToByte();
  putLocal();

  return _data;
}



MLint32 U3DBitStreamWriter::getDataPositionWithoutPadding()
{ 
  MLuint32 uBitCount = getBitCount();

  MLint32 dataPositionWithoutPadding = (uBitCount / 8) + ( ((uBitCount % 8) == 0) ? 0 : 1 );
  return dataPositionWithoutPadding; 
}



void U3DBitStreamWriter::alignToByte()
{
  // Check input(s)
  MLuint32 uBitCount = getBitCount();
  uBitCount = (8 - (uBitCount & 7)) & 7;
  _dataBitOffset += uBitCount;

  if(_dataBitOffset >= 32)
  {
      _dataBitOffset -= 32;
      incrementPosition();
  }
}

void U3DBitStreamWriter::alignTo4Byte()
{
  if(_dataBitOffset > 0)
  {
    _dataBitOffset = 0;
    incrementPosition();
  }
}

/*
* writeSymbol
* Write the given symbol to the datablock in the specified context.
* rEscape returns as false if the symbol was written successfully.
* rEscape will return true when writing in dynamically compressed
* contexts when the symbol to write has not appeared yet in the
* context's histogram. In this case, the escape symbol, 0, is
* written.
*/
void U3DBitStreamWriter::writeSymbol(MLuint32 context, MLuint32 symbol, bool &rEscape)
{
  symbol++;
  rEscape = false;
  MLuint32 totalCumFreq = 0;
  MLuint32 symbolCumFreq = 0;
  MLuint32 symbolFreq = 0;

  totalCumFreq = _contextManager.getTotalSymbolFrequency(context);
  symbolCumFreq = _contextManager.getCumulativeSymbolFrequency(context, symbol);
  symbolFreq = _contextManager.getSymbolFrequency(context, symbol);

  if(0 == symbolFreq) // The symbol has not occurred yet. Write out the escape symbol, 0.
  { 
    symbol = 0;
    symbolCumFreq = _contextManager.getCumulativeSymbolFrequency(context,symbol);
    symbolFreq = _contextManager.getSymbolFrequency(context,symbol);
  }

  if (0 == symbol) // The symbol is the escape symbol.
  { 
    rEscape = true;
  }

  // If totalCumFreq is 0, we will get a "divsion by zero" exception here.
  // This is OK!
  // Reason: This can occur only if the "context" parameter of a WriteCompressedUXX() 
  //         function is == U3D_StaticFull. This is illegal and _must_ lead to an
  //         exception!

  MLuint32 range = _high + 1 - _low;
  _high = _low - 1 + range * (symbolCumFreq + symbolFreq) / totalCumFreq;
  _low = _low + range * symbolCumFreq / totalCumFreq;
  _contextManager.addSymbol(context, symbol);

  //write bits
  MLuint32 bit = _low >> 15;

  while ((_high & mlU3D::HalfMask) == (_low & mlU3D::HalfMask))
  {
    _high &= ~mlU3D::HalfMask;
    _high += _high + 1;
    writeBit(bit);
    while(_underflow > 0)
    {
      _underflow--;
      writeBit((~bit) & 1);
    }
    _low &= ~mlU3D::HalfMask;
    _low += _low;
    bit = _low >> 15;
  }

  // Check for underflow
  // Underflow occurs when the values in low and high
  // approach each other, without leaving the lower resp. upper
  // half of the scaling interval. The range is not large enough
  // to code the next symbol. To avoid this, the interval is
  // artificially enlarged once the low is larger than the
  // first quarter and high is lower than the third quarter.
  while ((0 == (_high & mlU3D::QuarterMask)) && (mlU3D::QuarterMask == (_low & mlU3D::QuarterMask)))
  {
    _high &= ~mlU3D::HalfMask;
    _high <<= 1;
    _low <<= 1;
    _high |= mlU3D::HalfMask;
    _high |= 1;
    _low &= ~mlU3D::HalfMask;
    _underflow++;
  }
}


/*
* writeBit
* Write the given bit to the datablock.
*/
void U3DBitStreamWriter::writeBit(MLuint32 bit)
{
  // NOTE: Shift operations on U32s are only valid for shifts of 0 to 31 bits.
  MLuint32 mask = 1;
  bit &= mask;
  _dataLocal &= ~(mask << _dataBitOffset);
  _dataLocal |= (bit << _dataBitOffset);
  _dataBitOffset += 1;

  if(_dataBitOffset >= 32)
  {
    _dataBitOffset -= 32;
    incrementPosition();
  }
}

/*
* incrementPosition
* Updates the values of the data block stored in dataLocal and
dataLocalNext
* to the next values in the data block.
*/
void U3DBitStreamWriter::incrementPosition()
{
  _dataPosition++;
  checkPosition();
  _data[_dataPosition-1] = _dataLocal;
  _dataLocal = _dataLocalNext;
  _dataLocalNext = _data[_dataPosition+1];
}

/*
* getLocal
* store the initial 64 bits of the data block in dataLocal and
* dataLocalNext
*/
void U3DBitStreamWriter::getLocal()
{
  checkPosition();
  _dataLocal = _data[_dataPosition];
  _dataLocalNext = _data[_dataPosition+1];
}

/*
* putLocal
* stores the local values of the data to the data array
*
*/
void U3DBitStreamWriter::putLocal()
{
  _data[_dataPosition] = _dataLocal;
  _data[_dataPosition+1] = _dataLocalNext;
}

/*
* checkPosition
* checks that the array allocated for writing is large
* enough. Reallocates if necessary.
*/
void U3DBitStreamWriter::checkPosition()
{
  if((MLuint32)_dataPosition + 2 > _data.size())
  {
      allocateDataBuffer(_dataPosition + 2 + mlU3D::DATASIZEINCREMENT);
  }
}

/*
* allocateDataBuffer
* Creates and new array for storing the data written. Copies
* values of the old array to the new array.
*/
void U3DBitStreamWriter::allocateDataBuffer(MLint32 size)
{
  _data.resize(size);
}


ML_END_NAMESPACE
