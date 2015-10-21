//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_FrameOfReferenceIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_FrameOfReferenceIE.h"









ML_START_NAMESPACE


void FrameOfReferenceIE::AddFrameOfReferenceIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  FrameOfReferenceIE::_addFrameOfReferenceModule(datasetPointer);
}


void FrameOfReferenceIE::_addFrameOfReferenceModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.7.4.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Frame of Reference UID       | (0020,0052) | 1 | Uniquely identifies the frame of reference for a Series. See C.7.4.1.1.1 for further explanation.
  // Add Position Reference Indicator | (0020,1040) | 2 | Part of the patient's anatomy used as a reference, such as the iliac crest, orbitalmedial, sternal notch, symphysis pubis, xiphoid, lower coastal margin, external auditory meatus. See C.7.4.1.1.2 for further explanation.

  DcmPersonName* PatientNameElement = new DcmPersonName(DCM_PatientName);
  PatientNameElement->putNameComponents("Doe", "John", "", "Dr.", "II.");

  if (datasetPointer->insert(PatientNameElement, true, false).bad())
  {
    std::cout << "Adding patient name (0010,0010) failed.\n";
  }

  DcmLongString* PatientIDElement = new DcmLongString(DCM_PatientID);
  PatientIDElement->putOFStringArray("0123456789");

  if (datasetPointer->insert(PatientIDElement, true, false).bad())
  {
    std::cout << "Adding patient ID (0010,0020) failed.\n";
  }

  DcmDate* PatientBirthDateElement = new DcmDate(DCM_PatientBirthDate);
  PatientBirthDateElement->setOFDate(OFDate(1972,6,26));

  if (datasetPointer->insert(PatientBirthDateElement, true, false).bad())
  {
    std::cout << "Adding patient birth date (0010,0030) failed.\n";
  }

  DcmCodeString* PatientSexElement = new DcmCodeString(DCM_PatientSex);
  PatientSexElement->putOFStringArray("M");

  if (datasetPointer->insert(PatientSexElement, true, false).bad())
  {
    std::cout << "Adding patient sex (0010,0040) failed.\n";
  }
}







ML_END_NAMESPACE

