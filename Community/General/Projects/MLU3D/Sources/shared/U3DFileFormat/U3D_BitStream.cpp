//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data 
// block as specified in Standard ECMA-363 (Universal 3D File Format), 4th edition 
// (June 2007) (referred below as "the Standard")
//
// \file    U3D_BitStream.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
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


static const MLuint32 FastNotMask[] = { 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF };
static const MLuint32 ReadCount[] = { 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

U3DBitStreamReader::U3DBitStreamReader(){
	resetState();
}

void U3DBitStreamReader::resetState(){
	contextManager = ContextManager();
	high = 0x0000FFFF;
	low = 0;
	underflow = 0;
	code = 0;
	dataBitOffset = 0;
	dataLocal = 0;
	dataLocalNext = 0;
	dataPosition = 0;
	counter = 0;
	data.clear();
}


std::string U3DBitStreamReader::printState(){
	std::string ret;
	ret += "h: ";
	ret += std::to_string(high);
	ret += " l: ";
	ret += std::to_string(low);
	ret += " u: ";
	ret += std::to_string(underflow);
	ret += " c: ";
	ret += std::to_string(contextManager.hash());
	return ret;
}

MLuint8 U3DBitStreamReader::readU8(){
	MLuint32 uValue = readSymbol(mlU3D::Context_Uncompressed8);
	uValue--;
	swapBits8(uValue);
	counter++;
	return (MLuint8)uValue;
}

MLuint16 U3DBitStreamReader::readU16(){
	MLuint8 lowH = readU8();
	MLuint8 highH = readU8();
	return ((MLuint16)lowH) | (((MLuint16)highH) << 8);
}

MLuint32 U3DBitStreamReader::readU32(){
	MLuint16 lowH = readU16();
	MLuint16 highH = readU16();
	return ((MLuint32)lowH) | (((MLuint32)highH) << 16);
}

MLuint64 U3DBitStreamReader::readU64(){
	MLuint32 lowH = readU32();
	MLuint32 highH = readU32();
	return ((MLuint64)lowH) | (((MLuint64)highH) << 32);
}

MLint16 U3DBitStreamReader::readI16(){
	return (MLint16)readU16();
}

MLint32 U3DBitStreamReader::readI32(){
	return (MLint32)readU32();
}

MLfloat U3DBitStreamReader::readF32(){
	MLuint32 buffer = readU32();
	MLfloat* bufferPtr = (MLfloat*)&buffer;
	return *bufferPtr;
}

MLuint32 U3DBitStreamReader::readCompressedU32(MLuint32 context) {
	MLuint32 rValue;
	MLuint32 symbol = 0;
	if (context != mlU3D::Context_Uncompressed8 && context < mlU3D::MaxRange) { // the
		symbol = readSymbol(context);
		if (symbol != 0) { // the symbol is compressed
			rValue = symbol - 1;
		}
		else { // escape character, the symbol was not compressed
			rValue = readU32();
			this->contextManager.addSymbol(context, rValue + 1);
		}
	}
	else { // The context specified is uncompressed.
		rValue = readU32();
	}
	return rValue;
}


MLuint16 U3DBitStreamReader::readCompressedU16(MLuint32 context){
	MLuint32 symbol = 0;
	if (context != mlU3D::Context_Uncompressed8 && context < mlU3D::MaxRange){
		symbol = readSymbol(context);
		if (symbol != 0){
			return (MLuint16)(symbol - 1);
		}
		else {
			MLuint16 ret = readU16();
			contextManager.addSymbol(context, ret + 1U);
			return ret;
		}
	}
	else {
		return readU16();
	}
}

MLuint8 U3DBitStreamReader::readCompressedU8(MLuint32 context){
	MLuint32 symbol = 0;
	if (context != mlU3D::Context_Uncompressed8 && context < mlU3D::MaxRange){
		symbol = readSymbol(context);
		if (symbol != 0){
			return (MLuint8)(symbol - 1);
		}
		else {
			MLuint8 ret = readU8();
			contextManager.addSymbol(context, ret + 1U);
			return ret;
		}
	}
	else {
		return readU8();
	}
}

void U3DBitStreamReader::setDataBlock(std::ifstream &fileStream, MLuint32 size){
	resetState();
	MLuint32 length = size / sizeof(MLuint32);
	if (size % sizeof(MLuint32) != 0){
		length++;
	}
	data.reserve(length);
	for (MLuint32 i = 0; i < length; i++){
		MLuint32 r = 0;
		fileStream.read((char*)(&r), sizeof(MLuint32));
		data.push_back(r);
	}
	getLocal();
}

void U3DBitStreamReader::swapBits8(MLuint32 &rValue){
	rValue = (mlU3D::Swap8[(MLuint32)((rValue)& 0xf)] << 4) |
		(mlU3D::Swap8[(MLuint32)((rValue) >> 4)]);
}


MLuint32 U3DBitStreamReader::readSymbol(MLuint32 context) {
	MLuint32 rSymbol;
	MLuint32 uValue = 0;

	const MLuint32 position = getBitCount();
	this->code = readBit();
	this->dataBitOffset += (MLuint32) this->underflow;
	while (this->dataBitOffset >= 32) {
		this->dataBitOffset -= 32;
		incrementPosition();
	}
	const MLuint32 temp = read15Bits();
	this->code <<= 15;
	this->code |= temp;
	seekToBit(position);

	const MLuint32 totalCumFreq =
		this->contextManager.getTotalSymbolFrequency(context);

	const MLuint32 range = this->high + 1 - this->low;

	const MLuint32 codeCumFreq =
		((totalCumFreq)* (1 + this->code - this->low) - 1) / (range);

	uValue = this->contextManager.getSymbolFromFrequency(context, codeCumFreq);

	MLuint32 valueCumFreq =
		this->contextManager.getCumulativeSymbolFrequency(context, uValue);
	MLuint32 valueFreq =
		this->contextManager.getSymbolFrequency(context, uValue);
	MLuint32 currentLow = this->low;
	MLuint32 currentHigh = this->high;
	currentHigh = currentLow - 1 + range * (valueCumFreq + valueFreq) / totalCumFreq;
	currentLow = currentLow + range * (valueCumFreq) / totalCumFreq;
	this->contextManager.addSymbol(context, uValue);
	MLuint32 bitCount;
	MLuint32 maskedLow;
	MLuint32 maskedHigh;

	bitCount =
		(MLuint32)ReadCount[(MLuint32)(((currentLow >> 12) ^ (currentHigh >> 12)) & 0x0000000F)];
	currentLow &= FastNotMask[bitCount];
	currentHigh &= FastNotMask[bitCount];
	currentHigh <<= bitCount;
	currentLow <<= bitCount;
	currentHigh |= (1 << bitCount) - 1;

	maskedLow = mlU3D::HalfMask & currentLow;
	maskedHigh = mlU3D::HalfMask & currentHigh;
	while (((maskedLow | maskedHigh) == 0) ||
		((maskedLow == mlU3D::HalfMask) && maskedHigh == mlU3D::HalfMask))
	{
		currentLow = (mlU3D::NotHalfMask & currentLow) << 1;
		currentHigh = ((mlU3D::NotHalfMask & currentHigh) << 1) | 1;
		maskedLow = mlU3D::HalfMask & currentLow;
		maskedHigh = mlU3D::HalfMask & currentHigh;
		bitCount++;
	}
	const MLuint32 savedBitsLow = maskedLow;
	const MLuint32 savedBitsHigh = maskedHigh;
	if (bitCount > 0) {
		bitCount += (int) this->underflow;
		this->underflow = 0;
	}

	maskedLow = mlU3D::QuarterMask & currentLow;
	maskedHigh = mlU3D::QuarterMask & currentHigh;
	MLuint64 currentUnderflow = 0;
	while ((maskedLow == 0x4000) && (maskedHigh == 0)) {
		currentLow &= mlU3D::NotThreeQuarterMask;
		currentHigh &= mlU3D::NotThreeQuarterMask;
		currentLow += currentLow;
		currentHigh += currentHigh;
		currentHigh |= 1;
		maskedLow = mlU3D::QuarterMask & currentLow;
		maskedHigh = mlU3D::QuarterMask & currentHigh;
		currentUnderflow++;
	}

	this->underflow += (MLuint32)currentUnderflow;
	currentLow |= savedBitsLow;
	currentHigh |= savedBitsHigh;
	this->low = currentLow;
	this->high = currentHigh;

	this->dataBitOffset += bitCount;
	while (this->dataBitOffset >= 32) {
		this->dataBitOffset -= 32;
		incrementPosition();
	}

	rSymbol = uValue;
	return rSymbol;
}

MLuint32 U3DBitStreamReader::getBitCount(){
	return (MLuint32)((this->dataPosition << 5) + this->dataBitOffset);
}

MLuint32 U3DBitStreamReader::readBit(){
	MLuint32 uValue = 0;
	uValue = this->dataLocal >> this->dataBitOffset;
	uValue &= 1;
	this->dataBitOffset++;
	if (this->dataBitOffset >= 32){
		this->dataBitOffset -= 32;
		incrementPosition();
	}
	return uValue;
}

MLuint32 U3DBitStreamReader::read15Bits(){
	MLuint32 uValue = this->dataLocal >> this->dataBitOffset;
	if (this->dataBitOffset > 17){
		uValue |= (this->dataLocalNext << (32 - this->dataBitOffset));
	}
	uValue += uValue;

	uValue = (mlU3D::Swap8[(uValue >> 12) & 0xf])
		| ((mlU3D::Swap8[(uValue >> 8) & 0xf]) << 4)
		| ((mlU3D::Swap8[(uValue >> 4) & 0xf]) << 8)
		| ((mlU3D::Swap8[uValue & 0xf]) << 12);

	this->dataBitOffset += 15;
	if (dataBitOffset >= 32){
		this->dataBitOffset -= 32;
		incrementPosition();
	}
	return uValue;
}

void U3DBitStreamReader::incrementPosition(){
	this->dataPosition++;
	if (this->dataPosition < this->data.size()){
		this->dataLocal = this->data[dataPosition];
	} else {
		std::cerr << "Invalid BitSream read!" << std::endl;
		abort();
	}
	if (this->data.size() > this->dataPosition + 1){
		this->dataLocalNext = this->data[this->dataPosition + 1];
	}
	else {
		this->dataLocalNext = 0;
	}
}

void U3DBitStreamReader::seekToBit(MLuint32 position){
	this->dataPosition = position >> 5;
	this->dataBitOffset = (MLint32)(position & 0x1F);
	getLocal();
}

void U3DBitStreamReader::getLocal(){
	this->dataLocal = this->data[this->dataPosition];
	if (this->data.size() > this->dataPosition + 1){
		this->dataLocalNext = this->data[this->dataPosition + 1];
	}
}


ML_END_NAMESPACE
