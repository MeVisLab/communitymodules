//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_EquipmentIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_EquipmentIE.h"









ML_START_NAMESPACE


void EquipmentIE::AddEquipmentIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  EquipmentIE::_addGeneralEquipmentModule(datasetPointer);

  // Mandatory
  EquipmentIE::_addEnhancedGeneralEquipmentModule(datasetPointer);

}


void EquipmentIE::_addGeneralEquipmentModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.7.5.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Manufacturer        | (0008,0070) | 2 | Manufacturer of the equipment that produced the composite instances.
  // Add Institution Name    | (0008,0080) | 3 | Institution where the equipment that produced the composite instances is located.
  // Add Institution Address | (0008,0081) | 3 | Mailing address of the institution where the equipment that produced the composite instances is located.
  // Add Software Versions   | (0018,1020) | 3 | Manufacturer’s designation of software version of the equipment that produced the composite instances.

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

void EquipmentIE::_addEnhancedGeneralEquipmentModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.7.5.2
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // This table contains a subset of the attributes of General Equipment Module (Table C.7-8) but the Type Designation is changed into Type 1. 
  // Including this module in an IOD overwrites the Type Designation of the General Equipment Module. 

  // Add Manufacturer 	            | (0008,0070) | 1	| Manufacturer of the equipment that produced the composite instances.
  // Add Manufacturer’s Model Name 	| (0008,1090) | 1	| Manufacturer’s model name of the equipment that produced the composite instances.
  // Add Device Serial Number 	    | (0018,1000) | 1	| Manufacturer’s serial number of the equipment that produced the composite instances.
  // Add Software Versions 	        | (0018,1020) | 1	| Manufacturer’s designation of software version of the equipment that produced the composite instances. See Section C.7.5.1.1.3. 

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

