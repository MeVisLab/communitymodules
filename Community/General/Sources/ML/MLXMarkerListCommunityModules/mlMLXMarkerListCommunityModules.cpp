//----------------------------------------------------------------------------------
//! The ML module class MLXMarkerListCommunityModules.
/*!
// \file    mlMLXMarkerListCommunityModules.cpp
// \author  Coert Metz
// \date    2010-03-05
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlMLXMarkerListCommunityModules.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(MLXMarkerListCommunityModules, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
MLXMarkerListCommunityModules::MLXMarkerListCommunityModules ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("MLXMarkerListCommunityModules::MLXMarkerListCommunityModules ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Connect input field(s) with output field(s) to notify
  // connected modules if input image(s) change.
  for (int inIdx=0; inIdx < 0; ++inIdx){
    for (int outIdx=0; outIdx < 0; ++outIdx){
      getInField(inIdx)->attachField(getOutField(outIdx));
    }
  }

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();

  // Activate inplace data buffers for output outIndex and input inIndex.
  // setOutImgInplace(outIndex, inIndex);

  // Activate page data bypass from input inIdx to output outIdx.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outIndex, inIndex);

  // Activate parallel execution of calcOutSubImage.
  // setThreadSupport(supportMode);
  // with supportMode =
  //   NO_THREAD_SUPPORT                 //! The module is not thread safe at all.
  //   CALC_OUTSUBIMAGE_ON_STD_TYPES     //! calcOutSubImage can be called in parallel for scalar voxel types.
  //   CALC_OUTSUBIMAGE_ON_CARRIER_TYPES //! calcOutSubImage can be called in parallel for carrier voxel types.
  //   CALC_OUTSUBIMAGE_ON_ALL_TYPES     //! calcOutSubImage can be called in parallel for all voxel types.
  // Warning: You should be familiar with multithreading before activating this feature.

  // Specify whether the module can only process standard scalar voxel types or
  // also registered voxel types (vec2, mat2, complexf, Vector, etc.)
  // setVoxelDataTypeSupport(permittedTypes);
  // with permittedTypes =
  //   ONLY_STANDARD_TYPES               //! Only standard scalar voxels are supported.
  //   FULLY_OPERATIONAL                 //! Scalar and registered voxels types are supported.
  //   MINIMUM_OPERATIONAL               //! Scalar and registered voxel types are supported.
  //                                     //! Voxel operators are not used by algorithm.
  //
  // See ML Programming Guide, "Configuring Image Processing Behaviour of the BaseOp"
  // for further details.
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void MLXMarkerListCommunityModules::handleNotification (Field *field)
{
  ML_TRACE_IN("MLXMarkerListCommunityModules::handleNotification ()");

  // Handle changes of module parameters and connectors here.
}

ML_END_NAMESPACE

