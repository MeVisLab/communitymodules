//----------------------------------------------------------------------------------
//! This abstract class provides a basis for an U3D data block as specified in
//! Standard ECMA-363 (Universal 3D File Format), 4th edition (June 2007) (referred below as "the Standard")
//! Chapter 9
/*!
// \file    U3D_DataBlock.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-01-20
*/
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_DataBlock.h"


ML_START_NAMESPACE


U3DDataBlock::U3DDataBlock()
{
  blockType       = 0;
  priority        = 0;
}


ML_END_NAMESPACE


