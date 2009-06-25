//! The ML module class PlaneClip to intersect an image with a plane.
//! \file   AnalyzeHeader.h
//! \author Bart De Dobbelaer
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

////////////////////////////////////////////////////////////////////////

#ifndef __mlPlaneClip_H__
#define __mlPlaneClip_H__

#include "../MLImageUtilitiesSystem.h"
#include "mlOperatorIncludes.h"

#include "mlPlaneField.h"

ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

//! The ML module class PlaneClip to intersect an image with a plane.
class MLIMAGEUTILITIES_EXPORT PlaneClip : public BaseOp {
public:
  PlaneClip();
  virtual void handleNotification (Field *field);
  virtual void activateAttachments();
  virtual void calcOutImageProps (int outIndex);
  virtual SubImgBox calcInSubImageBox (int inIndex, const SubImgBox &outSubImgBox, int outIndex);
  virtual void calcOutSubImage (SubImg *outSubImg, int outIndex, SubImg *inSubImgs);
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int outIndex,
                        TSubImg<T> *inSubImg);
private:
  PlaneField *_planeFld;
  BoolField *_autoApplyFld;
  NotifyField *_applyFld;
  ML_BASEOP_CLASS_HEADER(PlaneClip)
};

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

#endif
