//----------------------------------------------------------------------------------
//! The ML module class MLXMarkerListCommunityModules.
/*!
// \file    mlMLXMarkerListCommunityModules.h
// \author  Coert Metz
// \date    2010-03-05
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __mlMLXMarkerListCommunityModules_H
#define __mlMLXMarkerListCommunityModules_H


// Local includes
#include "EMCMLXMarkerListCommunityModulesSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE


//! 
class EMCMLXMARKERLISTCOMMUNITYMODULES_EXPORT MLXMarkerListCommunityModules : public BaseOp
{
public:

  //! Constructor.
  MLXMarkerListCommunityModules ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

private:


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(MLXMarkerListCommunityModules)
};


ML_END_NAMESPACE

#endif // __mlMLXMarkerListCommunityModules_H

