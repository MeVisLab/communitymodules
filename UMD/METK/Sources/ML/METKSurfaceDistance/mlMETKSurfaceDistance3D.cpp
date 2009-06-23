// Local includes

#include "../METK.h"
#include "mlMETKSurfaceDistance3D.h"

#include <Inventor/nodes/SoSeparator.h>
#include "SoColorShape.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_CLASS_SOURCE(METKSurfaceDistance3D, ObjMgrClient);

//----------------------------------------------------------------------------------
//! Constructor / Destructor
//----------------------------------------------------------------------------------
METKSurfaceDistance3D::METKSurfaceDistance3D (void) : inherited(1,0,ObjMgrClient::EVENTS_SELECTED)
{
   ML_TRACE_IN("METKSurfaceDistance3D::METKSurfaceDistance3D()");

   // Suppress calls of handleNotification on field changes.
   handleNotificationOff();

   _outInventor = getFieldContainer()->addSoNode("outInventor");

   _nearDistance      = getFieldContainer()->addFloat("nearDistance");
   _nearDistanceColor = getFieldContainer()->addColor("nearDistanceColor");
   _farDistance        = getFieldContainer()->addFloat("farDistance");
   _farDistanceColor   = getFieldContainer()->addColor("farDistanceColor");
   _nearDistance->setFloatValue(2.0);
   _farDistance->setFloatValue(5.0);
   _nearDistanceColor->setColorValue(1,0,0);
   _farDistanceColor->setColorValue(1,1,0);

   _outGroup = new SoSeparator();
   // everything this group contains will always be valid
   _outGroup->ref();
   _outInventor->setSoNodeValue(_outGroup);

   _inputImageIsValid = false;
   _image      = NULL;
   _inImageBox = NULL;
   _memSlice = 0;

   clearAcceptedObjectIDs();
   addAcceptedObjectID("*");
   clearAcceptedInfoLayerNames();
   addAcceptedInfoLayerName(LAY_APPEARANCE);

   // Reactivate calls of handleNotification on field changes.
   handleNotificationOn();
}

METKSurfaceDistance3D::~METKSurfaceDistance3D (void) {
   ML_TRACE_IN("METKSurfaceDistance3D::~METKSurfaceDistance3D()");
   T_ColorShapeIterator iter;
   if (_inImageBox) { delete _inImageBox; _inImageBox = 0; }
   for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
      (*iter)->unref();
      _colorShapeVec.erase(iter);
      iter--;
   }
   if (_memSlice) { MLFree(_memSlice); _memSlice = 0; }
}


void METKSurfaceDistance3D::activateAttachments() {
   ML_TRACE_IN("METKSurfaceDistance3D::activateAttachments()");
   clearAcceptedObjectIDs();
   addAcceptedObjectID("*");
   clearAcceptedInfoLayerNames();
   addAcceptedInfoLayerName(LAY_APPEARANCE);
   addAcceptedInfoLayerName(LAY_GLOBALEVENTS);
   //addAcceptedInfoLayerName(LAY_FILES);

   sendNotification();

   // Don't forget to call the super class functionality, it enables field
   // notifications for your module again. 
   // SUPER_CLASS is the class you derive from (usually BaseOp).
   ObjMgrClient::activateAttachments();
}



//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void METKSurfaceDistance3D::handleNotification(Field *field) {
   ML_TRACE_IN("METKSurfaceDistance3D::handleNotification(Field *field)");
   if (field == _nearDistance || field == _nearDistanceColor || field == _farDistance || field == _farDistanceColor) {
      if (_nearDistance->getFloatValue() > _farDistance->getFloatValue()) {
         _nearDistance->setFloatValue(_farDistance->getFloatValue());
      }
      if (_nearDistance->getFloatValue() < 0.0f) {
         _nearDistance->setFloatValue(0.0f);
      }
      if (_farDistance->getFloatValue() < 0.0f) {
         _farDistance->setFloatValue(0.0f);
      }
      T_ColorShapeIterator iter;
      for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
         (*iter)->_nearDistance = _nearDistance->getFloatValue();
         (*iter)->_farDistance = _farDistance->getFloatValue();
         float r, g, b;
         _nearDistanceColor->getColorValue(r, g, b);
         (*iter)->_nearDistanceColor.setValue(r, g, b);
         _farDistanceColor->getColorValue(r, g, b);
         (*iter)->_farDistanceColor.setValue(r, g, b);
         (*iter)->compute();
      }
   }

   if (field == getInField(0)){             
      _inputImageIsValid = false;
      _image = getUpdatedInImg(0);

      if (_image){
         _inImageBox = new SubImgBox(_image->getBoxFromImgExt());
         _inputImageIsValid = true;
         T_ColorShapeIterator iter;
         for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
            (*iter)->compute();
         }

      } else {
         _inputImageIsValid = false;

         if (_inImageBox) { delete _inImageBox; _inImageBox = 0; }
         _inImageBox = NULL;
         _image      = NULL;
      }
   }
   inherited::handleNotification(field);
}


void METKSurfaceDistance3D::handleObjMgrNotification() {
   ML_TRACE_IN("METKSurfaceDistance3D::handleObjMgrNotification()");
   omEventContainer myEventList = getEventContainer();

   //Durchiterieren der EventList
   omEventContainer::const_iterator iter;
   for ( iter = myEventList.begin();iter!=myEventList.end(); iter++)
   {
      ObjMgrEvent myEvent = (*iter);

      //Status has changed
      if (myEvent.infoID == INF_SURFACEDIST || myEvent.infoID == INF_VISIBLE || myEvent.infoID == INF_IVPOINTER) {
         const omObjectContainer* oc = getObjContainer();
         if (oc) {
            const omObject &obj = (*oc)[myEvent.objectID];
            if (obj.isValid()) {
               const bool visible = obj[LAY_APPEARANCE][INF_VISIBLE];
               bool sdValid = obj.hasAttribute(LAY_APPEARANCE, INF_SURFACEDIST);
               bool pValid = obj.hasAttribute(LAY_FILES, INF_IVPOINTER);

               if (pValid && sdValid) {
                  const bool surfDist = obj[LAY_APPEARANCE][INF_SURFACEDIST];
                  const int pointer = obj[LAY_FILES][INF_IVPOINTER].get_MLint32();
                  if (pointer) {
                     if (visible && surfDist) {
                        addStructure((SoNode*) pointer);
                     } else {
                        removeStructure((SoNode*) pointer);
                     }
                  }
               }
            }
         }
      }
      else if (myEvent.infoID == INF_CASELOADED) {
         const omObjectContainer* oc = getObjContainer();
         if (oc) {
            const omObject &obj = (*oc)[myEvent.objectID];
            if (obj.isValid()) {
               const std::string status = obj[LAY_GLOBALEVENTS][INF_CASELOADED];
               if (status == MSG_LOADED) {
                  // scan all objects for already visible surface distances
                  for (omObjectContainer::const_iterator iter = oc->begin(); iter != oc->end(); iter++) {
                     if (iter->second.isValid() && iter->second.exists(LAY_APPEARANCE)) {
                        const bool visible = iter->second[LAY_APPEARANCE][INF_VISIBLE];

                        bool pValid = iter->second.hasAttribute(LAY_FILES, INF_IVPOINTER);
                        bool sdValid = iter->second.hasAttribute(LAY_APPEARANCE, INF_SURFACEDIST);
                        if (pValid && sdValid) {
                           const bool surfDist = iter->second[LAY_APPEARANCE][INF_SURFACEDIST];
                           const int pointer   = iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32();
                           if (visible && surfDist && pointer) {
                              addStructure((SoNode*) iter->second[LAY_FILES][INF_IVPOINTER].get_MLint32());
                           }
                        }
                     }
                  }
               }
               else if (status == MSG_CLEANUP) {
                  T_ColorShapeIterator iter;
                  if (_inImageBox) { delete _inImageBox; _inImageBox = 0; }
                  for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
                     (*iter)->unref();
                     if (_outGroup->findChild(*iter) != -1) {
                        _outGroup->removeChild(*iter);
                     }
                     _colorShapeVec.erase(iter);
                     iter--;
                  }
                  if (_memSlice) { MLFree(_memSlice); _memSlice = 0; }
               }
            }
         }
      }
   }
   clearEventContainer();
}


void METKSurfaceDistance3D::addStructure(SoNode* node) {
   ML_TRACE_IN("METKSurfaceDistance3D::addStructure(SoNode* node)");
   T_ColorShapeIterator iter;
   SoColorShape* cs = 0;

   for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
      if ((*iter)->input.getValue() == node) {
         cs = (*iter);
      }
   }
   if (cs == 0) {
      cs = new SoColorShape();
      _colorShapeVec.push_back(cs);
      cs->ref();
      cs->setMLClass(this);
      cs->input.setValue(node);
   }
   if (_inputImageIsValid) {
      cs->compute();
      _outGroup->addChild(cs);
   }
}


void METKSurfaceDistance3D::removeStructure(SoNode* node) {
   ML_TRACE_IN("METKSurfaceDistance3D::removeStructure(SoNode* node)");
   T_ColorShapeIterator iter;
   SoColorShape* cs = 0;

   for (iter = _colorShapeVec.begin(); iter != _colorShapeVec.end(); iter++) {
      if ((*iter)->input.getValue() == node) {
         cs = (*iter);
      }
   }
   if (cs && _outGroup->findChild(cs) != -1) {
      _outGroup->removeChild(cs);
   }
}


void METKSurfaceDistance3D::setBoundingBox(SbBox3f* invBBox) {
   ML_TRACE_IN("METKSurfaceDistance3D::setBoundingBox(SbBox3f* invBBox)");
   if (_inputImageIsValid) {
      vec3 bbv1, bbv2;
      bbv1[0] = invBBox->getMin()[0];
      bbv1[1] = invBBox->getMin()[1];
      bbv1[2] = invBBox->getMin()[2];

      bbv2[0] = invBBox->getMax()[0];
      bbv2[1] = invBBox->getMax()[1];
      bbv2[2] = invBBox->getMax()[2];

      _image->transformToVoxelCoord(bbv1, bbv1);
      _image->transformToVoxelCoord(bbv2, bbv2);

      SubImgBox bbox;
      bbox.v1.x = bbv1[0]; bbox.v1.y = bbv1[1]; bbox.v1.z = bbv1[2]; bbox.v1.c = bbox.v1.t = bbox.v1.u = 0;
      bbox.v2.x = bbv2[0]; bbox.v2.y = bbv2[1]; bbox.v2.z = bbv2[2]; bbox.v2.c = bbox.v2.t = bbox.v2.u = 0;

      // intersect nodesBB with imageBB
      _intersectBB = bbox.intersect(*_inImageBox);

      // get according sub image
      if (_memSlice) { MLFree(_memSlice); _memSlice = 0; }
      getTile(getInOp(0), getInOpIndex(0), _intersectBB, MLfloatType, &_memSlice);       
      _subImageMemory = (float*)_memSlice;

      Vector intersectExt = _intersectBB.getExt();

      _yStride =  intersectExt.x;
      _zStride = (intersectExt.y) * _yStride;
   }
}


float METKSurfaceDistance3D::getImageValue(const SbVec3f* vertex) {
   ML_TRACE_IN("METKSurfaceDistance3D::getImageValue(const SbVec3f* vertex)");
   // if intersection contains node, retrieve according 
   // value, else fill in default value
   float imgVal = -1;
   if (_inputImageIsValid) {
      // compute node's position in voxel space
      vec3 pos; 
      pos[0] = vertex->getValue()[0];
      pos[1] = vertex->getValue()[1];
      pos[2] = vertex->getValue()[2];

      _image->transformToVoxelCoord(pos, pos);

      Vector nodeVPos;
      nodeVPos.x = pos[0];
      nodeVPos.y = pos[1];
      nodeVPos.z = pos[2];
      if (_intersectBB.contains(nodeVPos)){
         imgVal = _subImageMemory[ (int)floor ((nodeVPos.x - _intersectBB.v1.x))            + 
            (int)floor(((nodeVPos.y - _intersectBB.v1.y) * _yStride)) + 
            (int)floor(((nodeVPos.z - _intersectBB.v1.z) * _zStride)) ];        
      }
   }
   return imgVal;
}


ML_END_NAMESPACE

