//----------------------------------------------------------------------------------
//! This source file contains definitions for a PRC BitDtream class
/*! 
//
// \file    PRC_BitStream.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-08-01
*/
//----------------------------------------------------------------------------------


#include <zlib.h>

#include "PRC_Defines.h"
#include "PRC_BitStream.h"
#include "PRC_DataTypes.h"


#include "mlModuleIncludes.h"

ML_START_NAMESPACE


using std::string;
using std::cerr;
using std::endl;


void PRC_BitStream::compress()
{
  compressedDataSize = 0;

  z_stream ZLIB_Stream;

  ZLIB_Stream.zalloc = Z_NULL;
  ZLIB_Stream.zfree  = Z_NULL;
  ZLIB_Stream.opaque = Z_NULL;

  if (deflateInit(&ZLIB_Stream,Z_DEFAULT_COMPRESSION) != Z_OK)
  {
    cerr << "PRC_BitStream::compress(): Compression initialization failed." << endl;
    return;
  }

  unsigned int sizeAvailable = deflateBound(&ZLIB_Stream,getSize());

  MLuint8 *compressedData = (MLuint8*)malloc(sizeAvailable);

  ZLIB_Stream.avail_in  = getSize();
  ZLIB_Stream.next_in   = (unsigned char*)data;
  ZLIB_Stream.next_out  = (unsigned char*)compressedData;
  ZLIB_Stream.avail_out = sizeAvailable;

  int code;
  unsigned int chunks = 0;

  while ( (code = deflate(&ZLIB_Stream,Z_FINISH)) == Z_OK)
  {
    ++chunks;
    compressedDataSize   = sizeAvailable - ZLIB_Stream.avail_out;
    compressedData       = (MLuint8*)realloc(compressedData,CHUNK_SIZE*chunks);
    ZLIB_Stream.next_out = (Bytef*)(compressedData + compressedDataSize);

    ZLIB_Stream.avail_out += CHUNK_SIZE;
    
    sizeAvailable += CHUNK_SIZE;
  }

  compressedDataSize = sizeAvailable - ZLIB_Stream.avail_out;

  if (code != Z_STREAM_END)
  {
    cerr << "PRC_BitStream::compress(): Compression error." << endl;
    deflateEnd(&ZLIB_Stream);
    free(compressedData);
    return;
  }

  compressed = true;

  free(data);
  data = compressedData;

  deflateEnd(&ZLIB_Stream);
}

void PRC_BitStream::write(std::ostream &outStream) const
{
  if (compressed)
  {
    outStream.write((char*)data,compressedDataSize);
  }
  else
  {
     cerr << "PRC_BitStream::write(): Attempt to write stream before compression." << endl;
     exit(1);
  }
}

unsigned int PRC_BitStream::getSize() const
{
  if (compressed)
  {
    return compressedDataSize;
  }
  else
  {
    return byteIndex+1;
  }
}

MLuint8* PRC_BitStream::getData()
{
  return data;
}

PRC_BitStream& PRC_BitStream::operator <<(bool b)
{
  writeBit(b);
  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(MLuint32 u)
{
  while (u != 0)
  {
    writeBit(1);
    writeByte(u & 0xFF);
    u >>= 8;
  }

  writeBit(0);

  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(MLuint8 u)
{
  writeByte(u);
  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(int32_t i)
{
  MLuint8 lastByte = 0;

  while ( !(((i == 0)&&((lastByte & 0x80)==0)) || ((i == -1)&&((lastByte & 0x80) != 0))) )
  {
    writeBit(1);
    lastByte = i & 0xFF;
    writeByte(lastByte);
    i >>= 8;
  }

  writeBit(0);
  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(double value)
{
  // write a double
  if(compressed)
  {
    cerr << "PRC_BitStream::operator <<(double): Cannot write to a stream that has been compressed." << endl;
    return *this;
  }

  union PRC_IEEE754_Double *pid = (union PRC_IEEE754_Double *)&value;
  
  int      i, fSaveAtEnd;
  PRC_Byte *pb, *pbStart, *pbStop, *pbEnd, *pbResult, bSaveAtEnd = 0;

  struct PRC_CodingOfFrequent_DoubleOrExponent codingOfFrequent, *CodingOfFrequentPointer;

  codingOfFrequent.UnsignedOrDouble.Value = value;

  CodingOfFrequentPointer = (struct PRC_CodingOfFrequent_DoubleOrExponent *)bsearch(&codingOfFrequent, codingOfFrequentArray, sizeof(codingOfFrequentArray)/sizeof(CodingOfFrequentPointer[0]), sizeof(CodingOfFrequentPointer[0]), compareCodingOfFrequent);

  while ( (CodingOfFrequentPointer > codingOfFrequentArray) && (EXPONENT(CodingOfFrequentPointer->UnsignedOrDouble.Value) == EXPONENT((CodingOfFrequentPointer-1)->UnsignedOrDouble.Value)) )
  {
    CodingOfFrequentPointer--;
  }

  assert(CodingOfFrequentPointer);

  while (CodingOfFrequentPointer->Type == PRC_ValueType_Double)
  {
    if (fabs(value)==CodingOfFrequentPointer->UnsignedOrDouble.Value)
    {
      break;
    }
    CodingOfFrequentPointer++;
  }

  for (i=1<<(CodingOfFrequentPointer->NumberOfBits-1);i>=1;i>>=1)
  {
    writeBit((CodingOfFrequentPointer->Bits&i)!=0);
  }

  if ( (!memcmp(&value,stadwZero,sizeof(value))) || (!memcmp(&value,stadwNegativeZero,sizeof(value)))  )
  {
    return *this;
  }

  writeBit(pid->ieee.negative);

  if (CodingOfFrequentPointer->Type == PRC_ValueType_Double)
  {
    return *this;
  }

  if(  (pid->ieee.mantissa0 == 0) && (pid->ieee.mantissa1 == 0)  )
  {
    writeBit(0);
    return *this;
  }

  writeBit(1);

  pb = ((PRC_Byte *)&value)+6;

  writeBits((*pb)&0x0F,4);

  NEXTBYTE(pb);
  pbStart=pb;

  pbEnd = pbStop = ((PRC_Byte *)&value);

  if ((fSaveAtEnd=(*pbStop!=*BEFOREBYTE(pbStop)))!=0)
  {
    bSaveAtEnd=*pbEnd;
  }

  PREVIOUSBYTE(pbStop);

  while(*pbStop==*BEFOREBYTE(pbStop))
  {
    PREVIOUSBYTE(pbStop);
  }

  for (; MOREBYTE(pb,pbStop); NEXTBYTE(pb))
  {
    if (  (pb!=pbStart) && ((pbResult=SEARCHBYTE(BEFOREBYTE(pb),*pb,DIFFPOINTERS(pb,pbStart))) != NULL)  )
    {
      writeBit(0);
      writeBits(DIFFPOINTERS(pb,pbResult),3);
    }
    else
    {
      writeBit(1);
      writeByte(*pb);
    }
  }

  if (!MOREBYTE(BEFOREBYTE(pbEnd),pbStop))
  {
    if (fSaveAtEnd)
    {
      writeBit(0);
      writeBits(6,3);
      writeByte(bSaveAtEnd);
    }
    else
    {
      writeBit(0);
      writeBits(0,3);
    }
  }
  else
  {
    if ((pbResult=SEARCHBYTE(BEFOREBYTE(pb),*pb,DIFFPOINTERS(pb,pbStart))) != NULL)
    {
      writeBit(0);
      writeBits(DIFFPOINTERS(pb,pbResult),3);
    }
    else
    {
      writeBit(1);
      writeByte(*pb);
    }
  }

  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(const char* s)
{
  if (s == NULL)
  {
    writeBit(false); // string is NULL
    return *this;
  }

  string str(s);
  *this << str;

  return *this;
}

PRC_BitStream& PRC_BitStream::operator <<(const string& s)
{
  if (s == "")
  {
    writeBit(false); // string is NULL
    return *this;
  }

  writeBit(true);

  size_t l = s.length();

  *this << static_cast<MLuint32>(l);

  for (size_t i = 0; i < l; ++i)
  {
    writeByte(s[i]);
  }

  return *this;
}

void PRC_BitStream::writeBit(bool b)
{
  if (compressed)
  {
    cerr << "PRC_BitStream::writeBit(bool): Cannot write to a stream that has been compressed." << endl;
    return;
  }

  if (b)
  {
    data[byteIndex] |= (0x80 >> bitIndex);
  }

  nextBit();
}

void PRC_BitStream::writeBits(MLuint32 u, MLuint8 bits)
{
  if (bits > 32)
  {
    return;
  }
  else
  {
    for (MLuint32 mask = (1 << (bits-1)); mask != 0; mask >>= 1)
    {
      writeBit((u&mask) != 0);
    }
  }
}

void PRC_BitStream::writeByte(MLuint8 u)
{
  if (compressed)
  {
    cerr << "PRC_BitStream::writeByte(MLuint8): Cannot write to a stream that has been compressed." << endl;
    return;
  }

  if(bitIndex == 0)
  {
    data[byteIndex] = u;
    nextByte();
  }
  else
  {
    data[byteIndex] |= (u >> bitIndex);
    unsigned int obi = bitIndex;
    nextByte();
    data[byteIndex] |= (u << (8-obi));
    bitIndex = obi; // Bit index is not changed by writing 8 bits
  }
}

void PRC_BitStream::nextBit()
{
  ++bitIndex;

  if (bitIndex == 8)
  {
    nextByte();
  }
}

void PRC_BitStream::nextByte()
{
  ++byteIndex;

  if (byteIndex >= allocatedLength)
  {
    getAChunk();
  }

  data[byteIndex] = 0; // clear the garbage data
  bitIndex = 0;
}

void PRC_BitStream::getAChunk()
{
   if (allocatedLength == 0)
   {
     data = (MLuint8*)realloc((void*)data,CHUNK_SIZE);
   }
   else
   {
     data = (MLuint8*)realloc((void*)data,2*allocatedLength);
   }

   if (data != NULL)
   {
     if(allocatedLength == 0)
     {
       allocatedLength = CHUNK_SIZE;
       *data = 0; // Clear first byte
     }
     else
     {
       allocatedLength *= 2;
     }
   }
   else
   {
     // Warn about memory problem!
     cerr << "PRC_BitStream::getAChunk(): Memory allocation error." << endl;
     exit(1);
   }
}

ML_END_NAMESPACE

