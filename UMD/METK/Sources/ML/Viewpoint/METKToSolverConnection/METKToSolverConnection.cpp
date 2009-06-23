// Local includes
#include "METK.h"
#include "METKToSolverConnection.h"

#include <Inventor/nodes/SoSwitch.h>
//#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>

#include <iostream>
#include <stdio.h>

using namespace std;

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKToSolverConnection, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor / Destructor
//----------------------------------------------------------------------------------
METKToSolverConnection::METKToSolverConnection() : inherited(0,0,ObjMgrClient::EVENTS_NONE)
{
  ML_TRACE_IN("METKToSolverConnection::METKToSolverConnection()");

  // Suppress calls of handleNotification on field changes.
  handleNotificationOff();
  _outScene = getFieldContainer()->addSoNode("outScene");
  _init = getFieldContainer()->addNotify("init");
  _dataPath = getFieldContainer()->addString("dataPath");

//  _Sep = new SoSeparator();
  _Switch = new SoSwitch();
  _Switch->whichChild = -3;
  _outScene->setSoNodeValue(_Switch);

  clearAcceptedObjectIDs();
  clearAcceptedInfoLayerNames();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


METKToSolverConnection::~METKToSolverConnection() {
}


void METKToSolverConnection::activateAttachments()
{
   clearAcceptedObjectIDs();
   clearAcceptedInfoLayerNames();
   
   sendNotification();
   
   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKToSolverConnection::handleNotification (Field *field)
{

  //-----------------------------------------------------------------------------
  //INIT
  if (field == _init)
  {
    SoMaterial *oColor;
    int iCount = -1;
    omObjectContainer* oc = getObjContainer();
    const std::string* strErg;
    if (oc)  

    {
      getObjAttributeString(O_CASEOBJECT, LAY_CASEOBJECT_CASE, INF_CASEOBJECT_CASE_DIRECTORY, path);
      _dataPath->setStringValue(path.c_str());
      pathIds = new string(_dataPath->getStringValue());
      pathIds->append("camera_ids.dat");                                //wenn fertig hier später ids.dat!!!!!!!!!!!!!
      ofstream fout;
      fout.open(pathIds->c_str(), ios::out);
      
      strErg = getFirstStructure();
     
      if (!strErg) return;
      oColor = new SoMaterial();
      oColor->diffuseColor.setValue((rand()%100)/100.0,(rand()%100)/100.0,(rand()%100)/100.0);
      _Switch->addChild(oColor);
      _Switch->addChild((SoNode*)getIVPointer());
      iCount++;
      cout<<iCount<<" "<<(*strErg)<<endl;
      fout<<iCount<<" "<<(*strErg)<<endl;      
      while (strErg)
      {
        strErg = getNextStructure();
        if (strErg)
        {
          oColor = new SoMaterial();
          oColor->diffuseColor.setValue((rand()%100)/100.0,(rand()%100)/100.0,(rand()%100)/100.0);
          _Switch->addChild(oColor);
          _Switch->addChild((SoNode*)getIVPointer());
          iCount++;
          cout<<iCount<<" "<<(*strErg)<<endl;
          fout<<iCount<<" "<<(*strErg)<<endl;
        }
      }
      fout.close();
    }
 
  }
  inherited::handleNotification(field);

}





const std::string* METKToSolverConnection::getFirstStructure() {
  omObjectContainer* oc = getObjContainer();
   if (oc) {
      for (_object = oc->begin(); _object != oc->end(); _object++) {
        if (_object->second.isValid() && _object->second.exists(LAY_FILES)) {
            std::string objectType = _object->second[LAY_GLOBAL][INF_OBJTYPE].get_std_string();
            std::string imageType  = _object->second[LAY_IMAGE][INF_IMAGETYPE].get_std_string();
            if (objectType == "Result" && imageType == "Segmentation" && getIVPointer()) {
               return &_object->first;
            }
         }
      }
   }
   return 0;
}


const std::string* METKToSolverConnection::getNextStructure() {
   omObjectContainer* oc = getObjContainer();
   if (oc) {
       _object++;
       for (_object; _object != oc->end(); _object++) {
         if (_object->second.isValid() && _object->second.exists(LAY_FILES)) {
            std::string objectType = _object->second[LAY_GLOBAL][INF_OBJTYPE].get_std_string();
            std::string imageType  = _object->second[LAY_IMAGE][INF_IMAGETYPE].get_std_string();
            if (objectType == "Result" && imageType == "Segmentation" && getIVPointer()) {
               return &_object->first;
            }
         }
      }
   }
   return 0;
   return &_object->first;
}

const std::string* METKToSolverConnection::getCurrentStructure() {
   return &_object->first;
}


const SoNode* METKToSolverConnection::getIVPointer() {
   if (_object->second.hasAttribute(LAY_FILES, INF_IVPOINTER))
   {
     return (SoNode*) _object->second[LAY_FILES][INF_IVPOINTER].get_MLint32();  
   }
   else
      return 0;
}


const float METKToSolverConnection::getImportance() {
   return getImportance(&_object->first);
}


const SoNode* METKToSolverConnection::getIVPointer(const std::string* name) {
   omObjectContainer* oc = getObjContainer();
   if (oc) {
      if ((*oc).exists(*name)) {
         return (SoNode*) (*oc)[*name][LAY_FILES][INF_IVPOINTER].get_MLint32();
      }
   }
   return 0;
}


const float METKToSolverConnection::getImportance(const std::string* name) {
   omObjectContainer* oc = getObjContainer();
   if (oc) {
      if ((*oc).exists(*name)) {
         if ((*oc)[*name].hasAttribute(LAY_DESCRIPTION, INF_IMPORTANCE)) {
            return (*oc)[*name][LAY_DESCRIPTION][INF_IMPORTANCE].get_double();
         } else {
            if ((*oc)[*name].hasAttribute(LAY_APPEARANCE, INF_TRANSPARENCY)) {
               return 1.0 - (*oc)[*name][LAY_APPEARANCE][INF_TRANSPARENCY].get_double();
            }
         }
      }
   }
   return 0;
}

bool METKToSolverConnection::getObjAttributeString(const string ObjID, const string LayerID, const string InfoID, string& value)
{
	omAttribute attr;
	if (getObjAttribute(ObjID,LayerID,InfoID, attr))
	{
		value = attr.getStringValue();
		return true;
	}
	return false;	
}

bool METKToSolverConnection::getObjAttribute(const string ObjID, const string LayerID, const string InfoID, omAttribute& attr)
{
	const omObjectContainer *oc= getConstObjContainer();
	if(oc == NULL)
	{
		//Debug("ObjContainer not found!",DL_LOW);
	}
	else
	{
		if (!(*oc).exists(ObjID))
		{
//			Debug("invalidObject",DL_LOW);
		}
		else
		{
			// Get read-only access to object with given ID.
			const omObject &obj= (*oc)[ObjID];
			if (obj.hasAttribute(LayerID,InfoID))
			{
				attr = obj.getAttribute(LayerID,InfoID);
				return true;
			}
		}
	}
	return false;
}
   
   ML_END_NAMESPACE
      
