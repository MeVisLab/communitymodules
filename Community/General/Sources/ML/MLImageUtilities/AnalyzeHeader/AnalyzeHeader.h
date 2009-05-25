
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

class MLIMAGEUTILITIES_EXPORT AnalyzeHeader : public BaseOp {
public:
  AnalyzeHeader();
  virtual void handleNotification (Field *field);
  virtual void activateAttachments();
private:
  StringField *_filenameFld;


  FloatField *_funused1Fld;
  FloatField *_funused2Fld;
  FloatField *_funused3Fld;


  void AnalyzeHeader::swap(void *s,int bytes);
  bool AnalyzeHeader::checkEndianness();

  ML_BASEOP_CLASS_HEADER(AnalyzeHeader)
};

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE

#endif
