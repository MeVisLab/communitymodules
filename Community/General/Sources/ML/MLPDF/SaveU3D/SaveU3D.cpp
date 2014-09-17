//----------------------------------------------------------------------------------
//! This class composes WEMs, Fibers and XMarkers to a U3D file.
/*!
// \file    SaveU3D.cpp
// \author  Axel Newe
// \date    2014-01-20
//
// Creates U3D file from WEMs and XMarkers
*/
//----------------------------------------------------------------------------------


// Local includes
#include "SaveU3D.h"
#include "U3DFileFormat/U3D_Tools.h"
#include "U3DFileFormat/U3D_Constants.h"
#include "U3DFileFormat/U3D_DataBlockWriter.h"
#include "U3DFileFormat/U3D_FileWriter.h"

#include <ColoredMarkerList.h>

// ML includes
#include "mlVersion.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(SaveU3D, WEMInspector);


//***********************************************************************************


SaveU3D::SaveU3D (std::string type) : WEMInspector(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.
  (_inPointPositionsFld  = addBase("inPointPositions"))->setBaseValueAndAddAllowedType(&_inPointPositions);
  (_inLinePositionsFld   = addBase("inLinePositions"))->setBaseValueAndAddAllowedType(&_inLinePositions);
  (_inLineConnectionsFld = addBase("inLineConnections"))->setBaseValueAndAddAllowedType(&_inLineConnections);
  _inPointPositionsFld->addAllowedType<ml::ColoredMarkerList>();
  _inLinePositionsFld->addAllowedType<ml::ColoredMarkerList>();

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
  (_viewsSpecificationFld      = addString("viewsSpecification"))->setStringValue("");
  (_lightsSpecificationFld     = addString("lightsSpecification"))->setStringValue("");
  (_metaDataSpecificationFld   = addString("metaDataSpecification"))->setStringValue("");

  (_defaultViewNameFld     = addString("defaultViewName"))->setStringValue("DefaultView");
  (_defaultLightNameFld    = addString("defaultLightName"))->setStringValue("DefaultAmbientLight");

  (_defaultMaterialDiffuseColorFld  = addColor("defaultMaterialDiffuseColor"))->setVector3Value(Vector3(0.65f,0.65f,0.65f));
  (_defaultMaterialSpecularColorFld = addColor("defaultMaterialSpecularColor"))->setVector3Value(Vector3(0.75f,0.75f,0.75f));
  (_defaultMaterialAmbientColorFld  = addColor("defaultMaterialAmbientColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  (_defaultMaterialEmissiveColorFld = addColor("defaultMaterialEmissiveColor"))->setVector3Value(Vector3(0.0f,0.0f,0.0f));
  (_defaultMaterialAlphaFld         = addFloat("defaultMaterialAlpha"))->setFloatValue(1.0f);

  (_defaultLightColorFld = addColor("defaultLightColor"))->setVector3Value(Vector3(1.0f,1.0f,1.0f));
  (_defaultLightIntensityFld = addDouble("defaultLightIntensity"))->setDoubleValue(1.0f);

  (_defaultBoundingBoxMetaDataFld = addBool("defaultBoundingBoxMetaData"))->setBoolValue(true);
  (_addDefaultViewNodeFld         = addBool("addDefaultViewNode"))->setBoolValue(true);
  (_addDefaultLightNodeFld        = addBool("addDefaultLightNode"))->setBoolValue(true);

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


SaveU3D::~SaveU3D()
{
  // destroy own dynamic data structures here
}


//***********************************************************************************


void SaveU3D::handleNotification (Field* field)
{
  if (field == _mlSaveFld) 
  {
    // Call the save routine.
    saveButtonClicked();
  } 

  if (field == _inPointPositionsFld)
  {
    ml::Base *inValue = _inPointPositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inPointPositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
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
    ml::Base *inValue = _inLinePositionsFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::ColoredMarkerList))
      {
        ml::ColoredMarkerList* inList = ((ml::ColoredMarkerList*)inValue);
        _inLinePositions = inList->toXMarkerList();
      }
      else
      {
        ml::XMarkerList* inList = ((ml::XMarkerList*)inValue);
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


void SaveU3D::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMInspector::activateAttachments();
}


//***********************************************************************************


void SaveU3D::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  WEMInspector::_process();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();

}


//***********************************************************************************


void SaveU3D::saveButtonClicked()
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

    if (last4 != ".u3d") 
    { 
      filename.append(".u3d"); 
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
    saveU3DToFileStream(ofstream);
    ofstream.close();
    status = "U3D file saved.";
  } 
  else 
  {
    status = "Could not open specified file.";
  }

  _statusFld->setStringValue(status);

}


//***********************************************************************************


void SaveU3D::saveU3DToFileStream(std::ofstream& ofstream)
{
  _progressFld->setFloatValue(0.0f);

  WEMPtr saveWEM = NULL;
  ML_CHECK_NEW(saveWEM,WEM());

  outU3DFile = NULL; 

  // Clear object info vector;
  _u3dObjectInfoVector.clear();

  // Clear geometry vectors
  _pointSetsGeometryVector.clear();
  _lineSetsGeometryVector.clear();

  // These vectors contain all geometry related data
  U3DPointSetInfoVector  U3DPointSetInfoVector;
  U3DLineSetInfoVector U3DLineSetInfoVector;
  U3DMeshInfoVector  meshInfoVector;

  // Stores the model bounding box data & its center. Shall be modified only with UpdateBoundingBox() method!
  ModelBoundingBoxStruct modelBoundingBox;

  // Get default parameters from field values
  defaultValues = getDefaultValuesFromFields(); 

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  _statusFld->setStringValue("Analyzing input data.");

  // Scan all data from input field and collect base info for point sets.
  PreProcessPointSetData(U3DPointSetInfoVector, modelBoundingBox);

  // Scan all data from input field and collect base info for line sets.
  PreProcessLineSetData(U3DLineSetInfoVector, modelBoundingBox);

  // Scan all WEM patches, triangulate them if necessary and collect base info.
  PreProcessMeshData(saveWEM, meshInfoVector, modelBoundingBox);

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  _progressFld->setFloatValue(0.1f);

  if ( _u3dObjectInfoVector.size() > 0 )  // If at least one valid object (WEM patch, XMarker or FiberSet) was added.
  {
    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Preparation
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    ML_CHECK_NEW(outU3DFile,U3DFileWriter());

    MetaDataVector metaData;
    MetaDataStruct metaDataPair;

    std::string VersionString = "MeVisLab ( ML version  )"; 
    VersionString.insert(22,ML_VERSION_STRING);

    metaDataPair.key = "CreatedBy";
    metaDataPair.value = VersionString;
    metaData.push_back(metaDataPair);  

	  outU3DFile->addStandardBlock_PriorityUpdate((MLuint32)0x00000001);

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add nodes
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _statusFld->setStringValue("Adding main nodes.");

    // Add default view node ===================================================================
    addDefaultViewNode();

	  // Add all group nodes =====================================================================
    GroupNodeVector groupNodes = assembleU3DGroupNodeInfo(_u3dObjectInfoVector);
    makeGroupPathNamesUnique(groupNodes, _u3dObjectInfoVector);
	  addAllGroupNodes(groupNodes);   

    // Add a model node for each U3D object ====================================================
	  addU3DModelNodes();
    
    // Add default light node ==================================================================
    addDefaultLightNode();

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add models
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

	  // Add point sets ==========================================================================

    // Add a point set modifier chain for each point set
	  AddAllPointSetModifierChains(U3DPointSetInfoVector);
	 
	  // Add line sets ===========================================================================

    // Add a line set modifier chain for each line set
    AddAllLineSetModifierChains(U3DLineSetInfoVector);

	  // Add meshes ==============================================================================

    // Add a CLOD mesh declaration modifier chain for each mesh 
    AddAllCLODMeshModifierChains(meshInfoVector, saveWEM);

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add resources
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _progressFld->setFloatValue(0.9f);

    // Add shader & material resource for each U3D object ======================================
    addShaderAndMaterialResources();

	  // Add light resources =====================================================================
    addLightResources();

    // Add view resources ======================================================================
    addViewResources(); 

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Finishing moves
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    // Write meta data into meta data vector ===================================================
    addMetaData(metaData, modelBoundingBox);

    // Write data to disk & free mem ===========================================================

    outU3DFile->writeToFileStream(ofstream, metaData);

    ML_DELETE(outU3DFile);
    outU3DFile = NULL;

  }
  else
  {
    _statusFld->setStringValue("No objects to export found.");
  }
  
  _progressFld->setFloatValue(1.0f);

}


//***********************************************************************************




ML_END_NAMESPACE