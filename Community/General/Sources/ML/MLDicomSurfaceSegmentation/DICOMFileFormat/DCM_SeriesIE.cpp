//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_SeriesIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_SeriesIE.h"









ML_START_NAMESPACE


void SeriesIE::AddSeriesIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  SeriesIE::_addGeneralSeriesModule(datasetPointer);

  // Mandatory
  SeriesIE::_addSegmentationSeriesModule(datasetPointer);

  // User optional
  //SeriesIE::_addClinicalTrialSeriesModule(datasetPointer);
}


void SeriesIE::_addGeneralSeriesModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.7.3.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Modality                           | (0008,0060) | 1  | Type of equipment that originally acquired the data used to create the images in this Series. See C.7.3.1.1.1 for Defined Terms.
  // Add Series Instance UID                | (0020,000E) | 1  | Unique identifier of the Series.
  // Add Series Number                      | (0020,0011) | 2  | A number that identifies this Series.
  // Add Laterality                         | (0020,0060) | 2C | Laterality of (paired) body part examined. Required if the body part examined is a paired structure and Image Laterality (0020,0062) or Frame Laterality (0020,9072) are not sent. Enumerated Values: R = right L = left
  // Add Series Date                        | (0008,0021) | 3  | Date the Series started.
  // Add Series Time                        | (0008,0031) | 3  | Time the Series started.
  // Add Protocol Name                      | (0018,1030) | 3  | User-defined description of the conditions under which the Series was performed.
  // Add Series Description                 | (0008,103E) | 3  | User provided description of the Series
  // Add Related Series Sequence            | (0008,1250) | 3  | Identification of Series significantly related to this Series. Zero or more Items may be present.
  // Add >Study Instance UID                | (0020,000D) | 1C | Instance UID of Study to which the related Series belongs
  // Add >Series Instance UID               | (0020,000E) | 1C | Instance UID of Related Series
  // Add Purpose of Reference Code Sequence | (0040,A170) | 2  | Describes the purpose for which the reference is made. Zero or more Items may be present. When absent, implies that the reason for the reference is unknown.
  //     >>Include Code Sequence Macro Table 8.8-1 DCID 7210
  // Add Body Part Examined                 (0018,0015) 3 Text description of the part of the body examined. Defined Terms: SKULL, CSPINE, TSPINE, LSPINE, SSPINE, COCCYX, CHEST, CLAVICLE, BREAST, ABDOMEN, PELVIS, HIP, SHOULDER, ELBOW, KNEE, ANKLE, HAND, FOOT, EXTREMITY, HEAD, HEART, NECK, LEG, ARM, JAW
  //   Note: Some IODs support the Anatomic Region Sequence (0008,2218), which can provide a more comprehensive mechanism for specifying the body part being examined.

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

void SeriesIE::_addSegmentationSeriesModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.8.20.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Modality                                     | (0008,0060) | 1  | Modality Type Enumerated Value: SEG
  // Add Series Number                                | (0020,0011) | 1  | A number that identifies this Series
  // Add Referenced Performed Procedure Step Sequence | (0008,1111) | 1C | Uniquely identifies the Performed Procedure Step SOP Instance to which the Series is related (e.g. a Modality or General-Purpose Performed Procedure Step SOP). 
  //   Only a single Item is permitted in this sequence. Required if the SOP Instance was created in a workflow managed with the Modality Performed Procedure Step SOP Class or General Purpose Performed Procedure Step SOP Class.
  // Add >Referenced SOP Class UID                    | (0008,1150) | 1  | Uniquely identifies the referenced SOP Class.
  // Add >Referenced SOP Instance UID                 | (0008,1155) | 1  | Uniquely identifies the referenced SOP Instance.

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

