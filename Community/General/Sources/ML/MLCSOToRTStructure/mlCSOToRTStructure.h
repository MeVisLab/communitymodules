//----------------------------------------------------------------------------------
//! The ML module class CSOToRTStructure.
/*!
// \file    mlCSOToRTStructure.h
// \author  linal
// \date    2010-01-20
//
// convert CSOs to RT Structure
*/
//----------------------------------------------------------------------------------


#ifndef __mlCSOToRTStructure_H
#define __mlCSOToRTStructure_H


// Local includes
#include "DCMTree_UIDGenerator.h"
#include "MLCSOToRTStructureSystem.h"
#include <CSOBase/CSOList.h>

// ML includes
#include "mlOperatorIncludes.h"
#include "DCMTree_Lib.h"
#include "DCMTree_IOParameter.h"
#include "DCMTree_Reader.h"
#include "DCMTree_Dict.h"
#include "DCMTree_Tag.h"
#include "DCMTree_Value.h"
#include "DCMTree_Unicode.h"
#include "DCMTree_Writer.h"
#include "DCMTree_Exception.h"
#include "DCMTree_Message.h"
#include "DCMTree_DicomTags.h"



ML_START_NAMESPACE


//! convert CSOs to RT Structure
class MLCSOTORTSTRUCTURE_EXPORT CSOToRTStructure : public BaseOp
{
public:

  //! Constructor.
  CSOToRTStructure ();

  //! D'structor.
  ~CSOToRTStructure () {}


  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);
  virtual void calcOutImageProps (int outIndex);
  DCMTree::TreePtr getTreeHeaderPtr() {return _treeHeaderPtr;}
  CSOList* getInputCSOs() {return _inputCSOs;}
  BaseOp::INPUT_HANDLE handleInput(int /*inIndex*/, INPUT_STATE /*state*/) const;
  //! Configures (in)validation handling of inputs which are not connected or up to date.
  static std::map<std::string, std::string>  _modalitySOPClassUIDMap;
  void activateAttachments ();
  void setValidFld(bool valid) {_validFld->setBoolValue(valid);}
  static void setValid(CSOToRTStructure* obj);
  //! Locking variable...
  bool _isInNotificationCB;
   //! update generated Tree
  void updateGenTreeHeader(PagedImg *img);

private:

  BaseField			*_inListFld;
  DCMTree::TreePtr  _treeHeaderPtr;
  DCMTree::TreePtr  _genTreePtr;
  CSOList			*_inputCSOs;
  NotifyField	    *_applyFld;
  BoolField         *_validFld;
  std::string       _refSopClassUID;
  static bool       isInitialized;

  //map of SOPInstUIDs and position
  std::map< float, std::string>  _mSOPInstUIDs;

   //! input DCM File name
  StringField		*_rtStructFileFld;
  float             _voxelSizeZ;

  //! private functions
  //! combine general headers
  bool combineHeader();
  //! get ReferenceImage SOPUID from a z cooridate
  std::string getRefUID(double z);
  //! add a Tag
  void addTag(DCMTree::TreePtr &parent, DCMTree::TagId tagId, std::string value, DCMTree::Vr vr);
  //! add a sequence
  void addSequence(DCMTree::TreePtr parentTree, DCMTree::TreePtr childTree, DCMTree::TagId tagId);
  //! add a sequence Tag
  void addSequenceTag(DCMTree::TreePtr &parent, DCMTree::TagId tagId);
  //! write RT Structure in DICOM tree to a file
  void writeToDCMFile(DCMTree::TreePtr tree, std::string dest);
  //! Combine SOPINstUID and contourImage Tag
  bool combineSOPInstUID(DCMTree::TagPtr conImageTag, std::string UID);
  //! add CSO Group Tags
  bool addCSOGroupTags(DCMTree::TagPtr csoGroupTag, std::string refFOfRef);
  //! add CSO Tags
  bool addCSOTags(DCMTree::TagPtr csoTag,DCMTree::TreePtr contourTree);
  //! add CSO Path points to RT Structure
  void addCSOPathPoint(DCMTree::TreePtr childTree, DCMTree::TagId tagId, DCMTree::Vr vr, ml::CSO* cso);
  //! add RTROIOB Tags
  void addRTROIObTags(DCMTree::TagPtr RTROIObTag, DCMTree::TreePtr RTROIObSeq);
  //! set valid Field
  void setValidField();



   //! Can be notified from anywhere in the network.
  static void CSOListNotifyObserverCB(void* userData, int notificationFlag);

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CSOToRTStructure)
};


ML_END_NAMESPACE

#endif // __mlCSOToRTStructure_H

