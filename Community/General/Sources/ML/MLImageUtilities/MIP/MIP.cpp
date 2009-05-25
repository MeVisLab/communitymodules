
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

#include "MIP.h"

ML_START_NAMESPACE

ML_BASEOP_CLASS_SOURCE(MIP, BaseOp);

////////////////////////////////////////////////////////////////////////

MIP::MIP (void) : BaseOp(1, 1) {
  ML_TRACE_IN("MIP::MIP()")
	
	FieldContainer *fields = getFieldContainer();

	char **strings = new char*[3];
	strings[0] = "X";
	strings[1] = "Y";
	strings[2] = "Z";
	directionFld = fields->addEnum("direction",strings,3);

	updateFld = fields->addNotify("update");
	autoUpdateFld = fields->addBool("autoUpdate");
	autoUpdateFld->setBoolValue(true);
	updateRequired=false;
}

////////////////////////////////////////////////////////////////////////

void MIP::activateAttachments() {
	if (autoUpdateFld->getBoolValue()) getOutField()->notifyAttachments();
	BaseOp::activateAttachments();
}

////////////////////////////////////////////////////////////////////////

void MIP::handleNotification (Field *field) {
  ML_TRACE_IN("MIP::handleNotification()")
	updateRequired=false;
	if ((field==getInField(0) || field==directionFld) && autoUpdateFld->getBoolValue()) {
		updateRequired=true;
		getOutField()->notifyAttachments();
	}
	if (field==updateFld || (field==autoUpdateFld && autoUpdateFld->getBoolValue())) {
		updateRequired=true;
		getOutField()->notifyAttachments();
	}
}

////////////////////////////////////////////////////////////////////////

void MIP::calcOutImageProps (int ) {
  ML_TRACE_IN("MIP::calcOutImageProps ()")
	PagedImg *inImg=getUpdatedInImg(0);
	if (updateRequired && inImg) {		
		Vector iID = inImg->getImgExt();
		Vector iPD = inImg->getPageExt();
		vec3 iVD = inImg->getVoxelSize();
		switch (directionFld->getEnumValue()) {
			case 0://X
				getOutImg()->setImgExt(Vector(iID.y,iID.z,1,iID.c,iID.t,iID.u));
				getOutImg()->setPageExt(Vector(iID.y,1,1,iPD.c,iPD.t,iPD.u));
				getOutImg()->setVoxelSize(iVD[1],iVD[2],iVD[0]);
			break;
			case 1://Y
				getOutImg()->setImgExt(Vector(iID.x,iID.z,1,iID.c,iID.t,iID.u));
				getOutImg()->setPageExt(Vector(iID.x,1,1,iPD.c,iPD.t,iPD.u));
				getOutImg()->setVoxelSize(iVD[0],iVD[2],iVD[1]);
			break;
			case 2://Z
				getOutImg()->setImgExt(Vector(iID.x,iID.y,1,iID.c,iID.t,iID.u));
				getOutImg()->setPageExt(Vector(iID.x,1,1,iPD.c,iPD.t,iPD.u));
				getOutImg()->setVoxelSize(iVD[0],iVD[1],iVD[2]);
			break;
		}
		getOutImg()->setMinVoxelValue(inImg->getMinVoxelValue());
		getOutImg()->setMaxVoxelValue(inImg->getMaxVoxelValue());
		getOutImg()->setDataType(inImg->getDataType());
		getOutImg()->setUpToDate();
	} else getOutImg()->setOutOfDate();
}

////////////////////////////////////////////////////////////////////////

SubImgBox MIP::calcInSubImageBox (int , const SubImgBox &outSubImgBox, int ) {
  ML_TRACE_IN("MIP::calcInSubImageBox ()")
  SubImgBox inBox = outSubImgBox;      

  //std::cout << "outbox " << inBox.v1.x << " " << inBox.v1.y << " " << inBox.v1.z <<
  //  " to " << inBox.v2.x << " " << inBox.v2.y << " " << inBox.v2.z << std::endl;

	switch (directionFld->getEnumValue()) {
		case 0://X
      inBox.v1.z = getInImg(0)->getImgExt().z-1-inBox.v1.y;
      inBox.v2.z = getInImg(0)->getImgExt().z-1-inBox.v1.y;
			inBox.v1.y = inBox.v1.x;
			inBox.v2.y = inBox.v2.x;
			inBox.v1.x = 0;  
			inBox.v2.x = getInImg(0)->getImgExt().x-1; 
		break;
		case 1://Y
			inBox.v1.z = getInImg(0)->getImgExt().z-1-inBox.v1.y;
			inBox.v2.z = getInImg(0)->getImgExt().z-1-inBox.v1.y;
			inBox.v1.y = 0;  
			inBox.v2.y = getInImg(0)->getImgExt().y-1; 
		break;
		case 2://Z
			inBox.v1.z = 0;  
			inBox.v2.z = getInImg(0)->getImgExt().z-1; 
		break;
	}

  //std::cout << "inbox " << inBox.v1.x << " " << inBox.v1.y << " " << inBox.v1.z <<
  //  " to " << inBox.v2.x << " " << inBox.v2.y << " " << inBox.v2.z << std::endl;

  return inBox; 
}

////////////////////////////////////////////////////////////////////////

CALC_OUTSUBIMAGE1_CPP(MIP);

////////////////////////////////////////////////////////////////////////

template <typename T>
void MIP::calcOutSubImage (TSubImg<T> *outSubImg, int , TSubImg<T> *inSubImg) {
  ML_TRACE_IN("template <typename T> MIP::calcOutSubImage ()")

  SubImgBox inbox = inSubImg->getBox();
  SubImgBox outbox = outSubImg->getBox(); 
  inbox = SubImgBox::intersect(inbox, getInImg(0)->getBoxFromImgExt()); 
	Vector p = inbox.v1;
  Vector q = p;
  q.z = 0;
	switch (directionFld->getEnumValue()) {
		case 0:
			q.y = getInImg(0)->getImgExt().z-1-inbox.v1.z;
		break;
		case 1:
			q.y = getInImg(0)->getImgExt().z-1-inbox.v1.z;
		break;
		case 2:
			q.y = inbox.v1.y;
		break;
	}

	for (p.u = inbox.v1.u;   p.u <= inbox.v2.u;  p.u++,q.u++) {
    for (p.t = inbox.v1.t;   p.t <= inbox.v2.t;  p.t++,q.t++) {
      for (p.c = inbox.v1.c;   p.c <= inbox.v2.c;  p.c++,q.c++) {
			
				switch (directionFld->getEnumValue()) {
					case 0://X
						for (p.y = inbox.v1.y, q.x = inbox.v1.y;  p.y <= inbox.v2.y;  p.y++, q.x++) { 
							T mI = 0;
							for (p.x = inbox.v1.x;  p.x <= inbox.v2.x;  p.x++) { 
								T iV = inSubImg->getImgVal(p);
								mI = (iV > mI) ? iV : mI; 
							}
							outSubImg->setImgVal(q,mI);
						}
					break;
					case 1://Y
						for (p.x = inbox.v1.x, q.x = inbox.v1.x;  p.x <= inbox.v2.x;  p.x++, q.x++) { 
							T mI = 0;
							for (p.y = inbox.v1.y;  p.y <= inbox.v2.y;  p.y++) { 
								T iV = inSubImg->getImgVal(p);
								mI = (iV > mI) ? iV : mI; 
							}
							outSubImg->setImgVal(q,mI);
						}
					break;
					case 2://Z
						for (p.x = inbox.v1.x, q.x = inbox.v1.x; p.x <= inbox.v2.x; p.x++, q.x++) {
							T mI = 0;
							for (p.z = inbox.v1.z; p.z <= inbox.v2.z; p.z++) {
								T iV = inSubImg->getImgVal(p);
								mI = (iV > mI) ? iV : mI; 
							}
							outSubImg->setImgVal(q,mI);
						}
					break;
				}
			
			}//c
		}//t
	}//u
  
}

////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
