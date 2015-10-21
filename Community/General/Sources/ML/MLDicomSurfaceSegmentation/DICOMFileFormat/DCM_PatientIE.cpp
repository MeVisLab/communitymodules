//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_PatientIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_PatientIE.h"









ML_START_NAMESPACE


void PatientIE::AddPatientIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  PatientIE::_addPatientModule(datasetPointer);

  // User optional
  //PatientIE::_addSpecimenModule(datasetPointer);

  // User optional
  //PatientIE::_addClinicalTrialSubjectModule(datasetPointer);
}


void PatientIE::_addPatientModule(SmartPtr_DCMDataSet datasetPointer)
{ 
  // Reference: C.7.1.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Patient's Name               | (0010,0010) | 2  | Patient's full name.
  // Add Patient ID                   | (0010,0020) | 2  | Primary hospital identification number or code for the patient.
  // Add Patient's Birth Date         | (0010,0030) | 2  | Birth date of the patient.
  // Add Patient's Sex                | (0010,0040) | 2  | Sex of the named patient. Enumerated Values: M = male F = female O = other
  // Add >Referenced SOP Class UID    | (0008,1150) | 1C | Uniquely identifies the referenced SOP Class. Required if Referenced Patient Sequence (0008,1120) is sent.
  // Add >Referenced SOP Instance UID | (0008,1155) | 1C | Uniquely identifies the referenced SOP Instance. Required if Referenced Patient Sequence (0008,1120) is sent.
  // Add Patient Comments             | (0010,4000) | 3  | User-defined additional information about the patient.

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

