//----------------------------------------------------------------------------------
//! The ML module class OsiriXExporter.
/*!
// \file    mlOsiriXExporter.h
// \author  Chunliang Wang
// \date    2009-08-04
//
// Export image to OsiriX
*/
//----------------------------------------------------------------------------------


#ifndef __mlOsiriXExporter_H
#define __mlOsiriXExporter_H

#import <Cocoa/Cocoa.h>
// Local includes
#include "MLOsiriXImporterSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#define id Id
#include <CSOBase/CSOModuleBase/CSOGenerator.h>
#undef id
@class MeVisLab2OsiriXTBridge;

ML_START_NAMESPACE


//! Export image to OsiriX
class MLOSIRIXIMPORTER_EXPORT OsiriXExporter : public BaseOp
{
public:

  //! Constructor.
  OsiriXExporter ();
  ~OsiriXExporter ();

  //MeVisLab2OsiriXTBridge
  MeVisLab2OsiriXTBridge* bridgeToOsiriX;
  BOOL calcInputImageProps(NSMutableDictionary* anImage,int inIndex);
  BOOL calcInSubImage(NSMutableDictionary* anImage, int inIndex );
  BOOL prepareCSOForOsiriX(NSMutableDictionary* anImage);

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  //! Update internal module state after load or clone and enable notification handling again.
  virtual void activateAttachments ();

  //! Configures (in)validation handling of inputs which are not connected or up to date.
  virtual INPUT_HANDLE handleInput(int inIndex, INPUT_STATE state) const;

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  //! CSO Marker input from OsiriX
  BaseField*   _InputCSOListFld;
  CSOListPtr   _InputCSOList;
  //! The name of this operation (should be unique on the system)
  StringField *_OperationNameFld;

  //! If triggered send results back to OsiriX
  NotifyField *_SendBackButtonFld;

  //@}


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(OsiriXExporter)
};


ML_END_NAMESPACE

#endif // __mlOsiriXExporter_H

