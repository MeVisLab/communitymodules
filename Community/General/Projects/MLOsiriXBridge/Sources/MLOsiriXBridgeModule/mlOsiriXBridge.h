//
//  mlOsiriXBridge.h
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

#ifndef __mlOsiriXBridge_H
#define __mlOsiriXBridge_H

// Local includes
#include "MLOsiriXBridgeSystem.h"

// ML includes
#include <mlModule.h>
#include <mlStringLineMultiField.h>

ML_START_NAMESPACE

class OsiriXBridgePrivate;

//! Receive dicom images from the OsiriX database
class MLOSIRIXBRIDGE_EXPORT OsiriXBridge : public Module
{
  typedef Module inherited;

public:

  //! Alternative target name for this module in the OsiriX toolbar pop-up menu
  /*! This name will take precedence over the instanceName.
   */
  StringField *AlternativeTargetNameFld;
  
  //! Outputs the path to OsiriX's Incoming folder (Read-only)
  StringField *OsiriXIncomingFolderFld;
  
  //! Retrieve the path to OsiriX's Incoming folder
  NotifyField *OsiriXIncomingUpdateFld;
  
  //! Automatically update the path to OsiriX's Incoming folder when the filename field gets an update 
  BoolField *OsiriXIncomingAutoUpdateFld;
  
  
  //! List of filenames of which the series consists
  StringLineMultiField *SlicesFileListFld;

  
  //! Employ eatDicom to convert the DICOM slices into an DICOM 3D dataset
  BoolField *UseEatDicomFld;
  
  //! Store the converted datasets in this folder
  StringField *DataStorageFolderFld;

  //! Additional options to eatDicom
  StringField *EatDicomOptionsFld;

  //! Provides access to the standard & error output of the dicom converter (Read-only)
  StringField *EatDicomOutputFld;
  
  //! Outputs the name of the DICOM 3D dataset (Read-only)
  StringField *FilenameFld;

  
  //! Constructor.
  OsiriXBridge();

  //! Destructor.
  ~OsiriXBridge();

  //! Setup internal state after loading
  virtual void activateAttachments();

protected:

  //! Handle field changes of the field \c field.
  virtual void handleNotification(Field *field);

private:

  OsiriXBridgePrivate *priv;

  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(OsiriXBridge)

};

ML_END_NAMESPACE

#endif // __mlOsiriXBridge_H
