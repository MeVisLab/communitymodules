//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationSave.
/*!
// \file    DicomSurfaceSegmentationSave.h
// \author  Axel Newe, Christian Hoffmann
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __SaveDicomSegmentation_H
#define __SaveDicomSegmentation_H


// Local includes
#include "MLDicomSurfaceSegmentationSystem.h"
#include "DICOMFileFormat/DCM_SharedTypes.h"
#include "dcmtk/dcmdata/dcostrmf.h"

#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMInspector.h>
//#include <DicomTagInterfaces/mlDicomPatientModuleTagInterface.h>

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

ML_START_NAMESPACE


//! 
class MLDICOMSURFACESEGMENTATION_EXPORT DicomSurfaceSegmentationSave : public WEMInspector
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(DicomSurfaceSegmentationSave)

public:

  //! Constructor.
  DicomSurfaceSegmentationSave (std::string type="DicomSurfaceSegmentationSave");

protected:

  //! Destructor.
  virtual ~DicomSurfaceSegmentationSave();

  //! Initialize module after loading.
  virtual void activateAttachments();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

  //! _process()-routine for correct processing.
  virtual void _process();

private:

  // Input fields for point sets and line sets (WEM input is generated automatically)
  BaseField *_inPointPositionsFld;
  BaseField *_inLinePositionsFld;
  BaseField *_inLineConnectionsFld;
  //BaseField *_inDicomTreeFld;

  // UI fields
  StringField*   _filenameFld;
  StringField*   _statusFld;



  //! The XMarkerList input for point positions of point clouds
  ml::XMarkerList _inPointPositions;  

  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  //! Pointer to the main DCM dataset
  SmartPtr_DCMDataSet _datasetPointer; 

  //! The main processing routine. Here, the own mesh algorithm can be implemented.
  void _saveDicomSurfaceSegmentation();

  void _assembleDicomData();

  void _testDicomTree();

};

ML_END_NAMESPACE

#endif // __mlSaveDicomSegmentation_H