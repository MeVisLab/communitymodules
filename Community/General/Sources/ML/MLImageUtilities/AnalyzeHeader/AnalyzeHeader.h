//! The ML module class AnalzeHeader to extract information from Analyze files.
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

#ifndef __mlAnalyzeHeader_H__
#define __mlAnalyzeHeader_H__

#include "../MLImageUtilitiesSystem.h"
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

//! The ML module class AnalzeHeader to extract information from Analyze files.
class MLIMAGEUTILITIES_EXPORT AnalyzeHeader : public BaseOp {
public:
  AnalyzeHeader();
  virtual void handleNotification (Field *field);
  virtual void activateAttachments();
private:
  StringField *_filenameFld;

  Vec4fField *_dimensionsFld;
  Vec4fField *_voxelSizeFld;
  Vec3fField *_originFld;

  FloatField *_funused1Fld;
  FloatField *_funused2Fld;
  FloatField *_funused3Fld;


  void swap(void *s,int bytes);
  bool checkEndianness();

  ML_BASEOP_CLASS_HEADER(AnalyzeHeader)
};

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

#endif
