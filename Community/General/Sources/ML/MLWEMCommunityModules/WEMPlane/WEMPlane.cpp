
/*
        This program is written by: 
        
            Bart De Dobbelaer
            
            Laboratory for Medical Imaging Research
            Radiologie/ESAT PSI
            Universitair Ziekenhuis Gasthuisberg
            Herestraat 49
            B-3000 Leuven
            Belgium
            
        Email: Bart.DeDobbelaer@uz.kuleuven.ac.be
*/

#include "WEMPlane.h"
#include <mlLine.h>
#include "WEMMarchingCubes.h"

ML_START_NAMESPACE

ML_BASEOP_CLASS_SOURCE(WEMPlane, WEMProcessor);

////////////////////////////////////////////////////////////////////////

WEMPlane::WEMPlane(std::string type) : WEMProcessor(type,false)
{
  handleNotificationOff();

  FieldContainer* fieldC = getFieldContainer();
  ML_CHECK(fieldC);
  
  (_planeFld = (PlaneField *)(fieldC->addField("plane","PlaneField","0,0,1,0")))->setPlaneValue(Plane());
  (_generateEdgesFld = fieldC->addBool("generateEdges"))->setBoolValue(true);

  handleNotificationOn();
}

////////////////////////////////////////////////////////////////////////
  
WEMPlane::~WEMPlane() 
{
  ML_TRACE_IN("WEMPlane::~WEMPlane")
}

////////////////////////////////////////////////////////////////////////

void WEMPlane::handleNotification(Field *field) 
{
  ML_TRACE_IN("WEMPlane::handleNotification()")

  if ((field == _planeFld) ||
      (field == _generateEdgesFld))
  {
    if (_autoApplyFld->getBoolValue()) { _process(); }
  }

  WEMProcessor::handleNotification(field);
}

////////////////////////////////////////////////////////////////////////

void WEMPlane::activateAttachments() 
{
  ML_TRACE_IN("WEMPlane::activateAttachments()")

  WEMProcessor::activateAttachments();
}

////////////////////////////////////////////////////////////////////////

void WEMPlane::_process() 
{
  ML_TRACE_IN("WEMPlane::_process()")
  
  _startProcessing();

  WEMProcessor::_process();

  if (_inWEM != NULL) {

    if (_inWEM->getNumWEMPatches()) {

      for (unsigned int i = 0; i < _inWEM->getNumWEMPatches(); i ++) { 

        WEMPatch* wemPatch = _inWEM->getWEMPatchAt(i);
        _buildPlane(wemPatch);
      }
    }

  }

  _finishProcessing();

  _notifyObservers();

}

////////////////////////////////////////////////////////////////////////

void WEMPlane::_finish(WEMPatch *wemPatch)
{
  ML_TRACE_IN("void WEMPlane::_finish() ");

  if (wemPatch != NULL) { 

    if (_generateEdgesFld->getBoolValue()) {
      wemPatch->buildEdgeConnectivity();
    }

    WEMGenerator::_finish(wemPatch);
  }
}

////////////////////////////////////////////////////////////////////////

void WEMPlane::_buildPlane(WEMPatch* wemPatch) 
{
  ML_TRACE_IN("WEMPlane::_buildPlane(WEMPatch* wemPatch) "); 

  if (wemPatch == NULL) { return; }

  Plane plane = Plane();
  _planeFld->getPlaneValue(plane);    

  wemPatch->computeBoundingBox();
  Vector3 min = wemPatch->getBoundingBox()->getMin();
  Vector3 max = wemPatch->getBoundingBox()->getMax();

  WEMTrianglePatch* planePatch = NULL;
  ML_CHECK_NEW(planePatch,WEMTrianglePatch());

  vec3 bbNodes[8], intersect;
  unsigned char cubeindex = 0;
  int vertexlist[12];
  WEMNode* node = NULL;

  bbNodes[0] = vec3(min[0],min[1],max[2]);
  bbNodes[1] = vec3(max[0],min[1],max[2]);
  bbNodes[2] = vec3(max[0],min[1],min[2]);
  bbNodes[3] = vec3(min[0],min[1],min[2]);
  bbNodes[4] = vec3(min[0],max[1],max[2]);
  bbNodes[5] = vec3(max[0],max[1],max[2]);
  bbNodes[6] = vec3(max[0],max[1],min[2]);
  bbNodes[7] = vec3(min[0],max[1],min[2]);

  if (plane.isInHalfSpace(bbNodes[0])) cubeindex |= 1;
  if (plane.isInHalfSpace(bbNodes[1])) cubeindex |= 2;
  if (plane.isInHalfSpace(bbNodes[2])) cubeindex |= 4;
  if (plane.isInHalfSpace(bbNodes[3])) cubeindex |= 8;
  if (plane.isInHalfSpace(bbNodes[4])) cubeindex |= 16;
  if (plane.isInHalfSpace(bbNodes[5])) cubeindex |= 32;
  if (plane.isInHalfSpace(bbNodes[6])) cubeindex |= 64;
  if (plane.isInHalfSpace(bbNodes[7])) cubeindex |= 128;

  if (edge3DTable[cubeindex] == 0) return;
  if (edge3DTable[cubeindex] & 1) {//edge between 0 and 1
    vertexlist[0] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[0],bbNodes[1]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 2) {//edge between 1 and 2
    vertexlist[1] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[1],bbNodes[2]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 4) {//edge between 2 and 3
    vertexlist[2] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[2],bbNodes[3]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 8) {//edge between 3 and 0
    vertexlist[3] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[3],bbNodes[0]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 16) {//edge between 4 and 5
    vertexlist[4] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[4],bbNodes[5]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 32) {//edge between 5 and 6
    vertexlist[5] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[5],bbNodes[6]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 64) {//edge between 6 and 7
    vertexlist[6] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[6],bbNodes[7]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 128) {//edge between 7 and 4
    vertexlist[7] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[7],bbNodes[4]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 256) {//edge between 0 and 4
    vertexlist[8] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[0],bbNodes[4]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 512) {//edge between 1 and 5
    vertexlist[9] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[1],bbNodes[5]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 1024) {//edge between 2 and 6
    vertexlist[10] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[2],bbNodes[6]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }
  if (edge3DTable[cubeindex] & 2048) {//edge between 3 and 7
    vertexlist[11] = planePatch->getNumNodes();
    node = planePatch->addNode();
    plane.intersect(Line(bbNodes[3],bbNodes[7]),intersect);
    node->setPosition(intersect[0],intersect[1],intersect[2]);
  }

  WEMTriangle* triangle = NULL;
  unsigned int i = 0;

  for (i = 0; i < 13; i += 3) {
    if (triTable[cubeindex][i] != -1) {
      triangle = planePatch->addTriangle();

      node = planePatch->getNodeAt(vertexlist[triTable[cubeindex][i]]);
      triangle->setNode(0,node);
      node->addFace(triangle);

      node = planePatch->getNodeAt(vertexlist[triTable[cubeindex][i+1]]);
      triangle->setNode(1,node);
      node->addFace(triangle);

      node = planePatch->getNodeAt(vertexlist[triTable[cubeindex][i+2]]);
      triangle->setNode(2,node);
      node->addFace(triangle);
    }
  }

  _finish(planePatch);
  _addWEMPatch(planePatch);

}
    
////////////////////////////////////////////////////////////////////////

ML_END_NAMESPACE
