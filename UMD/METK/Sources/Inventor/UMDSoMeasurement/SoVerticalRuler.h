//! Inventor node class SoVerticalRuler derived from SoSeparator.
//! \file SoVerticalRuler.h

#ifndef __SOVERTICALRULER_H
#define __SOVERTICALRULER_H

#include "UMDMeasurementInit.h"

#include <Inventor/fields/SoFields.h>

//! Inventor node class SoVerticalRuler derived from SoSeparator.
class SO_MEASUREMENT_CLASS_SPEC SoVerticalRuler : public SoSeparator {

  SO_NODE_HEADER(SoVerticalRuler);

public:

  SoSFColor color;

  //! inventor runtime type system
  static void   initClass();

  //! Constructor
  SoVerticalRuler();

protected:
   //! Protected destructor
   virtual      ~SoVerticalRuler();
   virtual void GLRender(SoGLRenderAction* action);
   virtual void GLRenderBelowPath(SoGLRenderAction* action);
};

#endif // __SOVERTICALRULER_H
