
//! The ML ObjMgrClient module METKToSolverConnection.
//! \file METKToSolverConnection.h

#ifndef __METKToSolverConnection_H
#define __METKToSolverConnection_H


// Local includes
#include "ViewpointSystem.h"

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

#include <ObjMgr/mlObjMgrClient.h>
#include <inttypes.h>

#include "kBasics.h"
#include "METK.h"

class SoSwitch;
ML_START_NAMESPACE

//! The ML ObjMgrClient module METKToSolverConnection.
class VIEWPOINT_EXPORT METKToSolverConnection : public ObjMgrClient
{
public:

  //! Constructor.
  METKToSolverConnection();
  ~METKToSolverConnection (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);
  virtual void activateAttachments();

  const std::string* getFirstStructure();
  const std::string* getNextStructure();
  const std::string* getCurrentStructure();
  const float getImportance();
  const SoNode* getIVPointer();
  const float getImportance(const std::string* name);
  const SoNode* getIVPointer(const std::string* name);
  bool getObjAttribute(const string ObjID, const string LayerID, const string InfoID, omAttribute& attr);
  bool getObjAttributeString(const string ObjID, const string LayerID, const string InfoID, string& value);



private:

   typedef ObjMgrClient inherited;

   // ----------------------------------------------------------
   //@{ \name Module field declarations
   // ----------------------------------------------------------
   //@}

   SoNodeField* _outScene;
   NotifyField* _init;
   StringField* _dataPath;
//   SoSeparator* _Sep;
   SoSwitch* _Switch;
   string path;
   string* pathIds;

   omObjectContainer::iterator _object;

   //! Implements interface for the runtime type system of the ML.
   ML_CLASS_HEADER(METKToSolverConnection)
};


ML_END_NAMESPACE

#endif // __METKToSolverConnection_H


