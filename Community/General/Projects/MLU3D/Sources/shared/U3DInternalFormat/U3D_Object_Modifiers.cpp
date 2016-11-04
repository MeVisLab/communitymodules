//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_Modifiers.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


// Local includes
#include "U3D_Object_Modifiers.h"


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class ModifierBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  //! Implements code for the runtime type system of the ML
  ML_CLASS_SOURCE(mlU3D::ModifierBase, RefCountedBase);

  //----------------------------------------------------------------------------------

  ModifierBase::ModifierBase() : RefCountedBase()
  {
    
  }

  //----------------------------------------------------------------------------------

  ModifierBase::~ModifierBase()
  {
  }

  //----------------------------------------------------------------------------------






  //----------------------------------------------------------------------------------

} // end namespace mlU3D

ML_END_NAMESPACE
