//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    DicomSurfaceSegmentationLoad.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "DicomSurfaceSegmentationLoad.h"
#include "tools/dicom/dicomDatasetParser.h"
#include "tools/dicom/dicomSSODatasetParser.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(DicomSurfaceSegmentationLoad, WEMGenerator);


DicomSurfaceSegmentationLoad::DicomSurfaceSegmentationLoad(std::string type) : WEMGenerator(type)
{
  //Suppress calls of handleNotification on field changes to
  //avoid side effects during initialization phase.
  handleNotificationOff();

  // Init globals
  _datasetPointer.reset();

  // Output fields 
  (_outPointSetPositionsFld  = addBase("outPointSetPositions"))  ->setBaseValueAndAddAllowedType(&_outPointSetPositions);
  (_outLineSetPositionsFld   = addBase("outLineSetPositions"))   ->setBaseValueAndAddAllowedType(&_outLineSetPositions);
  //(_outLineSetConnectionsFld = addBase("outLineSetConnections")) ->setBaseValueAndAddAllowedType(&_outLineSetConnections);

  // UI fields
  _filenameFld    = addString("filename", "");
  _unloadFld      = addNotify("unload");
  _outputValidFld = addBool("outputValid", false);

  _numPointSetsFld      = addInt("numPointSets", 0);
  _numLineSetsFld       = addInt("numLineSets", 0);
  _numMeshesFld         = addInt("numMeshes", 0);
  _numUnknownObjectsFld = addInt("numUnknownObjects", 0);

  _pointSetsInfoFld      = addString("pointSetsInfo", POINTSETINFOCOLUMNS);
  _lineSetsInfoFld       = addString("lineSetsInfo", LINESETINFOCOLUMNS);
  _meshesInfoFld         = addString("meshesInfo", MESHINFOCOLUMNS);
  _unknownObjectsInfoFld = addString("unknownObjectsInfo", UNKNOWNOBJECTINFOCOLUMNS);

  _pointSetsExportMarkerTypeStartValueFld = addInt("pointSetsExportMarkerTypeStartValue", 1);
  _lineSetsExportMarkerTypeStartValueFld  = addInt("lineSetsExportMarkerTypeStartValue", 1);
  _meshesIDStartValueFld                  = addInt("meshesIDStartValueFld", 1);

  _allDicomTagsFld = addString("allDicomTags", "");

  _tagName0Fld = addString("tagName0", "");
  _tagName1Fld = addString("tagName1", "");
  _tagName2Fld = addString("tagName2", "");
  _tagName3Fld = addString("tagName3", "");
  _tagName4Fld = addString("tagName4", "");
  _tagName5Fld = addString("tagName5", "");
  _tagName6Fld = addString("tagName6", "");
  _tagName7Fld = addString("tagName7", "");
  _tagName8Fld = addString("tagName8", "");
  _tagName9Fld = addString("tagName9", "");

  _tagValue0Fld = addString("tagValue0", "");
  _tagValue1Fld = addString("tagValue1", "");
  _tagValue2Fld = addString("tagValue2", "");
  _tagValue3Fld = addString("tagValue3", "");
  _tagValue4Fld = addString("tagValue4", "");
  _tagValue5Fld = addString("tagValue5", "");
  _tagValue6Fld = addString("tagValue6", "");
  _tagValue7Fld = addString("tagValue7", "");
  _tagValue8Fld = addString("tagValue8", "");
  _tagValue9Fld = addString("tagValue9", "");

  //Reactivate calls of handleNotification on field changes
  handleNotificationOn();
}

DicomSurfaceSegmentationLoad::~DicomSurfaceSegmentationLoad()
{
  _unloadFile();
}

void DicomSurfaceSegmentationLoad::activateAttachments()
{
  WEMGenerator::activateAttachments();
}

void DicomSurfaceSegmentationLoad::handleNotification(Field* field)
{
  if (field == _unloadFld)
  {
    _unloadFile();
  }
  /*
  else if (_autoApplyFld->getBoolValue()) 
  {
    _process();
  }
  */

  WEMGenerator::handleNotification(field);
}

void DicomSurfaceSegmentationLoad::_unloadFile()
{
  if(_datasetPointer) 
  {
    _datasetPointer.reset();
  }

  _filename = "";  
  _dicomAllObjectsVector.clear();
  _distributeObjectsToVectors();   

  _updateOutputFields();
  _updateInfoFields();
  _updateDicomTagFields();

  _outputValidFld->setBoolValue(false);
}

void DicomSurfaceSegmentationLoad::_updateInfoFields()
{  
  MLint pointSetsExportMarkerTypeStartValue  = _pointSetsExportMarkerTypeStartValueFld->getIntValue();
  std::string pointSetsInfo = POINTSETINFOCOLUMNS;
  for (std::size_t i=0; i< _dicomPointSetsVector.size(); i++)
  {
    pointSetsInfo += ";" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(pointSetsExportMarkerTypeStartValue + i) + "," + _dicomPointSetsVector[i].getSegmentLabel() + "," + _dicomPointSetsVector[i].getAnatomicRegion();
  }

  MLint lineSetsExportMarkerTypeStartValue  = _lineSetsExportMarkerTypeStartValueFld->getIntValue();
  std::string lineSetsInfo = LINESETINFOCOLUMNS;
  for (std::size_t i=0; i< _dicomLineSetsVector.size(); i++)
  {
    lineSetsInfo += ";" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(lineSetsExportMarkerTypeStartValue + i) + "," + _dicomLineSetsVector[i].getSegmentLabel() + "," + _dicomLineSetsVector[i].getAnatomicRegion();
  }

  MLint meshIDStartValue  = _meshesIDStartValueFld->getIntValue();
  std::string meshesInfo = MESHINFOCOLUMNS;
  for (std::size_t i=0; i < _dicomMeshesVector.size(); i++)
  {
    meshesInfo += ";" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(meshIDStartValue + i) + "," + _dicomMeshesVector[i].getSegmentLabel() + "," + _dicomMeshesVector[i].getAnatomicRegion();
  }

  std::string unknownObjectsInfo = UNKNOWNOBJECTINFOCOLUMNS;
  for (std::size_t i=0; i < _dicomUnknownObjectsVector.size(); i++)
  {
    unknownObjectsInfo += ";" + boost::lexical_cast<std::string>(i) + "," + boost::lexical_cast<std::string>(i) + "," + _dicomUnknownObjectsVector[i].getSegmentLabel() + "," + _dicomUnknownObjectsVector[i].getAnatomicRegion();
  }

  _numPointSetsFld->setIntValue(_dicomPointSetsVector.size());
  _numLineSetsFld->setIntValue(_dicomLineSetsVector.size());
  _numMeshesFld->setIntValue(_dicomMeshesVector.size());
  _numUnknownObjectsFld->setIntValue(_dicomUnknownObjectsVector.size());

  _pointSetsInfoFld->setStringValue(pointSetsInfo);
  _lineSetsInfoFld->setStringValue(lineSetsInfo);
  _meshesInfoFld->setStringValue(meshesInfo);
  _unknownObjectsInfoFld->setStringValue(unknownObjectsInfo);
}

void DicomSurfaceSegmentationLoad::_updateDicomTagFields()
{  
  std::string allTagsString = "";

  if (_dicomAllObjectsVector.size() > 0)
  {
    DicomDatasetParser datasetParser(_datasetPointer);

    allTagsString += "(0008,0018) SOPInstanceUID: " + datasetParser.getStringAttribute(DCM_SOPInstanceUID) + "\n";
    allTagsString += "(0020,000d) StudyInstanceUID: " + datasetParser.getStringAttribute(DCM_StudyInstanceUID) + "\n";
    allTagsString += "(0020,000e) SeriesInstanceUID: " + datasetParser.getStringAttribute(DCM_SeriesInstanceUID) + "\n";
    allTagsString += "(0008,1150) Referenced SOP Class UID : ?\n";
    allTagsString += "(0008,1155) Referenced SOP Instance UID: " + std::string(_dicomAllObjectsVector[0].getReferencedSeriesInstanceId()) + "\n";
  }
    
  _allDicomTagsFld->setStringValue(allTagsString);
}

void DicomSurfaceSegmentationLoad::_updateOutputFields()
{
  handleNotificationOff();

  _outPointSetPositions.clear();
  _outLineSetPositions.clear();
  //_outLineSetConnections.clear();
  _outWEM->removeAll();

  for (std::size_t i=0; i<_dicomPointSetsVector.size(); i++)
  {
    _addPointSetToOutput((int)i);
  } 
  
  for (std::size_t i=0; i<_dicomLineSetsVector.size(); i++)
  {
    _addLineSetToOutput((int)i);
  }
  
  for (std::size_t i=0; i<_dicomMeshesVector.size(); i++)
  {
    _addMeshToOutput((int)i);
  } 
  
  handleNotificationOn();

  _outPointSetPositionsFld->touch();
  _outLineSetPositionsFld->touch();
  //_outLineSetConnectionsFld->touch();
  _notifyObservers();
}

void DicomSurfaceSegmentationLoad::_addPointSetToOutput(const int pointSetNumber)
{
  int pointSetsExportMarkerTypeStartValue = (int)_pointSetsExportMarkerTypeStartValueFld->getIntValue();

  Element3D pointSetElement = _dicomPointSetsVector[pointSetNumber];
  std::vector<Vector3> pointSetPositions = pointSetElement.getReferencedSurface().getCoordinates();
  const std::string segmentLabel = pointSetElement.getSegmentLabel();

  for (size_t p=0; p < pointSetPositions.size(); p++) 
  {
    XMarker pointSetPosition(pointSetPositions[p]);
    pointSetPosition.type = pointSetsExportMarkerTypeStartValue + pointSetNumber;
    pointSetPosition.setName(segmentLabel.c_str());
    _outPointSetPositions.push_back(pointSetPosition);
  }
}

void DicomSurfaceSegmentationLoad::_addLineSetToOutput(const int lineSetNumber)
{
  int lineSetsExportMarkerTypeStartValue  = (int)_lineSetsExportMarkerTypeStartValueFld->getIntValue();

  Element3D lineSetElement = _dicomLineSetsVector[lineSetNumber];
  std::vector<Vector3> lineSetPositions = lineSetElement.getReferencedSurface().getCoordinates();
  const std::string segmentLabel = lineSetElement.getSegmentLabel();
  
  for (size_t l=0; l < lineSetPositions.size(); l++) 
  {
    XMarker lineSetPosition(lineSetPositions[l]);
    lineSetPosition.type = lineSetsExportMarkerTypeStartValue + lineSetNumber;
    lineSetPosition.setName(segmentLabel.c_str());
    _outLineSetPositions.push_back(lineSetPosition);
  }
}

void DicomSurfaceSegmentationLoad::_addMeshToOutput(const int meshNumber)
{
  if(meshNumber >= _dicomMeshesVector.size())
  {
    return;
  }

  int meshesIDStartValue = (int)_meshesIDStartValueFld->getIntValue();

  const Element3D meshElement     = _dicomMeshesVector[meshNumber];

  const std::string segmentLabel  = meshElement.getSegmentLabel();   
  const Coordinates3D meshSurface = meshElement.getReferencedSurface();

  const std::vector<Vector3> wemPositions              = meshSurface.getCoordinates();
  const std::vector<unsigned long> meshPositionIndices = meshSurface.getCoordinateIndices();
  const std::vector<Vector3> meshNormals               = meshSurface.getNormals();

  WEMTrianglePatch* newPatch = NULL;
  ML_CHECK_NEW(newPatch, WEMTrianglePatch());

  // Add WEM nodes
  for (unsigned long p = 0; p < wemPositions.size(); p++) 
  {
    WEMNode* newNode = newPatch->addNode();

    newNode->setPosition(wemPositions[p]);

    if(!meshNormals.empty())
    {
      newNode->setNormal(meshNormals[p]);
    }
    else
    {
      newNode->computeNormal();
    }
  }

  // Add WEM faces
  for (size_t f = 0; f < meshPositionIndices.size(); f += 3) 
  {
    WEMTriangle* newFace = newPatch->addTriangle();

    for (unsigned int i = 0; i < 3; i++) 
    {
      WEMNode* node = newPatch->getNodeAt(meshPositionIndices[f+i]);
      newFace->setNode(i, node);
      node->addFace(newFace);
    }

    newFace->computeNormal();
  }
  
  newPatch->setLabel(segmentLabel);
  //newPatch->setId(_outWEM->getCurrentWEMPatchId());
  newPatch->setId(meshesIDStartValue + meshNumber);
  newPatch->setType(_type);

  ML_NAMESPACE::WEMPrimitiveValueList* primitiveValueList = newPatch->createOrGetPrimitiveValueList("LUT");
  primitiveValueList->setValue(0, static_cast<double>(meshNumber));

  newPatch->buildEdgeConnectivity();
  _outWEM->addWEMPatch(newPatch);
  _finish(newPatch);
}

void DicomSurfaceSegmentationLoad::_distributeObjectsToVectors()
{
  _dicomPointSetsVector.clear();
  _dicomLineSetsVector.clear();
  _dicomMeshesVector.clear();
  _dicomUnknownObjectsVector.clear();    

  for (std::vector<Element3D>::iterator it = _dicomAllObjectsVector.begin();  it != _dicomAllObjectsVector.end();  ++it)
  {
    Element3D thisElement = *it;

    switch(thisElement.getReferencedSurface().getElementType()) 
    {
      case Coordinates3D::ElementType_Marker:
      {
        _dicomPointSetsVector.push_back(thisElement);
        break;
      }
      case Coordinates3D::ElementType_Centerline:
      {
        _dicomLineSetsVector.push_back(thisElement);
        break;
      }
      case Coordinates3D::ElementType_Unknown:
      {
        _dicomUnknownObjectsVector.push_back(thisElement);
        break;
      }
      case Coordinates3D::ElementType_Segmentation:
      {
        _dicomMeshesVector.push_back(thisElement);
        break;
      }
    } // switch
  }

}

bool DicomSurfaceSegmentationLoad::_loadDataset()
{
  bool success = false;

  SmartPtr_DCMDataSet newDataset(new DcmDataset());
  _datasetPointer = newDataset;

  const OFCondition status = _datasetPointer->loadFile(_filename.c_str(), EXS_LittleEndianExplicit);

  if(status.bad()) 
  {
    std::cerr << "Loading the dataset failed: " << status.text() << std::endl;
    throw std::runtime_error(status.text());
  }
  else
  {
    success = true;
  }

  return success;
}

void DicomSurfaceSegmentationLoad::_loadFile()
{
  _filename = _filenameFld->getStringValue();

  _dicomAllObjectsVector.clear();    

  if (_filename != "")
  {
    bool datasetLoaded = _loadDataset();

    if(!datasetLoaded) 
    {
      std::cerr << "Loading the dataset " + _filename + " failed." << std::endl;
      _unloadFile();
      return;
    }

    _parseDataset();
  }
  
  _distributeObjectsToVectors();

  _updateInfoFields();
  _updateDicomTagFields();
  _updateOutputFields();

  if(_datasetPointer) 
  {
    _outputValidFld->setBoolValue(true);
  }
  else
  {
    _outputValidFld->setBoolValue(false);
  }
}

void DicomSurfaceSegmentationLoad::_process()
{
  _startProcessing(); 
  _loadFile();
  _finishProcessing(); 
}




void DicomSurfaceSegmentationLoad::_parseDataset()
{
  _dicomAllObjectsVector.clear();
  std::vector<Element3D> elements;

  try 
  {
    DicomSSODatasetParser datasetParser(_datasetPointer);
    elements = datasetParser.parseDataset(true);
    _dicomAllObjectsVector = elements;
  }
  catch(const std::exception&) 
  {
    std::cerr << "Parsing dataset failed." << std::endl;
  }  

}






// **********************************************
















ML_END_NAMESPACE
