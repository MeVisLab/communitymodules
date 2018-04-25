//----------------------------------------------------------------------------------
// U3DSave module.
//
// \file    U3DSave.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-10-01
//
// Saves a U3D to disk.
//----------------------------------------------------------------------------------


// Local includes
#include "U3DSave.h"
#include "../shared/MLU3D_Tools.h"

// ML includes
#include "mlUnicode.h"



ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(U3DSave, U3DInspector);


//***********************************************************************************


U3DSave::U3DSave() : U3DInspector()
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add input fields to the module and set allowed types.

  // Add output fields to the module and set allowed types.

  // Add and initialize parameter fields
  (_fileNameFld = addString("filename"))->setStringValue("");
  _saveFld      = addNotify("save");
  (_autoSaveFld = addBool("autoSave"))->setBoolValue(false);
  _finishedFld  = addNotify("finishedTrigger");

  (_statusFld   = addString("status"))->setStringValue("Idle.");
  (_progressFld = addProgress("progress"))->setFloatValue(0.0f);
  (_successFld  = addBool("success"))->setBoolValue(false);

  // Init global variables

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


//***********************************************************************************


U3DSave::~U3DSave()
{
  // Destroy own dynamic data structures here
}


//***********************************************************************************


void U3DSave::handleNotification(Field* field)
{
  // Call parent class first to make sure that input object is set correctly.
  U3DInspector::handleNotification(field);
}


//***********************************************************************************


void U3DSave::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  U3DInspector::activateAttachments();
}


//***********************************************************************************


void U3DSave::process()
{
  if (_inU3DValid)
  {
    saveButtonClicked();
  }
}


//***********************************************************************************

void U3DSave::saveButtonClicked()
{
  bool success = false;

  std::string filename = _fileNameFld->getStringValue();
  if (filename == "")
  {
    _statusFld->setStringValue("No filename specified.");
    return;
  }

  const unsigned int filenameLength = static_cast<unsigned int>(filename.length());

  std::string last4 = "";

  if (filenameLength > 4)
  {
    last4 = filename.substr(filenameLength - 4, 4);

    if (last4 != ".u3d")
    {
      filename.append(".u3d");
      _fileNameFld->setStringValue(filename);
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
    ofstream.open(filename16, (std::ios::binary | std::ios::out));
#if (_MSC_VER >= 1310)
    MLFree(filename16);
#endif
  }
  else
  {
    status = "Could not convert filename.";
  }
#else
  ofstream.open(filename.c_str(), (std::ios::binary | std::ios::out));
#endif

  if (ofstream.is_open())
  {
    success = saveU3DToFileStream(ofstream);
    ofstream.close();

    if (success)
    {
      status = "U3D file saved.";
    }
    else
    {
      status = "U3D file could not be saved.";
    }
  }
  else
  {
    status = "Could not open specified file.";
  }

  _statusFld->setStringValue(status);
  _successFld->setBoolValue(success);
  _finishedFld->touch();
}


//***********************************************************************************

mlU3D::MetaDataVector U3DSave::_assembleMetaData()
{
  mlU3D::MetaDataVector metaData = _inU3DObject->metaData;
  mlU3D::MetaDataStruct metaDataPair;

  metaDataPair.key = "CreatedBy";
  metaDataPair.value = "U3DSave module from MeVisLab MLU3D library (v" + mlU3D::U3DTools::getLibraryVersionNumberString() + ") by Axel Newe (axel.newe@fau.de)";
  metaData.push_back(metaDataPair);

  if (_inU3DObject->defaultValues.addDefaultBoundingBoxMetaData)
  {
    metaDataPair.key = "BoundingBoxCenter";
    metaDataPair.value = mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.center.x)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.center.y)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.center.z));

    metaData.push_back(metaDataPair);

    metaDataPair.key = "BoundingBoxStart";
    metaDataPair.value = mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.start.x)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.start.y)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.start.z));

    metaData.push_back(metaDataPair);

    metaDataPair.key = "BoundingBoxEnd";
    metaDataPair.value = mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.end.x)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.end.y)) + " " +
      mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.end.z));

    metaData.push_back(metaDataPair);

    metaDataPair.key = "BoundingBoxRadius";
    metaDataPair.value = mlU3D::floatToString(static_cast<float>(_inU3DObject->modelBoundingBox.radius));

    metaData.push_back(metaDataPair);
  }

  return metaData;
}


//***********************************************************************************

bool U3DSave::saveU3DToFileStream(std::ofstream& ofstream)
{
  bool success = false;

  _progressFld->setFloatValue(0.0f);

  _outU3DFile = NULL;
  _networkPanelStatusFld->setStringValue("Saving...");

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  if (_inU3DObject->getNumGeometryGenerators() > 0)  // If at least one valid object (WEM patch, XMarker or FiberSet) was added.
  {
    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Preparation
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    ML_CHECK_NEW(_outU3DFile, U3DFileWriter());

    _outU3DFile->addStandardBlock_PriorityUpdate((MLuint32)0x00000001);

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add node blocks
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _statusFld->setStringValue("Adding main nodes.");

    _addViewNodes();
    _addGroupNodes();
    _addModelNodes();
    _addLightNodes();

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add geometry generator blocks
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    // Add point sets ==========================================================================

    // Add a point set modifier chain for each point set
    _addPointSetGeometry();
    _progressFld->setFloatValue(0.1f);


    // Add line sets ===========================================================================

    // Add a line set modifier chain for each line set
    _addLineSetGeometry();
    _progressFld->setFloatValue(0.2f);

    // Add meshes ==============================================================================

    // Add a CLOD mesh declaration modifier chain for each mesh
    _addCLODMeshGeometry();
    _progressFld->setFloatValue(0.9f);

	_addTextureDeclarations();


    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Add resources
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    _progressFld->setFloatValue(0.9f);

    // Add textures, shaders & material resources ========================================================
    _addShaders();
    _addMaterialResources();
	_addTextureResources();

    // Add light resources =====================================================================
    _addLightResources();

    // Add view resources ======================================================================
    _addViewResources();

    // Add motion resources ====================================================================
    _addMotionResources(); // Does nothing in the current version.

    // +++++++++++++++++++++++++++++++++++++++++++++++++
    //
    // Finishing moves
    //
    // +++++++++++++++++++++++++++++++++++++++++++++++++

    mlU3D::MetaDataVector metaData = _assembleMetaData();

    // Write data to disk & free mem ===========================================================

    success = _outU3DFile->writeToFileStream(ofstream, metaData);

    ML_DELETE(_outU3DFile);
    _outU3DFile = NULL;
    _networkPanelStatusFld->setStringValue("Saved.");
  }
  else
  {
    // Todo: status field is overwritten in saveButtonClicked(). Error message / problem description should be forwarded to there.
    _statusFld->setStringValue("No objects to export found.");
    _networkPanelStatusFld->setStringValue("Nothing saved.");
  }


  _progressFld->setFloatValue(1.0f);

  return success;
}


//***********************************************************************************


ML_END_NAMESPACE