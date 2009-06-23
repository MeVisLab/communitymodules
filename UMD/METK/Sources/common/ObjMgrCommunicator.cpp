// Local includes
#include "ObjMgrCommunicator.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(ObjMgrCommunicator,ObjMgrClient);


ObjMgrCommunicator::ObjMgrCommunicator(void) : inherited(0,0,ObjMgrClient::EVENTS_NONE)
{

}


void ObjMgrCommunicator::handleObjMgrNotification()
{
  omEventContainer myEventList = getEventContainer();

  //Durchiterieren der EventList
  omEventContainer::const_iterator iter;
  for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
  {
    ObjMgrEvent myEvent = (*iter);

    //if (myEvent.objectID==fldAnimationViewerName->getStringValue() && myEvent.layerID==LAY_VIEWER_PROPERTIES && myEvent.infoID==INF_VIEWER_PROPERTIES_VIEWERSTATUS)
  }
  clearEventContainer();
}

//!Set the value of an attribute in the ObjMgr-DB - no Notification will be send
void ObjMgrCommunicator::setObjAttribute(const string ObjID, const string LayerID, const string InfoID, void* value, const string omInfoType, const bool createIfNotExists, const bool persistent, const bool ignoreType)
{
  // Get writable access to object container
  bool attrCreated = false;
  omObjectContainer* oc = getObjContainer();
  if(oc == NULL)
  {
    kDebug::Debug("ObjContainer not found!",kDebug::DL_LOW);
  }
  else
  {
    if (!(*oc).exists(ObjID) && !createIfNotExists)
    {
      kDebug::Debug("invalidObject "+ObjID,kDebug::DL_LOW);
    }
    else
    {
      omObject& obj = (*oc)[ObjID];


      omAttribute& attr = obj.getAttribute(LayerID,InfoID);

      if (!obj.hasAttribute(LayerID,InfoID) && createIfNotExists)
      {
        attr.createDataType(omInfoType);
        attrCreated = true;
      }
      if (obj.hasAttribute(LayerID,InfoID))
      {
        if (string(attr.getDataType()->getName())!=omInfoType && !ignoreType)
        {
          obj.removeAttribute(LayerID,InfoID);
          attr.createDataType(omInfoType);
          attrCreated = true;
        }
      }

      attr.flags().markPersistent(persistent);

      //Diese Sache mit den void*-Pointern zur Verallgemeinerung wird uns irgendwann mal mächtig um die Ohren fliegen ...
      if (omInfoType == omINFOTYPE_STRING)
      {
        obj[LayerID][InfoID].setStringValue(*((string *)value));
      }
      else if (omInfoType == omINFOTYPE_DOUBLE)
      {
        const double oldV = obj[LayerID][InfoID];
        const double newV = *((double *)value);
        if (fabs(oldV-newV)>=0.000001f || attrCreated)
          obj[LayerID][InfoID] = *((double *)value);
      }
      else if (omInfoType == omINFOTYPE_INT32)
      {
        const double oldV = obj[LayerID][InfoID];
        const double newV = *((int *)value);
        if (fabs(oldV-newV)>=0 || attrCreated)
          obj[LayerID][InfoID] = *((int *)value);
      }
      else if (omInfoType == omINFOTYPE_VEC3)
      {
        const vec3 tempVec3 = *(vec3*)value;
        const vec3 objVec3 = obj[LayerID][InfoID].get_vec3();
        const SbVec3f oldV = SbVec3f(objVec3[0],objVec3[1],objVec3[2]);
        const SbVec3f newV = SbVec3f(tempVec3[0],tempVec3[1],tempVec3[2]);

        if(kBasics::maxDiff(oldV,newV)>=0.000001f || attrCreated)
        {
          obj[LayerID][InfoID] = tempVec3;
        }
      }
      else if (omInfoType == omINFOTYPE_VEC4)
      {
        const vec4 tempVec4 = *(vec4*)value;
        const vec4 objVec4 = obj[LayerID][InfoID].get_vec4();
        const SbVec4f oldV = SbVec4f(objVec4[0],objVec4[1],objVec4[2],objVec4[3]);
        const SbVec4f newV = SbVec4f(tempVec4[0],tempVec4[1],tempVec4[2],tempVec4[3]);

        if (kBasics::maxDiff(oldV,newV)>=0.000001f || attrCreated)
          obj[LayerID][InfoID] = tempVec4;
      }
      else if (omInfoType == omINFOTYPE_BOOL)
      {
        const bool oldV = obj[LayerID][InfoID];
        if (oldV!=*(bool *)value || attrCreated)
          obj[LayerID][InfoID] = *(bool *)value;
      }
      else if (omInfoType == omINFOTYPE_MESSAGE)
      {
        obj[LayerID][InfoID] = *((omMessage *)value);
      }
    }

  }
}


bool ObjMgrCommunicator::getObjAttribute(const string ObjID, const string LayerID, const string InfoID, omAttribute& attr)
{
  const omObjectContainer *oc= getConstObjContainer();
  if(oc == NULL)
  {
    kDebug::Debug("ObjContainer not found!",kDebug::DL_LOW);
  }
  else
  {
    if (!(*oc).exists(ObjID))
    {
      kDebug::Debug("invalidObject",kDebug::DL_LOW);
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



bool ObjMgrCommunicator::getObjAttributeVec3f(const string ObjID, const string LayerID, const string InfoID, SbVec3f& vec3f)
{
  omAttribute attr;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    vector<string> VecSplit;
    kBasics::split(attr.getStringValue(),' ',3,&VecSplit);

    if (VecSplit.size()!=3)
    {
      kDebug::Debug("WrongSplitSize: " + kBasics::IntToString(VecSplit.size()),kDebug::DL_HIGH);
      return false;
    }

    vec3f.setValue(kBasics::StringToFloat(VecSplit[0]),kBasics::StringToFloat(VecSplit[1]),kBasics::StringToFloat(VecSplit[2]));
    return true;
  }
  return false;
}



bool ObjMgrCommunicator::getObjAttributeVec4f(const string ObjID, const string LayerID, const string InfoID, SbVec4f& vec4f)
{
  omAttribute attr;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    vector<string> VecSplit;
    kBasics::split(attr.getStringValue(),' ',4,&VecSplit);

    if (VecSplit.size()!=4)
    {
      kDebug::Debug("WrongSplitSize: " + kBasics::IntToString(VecSplit.size()),kDebug::DL_HIGH);
      return false;
    }

    vec4f.setValue(kBasics::StringToFloat(VecSplit[0]),kBasics::StringToFloat(VecSplit[1]),kBasics::StringToFloat(VecSplit[2]),kBasics::StringToFloat(VecSplit[3]));
    return true;
  }
  return false;
}



bool ObjMgrCommunicator::getObjAttributeString(const string ObjID, const string LayerID, const string InfoID, string& value)
{
  omAttribute attr;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    value = attr.getStringValue();
    return true;
  }
  return false;
}



bool ObjMgrCommunicator::getObjAttributeFloat(const string ObjID, const string LayerID, const string InfoID, float& value)
{
  omAttribute attr;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    value = kBasics::StringToFloat(attr.getStringValue());
    return true;
  }
  return false;
}



bool ObjMgrCommunicator::getObjAttributeInt(const string ObjID, const string LayerID, const string InfoID, int& value)
{
  omAttribute attr;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    value = kBasics::StringToInt(attr.getStringValue());
    return true;
  }
  return false;
}



bool ObjMgrCommunicator::getObjAttributeBool(const string ObjID, const string LayerID, const string InfoID, bool& value)
{
  omAttribute attr;
  value = false;
  if (getObjAttribute(ObjID,LayerID,InfoID, attr))
  {
    value = kBasics::StringToBool(attr.getStringValue());
    return true;
  }
  return false;
}


ML_END_NAMESPACE
