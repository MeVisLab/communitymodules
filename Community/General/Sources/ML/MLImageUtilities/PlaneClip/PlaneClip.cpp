
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

#include "PlaneClip.h"

ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

ML_BASEOP_CLASS_SOURCE(PlaneClip, BaseOp);

PlaneClip::PlaneClip() : BaseOp(1, 2) {
  ML_TRACE_IN("PlaneClip::PlaneClip()")
  handleNotificationOff();
  
  (_planeFld = (PlaneField *)(getFieldContainer()->addField("plane","PlaneField","0,0,1,0")))->setPlaneValue(Plane());

  (_autoApplyFld = getFieldContainer()->addBool("autoApply"))->setBoolValue(false);
  _applyFld = getFieldContainer()->addNotify("apply");
        
  handleNotificationOn();
}

////////////////////////////////////////////////////////////////////////

void PlaneClip::handleNotification (Field *field) {
  ML_TRACE_IN("PlaneClip::handleNotification()")
  if (field == _applyFld || 
      (_autoApplyFld->getBoolValue() && (
        field == _planeFld ||
        field == getInField(0)))
        ) {
		getOutField(0)->notifyAttachments();
		getOutField(1)->notifyAttachments();
  }
  BaseOp::handleNotification(field);
}

////////////////////////////////////////////////////////////////////////

void PlaneClip::activateAttachments () {
  ML_TRACE_IN("PlaneClip::handleNotification()")
  BaseOp::activateAttachments();
}

////////////////////////////////////////////////////////////////////////

void PlaneClip::calcOutImageProps (int outIndex) {
  ML_TRACE_IN("PlaneClip::calcOutImageProps ()")

	PagedImg *inImg=getUpdatedInImg(0);
	if (inImg) {		
		getOutImg(outIndex)->setImgExt(inImg->getImgExt());
		getOutImg(outIndex)->setPageExt(inImg->getPageExt());
		getOutImg(outIndex)->setVoxelSize(inImg->getVoxelSize());
		getOutImg(outIndex)->setMinVoxelValue(inImg->getMinVoxelValue());
		getOutImg(outIndex)->setMaxVoxelValue(inImg->getMaxVoxelValue());
		getOutImg(outIndex)->setDataType(inImg->getDataType());
		getOutImg(outIndex)->setUpToDate();
	} else getOutImg(outIndex)->setOutOfDate();
}

////////////////////////////////////////////////////////////////////////

SubImgBox PlaneClip::calcInSubImageBox (int , const SubImgBox &outSubImgBox, int ) {
  ML_TRACE_IN("PlaneClip::calcInSubImageBox ()")

  return outSubImgBox;
}

////////////////////////////////////////////////////////////////////////

CALC_OUTSUBIMAGE1_CPP(PlaneClip);

template <typename T>
void PlaneClip::calcOutSubImage (TSubImg<T> *outSubImg, int outIndex, TSubImg<T> *inSubImg) {
  ML_TRACE_IN("template <typename T> PlaneClip::calcOutSubImage ()")
                                     
  SubImgBox inbox = inSubImg->getBox();
  SubImgBox outbox = outSubImg->getBox(); 
  inbox = SubImgBox::intersect(inbox, getInImg(0)->getBoxFromImgExt()); 
	Vector p = inbox.v1;                                     
  Plane plane = Plane(); _planeFld->getPlaneValue(plane);  
  vec3 pos;
                                     
	for (p.u = inbox.v1.u;   p.u <= inbox.v2.u;  p.u++) {
    for (p.t = inbox.v1.t;   p.t <= inbox.v2.t;  p.t++) {
      for (p.c = inbox.v1.c;   p.c <= inbox.v2.c;  p.c++) {
        for (p.z = inbox.v1.z;   p.z <= inbox.v2.z;  p.z++) {
          for (p.y = inbox.v1.y;  p.y <= inbox.v2.y;  p.y++) { 
            for (p.x = inbox.v1.x;  p.x <= inbox.v2.x;  p.x++) { 
              pos = vec3(p.x,p.y,p.z);
              if (plane.isInHalfSpace(pos)) {
                outSubImg->setImgVal(p,(outIndex == 0) ? inSubImg->getImgVal(p) : (T)0);
              } else {
                outSubImg->setImgVal(p,(outIndex == 0) ? (T)0 : inSubImg->getImgVal(p));
              }
            }
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
