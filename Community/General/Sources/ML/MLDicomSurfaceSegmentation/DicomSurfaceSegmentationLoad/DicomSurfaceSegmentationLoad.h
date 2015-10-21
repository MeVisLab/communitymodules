//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    DicomSurfaceSegmentationLoad.h
// \author  Axel Newe, Christian Hoffmann
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __LoadDicomSegmentation_H
#define __LoadDicomSegmentation_H

// Local includes
#include <dcmtk/config/osconfig.h> //make sure OS specific configuration is included first

#include "MLDicomSurfaceSegmentationSystem.h"

#include <boost/current_function.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <dcmtk/dcmdata/dctk.h>
#include <WEMBase/WEM.h>
#include <WEMBase/WEMModuleBase/WEMGenerator.h>

#include "tools/dicom/element3D.h"

#include "DICOMFileFormat/DCM_SharedTypes.h"

//ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <IndexPairList.h>


ML_START_NAMESPACE

#define  POINTSETINFOCOLUMNS         "#,XMarker Type,Label,Anatomic Region"
#define  LINESETINFOCOLUMNS          "#,XMarker Type,Label,Anatomic Region"
#define  MESHINFOCOLUMNS             "#,WEM ID,Label,Anatomic Region"
#define  UNKNOWNOBJECTINFOCOLUMNS    "#,Label,Anatomic Region"


//! 
class MLDICOMSURFACESEGMENTATION_EXPORT DicomSurfaceSegmentationLoad : public WEMGenerator
{
  //! Implements interface for the runtime type system of the ML.
	ML_MODULE_CLASS_HEADER(DicomSurfaceSegmentationLoad); 

public:

  //! Constructor.
	DicomSurfaceSegmentationLoad(std::string="DicomSurfaceSegmentationLoad");

protected:

  //! Destructor.
	virtual ~DicomSurfaceSegmentationLoad();

  //! Initialize module after loading.
  virtual void activateAttachments();
	
  //! Handle field changes of the field Field.
  virtual void handleNotification (Field*);
	
  //! _process()-routine for correct processing.
  virtual void _process();

private:

  // Output fields
	BaseField*   _outPointSetPositionsFld;
	BaseField*   _outLineSetPositionsFld;
  //BaseField*   _outLineSetConnectionsFld;

  // UI fields
	StringField* _filenameFld;

	NotifyField* _unloadFld;
  BoolField*   _outputValidFld;

	IntField*    _numPointSetsFld;
	IntField*    _numLineSetsFld;
	IntField*    _numMeshesFld;
  IntField*    _numUnknownObjectsFld;


	StringField* _pointSetsInfoFld;
	StringField* _lineSetsInfoFld;
	StringField* _meshesInfoFld;
	StringField* _unknownObjectsInfoFld;

	IntField*    _pointSetsExportMarkerTypeStartValueFld;
	IntField*    _lineSetsExportMarkerTypeStartValueFld;
	IntField*    _meshesIDStartValueFld;

  // DICOM tags fields
  StringField* _allDicomTagsFld;

  StringField* _tagName0Fld;
  StringField* _tagName1Fld;
  StringField* _tagName2Fld;
  StringField* _tagName3Fld;
  StringField* _tagName4Fld;
  StringField* _tagName5Fld;
  StringField* _tagName6Fld;
  StringField* _tagName7Fld;
  StringField* _tagName8Fld;
  StringField* _tagName9Fld;

  StringField* _tagValue0Fld;
  StringField* _tagValue1Fld;
  StringField* _tagValue2Fld;
  StringField* _tagValue3Fld;
  StringField* _tagValue4Fld;
  StringField* _tagValue5Fld;
  StringField* _tagValue6Fld;
  StringField* _tagValue7Fld;
  StringField* _tagValue8Fld;
  StringField* _tagValue9Fld;

	// Globals
	XMarkerList _outPointSetPositions;       // Output point positions
	XMarkerList _outLineSetPositions;        // Output line positions
	//IndexPairList _outLineSetConnections;    // Output line connections

	std::string _filename;              // File name including path

	std::vector<Element3D> _dicomPointSetsVector;        // List of all point sets
	std::vector<Element3D> _dicomLineSetsVector;         // List of all line sets
	std::vector<Element3D> _dicomMeshesVector;           // List of all meshes
	std::vector<Element3D> _dicomUnknownObjectsVector;   // List of all unknown objects

  std::vector<Element3D> _dicomAllObjectsVector;

  SmartPtr_DCMDataSet _datasetPointer; 



	// Methods
  void _loadFile();	
  void _unloadFile();
  bool _loadDataset();
  void _parseDataset();
  void _distributeObjectsToVectors();
	void _updateInfoFields();
	void _updateDicomTagFields();
  void _updateOutputFields();
	void _addPointSetToOutput(const int pointSetNumber);
	void _addLineSetToOutput(const int lineSetNumber);
	void _addMeshToOutput(const int meshNumber);
	  
};

ML_END_NAMESPACE

#endif // __LoadDicomSegmentation_H