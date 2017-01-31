//
//  mlOsiriXBridge.mm
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

// Local includes
#include "mlOsiriXBridge.h"

#if defined(MACOS)
# import "mlOsiriXBridge_p.h"
#endif

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(OsiriXBridge, Module);

//----------------------------------------------------------------------------------
//! C-tor
//----------------------------------------------------------------------------------
OsiriXBridge::OsiriXBridge() : Module(0,0), priv(NULL)
{
  ML_TRACE_IN("OsiriXBridge::OsiriXBridge()")

#if !defined(MACOS) && defined(DEBUG)
  ML_PRINT_INFO("OsiriXBridge::OsiriXBridge()", "This module is only fully supported when compiled and run on macOS!")
#endif
  
  handleNotificationOff();
  
  AlternativeTargetNameFld    = getFieldContainer()->addString("alternativeTargetName");

  UpdateClientInfoFld         = getFieldContainer()->addNotify("updateClientInfo");
  
  OsiriXIncomingFolderFld     = getFieldContainer()->addString("osirixIncomingDir");
  ClientAppBundleIdFld        = getFieldContainer()->addString("clientAppBundleId");
  
  SlicesFileListFld           = static_cast<StringLineMultiField *> (getFieldContainer()->addField(new StringLineMultiField("slicesFileList")));
  
  UseEatDicomFld              = getFieldContainer()->addBool("useEatDicom");
  DataStorageFolderFld        = getFieldContainer()->addString("dataStoreDir");
  EatDicomOptionsFld          = getFieldContainer()->addString("eatDicomOptions");
  EatDicomOutputFld           = getFieldContainer()->addString("eatDicomOutput");
  FilenameFld                 = getFieldContainer()->addString("filename");

  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! D-tor
//----------------------------------------------------------------------------------
OsiriXBridge::~OsiriXBridge()
{
  ML_TRACE_IN("OsiriXBridge::~OsiriXBridge()")
  
#if defined(MACOS)
  delete priv;
#endif
}

//----------------------------------------------------------------------------------
//! Setup internal state after loading
//----------------------------------------------------------------------------------
void
OsiriXBridge::activateAttachments()
{
  ML_TRACE_IN("OsiriXBridge::activateAttachments()")
  
#if defined(MACOS)
  priv = new OsiriXBridgePrivate(this);
#endif
  
  handleNotification(AlternativeTargetNameFld);
  handleNotification(UpdateClientInfoFld);
  
  // Activate notification handling
  inherited::activateAttachments();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void
OsiriXBridge::handleNotification(Field *field)
{
  ML_TRACE_IN("OsiriXBridge::handleNotification()")

#if defined(MACOS)
  if (!priv) return;

  if (field == UpdateClientInfoFld) {
    OsiriXIncomingFolderFld->setStringValue(priv->osirixIncomingFolder());
    ClientAppBundleIdFld->setStringValue(priv->clientAppBundleId());
  }
  else if (field == AlternativeTargetNameFld || field == getFieldContainer()->getField("instanceName")) {
    priv->registerWithClient();
  }
#endif
}

ML_END_NAMESPACE
