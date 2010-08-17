//----------------------------------------------------------------------------------
//! The ML module class CSOToRTStructure.
/*!
// \file    mlCSOToRTStructure.cpp
// \author  linal
// \date    2010-01-20
//
// convert CSOs to RT Structure
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlCSOToRTStructure.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include "mlDicomTreeImagePropertyExtension.h"
#include "mlDicomTreeAccess.h"
#include "DCMTree_StructuredMF.h"

#include <iostream>
#include <time.h>

using namespace boost::filesystem; 
using namespace std;

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CSOToRTStructure, BaseOp);

std::map<std::string, std::string> CSOToRTStructure::_modalitySOPClassUIDMap;
bool CSOToRTStructure::isInitialized = false;

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CSOToRTStructure::CSOToRTStructure ()
  : BaseOp(1, 1)
{
  ML_TRACE_IN("CSOToRTStructure::CSOToRTStructure ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();
  //! Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();
  getInField(0)->attachField(getOutField(0));
  _inputCSOs = NULL;
  
  ML_CHECK(&fields);

  (_inListFld = fields.addBase("inCSOList"))->setBaseValue(NULL);
 
  _rtStructFileFld = fields.addString("rtStructFile");
  _applyFld = fields.addNotify("apply");
  _validFld = fields.addToggle("valid");
  _validFld->setIntValue(false);
  //_inListFld->attachField(_validFld);
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void CSOToRTStructure::handleNotification (Field *field) {
  ML_TRACE_IN("CSOToRTStructure::handleNotification ()");
  
  // Handle changes of module parameters and connectors here.
  // check input values
  if (field == _inListFld) {
    if ( !_isInNotificationCB ) {  
		CSOList::removeNotificationObserverFromAllCSOLists( CSOListNotifyObserverCB,  
		this);  
	} 
    Base *csoListB = _inListFld->getBaseValue();
    _inputCSOs = mlbase_cast<CSOList*>(csoListB);
    if (_inputCSOs == NULL) {
		return;
    }
    if (!_isInNotificationCB) {  
		_inputCSOs->addNotificationObserver(CSOListNotifyObserverCB, this);  
    }
  }else if (field == _applyFld){
	if (_treeHeaderPtr != NULL) {
	  DCMTree::TreePtr myNewTree(new DCMTree::Tree());
	  _genTreePtr = myNewTree;
	  if (combineHeader() == true) {
			writeToDCMFile(_genTreePtr, _rtStructFileFld->getStringValue());
	  }
	}
	}
  setValid(this);
}

//----------------------------------------------------------------------------------
//! Callback function for CSOList input field.
//----------------------------------------------------------------------------------
void CSOToRTStructure::CSOListNotifyObserverCB(void* userData, int notificationFlag) 
{ 
  CSOToRTStructure* myInst = static_cast<CSOToRTStructure*>(userData); 
  myInst->_isInNotificationCB = true; 
  if ( (notificationFlag & CSOList::NOTIFICATION_CSO_FINISHED   ) || 
       (notificationFlag & CSOList::NOTIFICATION_GROUP_FINISHED ) || 
       (notificationFlag & CSOList::NOTIFICATION_CSO_SELECTION  ) || 
       (notificationFlag & CSOList::NOTIFICATION_GROUP_SELECTION) ){ 
      // Do your notification handling here 
    setValid(myInst);
  } 
  myInst->_isInNotificationCB = false; 
} 

//----------------------------------------------------------------------------------
//! Set Valid field in UI. If TreeHeader and inputCSO are not NULL, valid is checked
//----------------------------------------------------------------------------------
void CSOToRTStructure::setValid(CSOToRTStructure* obj) 
{
  if (obj->getTreeHeaderPtr() == NULL && obj->getUpdatedInImg(0) != NULL) {
    PagedImg *img = obj->getUpdatedInImg(0);
	obj->updateGenTreeHeader(img);
  }
  if (obj->getUpdatedInImg(0) == NULL || obj->getInputCSOs() == NULL) { 
	obj->setValidFld(false);
		  
  }else {
	obj->setValidFld(true);  
  }
}

//----------------------------------------------------------------------------------
//! ActiveAttachments
//----------------------------------------------------------------------------------
void CSOToRTStructure::activateAttachments ()
{
  ML_TRACE_IN("test::activateAttachments ()");

  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  // For details see section "Module Persistence and Overloading activateAttachments()"
  // in ML Programming Guide.
  BaseOp::activateAttachments ();
}

//----------------------------------------------------------------------------------
//! Handle Input
//----------------------------------------------------------------------------------
BaseOp::INPUT_HANDLE CSOToRTStructure::handleInput(int /*inIndex*/, INPUT_STATE /*state*/) const
{
  ML_TRACE_IN("ChamferMatching::handleInput ()")

  return INVALIDATE; 
}

//----------------------------------------------------------------------------------
//! Combine SOPInstance UID
//----------------------------------------------------------------------------------
bool CSOToRTStructure::combineSOPInstUID(DCMTree::TagPtr conImageTag, std::string SOPCUID){
  if (_mSOPInstUIDs.size() == 0) {
		return false;
  }
  for(std::map<float,std::string>::const_iterator it = _mSOPInstUIDs.begin(); it != _mSOPInstUIDs.end(); ++it) {
    DCMTree::TreePtr childTree(new DCMTree::Tree());
	conImageTag->addSequenceItem(childTree);
	addTag(childTree, DCMTree::TagId(0x0008,0x1150), SOPCUID, DCMTree::UI);
	addTag(childTree, DCMTree::TagId(0x0008,0x1155),it->second, DCMTree::UI);
		
  }
  return true;
}

//----------------------------------------------------------------------------------
//! update Generated Tree
//----------------------------------------------------------------------------------
void CSOToRTStructure::updateGenTreeHeader(PagedImg *img) {
  if (!isInitialized) {
		// Add initClass calls from all other modules here...
	DCMTree_Utils::UIDGenerator::initialize("1.2.276.0.28","3","42");
	   // init the modality and SOPClassUID map
	CSOToRTStructure::_modalitySOPClassUIDMap.insert(std::pair<std::string,std::string>("RTSTRUCT","1.2.840.10008.5.1.4.1.1.481.3"));
	isInitialized = true;
  }

  const RuntimeType*               runtimeType       = DicomTreeImagePropertyExtension::getClassTypeId();  
  ImagePropertyExtensionContainer& propertyContainer = img->getImagePropertyContainer(); 
	  
  if ( propertyContainer.getNumEntries() > 0 ){ 
    DicomTreeImagePropertyExtension* propertyExtension = reinterpret_cast<DicomTreeImagePropertyExtension*> 
														   (propertyContainer.getFirstEntryOfType(runtimeType));  
	DCMTree::Const_TreePtr           dicomTreePtr      = getDicomTreeFromImagePropertyExtension(propertyExtension);  	
	if (dicomTreePtr == NULL)
		return;
	ml::Vector ext = img->getImgExt();
	DCMTree::TreePtr                 childTreePtr(new DCMTree::Tree());
	childTreePtr = dicomTreePtr->copy();
	DCMTree::StructuredMF structuredMF( childTreePtr, ext[2], ext[4], ext[5]);  
	// Get the extent of the dicom tree.  
	const DCMTree::StructuredMF::IndexVector& indices = structuredMF.getGridExtent();  
	if ( indices.size() > 0 ) { 
	  unsigned int num = indices[0];
	  DCMTree::TagPtr tagTPtr = structuredMF.tagForGridPosition_rw( DCMTree::TagId(0x0008,0x0033), 0, 0); 
	  std::string  tagTS=tagTPtr->toString();
	  for (unsigned int z=0; z< num; ++z) {  
		// Get the tag at slice z. 
		DCMTree::TagInfo info(DCMTree::TagId(0x0008,0x0018), DCMTree::UI, DCMTree::TagValueMultiplicity(1,1));
		DCMTree::TagPtr newTag(new DCMTree::Tag(info));
		structuredMF.setTagForGridPosition(newTag, z, 0);
		string SOPInstUID = DCMTree_Utils::UIDGenerator::uid();
		newTag->addValue(SOPInstUID);
		img->getVoxelSize(NULL, NULL, &_voxelSizeZ);
		ml::vec3 vec(0,0,z);
		ml::vec3 vecWorld;
		img->transformToWorldCoord(vec,vecWorld);
		float worldPosZ = vecWorld[2];
		_mSOPInstUIDs.insert(pair<float,std::string>(worldPosZ,SOPInstUID)); 
	  }  
	}
	_treeHeaderPtr = childTreePtr;
  }
}

//----------------------------------------------------------------------------------
//! calculate image props
//----------------------------------------------------------------------------------
void  CSOToRTStructure::calcOutImageProps (int /*outIndex*/){
  ml::PagedImg* img = getOutImg();
  if (img == NULL) 
    return;
	
  updateGenTreeHeader(img);
  if (_treeHeaderPtr == NULL) return;
  const RuntimeType*               runtimeType       = DicomTreeImagePropertyExtension::getClassTypeId();  
  ImagePropertyExtensionContainer& outPropertyContainer = getOutImg(0)->getImagePropertyContainer(); 
  DicomTreeImagePropertyExtension* outPropertyExtension = reinterpret_cast<DicomTreeImagePropertyExtension*> 
													   (outPropertyContainer.getFirstEntryOfType(runtimeType));  
  if (outPropertyExtension!= NULL)
    outPropertyExtension->setDicomTagTree(_treeHeaderPtr); 					
}

CALC_OUTSUBIMAGE1(CSOToRTStructure);
//----------------------------------------------------------------------------------
//! Get Reference image SOP UID.
//----------------------------------------------------------------------------------
std::string  CSOToRTStructure::getRefUID(double z)
{
  float key = 0;
  float minDist = _voxelSizeZ/2 + 0.0002;
  for(std::map<float,std::string>::const_iterator it = _mSOPInstUIDs.begin(); it != _mSOPInstUIDs.end(); ++it) {
    if (minDist > abs(it->first - z)) {
	  minDist = abs(it->first - z);
	  key = it->first;
	}
  }
  if (key != 0) {
	std::map<float,std::string>::const_iterator rt = _mSOPInstUIDs.find(key);
	return rt->second;
  } else 
	return "";
}
	
//----------------------------------------------------------------------------------
//! add a Sequence tree
//----------------------------------------------------------------------------------
void CSOToRTStructure::addSequence(DCMTree::TreePtr parentTree, DCMTree::TreePtr childTree, DCMTree::TagId tagId)
{
  addSequenceTag(parentTree, tagId);
  DCMTree::TagPtr childSeq = parentTree->getTag_rw(tagId);
  childSeq->addSequenceItem(childTree);
}

//----------------------------------------------------------------------------------
//! add Sequence Tag  with a tagId
//----------------------------------------------------------------------------------
void CSOToRTStructure::addSequenceTag(DCMTree::TreePtr &parent, DCMTree::TagId tagId)
{
  DCMTree::TagInfo info(tagId, DCMTree::SQ, DCMTree::TagValueMultiplicity(1,0xffff));
  DCMTree::TagPtr newTag(new DCMTree::Tag(info));
  parent->addTag(tagId, newTag);
}

 
//----------------------------------------------------------------------------------
//! add a Tag with tagId and value into a tree  
//----------------------------------------------------------------------------------
void CSOToRTStructure::addTag(DCMTree::TreePtr &parent, DCMTree::TagId tagId, std::string value, DCMTree::Vr vr) 
{
  DCMTree::TagInfo info(tagId, vr, DCMTree::TagValueMultiplicity(1,1));
  DCMTree::TagPtr newTag(new DCMTree::Tag(info));
  newTag->addValue(value);
  parent->addTag(tagId, newTag);
}

//----------------------------------------------------------------------------------
//! generates the RT structure file. It combine some common headers from Input Image file and from input CSO 
//----------------------------------------------------------------------------------
bool CSOToRTStructure::combineHeader() 
{
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0000), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0000)));
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0001), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0001)));
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0002), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0002)));
  addTag(_genTreePtr, DCMTree::TagId(0x0002,0x0002), _modalitySOPClassUIDMap["RTSTRUCT"], DCMTree::UI);
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0003), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0003)));
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0010), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0010)));
  _genTreePtr->addTag(DCMTree::TagId(0x0002,0x0012), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0012)));

	time_t now;
	struct tm *timeinfo;
  char buffer[10];
	char buffer1[10];
	time(&now);
  timeinfo = localtime(&now);
	strftime(buffer, 10, "%H%M%S",timeinfo);
  strftime(buffer1, 10, "%Y%m%d",timeinfo);	
	addTag(_genTreePtr, DCMTree::TagId(0x0008, 0x0012), buffer1, DCMTree::DA);
	addTag(_genTreePtr, DCMTree::TagId(0x0008, 0x0013), buffer, DCMTree::TM);

  string transferSUID = _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0010))->toString();
	addTag(_genTreePtr, DCMTree::TagId(0x0008,0x0016), _modalitySOPClassUIDMap["RTSTRUCT"], DCMTree::UI);
	//generate the SOP Instance UID for the RT structure file
	addTag(_genTreePtr, DCMTree::TagId(0x0008,0x0018),DCMTree_Utils::UIDGenerator::uid(), DCMTree::UI);
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x0020), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0020)));
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x0030), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0030)));
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x0050), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0050)));

	DCMTree::TagPtr modTagPtr = _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0060));
	string modality = modTagPtr->toString();
	addTag(_genTreePtr, DCMTree::TagId(0x0008,0x0060),"RTSTRUCT", DCMTree::CS);
    
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x0070), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0070)));
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x0090), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x0090)));
	//hard code "Mevis" here, need to find how to get machine name
	addTag(_genTreePtr, DCMTree::TagId(0x0008,0x1010), "MevisLab", DCMTree::SH);
	_genTreePtr->addTag(DCMTree::TagId(0x0008,0x103e), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x103e)));
  addTag(_genTreePtr, DCMTree::TagId(0x0008,0x103e), "Structure Set", DCMTree::LO);
  _genTreePtr->addTag(DCMTree::TagId(0x0008,0x1090), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0008,0x1090)));
	_genTreePtr->addTag(DCMTree::TagId(0x0010,0x0010), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0010,0x0010)));
	_genTreePtr->addTag(DCMTree::TagId(0x0010,0x0020), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0010,0x0020)));
	_genTreePtr->addTag(DCMTree::TagId(0x0010,0x0030), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0010,0x0030)));
	_genTreePtr->addTag(DCMTree::TagId(0x0010,0x0040), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0010,0x0040)));

	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x000d), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x000d)));
	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x000e), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x000e)));
	std::string seriesInstUID = _treeHeaderPtr->getTag(DCMTree::TagId(DCMTree::TagId(0x0020,0x000e)))->toString();

	_genTreePtr->addTag(DCMTree::TagId(0x0010,0x0040), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0010,0x0040)));
	addTag(_genTreePtr,DCMTree::TagId(0x0018,0x1020), "OS 8.1.18", DCMTree::LO);
	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x000d), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x000d)));
	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x000e), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x000e)));
	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x0010), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x0010)));
	_genTreePtr->addTag(DCMTree::TagId(0x0020,0x0011), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x0011)));
	_genTreePtr->getTag_rw(DCMTree::TagId(0x0020,0x0011))->setValue("1");
	string label = _genTreePtr->getTag_rw(DCMTree::TagId(0x0008,0x0060))->toString();
	label.append("_1");
	addTag(_genTreePtr, DCMTree::TagId(0x3006,0x0002), label, DCMTree::SH);
	addTag(_genTreePtr, DCMTree::TagId(0x3006,0x0008),"",DCMTree::DA);
	addTag(_genTreePtr,DCMTree::TagId(0x3006,0x0009),"",DCMTree::TM);
    
	DCMTree::TreePtr embeddedTree(new DCMTree::Tree());
	addSequence(_genTreePtr, embeddedTree, DCMTree::TagId(0x3006,0x0010));
	

	embeddedTree->addTag(DCMTree::TagId(0x0020,0x0052), _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x0052)));
	std::string frameofReferencedFrameUID = embeddedTree->getTag(DCMTree::TagId(0x0020,0x0052))->toString();

	DCMTree::TreePtr refStudySeq(new DCMTree::Tree());
	addSequence(embeddedTree, refStudySeq, DCMTree::TagId(0x3006,0x0012));
	
	_refSopClassUID = _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0002,0x0002))->toString();
	addTag(refStudySeq,DCMTree::TagId(0x0008,0x1150), _refSopClassUID, DCMTree::UI);

  string SOPInsUID = _treeHeaderPtr->getTag_rw(DCMTree::TagId(0x0020,0x0052))->toString(); 
	addTag(refStudySeq,DCMTree::TagId(0x0008,0x1155),SOPInsUID.substr(0, SOPInsUID.find_last_of(".")), DCMTree::UI); 
    
	DCMTree::TreePtr refSeriesSeq(new DCMTree::Tree());
	addSequence(refStudySeq, refSeriesSeq, DCMTree::TagId(0x3006,0x0014));

  addTag(refSeriesSeq,DCMTree::TagId(0x0020,0x000e),seriesInstUID,DCMTree::UI);
	addSequenceTag(refSeriesSeq,DCMTree::TagId(0x3006, 0x0016));
	DCMTree::TagPtr conImageTag = refSeriesSeq->getTag_rw(DCMTree::TagId(0x3006, 0x0016));
	
	std::string refSOPClassUID = _treeHeaderPtr->getTag(DCMTree::TagId(0x0008,0x0016))->toString();
	if (combineSOPInstUID(conImageTag, refSOPClassUID) == false) {
		return false;
	}
	addSequenceTag(_genTreePtr, DCMTree::TagId(0x3006,0x0020));
  DCMTree::TagPtr csoGroupTag = _genTreePtr->getTag_rw(DCMTree::TagId(0x3006, 0x0020));
	std::string studyID = (_genTreePtr->getTag(DCMTree::TagId(0x0020,0x000d)))->toString();
	if (addCSOGroupTags(csoGroupTag, frameofReferencedFrameUID) == false) {
		return false;
	}
	DCMTree::TreePtr ROIContourSeq(new DCMTree::Tree());
  addSequence(_genTreePtr, ROIContourSeq, DCMTree::TagId(0x3006,0x0039));
	
	DCMTree::TagPtr csoTag = _genTreePtr->getTag_rw(DCMTree::TagId(0x3006, 0x0039));
	if (addCSOTags(csoTag, ROIContourSeq) == false) {
		return false;
	}

  // The following tags are not necessary in RT structure file 
  DCMTree::TreePtr RTROIObSeq(new DCMTree::Tree());
  addSequence(_genTreePtr, RTROIObSeq, DCMTree::TagId(0x3006,0x0080));
	
	DCMTree::TagPtr RTROIObTag = _genTreePtr->getTag_rw(DCMTree::TagId(0x3006, 0x0080));
	addRTROIObTags(RTROIObTag, RTROIObSeq);
	return true;
}

//----------------------------------------------------------------------------------
//! Add the RTR IOB Tags in RT structure
//----------------------------------------------------------------------------------
void CSOToRTStructure::addRTROIObTags(DCMTree::TagPtr RTROIObTag, DCMTree::TreePtr /*RTROIObSeq*/) 
{
	size_t seqSize = RTROIObTag->sequenceItems().size();
	if (seqSize != 0) {
		RTROIObTag->sequenceItems().erase(RTROIObTag->sequenceItems().begin(), 
		RTROIObTag->sequenceItems().end());
	}
	unsigned int groups = _inputCSOs->numGroups();
	for (unsigned int i = 0; i < groups; i ++) {
		CSOGroup* group = _inputCSOs->getGroupAt(i);
		DCMTree::TreePtr childTree(new DCMTree::Tree());
		RTROIObTag->addSequenceItem(childTree);
    std::stringstream s;
	  s << int(i+1);
    std::stringstream sId;
	  sId << int(group->getId());
    addTag(childTree, DCMTree::TagId(0x3006,0x0082),s.str(), DCMTree::IS); 
		addTag(childTree, DCMTree::TagId(0x3006,0x0084),sId.str(), DCMTree::IS); 
		addTag(childTree, DCMTree::TagId(0x3006,0x0085), group->getLabel(), DCMTree::SH);
		// hard code here
    addTag(childTree, DCMTree::TagId(0x3006,0x00a4), "ORGAN", DCMTree::CS);
		addTag(childTree, DCMTree::TagId(0x3006,0x00a6), "", DCMTree::PN);
	}
}

//----------------------------------------------------------------------------------
//! add CSO group Tags in RT structure
//----------------------------------------------------------------------------------
bool CSOToRTStructure::addCSOGroupTags(DCMTree::TagPtr csoGroupTag, std::string refFOfRef) {
	if (_inputCSOs== NULL){
		ML_PRINT_ERROR("mlCSOToRTStructure::addCSOGroupTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because there is no input CSOs.");
		return false;
	}
  unsigned int groupNum = _inputCSOs->numGroups();
  if (groupNum == 0) {
		ML_PRINT_ERROR("mlCSOToRTStructure::addCSOGroupTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because there are no CSO groups.");
		return false;
	}
	for (unsigned int i=0; i < _inputCSOs->numGroups(); i++) {
	  CSOGroup* group = _inputCSOs->getGroupAt(i);
		DCMTree::TreePtr childTree(new DCMTree::Tree());
		DCMTree::TagInfo childInfo1(DCMTree::TagId(0x3006,0x0022),DCMTree::IS,DCMTree::TagValueMultiplicity(1,1)); 
		DCMTree::TagInfo childInfo2(DCMTree::TagId(0x3006,0x0024),DCMTree::UI,DCMTree::TagValueMultiplicity(1,1)); 
		DCMTree::TagInfo childInfo3(DCMTree::TagId(0x3006,0x0026),DCMTree::LO,DCMTree::TagValueMultiplicity(1,1)); 
		DCMTree::TagInfo childInfo4(DCMTree::TagId(0x3006,0x0036),DCMTree::CS,DCMTree::TagValueMultiplicity(1,1)); 
		DCMTree::TagPtr itemTag1(new DCMTree::Tag(childInfo1));
		DCMTree::TagPtr itemTag2(new DCMTree::Tag(childInfo2));
		DCMTree::TagPtr itemTag3(new DCMTree::Tag(childInfo3));
		DCMTree::TagPtr itemTag4(new DCMTree::Tag(childInfo4));
		std::stringstream s;
		s << int(group->getId());
		itemTag1->addValue(s.str());
		itemTag2->addValue(refFOfRef);
		std::string label = group->getLabel();
		if (label.empty()) {
		  label.assign("Group ");
			std::stringstream s;
		  s << int(group->getId());
		  label.append(s.str());

		  ML_PRINT_WARNING("mlCSOToRTStructure::addCSOGroupTags",ML_BAD_PARAMETER, "There is no CSO group label.Add default as group [number]");
		}
		itemTag3->addValue(label);
		childTree->addTag(childInfo1.id(), itemTag1);
		childTree->addTag(childInfo2.id(), itemTag2);
		childTree->addTag(childInfo3.id(), itemTag3);
		childTree->addTag(childInfo4.id(), itemTag4);
		csoGroupTag->addSequenceItem(childTree);
	}
	return true;
}

//----------------------------------------------------------------------------------
//! add CSO Tags into RT structure
//----------------------------------------------------------------------------------
bool CSOToRTStructure::addCSOTags(DCMTree::TagPtr csoTag, DCMTree::TreePtr /*contourTree*/) {
	if (_inputCSOs== NULL){
		ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because there is no input CSOs.");
		return false;
	}
  unsigned int num = _inputCSOs->numCSO();
	if (num == 0) {
		ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because there is no input CSOs.");
		return false;
	}
	unsigned int groupNum = _inputCSOs->numGroups();
	if (groupNum == 0) {
		ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because there are no CSO groups.");
		return false;
	}
	size_t seqSize = csoTag->sequenceItems().size();
	if (seqSize != 0) {
		csoTag->sequenceItems().erase(csoTag->sequenceItems().begin(), 
		csoTag->sequenceItems().end());
	}
	for (unsigned int gInd = 0; gInd < groupNum; gInd ++) {
		CSOGroup* group = _inputCSOs->getGroupAt(gInd);
		if (group == NULL || group->numCSO() == 0){ 
			ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "RT structure file cannot be generated because some groups donot contain any CSOs.");
			return false;
		}
		DCMTree::TreePtr csoGTree(new DCMTree::Tree());
	
		ml::vec3 color = group->getColor();
		string strColor[3];
		DCMTree::TagInfo info(DCMTree::TagId(0x3006,0x002a), DCMTree::IS,DCMTree::TagValueMultiplicity(1,1)); 
		DCMTree::TagPtr newTag(new DCMTree::Tag(info));
		csoGTree->addTag(DCMTree::TagId(0x3006,0x002a), newTag);
		for (int i = 0; i < 3; i++) {
			std::stringstream s;
			s << int(color[i]*255);
			strColor[i].assign(s.str());
			newTag->addValue(strColor[i]);
		}	      
		DCMTree::TreePtr childGTree(new DCMTree::Tree());
		addSequence(csoGTree, childGTree, DCMTree::TagId(0x3006,0x0040));
		DCMTree::TagPtr childGTag = csoGTree->getTag_rw(DCMTree::TagId(0x3006, 0x0040));
		size_t seqGSize = childGTag->sequenceItems().size();
		if (seqGSize != 0) {
			childGTag->sequenceItems().erase(childGTag->sequenceItems().begin(), 
			childGTag->sequenceItems().end());
		}
            
		unsigned int csoNum = group->numCSO();
		bool colorAdded = false;
    for (unsigned int in = 0; in < csoNum; in++) {
			CSO* cso = group->getCSOAt(in);
			//std::cout << cso->getId();
			if (cso->isInPlane()) {
				CSOSeedPoint *sp = cso->getFirstSeedPoint();
				double z = sp->worldPosition[2];
				string refUID = getRefUID(z);
				if (refUID.empty()){ 
				  ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "There are no matching slices for a CSO." + cso->getId());
				  return false;
				}
				DCMTree::TreePtr csoCTree(new DCMTree::Tree());
				DCMTree::TreePtr seqTree(new DCMTree::Tree());

				addSequence(csoCTree, seqTree, DCMTree::TagId(0x3006,0x0016));
				DCMTree::TagPtr UIDs = csoCTree->getTag_rw(DCMTree::TagId(0x3006,0x0016));

				addTag(seqTree, DCMTree::TagId(0x0008,0x1150),_refSopClassUID, DCMTree::UI); 
				addTag(seqTree, DCMTree::TagId(0x0008,0x1155),refUID, DCMTree::UI); 
				addTag(csoCTree, DCMTree::TagId(0x3006,0x0042),"CLOSED_PLANAR", DCMTree::CS); 
				size_t ptNum = 0;
				for (unsigned int i = 0; i < cso->numPathPointLists(); i ++){
				  ml::CSOPathPoints* pt = cso->getPathPointsAt(i);
				  std::vector<vec3> vec;
				  pt->getWorldPositions(vec);
				  ptNum = ptNum + vec.size() + 1;
				}
				std::stringstream s;
				s << int(ptNum);
				addTag(csoCTree, DCMTree::TagId(0x3006,0x0046), s.str(), DCMTree::IS); 
				addCSOPathPoint(csoCTree, DCMTree::TagId(0x3006,0x0050), DCMTree::DS, cso);
				if (colorAdded == false) {
				  csoTag->addSequenceItem(csoGTree);
				   colorAdded = true;
				}
				childGTag->addSequenceItem(csoCTree);
			}else {
				 ML_PRINT_ERROR("mlCSOToRTStructure::addCSOTags",ML_BAD_PARAMETER, "CSO must be in one plane to proceed.");
				 return false;
			 }
		}
    if(colorAdded == true) {
			std::stringstream sg;
			sg << int(group->getId());
			addTag(csoGTree, DCMTree::TagId(0x3006,0x0084), sg.str(), DCMTree::IS); 
		}
	}
  return true;
}

//----------------------------------------------------------------------------------
//! Add CSO Path points
//----------------------------------------------------------------------------------
void CSOToRTStructure::addCSOPathPoint(DCMTree::TreePtr childTree, DCMTree::TagId tagId, DCMTree::Vr vr, ml::CSO* cso){
	DCMTree::TagInfo info(tagId, vr, DCMTree::TagValueMultiplicity(1,1));
	DCMTree::TagPtr newTag(new DCMTree::Tag(info));
	for (unsigned int i = 0; i < cso->numPathPointLists(); i ++){
		ml::CSOPathPoints* pt = cso->getPathPointsAt(i);
		std::vector<vec3> vec;
		pt->getWorldPositions(vec);
	  ml::CSOSeedPoint *tailPt = pt->tailSeedPoint;
		ml::vec3 vecT = tailPt->worldPosition;
		for (int i = 0; i < 3; i++){
			std::stringstream s;
			s << vecT[i];
			std::string m = s.str();
			newTag->addValue(s.str());
		}
	  for (unsigned int j = 0; j < vec.size(); j ++){
			for (int coordInd = 0; coordInd < 3; coordInd++) { 
				std::stringstream s;
				s << vec[j][coordInd];
				std::string m = s.str();
				newTag->addValue(s.str());
			}
		}
	}
	childTree->addTag(tagId, newTag);  
}

//----------------------------------------------------------------------------------
//! write to a DCMTree to a RT structure file
//----------------------------------------------------------------------------------
void CSOToRTStructure::writeToDCMFile(DCMTree::TreePtr tree, std::string dest) {

	DCMTree::MessagePtr messagePtr = DCMTree::MessagePtr( 
    new DCMTree::Message(DCMTree_Unicode::UTF8CharacterSet(), DCMTree::IMPLICIT_LITTLE_ENDIAN)); 
  messagePtr->setTags(tree); 

  // Create writer object 
	DCMTree::WriterPtr writer = DCMTree::Writer::create(DCMTree::Dict::singleton()); 
 
  // Write message 
	DCMTree::IOParameterPtr param(DCMTree::IOParameter::create(dest)); 
  writer->write(param, messagePtr); 
}

ML_END_NAMESPACE

