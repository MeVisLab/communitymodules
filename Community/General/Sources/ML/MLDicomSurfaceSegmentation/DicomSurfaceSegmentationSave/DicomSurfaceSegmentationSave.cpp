//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationSave.
/*!
// \file    DicomSurfaceSegmentationSave.cpp
// \author  Axel
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------


// Local includes
#include "DicomSurfaceSegmentationSave.h"
#include "DICOMFileFormat/DCM_PatientIE.h"
#include "DICOMFileFormat/DCM_SeriesIE.h"
#include "DICOMFileFormat/DCM_StudyIE.h"
#include "DICOMFileFormat/DCM_EquipmentIE.h"
#include "DICOMFileFormat/DCM_FrameOfReferenceIE.h"
#include "DICOMFileFormat/DCM_SurfaceIE.h"

// ML includes
//#include "mlDicomTreeRefCounted.h"



ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(DicomSurfaceSegmentationSave, WEMInspector);

//***********************************************************************************

DicomSurfaceSegmentationSave::DicomSurfaceSegmentationSave (std::string type) : WEMInspector(type,1)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  _datasetPointer.reset();

  // Input fields
  // Add input fields to the module and set allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);
  _inPointPositionsFld->addAllowedType<ml::ColoredMarkerList>();
  _inLinePositionsFld->addAllowedType<ml::ColoredMarkerList>();
  //(_inDicomTreeFld        = addBase("inDicomTree"))->addAllowedType<DicomTreeRefCounted>();

  // Add UI fields
  _filenameFld = addString("filename", "");
  _statusFld   = addString("status", "");

  _autoApplyFld->setBoolValue(false);
  _autoUpdateFld->setBoolValue(false);  
  _autoClearFld->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

DicomSurfaceSegmentationSave::~DicomSurfaceSegmentationSave()
{
  // destroy own dynamic data structures here
  if(_datasetPointer) 
  {
  	_datasetPointer.reset();
  }
}

//***********************************************************************************

void DicomSurfaceSegmentationSave::handleNotification (Field* field)
{
  // ... field notification handling code

  // if a field change should lead to a new computation, use
  // the following:

  // if (field == 'myField'){
  //    if (_autoApplyFld->getBoolValue()){ // this auto apply field is provided by the base class
  //      ...
  //    }
  // }


  // call parent class and handle apply/autoApply and in/outputs
  WEMInspector::handleNotification(field);
}

//***********************************************************************************

void DicomSurfaceSegmentationSave::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMInspector::activateAttachments();
}

//***********************************************************************************

void DicomSurfaceSegmentationSave::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  WEMInspector::_process();

  // Now call the own processing routine.
  _saveDicomSurfaceSegmentation();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();

}

//***********************************************************************************

void DicomSurfaceSegmentationSave::_saveDicomSurfaceSegmentation()
{
  std::string filename = _filenameFld->getStringValue();
  if (filename == "") 
  {
    _statusFld->setStringValue("No filename specified.");
    return;
  }

  const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

  std::string last4 = "";

  if (filenameLength > 4) 
  { 
    last4 = filename.substr(filenameLength-4, 4); 

    if (last4 != ".dcm") 
    { 
      filename.append(".dcm"); 
      _filenameFld->setStringValue(filename);
    }
  }

  std::string status;

  _assembleDicomData();

  DcmDataset* writeDatasetPtr = _datasetPointer.get();
  DcmFileFormat outputFile(writeDatasetPtr);
  
  if (outputFile.saveFile(filename.c_str(), EXS_LittleEndianImplicit).good())
  {
    status = "DICOM Surface Segmentation file saved.";
  }
  else
  {
    status = "Saving DICOM Surface Segmentation failed.";
  }  _statusFld->setStringValue(status);

  _datasetPointer->clear();

}

//***********************************************************************************

void DicomSurfaceSegmentationSave::_assembleDicomData()
{
  SmartPtr_DCMDataSet newDataset(new DcmDataset());
  _datasetPointer = newDataset;

  PatientIE::AddPatientIE(_datasetPointer);                     // C.7.1   Common Patient IE Modules
  StudyIE::AddStudyIE(_datasetPointer);                         // C.7.2   Common Study IE Modules
  SeriesIE::AddSeriesIE(_datasetPointer);                       // C.7.3   Common Series IE Modules & C.8.20.1 Segmentation Series Module
  FrameOfReferenceIE::AddFrameOfReferenceIE(_datasetPointer);   // C.7.4.1 Frame Of Reference Module
  EquipmentIE::AddEquipmentIE(_datasetPointer);                 // C.7.5   Common Equipment IE Modules
  SurfaceIE::AddSurfaceIE(_datasetPointer);                     // C.8.X.1 SurfaceSegmentation, C.X.1 Surface Mesh, C.12.1 SOP Common Module, C.12.2 Common Instance Reference
}


void DicomSurfaceSegmentationSave::_testDicomTree()
{


  if (_datasetPointer)
  {
	  DcmStack stack;
	  if(_datasetPointer->search(DCM_PatientName, stack).good())
    {
      if (stack.empty())
      {
        int j = 0;
        j++;
      }
      else
      {
        DcmObject* topObject = stack.pop();

        if (topObject)
        {
          DcmPersonName* ExtractedPersonName = static_cast<DcmPersonName*>(topObject);
          DcmPersonName StaticPersonName = *ExtractedPersonName;

          OFString formattedName;
          StaticPersonName.getFormattedName(formattedName);

          if (formattedName.empty())
          {
            int k = 0;
            k++;
          }
        }
      }
    }

  }

}

ML_END_NAMESPACE