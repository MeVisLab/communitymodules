// Local includes
#include "../METK.h"
#include "mlMETKPicking.h"

#include <Inventor/nodes/SoSeparator.h>
#include "SoPickObject.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKPicking, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor / Destructor
//----------------------------------------------------------------------------------
METKPicking::METKPicking() : inherited(0,0,ObjMgrClient::EVENTS_NONE)
{
	ML_TRACE_IN("METKPicking::METKPicking()");

	// Suppress calls of handleNotification on field changes.
	handleNotificationOff();

	_scrapLight = getFieldContainer()->addFloat("scrapLight");
	_importanceWeighting = getFieldContainer()->addFloat("importanceWeighting");
	_boundingBoxSizeWeighting = getFieldContainer()->addFloat("boundingBoxSizeWeighting");
   _showBoundingBoxes = getFieldContainer()->addBool("showBoundingBoxes");

	//_mustBePickable = getFieldContainer()->addBool("mustBePickable");
	_pickPixelTolerance = getFieldContainer()->addInt("pickPixelTolerance");

	_outPickedObject = getFieldContainer()->addSoNode("outPickedObject");
	_outScene = getFieldContainer()->addSoNode("outScene");
	_objName = getFieldContainer()->addString("objName");
	_objList = getFieldContainer()->addString("objList");
	_mouseOverName = getFieldContainer()->addString("mouseOverName");
	_mouseOverList = getFieldContainer()->addString("mouseOverList");

	_selection = getFieldContainer()->addVec3f("selection");

	_enableMouseOver = getFieldContainer()->addBool("enableMouseOver");
	_enableMouseOver->setBoolValue(false);

	_pickedObjectFound = false;

	_pickObject = new SoPickObject();
	_pickObject->ref();
	_pickObject->setMLClass(this);
	_pickObject->setEnableMouseOver(_enableMouseOver->getBoolValue());

	_scrapLight->setFloatValue(0.05);
	_importanceWeighting->setFloatValue(0.75);
	_boundingBoxSizeWeighting->setFloatValue(0.5);
   _showBoundingBoxes->setBoolValue(false);
	//_mustBePickable->setBoolValue(true);
	_pickPixelTolerance->setIntValue(3);

	_outScene->setSoNodeValue(_pickObject);
	_outPickedObject->setSoNodeValue(0);

	_objName->setStringValue("");
	_objList->setStringValue("");
	_mouseOverName->setStringValue("");
	_mouseOverList->setStringValue("");
	_selection->setVec3fValue(vec3(0, 0, 0));	

	clearAcceptedObjectIDs();
	clearAcceptedInfoLayerNames();

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}


METKPicking::~METKPicking() {
	ML_TRACE_IN("METKPicking::~METKPicking()");
	_pickObject->unref();
}


void METKPicking::activateAttachments()
{
	ML_TRACE_IN("METKPicking::activateAttachments()");
	clearAcceptedObjectIDs();
	clearAcceptedInfoLayerNames();

	_objName->setStringValue("");
	_objList->setStringValue("");
	_mouseOverName->setStringValue("");
	_mouseOverList->setStringValue("");
	_selection->setVec3fValue(vec3(0, 0, 0));

	sendNotification();

	// Don't forget to call the super class functionality, it enables field
	// notifications for your module again. 
	// SUPER_CLASS is the class you derive from (usually BaseOp).
	ObjMgrClient::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKPicking::handleNotification(Field *field)
{
	ML_TRACE_IN("METKPicking::handleNotification(Field *field)");

	_pickObject->setScrapLight(_scrapLight->getFloatValue());
	_pickObject->setBBWeight(_boundingBoxSizeWeighting->getFloatValue());
	_pickObject->setImportanceWeight(_importanceWeighting->getFloatValue());
	_pickObject->setPickPixelTolerance(_pickPixelTolerance->getIntValue());
	_pickObject->setshowBB(_showBoundingBoxes->getBoolValue());
	_pickObject->setEnableMouseOver(_enableMouseOver->getBoolValue());
	inherited::handleNotification(field);
}


const bool METKPicking::containsNode(const SoNode* node) {
	ML_TRACE_IN("METKPicking::containsNode(const SoNode* node)");
	omObjectContainer* oc = getObjContainer();
	_pickedObjectFound = false;
	if (oc) {
		for (omObjectContainer::const_iterator iter = oc->begin(); iter != oc->end(); iter++) {
			if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
				bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
				if (pValid) {
					const int pointer = iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32();
					if ((const SoNode*) pointer == node) {
						_pickedObjectIter = iter;
                  _pickedObjectFound = true;
						return true;
					}
				}
			}
		}
	}
	return false;
}


const float METKPicking::getTransparency(const SoNode* node) {
	ML_TRACE_IN("METKPicking::getTransparency(const SoNode* node)");

	if (_pickedObjectFound == true && ((const SoNode*) _pickedObjectIter->second[LAY_FILES][INF_IVPOINTER].get_MLint32() == node)) {
		return _pickedObjectIter->second[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
	} else {
		omObjectContainer* oc = getObjContainer();
		if (oc) {
			for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
				if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
					bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
					if (pValid) {
						const int pointer = iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32();
						if ((const SoNode*) pointer == node) {
							return iter->second[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
						}
					}
				}
			}
		}
	}
	return -1;
}


const float METKPicking::getImportance(const SoNode* node) {
	ML_TRACE_IN("METKPicking::getImportance(const SoNode* node)");

	if (_pickedObjectFound == true && ((const SoNode*) _pickedObjectIter->second[LAY_FILES][INF_IVPOINTER].get_MLint32() == node)) {
		if (_pickedObjectIter->second.hasAttribute(LAY_DESCRIPTION, INF_IMPORTANCE))
			return _pickedObjectIter->second[LAY_DESCRIPTION][INF_IMPORTANCE].get_double();
		else
			return 1 - _pickedObjectIter->second[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
	} else {
		omObjectContainer* oc = getObjContainer();
		if (oc) {
			for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
				if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
					bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
					if (pValid) {
						const int pointer = iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32();
						if ((const SoNode*) pointer == node) {
							return iter->second[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
							if (iter->second.hasAttribute(LAY_DESCRIPTION, INF_IMPORTANCE))
								return iter->second[LAY_DESCRIPTION][INF_IMPORTANCE].get_double();
							else
								return 1 - iter->second[LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
						}
					}
				}
			}
		}
	}
	return 0;
}

const bool METKPicking::getPickable(const SoNode* node) {
	ML_TRACE_IN("METKPicking::getPickable(const SoNode* node)");

	omObjectContainer* oc = getObjContainer();
	if ((*oc).exists(OBJ_CSO)) {
		if ((*oc)[OBJ_CSO].hasAttribute(LAY_GLOBAL, INF_IGNOREPICK)) {
			if ((*oc)[OBJ_CSO][LAY_GLOBAL][INF_IGNOREPICK].get_bool()) return true;
		} else return true;
	} else return true;

	if (_pickedObjectFound == true && (const SoNode*) _pickedObjectIter->second[LAY_FILES][INF_IVPOINTER].get_MLint32() == node) {
		if (_pickedObjectIter->second.hasAttribute(LAY_APPEARANCE, INF_PICKSTYLE) && _pickedObjectIter->second[LAY_APPEARANCE][INF_PICKSTYLE].get_std_string() != "UNPICKABLE")
			return true;
		else
			return false;
	} else {
		omObjectContainer* oc = getObjContainer();
		if (oc) {
			for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
				if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
					bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
					if (pValid) {
						const int pointer = iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32();
						if ((const SoNode*) pointer == node) {
							if (iter->second[LAY_APPEARANCE][INF_PICKSTYLE].get_std_string() == "UNPICKABLE")
								return false;
							else
								return true;
						}
					}
				}
			}
		}
	}
	return false;
}


void METKPicking::setPickedShape(SoNode* node, SbVec3f* selection, std::vector<SoNode*>* allObjInRay) {
	ML_TRACE_IN("METKPicking::setPickedShape(SoNode* node, SbVec3f* selection, std::vector<SoNode*>* allObjInRay)");
	_outPickedObject->setSoNodeValue(node);
	_selection->setVec3fValue(vec3(selection->getValue()[0], selection->getValue()[1], selection->getValue()[2]));
	if (node) {   
		omObjectContainer* oc = getObjContainer();
		if (oc) {
			for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
				if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
					bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
					if (pValid && node == (const SoNode*)iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32()) {
						_objName->setStringValue(iter->first);
					}
				}
			}
		}
	} else {
		_objName->setStringValue("");		
	}

	//Set All Objects in Pick Ray String
	std::string listString="";
	if (allObjInRay)
	{
		std::vector<SoNode*>::const_iterator iter;
		listString=",";
		for (iter=allObjInRay->begin(); iter!=allObjInRay->end(); iter++)
		{
			SoNode* tempNode = *iter;
			if (tempNode) {   
				omObjectContainer* oc = getObjContainer();
				if (oc) {
					for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
						if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
							bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
							if (pValid && tempNode == (const SoNode*)iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32()) {
								if (listString.find(","+iter->first+",")==std::string::npos)
								{
									listString+=iter->first+",";
								}
							}
						}
					}
				}
			}			
		}
	}
	listString = kBasics::trimString(listString,',');
	_objList->setStringValue(listString);
}






void METKPicking::setMouseOverShape(SoNode* node, std::vector<SoNode*>* allObjInRay)
{
	ML_TRACE_IN("METKPicking::setMouseOverShape(SoNode* node, SbVec3f* selection, std::vector<SoNode*>* allObjInRay)");
	if (node) {   
		omObjectContainer* oc = getObjContainer();
		if (oc) {
			for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
				if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
					bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
					if (pValid && node == (const SoNode*)iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32()) {
						_mouseOverName->setStringValue(iter->first);
					}
				}
			}
		}
	} else {
		_mouseOverName->setStringValue("");		
	}

	//Set All Objects in Pick Ray String
	/*std::string listString="";
	if (allObjInRay)
	{
		std::vector<SoNode*>::const_iterator iter;
		listString=",";
		for (iter=allObjInRay->begin(); iter!=allObjInRay->end(); iter++)
		{
			SoNode* tempNode = *iter;
			if (tempNode) {   
				omObjectContainer* oc = getObjContainer();
				if (oc) {
					for (omObjectContainer::iterator iter = oc->begin(); iter != oc->end(); iter++) {
						if (iter->second.isValid() && iter->second.exists(LAY_FILES)) {
							bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
							if (pValid && tempNode == (const SoNode*)iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32()) {
								if (listString.find(","+iter->first+",")==std::string::npos)
								{
									listString+=iter->first+",";
								}
							}
						}
					}
				}
			}			
		}
	}
	listString = kBasics::trimString(listString,',');
	_mouseOverList->setStringValue(listString);*/
}


ML_END_NAMESPACE

