//----------------------------------------------------------------------------------
// This header file contains the class definition for an U3D object
//
// \file    U3D_Object_Modifiers.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//----------------------------------------------------------------------------------


#ifndef _U3D_Object_Modifiers_H
#define _U3D_Object_Modifiers_H


// Local includes
#include "MLU3DSystem.h"

// Global includes
#include <mlModuleIncludes.h>


ML_START_NAMESPACE

namespace mlU3D {

  // ****************************************************************************************************
  //
  // Class ModifierBase: base class for all geometry generators
  //
  // ****************************************************************************************************

  class MLU3D_EXPORT ModifierBase : public RefCountedBase
  {
  public:
    //! Construct a base object where all properties set to default values.
    ModifierBase();

    //! Destructor, usually called automatically when the last RefCountedBase reference is removed.
    virtual ~ModifierBase();


  protected:


  private:

    //! Implements interface for the runtime type system of the ML.
    ML_CLASS_HEADER(ModifierBase);


  };

  //! Declares the smart pointer U3DObjectPtr from ModifierBase.
  ML_REFCOUNTED_PTR(ModifierBase);






} // end namespace mlU3D 

ML_END_NAMESPACE

#endif // _U3D_Object_Modifiers_H
