//----------------------------------------------------------------------------------
//! The ML module class SavePRC.
/*!
// \file    SavePRC.cpp
// \author  Axel Newe
// \date    2014-02-06
//
// Creates PRC file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "PRCFileFormat/PRC_File.h"
#include "PRCFileFormat/PRC_Tools.h"

#include <ColoredMarkerList.h>

// ML includes
#include "mlVersion.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(SavePRC, WEMInspector);


SavePRC::SavePRC (std::string type)
  : WEMInspector(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);

  // Add fields for selecting simple, straightforward mode
  (_simpleModePointSetFld = addBool("simpleModePointSet"))->setBoolValue(false);
  (_simpleModeLineSetFld = addBool("simpleModeLineSet"))->setBoolValue(false);
  (_simpleModeMeshFld = addBool("simpleModeMesh"))->setBoolValue(false);

  // Add fields to the module and set their values.
  (_modelPrefixPointCloudsFld = addString("modelPrefixPointClouds"))->setStringValue("PointSet");
  (_modelPrefixLineSetsFld    = addString("modelPrefixLineSets"))->setStringValue("LineSet");
  (_modelPrefixMeshesFld      = addString("modelPrefixMeshes"))->setStringValue("Mesh");
  //(_modelPrefixGlyphsFld      = addString("modelPrefixGlyphs"))->setStringValue("Glyph");       // Not supported by Acrobat

  (_pointCloudSpecificationFld = addString("pointCloudSpecification"))->setStringValue("");
  (_lineSetSpecificationFld    = addString("lineSetSpecification"))->setStringValue("");
  (_meshSpecificationFld       = addString("meshSpecification"))->setStringValue("");
  //(_glyphSpecificationFld      = addString("glyphSpecification"))->setStringValue("");          // Not supported by Acrobat
  //(_viewsSpecificationFld      = addString("viewsSpecification"))->setStringValue("");
  //(_lightsSpecificationFld     = addString("lightsSpecification"))->setStringValue("");
  //(_metaDataSpecificationFld   = addString("metaDataSpecification"))->setStringValue("");

  //(_defaultViewNameFld     = addString("defaultViewName"))->setStringValue("DefaultView");
  //(_defaultLightNameFld    = addString("defaultLightName"))->setStringValue("DefaultAmbientLight");

  //(_defaultMaterialDiffuseColorFld  = addColor("defaultMaterialDiffuseColor"))->setVector3Value(Vector3(0.65f,0.65f,0.65f));
  //(_defaultMaterialSpecularColorFld = addColor("defaultMaterialSpecularColor"))->setVector3Value(Vector3(0.75f,0.75f,0.75f));
  //(_defaultMaterialAmbientColorFld  = addColor("defaultMaterialAmbientColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  //(_defaultMaterialEmissiveColorFld = addColor("defaultMaterialEmissiveColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  //(_defaultMaterialTransparencyFld  = addFloat("defaultMaterialTransparency"))->setFloatValue(0.0f);

  //(_defaultLightColorFld = addColor("defaultLightColor"))->setVector3Value(Vector3(1.0f,1.0f,1.0f));
  //(_defaultLightIntensityFld = addDouble("defaultLightIntensity"))->setDoubleValue(1.0f);

  //(_defaultBoundingBoxMetaDataFld = addBool("defaultBoundingBoxMetaData"))->setBoolValue(true);
  //(_addDefaultViewNodeFld         = addBool("addDefaultViewNode"))->setBoolValue(true);
  //(_addDefaultLightNodeFld        = addBool("addDefaultLightNode"))->setBoolValue(true);

  (_mlFileNameFld = addString("filename"))->setStringValue("");

  _mlSaveFld = addNotify("save");

  (_statusFld     = addString("status"))    ->setStringValue("Idle.");
  (_progressFld   = addProgress("progress"))->setFloatValue(0.0f);

  // Turn off the automatic saving on all notifications
  _listenToFinishingNotificationsFld       ->setBoolValue(false);
  _listenToRepaintNotificationsFld         ->setBoolValue(false);
  _listenToSelectionChangedNotificationsFld->setBoolValue(false);

  _autoUpdateFld->setBoolValue(false);
  _autoApplyFld->setBoolValue(false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//***********************************************************************************

SavePRC::~SavePRC()
{
  // destroy own dynamic data structures here
}

//***********************************************************************************

void SavePRC::handleNotification (Field* field)
{
  if (field == _mlSaveFld) 
  {
    saveButtonClicked();
  } 

  if (field == _inPointPositionsFld)
  {
    ml::Base *inBaseValue = _inPointPositionsFld->getBaseValue();

    if (inBaseValue)
    {
      if (ML_BASE_IS_A(inBaseValue, ml::XMarkerList))
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inBaseValue);
        _inPointPositions.fromXMarkerList(*inList);
      }
      else
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inBaseValue);
         _inPointPositions = *inList;
      }
    }
    else
    {
      _inPointPositions.clear();
    }
  }

  if (field == _inLinePositionsFld)
  {
    ml::Base *inBaseValue = _inLinePositionsFld->getBaseValue();

    if (inBaseValue)
    {
      if (ML_BASE_IS_A(inBaseValue, ml::XMarkerList))
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inBaseValue);
        _inLinePositions.fromXMarkerList(*inList);
      }
      else
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inBaseValue);
         _inLinePositions = *inList;
      }
    }
    else
    {
      _inLinePositions.clear();
    }
  }

  if (field == _inLineConnectionsFld)
  {
    ml::IndexPairList* inList = ((ml::IndexPairList*)_inLineConnectionsFld->getBaseValue());

    if (inList != NULL)
    {
      _inLineConnections = (*inList);
    }
    else
    {
      _inLineConnections.clear();
    }

  }

  // call parent class and handle apply/autoApply and in/outputs
  WEMInspector::handleNotification(field);
}

//***********************************************************************************

void SavePRC::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMInspector::activateAttachments();
}

//***********************************************************************************

void SavePRC::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  WEMInspector::_process();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();

}

//***********************************************************************************

void SavePRC::saveButtonClicked()
{
  std::string filename = _mlFileNameFld->getStringValue();
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

    if (last4 != ".prc") 
    { 
      filename.append(".prc"); 
      _mlFileNameFld->setStringValue(filename);
    }
  }

  std::string status;
  std::ofstream ofstream;

#ifdef WIN32
#if (_MSC_VER >= 1310)
  MLuint16* filename16 = static_cast<MLuint16*>(MLConvertUTF8ToUTF16(filename.c_str()));
#else
  const char* filename16 = filename.c_str();
#endif
  if (filename16 != NULL) 
  {
    ofstream.open( filename16, (std::ios::binary | std::ios::out) );
#if (_MSC_VER >= 1310)
  MLFree(filename16);
#endif
  } 
  else 
  {
    status = "Could not convert filename.";
  }
#else
  ofstream.open( filename.c_str(), (std::ios::binary | std::ios::out) );
#endif

  if (ofstream.is_open())
  {
    savePRCToFileStream(ofstream);
    ofstream.close();
    status = "PRC file saved.";
  } 
  else 
  {
    status = "Could not open specified file.";
  }

  _statusFld->setStringValue(status);

}


//***********************************************************************************

void SavePRC::WriteNodeModelsToPRCFile(PRCFile &outPRCFile, PRCModelTreeNode modelTreeNode)
{
  if (modelTreeNode.ID != 0)   // Do not create a group for root node
  {
    outPRCFile.begingroup(modelTreeNode.Name.c_str());
  }

  WritePointSetDataToPRCFile(outPRCFile, modelTreeNode.ID);
  WriteLineSetDataToPRCFile(outPRCFile, modelTreeNode.ID);
  WriteMeshDataToPRCFile(outPRCFile, modelTreeNode.ID);
  // Write annotations

  for (MLintVector::iterator it = modelTreeNode.ChildrenIDs.begin(); it != modelTreeNode.ChildrenIDs.end(); ++it) 
  {
    MLint thisChildNodeIndex = *it;
    PRCModelTreeNode nextNode = getNodeFromPRCModelTree(_modelTree, thisChildNodeIndex);

    if (nextNode.ID != -1)  
    {
      WriteNodeModelsToPRCFile(outPRCFile, nextNode);
    }
    else  
    {
      // This can happen only if ThisChildNodeIndex was not found in model tree... and therefore should never happen!
    }
  }

  if (modelTreeNode.ID != 0)   // Do not close group for root node
  {
    outPRCFile.endgroup();
  }

}


//***********************************************************************************

void SavePRC::savePRCToFileStream(std::ofstream& ofstream)
{
  _progressFld->setFloatValue(0.0f);

  WEMPtr saveWEM = NULL;
  ML_CHECK_NEW(saveWEM,WEM());

  // Clear object info vector;
  _prcObjectInfoVector.clear();

  // Clear geometry vectors
  _pointSetsGeometryVector.clear();
  _lineSetsGeometryVector.clear();

  PRCMeshInfoVector meshInfoVector;

  // Stores the model bounding box data & its center. Shall be modified only with UpdateBoundingBox() method!
  ModelBoundingBoxStruct modelBoundingBox;

  // Get default parameters from field values
  //defaultValues = getDefaultValuesFromFields(); 
  
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  _statusFld->setStringValue("Analyzing input data.");

  // Scan all data from input field and collect base info for point sets.
  PreProcessPointSetData(modelBoundingBox);

  // Scan all data from input field and collect base info for line sets.
  PreProcessLineSetData(modelBoundingBox);

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  PreProcessMeshData(saveWEM, meshInfoVector, modelBoundingBox);

  
  GroupNodeVector groupNodes = assemblePRCGroupNodeInfo(_prcObjectInfoVector);
  mapParentTreeNodeIDs(_prcObjectInfoVector, groupNodes);

  _modelTree = assemblePRCModelTreeInfo(groupNodes);

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  _progressFld->setFloatValue(0.1f);

  PRCFile outPRCFile(ofstream);

  PRCModelTreeNode modelRootNode = getNodeFromPRCModelTree(_modelTree, 0);

  WriteNodeModelsToPRCFile(outPRCFile, modelRootNode);





  outPRCFile.finish();


  _progressFld->setFloatValue(1.0f);

}







ML_END_NAMESPACE