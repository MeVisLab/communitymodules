
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

#ifndef __MIP_H
#define __MIP_H

#include "../MLImageUtilitiesSystem.h"
#include "mlOperatorIncludes.h"

ML_START_NAMESPACE

class MLIMAGEUTILITIES_EXPORT MIP : public BaseOp {
public:
  MIP (void);
  inline EnumField *getDirectionField() {return directionFld;}
	inline NotifyField *getUpdateField() {return updateFld;}
	inline BoolField *getAutoUpdateField() {return autoUpdateFld;}
  virtual void handleNotification (Field *field);
	virtual void activateAttachments();
  virtual void calcOutImageProps (int );
  virtual SubImgBox calcInSubImageBox (int , const SubImgBox &outSubImgBox, int );
  virtual void calcOutSubImage (SubImg *outSubImg, int , SubImg *inSubImgs);
  template <typename T>
  void calcOutSubImage (TSubImg<T> *outSubImg, int , TSubImg<T> *inSubImg);
private:
	EnumField *directionFld;
	NotifyField *updateFld;
	BoolField *autoUpdateFld;
	bool updateRequired;
  ML_BASEOP_CLASS_HEADER(MIP)
};

ML_END_NAMESPACE

#endif
