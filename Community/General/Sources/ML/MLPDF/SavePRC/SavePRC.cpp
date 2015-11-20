//----------------------------------------------------------------------------------
//! The ML module class SavePRC.
/*!
// \file    SavePRC.cpp
// \author  Axel Newe
// \date    2015-08-01
//
// Creates PRC file from WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SavePRC.h"
#include "shared/SpecificationGenerator/MLPDF_SpecificationGenerator.h"
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

  //-------------------------------------------------------------------
  //! Strings for enum field: specification type 
  //-------------------------------------------------------------------
  const char* const NEW_SPECIFICATION_OBJECTTYPE_STRINGS[mlPDF::NUM_OBJECTTYPES] = {
    "OBJECTTYPE_POINTCLOUD",
    "OBJECTTYPE_LINESET",
    "OBJECTTYPE_MESH",
    "OBJECTTYPE_METADATA"
  };

  //-------------------------------------------------------------------
  //! Strings for enum field: model visibility 
  //-------------------------------------------------------------------
  const char* const NEW_SPECIFICATION_MODELVISIBILITY_STRINGS[mlPDF::NUM_MODELVISIBILITY] = {
    "MODELVISIBILITY_NOTVISIBLE",
    "MODELVISIBILITY_FRONTVISIBLE",
    "MODELVISIBILITY_BACKVISIBLE",
    "MODELVISIBILITY_FRONTANDBACKVISIBLE"
  };

  //! Add fields for Specification Generator
  (_newSpecificationFld                        = addString("newSpecification"))->setStringValue("");
  (_newSpecificationSelectedTabFld             = addInt("selectedTab"))->setIntValue(0);
  (_newSpecificationOutputValidFld             = addBool("newSpecificationOutputValid"))->setBoolValue(false);
  _newSpecificationAddFld                      = addNotify("newSpecificationAdd");
  (_newSpecificationTypeFld                    = addEnum("newSpecificationType", NEW_SPECIFICATION_OBJECTTYPE_STRINGS, mlPDF::NUM_OBJECTTYPES))->setEnumValue(mlPDF::OBJECTTYPE_MESH);
  (_newSpecificationObjectNameFld              = addString("newSpecificationObjectName"))->setStringValue("");
  (_newSpecificationGroupPathFld               = addString("newSpecificationGroupPath"))->setStringValue("");
  (_newSpecificationUseDefaultColorFld         = addBool("newSpecificationUseDefaultColor"))->setBoolValue(true);
  (_newSpecificationUseDefaultSpecularColorFld = addBool("newSpecificationUseDefaultSpecularColor"))->setBoolValue(true);
  (_newSpecificationColorFld                   = addColor("newSpecificationColor"))->setVector3Value(Vector3(0.651f,0.651f,0.651f));
  (_newSpecificationColorAlphaFld              = addFloat("newSpecificationColorAlpha"))->setFloatValue(1.0f);
  (_newSpecificationSpecularColorFld           = addColor("newSpecificationSpecularColor"))->setVector3Value(Vector3(0.75f,0.75f,0.75f));
  (_newSpecificationModelVisibilityFld         = addEnum("newSpecificationModelVisibility", NEW_SPECIFICATION_MODELVISIBILITY_STRINGS, mlPDF::NUM_MODELVISIBILITY))->setEnumValue(mlPDF::MODELVISIBILITY_FRONTANDBACKVISIBLE);
  (_newSpecificationMetaDataKeyFld             = addString("newSpecificationMetaDataKey"))->setStringValue("");
  (_newSpecificationMetaDataValueFld           = addString("newSpecificationMetaDataValue"))->setStringValue("");
  (_newSpecificationWEMLabelFld                = addString("newSpecificationWEMLabel"))->setStringValue("");
  (_newSpecificationPositionTypesFld           = addString("newSpecificationPositionTypes"))->setStringValue("");
  (_newSpecificationConnectionTypesFld         = addString("newSpecificationConnectionTypes"))->setStringValue("");

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

  if (field == _newSpecificationAddFld)
  {
    AddNewSpecification();
  }

  if (field == _newSpecificationTypeFld)
  {
    UpdateObjectTypeTabView();
  }

  if ( (field == _newSpecificationTypeFld) ||
       (field == _newSpecificationObjectNameFld) ||
       (field == _newSpecificationGroupPathFld) ||
       (field == _newSpecificationUseDefaultColorFld) ||
       (field == _newSpecificationUseDefaultSpecularColorFld) ||
       (field == _newSpecificationColorFld) ||
       (field == _newSpecificationColorAlphaFld) ||
       (field == _newSpecificationSpecularColorFld) ||
       (field == _newSpecificationModelVisibilityFld) ||
       (field == _newSpecificationMetaDataKeyFld) ||
       (field == _newSpecificationMetaDataValueFld) ||
       (field == _newSpecificationWEMLabelFld) ||
       (field == _newSpecificationPositionTypesFld) ||
       (field == _newSpecificationConnectionTypesFld)  
     )
  {
    UpdateNewSpecification();
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
  modelBoundingBox.start.x = ML_DOUBLE_MAX;
  modelBoundingBox.start.y = ML_DOUBLE_MAX;
  modelBoundingBox.start.z = ML_DOUBLE_MAX;
  modelBoundingBox.end.x   = ML_DOUBLE_MAX * -1;
  modelBoundingBox.end.y   = ML_DOUBLE_MAX * -1;
  modelBoundingBox.end.z   = ML_DOUBLE_MAX * -1;

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


//***********************************************************************************


void SavePRC::UpdateObjectTypeTabView()
{
  int newSpecificationType = _newSpecificationTypeFld->getEnumValue();

  if (newSpecificationType == mlPDF::OBJECTTYPE_POINTCLOUD)
  {
    _newSpecificationSelectedTabFld->setIntValue(0);
  }
  else if (newSpecificationType == mlPDF::OBJECTTYPE_LINESET)
  {
    _newSpecificationSelectedTabFld->setIntValue(1);
  }
  else if (newSpecificationType == mlPDF::OBJECTTYPE_MESH)
  {
    _newSpecificationSelectedTabFld->setIntValue(2);
  }
  else if (newSpecificationType == mlPDF::OBJECTTYPE_METADATA)
  {
    _newSpecificationSelectedTabFld->setIntValue(3);
  }
}


//***********************************************************************************


void SavePRC::AddNewSpecification()
{
  int newSpecificationObjectType = _newSpecificationTypeFld->getEnumValue();

  if (newSpecificationObjectType == mlPDF::OBJECTTYPE_POINTCLOUD)
  {
    _pointCloudSpecificationFld->setStringValue(_pointCloudSpecificationFld->getStringValue() + _newSpecificationFld->getStringValue());
  }
  else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_LINESET)
  {
    _lineSetSpecificationFld->setStringValue(_lineSetSpecificationFld->getStringValue() + _newSpecificationFld->getStringValue());
  }
  else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_MESH)
  {
    _meshSpecificationFld->setStringValue(_meshSpecificationFld->getStringValue() + _newSpecificationFld->getStringValue());
  }
  //else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_METADATA)
  //{
  //  _metaDataSpecificationFld->setStringValue(_metaDataSpecificationFld->getStringValue() + _newSpecificationFld->getStringValue());
  //}
}


//***********************************************************************************


void SavePRC::UpdateNewSpecification()
{
  std::string newSpecificationString = "";
  int newSpecificationObjectType = _newSpecificationTypeFld->getEnumValue();
  
//  if (callingField == ctx.field("newSpecificationType")):
//    UpdateTabView()
  
  if (newSpecificationObjectType == mlPDF::OBJECTTYPE_POINTCLOUD)
  {
    newSpecificationString = "<PointSet>\n";
    newSpecificationString += mlPDF::SpecificationGenerator::GetPositionTypes(_newSpecificationPositionTypesFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "\n";
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_LINESET)
  {
    newSpecificationString = "<LineSet>\n";
    newSpecificationString += mlPDF::SpecificationGenerator::GetPositionTypes(_newSpecificationPositionTypesFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetConnectionTypes(_newSpecificationConnectionTypesFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetColor(_newSpecificationColorFld->getColorValue(), _newSpecificationColorAlphaFld->getFloatValue(), _newSpecificationUseDefaultColorFld->getBoolValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "\n";
//    ctx.field("selectedTab").value = 1;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_MESH)
  {
    newSpecificationString = "<Mesh>\n";
    newSpecificationString += mlPDF::SpecificationGenerator::GetWEMLabel(_newSpecificationWEMLabelFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetObjectName(_newSpecificationObjectNameFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetGroupPath(_newSpecificationGroupPathFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetColor(_newSpecificationColorFld->getColorValue(), _newSpecificationColorAlphaFld->getFloatValue(), _newSpecificationUseDefaultColorFld->getBoolValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetSpecularColor(_newSpecificationSpecularColorFld->getColorValue(), _newSpecificationUseDefaultSpecularColorFld->getBoolValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetModelVisibility(_newSpecificationModelVisibilityFld->getEnumValue());
    newSpecificationString += "\n";
//    ctx.field("selectedTab").value = 2;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }
  else if (newSpecificationObjectType == mlPDF::OBJECTTYPE_METADATA)
  {
    newSpecificationString = "<MetaData>\n";
    newSpecificationString += mlPDF::SpecificationGenerator::GetMetaDataKey(_newSpecificationMetaDataKeyFld->getStringValue());
    newSpecificationString += mlPDF::SpecificationGenerator::GetMetaDataValue(_newSpecificationMetaDataValueFld->getStringValue());
    newSpecificationString += "\n";
//    ctx.field("selectedTab").value = 3;
    _newSpecificationOutputValidFld->setBoolValue(true);
  }

  _newSpecificationFld->setStringValue(newSpecificationString);
  
  return;

}


//***********************************************************************************




ML_END_NAMESPACE