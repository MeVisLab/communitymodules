//----------------------------------------------------------------------------------
//! This header file contains an abstract base class definitions for an U3D data block as specified in 
//! Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
//! Chapter 9
/*!
// \file    U3D_DataBlock.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


#ifndef _U3D_DataBlock_H
#define _U3D_DataBlock_H

#include "U3D_BitStream.h"


ML_START_NAMESPACE


class U3DDataBlock // Abstract base class for DataBlockWriter & DataBlockReader
{
public:

  U3DDataBlockFundamental blockType;
  U3DDataBlockFundamental priority;

  //! Standard constructor
  U3DDataBlock();

  //! Standard destructor
  virtual ~U3DDataBlock() {};

  U3DDataBlockFundamental getDataSize();
  U3DDataBlockFundamental getMetaDataSize();

};


ML_END_NAMESPACE

#endif // _U3D_DataBlock_H
