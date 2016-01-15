//----------------------------------------------------------------------------------
//! The ML module class PlaneClip.
/*!
// \file    mlPlaneClip.h
// \author  Bart De Dobbelaer, Hugo Kuijf
// \date    2012-07-11
//
// 
*/
//----------------------------------------------------------------------------------
/*
        This program is written by:

            Bart De Dobbelaer

            Laboratory for Medical Imaging Research
            Radiologie/ESAT PSI
            Universitair Ziekenhuis Gasthuisberg
            Herestraat 49
            B-3000 Leuven
            Belgium

        Email: Bart.DeDobbelaer@uz.kuleuven.ac.be
*/

#ifndef __mlPlaneClip_H
#define __mlPlaneClip_H


// Local includes
#include "../MLImageUtilitiesSystem.h"

// ML includes
#include <mlModuleIncludes.h>

ML_START_NAMESPACE


//! Clips an image with the given plane.
class MLIMAGEUTILITIES_EXPORT PlaneClip : public Module
{
public:

  //! Constructor.
  PlaneClip ();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field *field);

  // ----------------------------------------------------------
  //! \name Image processing methods.
  //@{
  // ----------------------------------------------------------

  //! Sets properties of the output image at output \p outputIndex.
  virtual void calculateOutputImageProperties (int outputIndex, PagedImage* outputImage);

  //! Creates the PlaneClipHandler for the given output image whenever calculateOutputImageProperties()
  //! is called and the outputImage is valid.
  virtual CalculateOutputImageHandler* createCalculateOutputImageHandler(PagedImage* outputImage);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! Plane
  PlaneField *_planeFld;
  //! Auto apply changes
  BoolField *_autoApplyFld;
  //! Apply changes when auto-apply is Off
  NotifyField *_applyFld;
  //! 
  EnumField *_intersectionModeFld;
  //!
  DoubleField *_volumeThresholdFld;
  //!
  BoolField *_useGlobalSubsampleFld;
  //!
  IntField *_globalSubsampleFld;
  //!
  IntField *_xSubsampleFld;
  //!
  IntField *_ySubsampleFld;
  //!
  IntField *_zSubsampleFld;
  //@}

  // Make the handler a friend so that it can access the private data
  // in its constructor.
  friend class PlaneClipHandler;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PlaneClip)
};


ML_END_NAMESPACE

#endif // __mlPlaneClip_H