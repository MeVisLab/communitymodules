//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_StudyIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_StudyIE.h"









ML_START_NAMESPACE


void StudyIE::AddStudyIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  StudyIE::_addGeneralStudyModule(datasetPointer);

  // User optional
  //StudyIE::_addPatientStudyModule(datasetPointer);

  // User optional
  //StudyIE::_addClinicalTrialStudyModule(datasetPointer);
}


void StudyIE::_addGeneralStudyModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.7.2.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Study Instance UID           | (0020,000D) | 1  | Unique identifier for the Study.
  // Add Study Date                   | (0008,0020) | 2  | Date the Study started.
  // Add Study Time                   | (0008,0030) | 2  | Time the Study started.
  // Add Referring Physician's Name   | (0008,0090) | 2  | Name of the patient's referring physician
  // Add Study ID                     | (0020,0010) | 2  | User or equipment generated Study identifier.
  // Add Accession Number             | (0008,0050) | 2  | A RIS generated number that identifies the order for the Study.
  // Add Study Description            | (0008,1030) | 3  | Institution-generated description or classification of the Study (component) performed.
  // Add >Referenced SOP Class UID    | (0008,1150) | 1C | Uniquely identifies the referenced SOP Class. Required if Referenced Study Sequence (0008,1110) is sent.
  // Add >Referenced SOP Instance UID | (0008,1155) | 1C | Uniquely identifies the referenced SOP Instance. Required if Referenced Study Sequence (0008,1110) is sent.

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

