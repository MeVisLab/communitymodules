//----------------------------------------------------------------------------------
// This header file contains an abstract base class definitions for an U3D data block as specified in 
// Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
// Chapter 9
//
// \file    U3D_DataBlock.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_DataBlock_H
#define _U3D_DataBlock_H

// Local includes
#include "U3D_BitStream.h"


ML_START_NAMESPACE


class U3DDataBlock // Abstract base class for DataBlockWriter & DataBlockReader
{
public:

  mlU3D::DataBlockFundamental blockType;
  mlU3D::DataBlockFundamental priority;

  //! Standard constructor
  U3DDataBlock();

  //! Standard destructor
  virtual ~U3DDataBlock() {};

  mlU3D::DataBlockFundamental getDataSize();
  mlU3D::DataBlockFundamental getMetaDataSize();

};


ML_END_NAMESPACE

#endif // _U3D_DataBlock_H
