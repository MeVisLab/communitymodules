//----------------------------------------------------------------------------------
// This header files contains definitions for a bit stream encoder for an U3D data 
// block as specified in Standard ECMA-363 (Universal 3D File Format), 4th edition 
// (June 2007) (referred below as "the Standard")
//
// \file    U3D_ContextManager.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_ContextManager_H
#define _U3D_ContextManager_H

// Local includes
#include "../MLU3D_Constants.h"


ML_START_NAMESPACE

typedef std::vector<MLuint16> CountVector;

// The Elephant is a value that determines the number of symbol occurences that are stored in each dynamic histogram.
// Limiting the number of occurences avoids overflow of the U16 array elements and allows the histogram to adapt to
// changing symbol distributions in files.
const MLuint32 Elephant = 0x00001fff;

// The maximum value that is stored in a histogram
const MLuint32 MaximumSymbolInHistogram = 0x0000FFFF;

// The ammount to increase the size of an array when reallocating an array.
const MLuint32 ArraySizeIncrement = 32;


class ContextManager
{
public:

  //! Standard constructor
  ContextManager() {};

  //! Standard destructor
  virtual ~ContextManager() {};

  void     addSymbol(const MLuint32 context, const MLuint32 symbol);
  MLuint32 getSymbolFrequency(const MLuint32 context, const MLuint32 symbol);
  MLuint32 getCumulativeSymbolFrequency(const MLuint32 context, const MLuint32 symbol);
  MLuint32 getTotalSymbolFrequency(const MLuint32 context);
  MLuint32 getSymbolFromFrequency(const MLuint32 context, const MLuint32 symbolFrequency);

private:

  CountVector _symbolCount[mlU3D::Context_StaticFull];     // An array of vectors that stores the number of occurrences of each symbol for each dynamic context.
  CountVector _cumulativeCount[mlU3D::Context_StaticFull]; // An array of vectors that stores the cumulative frequency of each symbol in each context. The value is the number of occurences of a symbol and every symbol with a larger value.

};



ML_END_NAMESPACE

#endif // _U3D_ContextManager_H
