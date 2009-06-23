//----------------------------------------------------------------------------------
//! The ML module class SoSceneWriterMD.
/*!
// \file    mlSoSceneWriterMD.h
// \author  Konrad Mühler
// \date    2006-05-15
//
// New SceneWriter with ping when IV-file saved
*/
//----------------------------------------------------------------------------------


#ifndef __mlSoSceneWriterMD_H
#define __mlSoSceneWriterMD_H


// Local includes
#ifndef __UMDSoSceneWriterMDSystem_H
#include "UMDSoSceneWriterMDSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif


// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoNode.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoWriteAction.h>
#include "XVLeaveScope.h"


ML_START_NAMESPACE


//! New SceneWriter with ping when IV-file saved
class UMDSOSCENEWRITERMD_EXPORT SoSceneWriterMD : public BaseOp
{
public:

  //! Constructor.
  SoSceneWriterMD (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);


private:

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //! 
  NotifyField *_writeScene;

  //! 
  NotifyField *_fileSaved;

  //! 
  StringField *_fileName;

  //! 
  SoNodeField *_inputObject;

  //!
  EnumField* _fileType;

  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(SoSceneWriterMD)

};


ML_END_NAMESPACE

#endif // __mlSoSceneWriterMD_H


