//----------------------------------------------------------------------------------
//! This header file contains definitions for a PRC BitDtream class
/*! 
//
// \file    PRC_BitStream.h
// \author  Axel Newe
// \date    2015-08-01
*/

#ifndef _PRC_BitStream_H
#define _PRC_BitStream_H

#include "mlModuleIncludes.h"

ML_START_NAMESPACE


#pragma warning( push )
  #pragma warning( disable : 4512 )
  // Suppress warning that assignment operator could not be generated.

class PRC_BitStream
{
public:

    PRC_BitStream(MLuint8*& buffer, unsigned int length) : byteIndex(0), bitIndex(0), allocatedLength(length), data(buffer), compressed(false)
    {
      if (data == 0)
      {
        getAChunk();
      }
    }

    unsigned int getSize() const;

    MLuint8* getData();

    PRC_BitStream& operator <<(const std::string&);
    PRC_BitStream& operator <<(bool);
    PRC_BitStream& operator <<(MLuint32);
    PRC_BitStream& operator <<(MLuint8);
    PRC_BitStream& operator <<(int32_t);
    PRC_BitStream& operator <<(double);
    PRC_BitStream& operator <<(const char*);

    void compress();
    void write(std::ostream &out) const;
  
private:

    MLuint8*& data;
    MLuint32  compressedDataSize;

    unsigned int byteIndex;
    unsigned int bitIndex;
    unsigned int allocatedLength;

    bool compressed;

    void writeBit(bool b);
    void writeBits(MLuint32,MLuint8);
    void writeByte(MLuint8);
    void nextByte();
    void nextBit();
    void getAChunk();

};

#pragma warning( pop ) 

ML_END_NAMESPACE

#endif // _PRC_BitStream_H
