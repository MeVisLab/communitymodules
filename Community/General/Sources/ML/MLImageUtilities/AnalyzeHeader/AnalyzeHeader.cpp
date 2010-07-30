
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

#include "AnalyzeHeader.h"

ML_START_NAMESPACE

////////////////////////////////////////////////////////////////////////

ML_BASEOP_CLASS_SOURCE(AnalyzeHeader, BaseOp);

AnalyzeHeader::AnalyzeHeader() : BaseOp(0, 0) {
  ML_TRACE_IN("AnalyzeHeader::AnalyzeHeader()")
  handleNotificationOff();
  
  (_filenameFld = getFieldContainer()->addString("filename"))->setStringValue("");

  (_funused1Fld = getFieldContainer()->addFloat("funused1"))->setFloatValue(0.0f);
  (_funused2Fld = getFieldContainer()->addFloat("funused2"))->setFloatValue(0.0f);
  (_funused3Fld = getFieldContainer()->addFloat("funused3"))->setFloatValue(0.0f);

  (_dimensionsFld = getFieldContainer()->addVec4f("dimensions"))->setVec4fValue(vec4(0.0f,0.0f,0.0f,0.0f));
  (_voxelSizeFld = getFieldContainer()->addVec4f("voxelSize"))->setVec4fValue(vec4(0.0f,0.0f,0.0f,0.0f));
  (_originFld = getFieldContainer()->addVec3f("origin"))->setVec3fValue(vec3(0.0f,0.0f,0.0f));
        
  handleNotificationOn();
}

////////////////////////////////////////////////////////////////////////

void AnalyzeHeader::swap(void *s,int bytes) {
  unsigned char *tp,temp;
  tp=(unsigned char *)s;
  if (bytes==2) {
    temp=tp[0];tp[0]=tp[1];tp[1]=temp;
  }
  if (bytes==4) {
    temp=tp[0];tp[0]=tp[3];tp[3]=temp;
    temp=tp[1];tp[1]=tp[2];tp[2]=temp;
  }
  if (bytes==8) {
    temp=tp[0];tp[0]=tp[7];tp[7]=temp;
    temp=tp[1];tp[1]=tp[6];tp[6]=temp;
    temp=tp[2];tp[2]=tp[5];tp[5]=temp;
    temp=tp[3];tp[3]=tp[4];tp[4]=temp;
  }
}

////////////////////////////////////////////////////////////////////////

bool AnalyzeHeader::checkEndianness() {
  char *ec=0;
  int et=1;
  
  ec=(char *)&et;
  if (*ec) return true; else return false;
}

////////////////////////////////////////////////////////////////////////

void AnalyzeHeader::handleNotification (Field *field) {
  ML_TRACE_IN("AnalyzeHeader::handleNotification()")
  if (field == _filenameFld) {

    std::string str = _filenameFld->getStringValue();
    char filename[1000];
    sprintf(filename,str.c_str());
      
    FILE *file;
    int nr,i;
    unsigned short d[8];
    bool swapendian;

    int hdrsize,extents,glmax,glmin,views,vols_added,start_field,field_skip,omax,
      omin,smax,smin;
	  unsigned char regular,hkey_un0,orient,vox_units[4],cal_units[8];
    char data_type[10],db_name[18],descrip[80],aux_file[24],generated[10],scannum[10],patient_id[10],exp_date[10],
      exp_time[10],hist_un0[3];
    unsigned short session_error,unused1,datatypean,bitpix,dim_un0,origin[5];
    float pixdim[8],vox_offset,cal_max,cal_min,compressed,verified;

    float funused1 = 0.0f, funused2 = 0.0f, funused3 = 0.0f;
    for (i = 0;i < 8; i ++) {
      d[i] = 0;
      pixdim[i] = 0.0f;
    }


    file=fopen(filename,"rb");
    if (file!=NULL) {
      nr=fread(&hdrsize,4,1,file);
     
      //we presume that the hdr-files are not corrupted -> size always 348
      //check itmp for endian swap
      if ((checkEndianness() && hdrsize!=348) ||
          (!checkEndianness() && hdrsize==348))
        swapendian=true; else swapendian=false;
     
      if (swapendian) swap(&hdrsize,4);
      if (nr > 0 && hdrsize == 348) {       
        fread(&data_type[0],10,1,file);
        fread(&db_name[0],18,1,file);
        fread(&extents,4,1,file);if (swapendian) swap(&extents,4);
        fread(&session_error,2,1,file);if (swapendian) swap(&session_error,2);
        fread(&regular,1,1,file);
        fread(&hkey_un0,1,1,file);       
        for (i=0;i<8;i++) {
          fread(&d[i],2,1,file);if (swapendian) swap(&d[i],2);
        }
        fread(&vox_units[0],4,1,file);
        fread(&cal_units[0],8,1,file);
        fread(&unused1,2,1,file);if (swapendian) swap(&unused1,2);
        fread(&datatypean,2,1,file);if (swapendian) swap(&datatypean,2);
        fread(&bitpix,2,1,file);if (swapendian) swap(&bitpix,2);
        fread(&dim_un0,2,1,file);if (swapendian) swap(&dim_un0,2);
        for (i=0;i<8;i++) {
          fread(&pixdim[i],4,1,file);if (swapendian) swap(&pixdim[i],4);
        }
        fread(&vox_offset,4,1,file);if (swapendian) swap(&vox_offset,4);
        fread(&funused1,4,1,file);if (swapendian) swap(&funused1,4);
        fread(&funused2,4,1,file);if (swapendian) swap(&funused2,4);
        fread(&funused3,4,1,file);if (swapendian) swap(&funused3,4);
        fread(&cal_max,4,1,file);if (swapendian) swap(&cal_max,4);
        fread(&cal_min,4,1,file);if (swapendian) swap(&cal_min,4);
        fread(&compressed,4,1,file);if (swapendian) swap(&compressed,4);
        fread(&verified,4,1,file);if (swapendian) swap(&verified,4);
        fread(&glmax,4,1,file);if (swapendian) swap(&glmax,4);
        fread(&glmin,4,1,file);if (swapendian) swap(&glmin,4);
       
        fread(&descrip[0],80,1,file);
        fread(&aux_file[0],24,1,file);
        fread(&orient,1,1,file);
        for (i=0;i<5;i++) {
          fread(&origin[i],2,1,file);if (swapendian) swap(&origin[i],2);
        };
        fread(&generated[0],10,1,file);
        fread(&scannum[0],10,1,file);
        fread(&patient_id,10,1,file);
        fread(&exp_date,10,1,file);
        fread(&exp_time,10,1,file);
        fread(&hist_un0,3,1,file);
        fread(&views,4,1,file);if (swapendian) swap(&views,4);
        fread(&vols_added,4,1,file);if (swapendian) swap(&vols_added,4);
        fread(&start_field,4,1,file);if (swapendian) swap(&start_field,4);
        fread(&field_skip,4,1,file);if (swapendian) swap(&field_skip,4);
        fread(&omax,4,1,file);if (swapendian) swap(&omax,4);
        fread(&omin,4,1,file);if (swapendian) swap(&omin,4);
        fread(&smax,4,1,file);if (swapendian) swap(&smax,4);
        fread(&smin,4,1,file);if (swapendian) swap(&smin,4);
       
      }
      fclose(file);
    }

    _funused1Fld->setFloatValue(funused1);
    _funused2Fld->setFloatValue(funused2);
    _funused3Fld->setFloatValue(funused3);
    _dimensionsFld->setVec4fValue(vec4(d[1],d[2],d[3],d[4]));
    _voxelSizeFld->setVec4fValue(vec4(pixdim[1],pixdim[2],pixdim[3],pixdim[4]));
    _originFld->setVec3fValue(vec3(origin[0],origin[1],origin[2]));

  }

  BaseOp::handleNotification(field);
}

////////////////////////////////////////////////////////////////////////

void AnalyzeHeader::activateAttachments () {
  ML_TRACE_IN("AnalyzeHeader::handleNotification()")
  BaseOp::activateAttachments();
}

////////////////////////////////////////////////////////////////////////


ML_END_NAMESPACE
