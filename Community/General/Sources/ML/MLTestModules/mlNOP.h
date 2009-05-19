//----------------------------------------------------------------------------------
//! The ML module class NOP.
/*!
// \file    mlNOP.h
// \author  Wolf Spindler
// \date    2009-05-19
//
// No operation module for testing purposes
*/
//----------------------------------------------------------------------------------


#ifndef __mlNOP_H
#define __mlNOP_H


// Local includes
#include "MLTestModulesSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE


//! No operation module for testing purposes
class MLTESTMODULES_EXPORT NOP : public BaseOp
{
public:

  //! Constructor.
  NOP ();

private:


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(NOP)
};


ML_END_NAMESPACE

#endif // __mlNOP_H

