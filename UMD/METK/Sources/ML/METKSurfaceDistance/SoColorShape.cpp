// #############################
// ####
// ####  SoColorShape.cpp
// ####
// #############################


#include "SoColorShape.h"

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/SbViewportRegion.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/engines/SoComputeBoundingBox.h>
#include <Inventor/fields/SoSFMatrix.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoFieldSensor.h>


#include <Inventor/sensors/SoNodeSensor.h>

SO_NODE_SOURCE(SoColorShape);

void SoColorShape::initClass() { 
   // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
   SO_NODE_INIT_CLASS(SoColorShape, SoNode, "SoNode");
}


SoColorShape::SoColorShape() {
   SO_NODE_CONSTRUCTOR(SoColorShape);

   SO_NODE_ADD_FIELD(input, (0));

   _extentX = _extentY = _extentZ = 0;
   _vertexPropArray = 0;
   _sizeCoordinate3 = 0;

   // Callback zum einsammeln der Koordinaten
   _myAction = new SoCallbackAction;
   _myAction->addPreCallback        (SoShape::getClassTypeId(), preCB, this);
   _myAction->addTriangleCallback   (SoShape::getClassTypeId(), triangleCB, this);

   //sensor = new SoFieldSensor(sensorCB, this);
   //sensor->attach(&input);

   _nearDistance = 2.0f;
   _farDistance = 5.0f;

   float temp = 0;
   _nearDistanceColor.setPackedValue(0xFF0000FF, temp);
   _farDistanceColor.setPackedValue(0xFFFF00FF, temp);

   _vertexSet = 0;
   _edgeSet = 0;
   _triangleSet = 0;
}


SoColorShape::~SoColorShape() {
   // ein bischen aufraeumen
   if (_myAction)    { delete _myAction;     _myAction    = 0; }

   if (_triangleSet) { 
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _triangleSet[index].clear(); 
      delete[] _triangleSet; 
      _triangleSet = 0; 
   }
   if (_edgeSet)     { 
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _edgeSet[index].clear();     
      delete[] _edgeSet;     
      _edgeSet = 0; 
   }
   if (_vertexSet)   {
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _vertexSet[index].clear();
      delete[] _vertexSet;     
      _vertexSet = 0; 
   }
   if (_vertexPropArray) { delete[] _vertexPropArray; _vertexPropArray = 0; }
   if (_sizeCoordinate3) { delete[] _sizeCoordinate3; _sizeCoordinate3 = 0; }

   //if (sensor) { delete sensor; sensor = NULL; }
}


//void SoColorShape::sensorCB(void* userData, SoSensor*) {
//   SoColorShape* obj = (SoColorShape*) userData;
//   obj->compute();
//}


void SoColorShape::compute() {
   enableNotify(false);
   // alles schoen auf nichts zurücksetzen
   if (_vertexPropArray) { delete[] _vertexPropArray; _vertexPropArray = 0; }
   if (_sizeCoordinate3) { delete[] _sizeCoordinate3; _sizeCoordinate3 = 0; }

   if (_vertexSet)   {
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _vertexSet[index].clear();
      delete[] _vertexSet;     
      _vertexSet = 0; 
   }
   if (_triangleSet) { 
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _triangleSet[index].clear(); 
      delete[] _triangleSet; 
      _triangleSet = 0; 
   }
   if (_edgeSet)     { 
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _edgeSet[index].clear();     
      delete[] _edgeSet;     
      _edgeSet = 0; 
   }
   removeAllChildren();

   if (input.getValue() != 0) {
      // Bounding Box bestimmen
      SoComputeBoundingBox* bbact = new SoComputeBoundingBox;
      SoTranslation *min = new SoTranslation, *max = new SoTranslation;
      min->ref(); max->ref();
      bbact->ref();
      bbact->node = input.getValue();
      min->translation.connectFrom(&bbact->min);
      max->translation.connectFrom(&bbact->max);
      SbBox3f bBox(min->translation.getValue(), max->translation.getValue());

      min->unref(), max->unref();
      bbact->unref();

      // Eingang hat sich geändert
      if (!bBox.isEmpty()) {
         // Array fuer _edgeSet und _triangleSet set-Instanzen initialisieren
         float xSize, ySize, zSize;
         bBox.getSize(xSize, ySize, zSize);
         _offSet = bBox.getMin();

         _extentX = xSize / (float) HASH_PARTITION + 1; 
         _extentY = ySize / (float) HASH_PARTITION + 1; 
         _extentZ = zSize / (float) HASH_PARTITION + 1;

         _vertexSet =   new std::set<Vertex*, ltVertex>[_extentX * _extentY * _extentZ];
         _edgeSet =     new std::set<Edge*,   ltEdge  >[_extentX * _extentY * _extentZ];
         _triangleSet = new std::set<Triangle*        >[_extentX * _extentY * _extentZ];

         _vertexPropArray = new SoVertexProperty*[_extentX * _extentY * _extentZ];
         int index = 0;
         for (index = 0; index < (_extentX * _extentY * _extentZ); index++)
            _vertexPropArray[index] = 0;

         _sizeCoordinate3 = new int[_extentX * _extentY * _extentZ];
         for (index = 0; index < (_extentX * _extentY * _extentZ); index++)
            _sizeCoordinate3[index] = -1;

         // Dreiecke aufsammeln
         _myAction->apply(input.getValue());

         // SoIndexedTriangleStripSet erstellen
         generateITSS();
      }
   }
   enableNotify(true);
   touch();
}


SoCallbackAction::Response SoColorShape::preCB(void* userData, SoCallbackAction* action, const SoNode* node) {
   SoColorShape* obj = (SoColorShape*) userData;

   // aktuelle Transformationsmatrix besorgen
   obj->_worldMatrix = obj->_myAction->getModelMatrix();

   SbBox3f bbox;
   SbVec3f center;
   ((SoShape*)node)->computeBBox(action, bbox, center);
   obj->_surDis->setBoundingBox(&bbox);

   return SoCallbackAction::CONTINUE;
}


void SoColorShape::triangleCB(void* userData, 
                              SoCallbackAction* /*action*/,
                              const SoPrimitiveVertex* v1,
                              const SoPrimitiveVertex* v2,
                              const SoPrimitiveVertex* v3) 
{
   SoColorShape* obj = (SoColorShape*) userData;

   // Dreieckseckpunkte besorgen und in Weltkoordinaten umwandeln
   SbVec3f vertex1, vertex2, vertex3;
   obj->_worldMatrix.multVecMatrix(v1->getPoint(), vertex1);
   obj->_worldMatrix.multVecMatrix(v2->getPoint(), vertex2);
   obj->_worldMatrix.multVecMatrix(v3->getPoint(), vertex3);

   float dist1, dist2, dist3;
   dist1 = obj->_surDis->getImageValue(&vertex1);
   dist2 = obj->_surDis->getImageValue(&vertex2);
   dist3 = obj->_surDis->getImageValue(&vertex3);

   SbVec3f normal1, normal2, normal3;
   normal1 = v1->getNormal();
   normal2 = v2->getNormal();
   normal3 = v3->getNormal();

   normal1.normalize();
   normal2.normalize();
   normal3.normalize();

   // Vertex ein wenig aus dem eigentlich Shape rausschieben
   vertex1 = vertex1 + normal1 * 0.5;
   vertex2 = vertex2 + normal2 * 0.5;
   vertex3 = vertex3 + normal3 * 0.5;

   obj->splitTriangle(vertex1, vertex2, vertex3, normal1, normal2, normal3, dist1, dist2, dist3);
}


void SoColorShape::splitTriangle(const SbVec3f&  point1, const SbVec3f&  point2, const SbVec3f&  point3, 
                                 const SbVec3f& normal1, const SbVec3f& normal2, const SbVec3f& normal3,
                                 const double&     dist1, const double&     dist2, const double&     dist3) 
{
   // Leider liegen all die Eckpunkte ja nicht genau auf den Grenzen, deswegen muessen die Dreiecke ggf. unterteilt werden.
   // Folgende Fälle gibt es:
   // 1.  alle 3 Eckpunkte haben den gleichen Farbindex, d.h. liegen in gleicher Entfernung --> einfach abspeichern
   // 2.  alle 3 Eckpunkte sind unterschiedlich. Einzige Moeglichkeit: eine Kante wird zweimal durchbrochen, die beiden anderen einmal --> Dreick unterteilen
   // 3a. nur ein Punkt ist anders, und er ist in der benachbarten Entfernungszone --> Dreick unterteilen
   // 3b. nur ein Punkt ist anders, und er ist zwei Zonen entfernt --> Dreick nochmal anders unterteilen

   // Das wird jetzt ne ganze schoene Indexfickerei...

   // color index (Entfernungszone) bestimmen
   int colorIndex1, colorIndex2, colorIndex3;

   if (dist1 < 0.0 || dist1 >= _farDistance)
      colorIndex1 = 0;
   else if (dist1 >= _nearDistance)
      colorIndex1 = 1;
   else
      colorIndex1 = 2;

   if (dist2 < 0.0 || dist2 >= _farDistance)
      colorIndex2 = 0;
   else if (dist2 >= _nearDistance)
      colorIndex2 = 1;
   else
      colorIndex2 = 2;

   if (dist3 < 0.0 || dist3 >= _farDistance)
      colorIndex3 = 0;
   else if (dist3 >= _nearDistance)
      colorIndex3 = 1;
   else
      colorIndex3 = 2;

   // hat das Dreieck nur eine Farbe, wenn ja, speichern
   if (colorIndex1 == colorIndex2 && colorIndex2 == colorIndex3) {
      storeTriangle(point1, point2, point3, normal1, normal2, normal3, colorIndex1);
   }
   else {
      // Dreieck unterteilen

      if (colorIndex1 != colorIndex2 && colorIndex1 != colorIndex3 && colorIndex2 != colorIndex3) {
         // alle Drei Punkte sind unterschiedlich
         // d.h. eine Kante enthält zwei Übergänge, und die beiden anderen nur einen
         float tdist1 = _nearDistance, tdist2 = _nearDistance, tdist3 = _nearDistance;
         if (colorIndex1 == 0 || dist2 > _farDistance)
            tdist1 = _farDistance;
         if (colorIndex2 == 0 || dist3 > _farDistance)
            tdist2 = _farDistance;
         if (colorIndex3 == 0 || dist1 > _farDistance)
            tdist3 = _farDistance;

         SbVec3f vtemp1 = interpolateVector(point2,   point3, dist2, dist3, tdist2);
         SbVec3f vnorm1 = interpolateVector(normal2, normal3, dist2, dist3, tdist2);

         SbVec3f vtemp2 = interpolateVector(point1,   point3, dist1, dist3, tdist3);
         SbVec3f vnorm2 = interpolateVector(normal3, normal1, dist1, dist3, tdist3);

         SbVec3f vtemp3 = interpolateVector(point1,   point2, dist1, dist2, tdist1);
         SbVec3f vnorm3 = interpolateVector(normal1, normal2, dist1, dist2, tdist1);

         // Doppeluebergang bestimmen
         // 1 entspricht dem gruenen Teil, der sich in der Mitte durchzieht
         SbVec3f vDoppel, nDoppel;
         if (colorIndex3 == 1) {
            vDoppel = interpolateVector(point1,   point2, dist1, dist2, _nearDistance);
            nDoppel = interpolateVector(normal1, normal2, dist1, dist2, _nearDistance);
            if (colorIndex1 == 0) storeTriangleAllDifferent(point1, point2, point3, vtemp1, vtemp2, vDoppel, vtemp3, normal1, normal2, normal3, vnorm1, vnorm2, nDoppel, vnorm3, 0, 2, 1);
            else                  storeTriangleAllDifferent(point1, point2, point3, vtemp1, vtemp2, vtemp3, vDoppel, normal1, normal2, normal3, vnorm1, vnorm2, vnorm3, nDoppel, 2, 0, 1);
         }
         if (colorIndex2 == 1) {
            vDoppel = interpolateVector(point1,  point3,  dist1, dist3, _nearDistance);
            nDoppel = interpolateVector(normal1, normal3, dist1, dist3, _nearDistance);
            if (colorIndex1 == 0) storeTriangleAllDifferent(point3, point1, point2, vtemp3, vtemp1, vtemp2, vDoppel, normal1, normal2, normal3, vnorm1, nDoppel, vnorm3, vnorm2, 2, 0, 1);
            else                  storeTriangleAllDifferent(point3, point1, point2, vtemp3, vtemp1, vDoppel, vtemp2, normal1, normal2, normal3, vnorm1, nDoppel, vnorm3, vnorm2, 0, 2, 1);
         }
         if (colorIndex1 == 1) {
            vDoppel = interpolateVector(point2,   point3, dist2, dist3, _nearDistance);
            nDoppel = interpolateVector(normal2, normal3, dist2, dist3, _nearDistance);
            if (colorIndex2 == 0) storeTriangleAllDifferent(point2, point3, point1, vtemp2, vtemp3, vDoppel, vtemp1, normal2, normal3, normal1, vnorm2, vnorm3, nDoppel, vnorm1, 0, 2, 1);
            else                  storeTriangleAllDifferent(point2, point3, point1, vtemp2, vtemp3, vtemp1, vDoppel, normal2, normal3, normal1, vnorm2, vnorm3, vnorm1, nDoppel, 2, 0, 1);
         }
      }
      else {
         // nur ein Punkt weicht ab

         // Dreieck geht ueber 2 Level
         if ((colorIndex1 == 0 || colorIndex2 == 0 || colorIndex3 == 0) && (colorIndex1 == 2 || colorIndex2 == 2 || colorIndex3 == 2)) {
            if (colorIndex2 == colorIndex3) {
               splitTriangle2Diff2Level(point1, point2, point3, normal1, normal2, normal3, dist1, dist2, dist3);
            } else if (colorIndex1 == colorIndex3) {
               splitTriangle2Diff2Level(point2, point3, point1, normal2, normal3, normal1, dist2, dist3, dist1);
            } else {
               splitTriangle2Diff2Level(point3, point1, point2, normal3, normal1, normal2, dist3, dist1, dist2);
            }
         }
         else {
            if (colorIndex1 != colorIndex2 && colorIndex2 == colorIndex3) {
               // point1 weicht ab
               float dist = _nearDistance;
               if (colorIndex1 == 0 || dist2 > _farDistance)
                  dist = _farDistance;
               SbVec3f vtemp1 = interpolateVector(point1,  point2,  dist1, dist2, dist);
               SbVec3f vtemp2 = interpolateVector(point1,  point3,  dist1, dist3, dist);
               SbVec3f ntemp1 = interpolateVector(normal1, normal2, dist1, dist2, dist);
               SbVec3f ntemp2 = interpolateVector(normal1, normal2, dist1, dist3, dist);

               storeTriangle(point1, vtemp1, vtemp2, normal1, ntemp1,  ntemp2, colorIndex1);
               storeTriangle(point2, point3, vtemp1, normal2, normal3, ntemp1, colorIndex2);
               storeTriangle(vtemp2, vtemp1, point3, ntemp2, ntemp1,  normal3, colorIndex2);
            }
            else if (colorIndex2 != colorIndex1 && colorIndex1 == colorIndex3) {
               // point2 weicht ab
               float dist = _nearDistance;
               if (colorIndex2 == 0 || dist1 > _farDistance)
                  dist = _farDistance;
               SbVec3f vtemp1 = interpolateVector(point2,  point1,  dist2, dist1, dist);
               SbVec3f vtemp2 = interpolateVector(point2,  point3,  dist2, dist3, dist);
               SbVec3f ntemp1 = interpolateVector(normal2, normal1, dist2, dist1, dist);
               SbVec3f ntemp2 = interpolateVector(normal2, normal3, dist2, dist3, dist);

               storeTriangle(point1, vtemp1, point3, normal1, ntemp1, normal3, colorIndex1);
               storeTriangle(point2, vtemp2, vtemp1, normal2, ntemp2, ntemp1,  colorIndex2);
               storeTriangle(vtemp1, vtemp2, point3, ntemp1, ntemp2,  normal3, colorIndex1);
            }
            else {
               // point3 weicht ab
               float dist = _nearDistance;
               if (colorIndex3 == 0 || dist1 > _farDistance)
                  dist = _farDistance;
               SbVec3f vtemp1 = interpolateVector(point3,  point1,  dist3, dist1, dist);
               SbVec3f vtemp2 = interpolateVector(point3,  point2,  dist3, dist2, dist);
               SbVec3f ntemp1 = interpolateVector(normal3, normal1, dist3, dist1, dist);
               SbVec3f ntemp2 = interpolateVector(normal3, normal2, dist3, dist2, dist);

               storeTriangle(point1, point2, vtemp1, normal1, normal2, ntemp1, colorIndex2);
               storeTriangle(point3, vtemp1, vtemp2, normal3, ntemp1,  ntemp2, colorIndex3);
               storeTriangle(vtemp2, vtemp1, point2, ntemp2, ntemp1,  normal2, colorIndex2);
            }
         }
      }
   }
}


const SbVec3f SoColorShape::interpolateVector(const SbVec3f& vertex1, const SbVec3f& vertex2, 
                                              const double& distance1, const double& distance2, 
                                              const double& distanceWanted) 
{
   double d, dw, normed1, normed2;
   dw = distanceWanted - distance1;
   d = distance2 - distance1;
   normed2 = dw / d;
   normed1 = 1 - normed2;

   return vertex1 * normed1 + vertex2 * normed2;
}


void SoColorShape::storeTriangleAllDifferent(const SbVec3f&    point1, const SbVec3f&    point2, const SbVec3f&    point3,
                                             const SbVec3f&   tpoint1, const SbVec3f&   tpoint2, const SbVec3f&   tpoint3,
                                             const SbVec3f&    dpoint,
                                             const SbVec3f&   normal1, const SbVec3f&   normal2, const SbVec3f&   normal3,
                                             const SbVec3f&  tnormal1, const SbVec3f&  tnormal2, const SbVec3f&  tnormal3,
                                             const SbVec3f&   dnormal,
                                             const int& colorIndex1,   const int& colorIndex2,   const int& colorIndex3) 
{
   // see also storeTriangleAllDifferent.pdf in the sources directory!
   storeTriangle(point1, dpoint, tpoint2, normal1, dnormal, tnormal2, colorIndex1);
   storeTriangle(tpoint1, point3, tpoint2, tnormal1, normal3, tnormal2, colorIndex3);
   storeTriangle(tpoint3, point2, tpoint1, tnormal3, normal2, tnormal1, colorIndex2);
   storeTriangle(tpoint2, tpoint3, tpoint1, tnormal2, tnormal3, tnormal1, 1);
   storeTriangle(dpoint, tpoint3, tpoint2, dnormal, tnormal3, tnormal2, 1);
   //storeTriangle(point1, point2, point3, normal1, normal2,  normal3, 4);//
}


void SoColorShape::splitTriangle2Diff2Level(const SbVec3f& vertex1, const SbVec3f& vertex2, const SbVec3f& vertex3, 
                                            const SbVec3f& normal1, const SbVec3f& normal2, const SbVec3f& normal3,
                                            const double& distance1, const double& distance2, const double& distance3) 
{
   // see also splitTriangle2Diff2Level.pdf in the sources directory!

   SbVec3f v12d2 = interpolateVector(vertex1, vertex2, distance1, distance2, _nearDistance);
   SbVec3f n12d2 = interpolateVector(normal1, normal2, distance1, distance2, _nearDistance);

   SbVec3f v12d5 = interpolateVector(vertex1, vertex2, distance1, distance2, _farDistance);
   SbVec3f n12d5 = interpolateVector(normal1, normal2, distance1, distance2, _farDistance);

   SbVec3f v13d2 = interpolateVector(vertex1, vertex3, distance1, distance3, _nearDistance);
   SbVec3f n13d2 = interpolateVector(normal1, normal3, distance1, distance3, _nearDistance);

   SbVec3f v13d5 = interpolateVector(vertex1, vertex3, distance1, distance3, _farDistance);
   SbVec3f n13d5 = interpolateVector(normal1, normal3, distance1, distance3, _farDistance);

   if (distance1 < _nearDistance) {
      storeTriangle(vertex1, v12d2, v13d2, normal1, n12d2, n13d2, 2);
      storeTriangle(v12d2, v13d5, v13d2, n12d2, n12d5, n13d2, 1);
      storeTriangle(v12d2, v12d5, v13d5, n12d2, n13d5, n12d5, 1);
   } else {
      storeTriangle(v12d2, vertex3, v13d2, n12d2, normal3, n13d2, 2);
      storeTriangle(v12d2, vertex2, vertex3, n12d2, normal2, normal3, 2);
      storeTriangle(v12d2, v13d2, v13d5, n12d2, n13d2, n12d5, 1);
      storeTriangle(v12d2, v13d5, v12d5, n12d2, n13d5, n12d5, 1);
   }
}


void SoColorShape::storeTriangle(const SbVec3f&  point1, const SbVec3f&  point2, const SbVec3f&  point3, 
                                 const SbVec3f& normal1, const SbVec3f& normal2, const SbVec3f& normal3,
                                 const int& colorIndex1) 
{

   // liegt das Dreieck komplett draußen, wenn ja, nicht speichern!
   if (colorIndex1 != 0) {
      // Schwerpunkt und Position im Array bestimmen
      SbVec3f barycenter;
      float baryX, baryY, baryZ;
      barycenter = point1 - _offSet;
      barycenter += point2 - _offSet;
      barycenter += point3 - _offSet;

      // durch 3 weil bestehend aus 3 Vektoren
      barycenter = barycenter / 3.0;
      // durch HASH_PARTITION weil aufteilen 
      barycenter = barycenter / float(HASH_PARTITION);
      barycenter.getValue(baryX, baryY, baryZ);

      int arrayPosition;
      arrayPosition = ((int)fabs(baryX)) +
         ((int)fabs(baryY)) * _extentX + 
         ((int)fabs(baryZ)) * _extentX * _extentY;

      // Eckpunkte abspeichern und Indices besorgen
      const Vertex *vertex1, *vertex2, *vertex3;
      vertex1 = insertVertex(point1, normal1, colorIndex1, arrayPosition);
      vertex2 = insertVertex(point2, normal2, colorIndex1, arrayPosition);
      vertex3 = insertVertex(point3, normal3, colorIndex1, arrayPosition);

      // Kanten finden bzw. anlegen
      Edge *edge1, *edge2, *edge3;
      edge3 = generateEdge(vertex1, vertex2, arrayPosition);
      edge1 = generateEdge(vertex2, vertex3, arrayPosition);
      edge2 = generateEdge(vertex3, vertex1, arrayPosition);

      // Dreieck ablegen
      Triangle* tri = new Triangle;
      tri->vertex1 = vertex1;
      tri->vertex2 = vertex2;
      tri->vertex3 = vertex3;
      tri->edge1 = edge1;
      tri->edge2 = edge2;
      tri->edge3 = edge3;
      _triangleSet[arrayPosition].insert(tri);

      // Den Kanten die Dreiecke zuweisen
      if (edge1->triangle1 == 0) edge1->triangle1 = tri;
      else edge1->triangle2 = tri;
      if (edge2->triangle1 == 0) edge2->triangle1 = tri;
      else edge2->triangle2 = tri;
      if (edge3->triangle1 == 0) edge3->triangle1 = tri;
      else edge3->triangle2 = tri;
   }
}


Edge* SoColorShape::generateEdge(const Vertex* p1, const Vertex* p2, const int arrayPosition) {
   Edge* edge = new Edge;
   std::set<Edge*, ltEdge>::iterator it;
   // zur Identifizierung immer den kleineren Vertex zuerst ablegen
   if (p1 < p2) {
      edge->vertex1 = p1;
      edge->vertex2 = p2;
   } else {
      edge->vertex1 = p2;
      edge->vertex2 = p1;
   }
   it = _edgeSet[arrayPosition].find(edge);
   if (it != _edgeSet[arrayPosition].end()) {
      // Kante ist bereits enthalten, Kante aus set zurückgeben
      delete edge;
      edge = *it;
   } else {
      // Kante ist noch nicht enthalten, ins set einfuegen
      _edgeSet[arrayPosition].insert(edge);
   }
   return edge;
}


const Vertex* SoColorShape::insertVertex(const SbVec3f vector, const SbVec3f normal, const int colorIndex, const int arrayPosition) {
   // alle gesammelten Punkte werden in einer hash map abgelegt
   std::set<Vertex*, ltVertex>::iterator it;
   Vertex* vertex = new Vertex(vector, colorIndex); 

   // VertexProperty bereits erstellt?
   if (!_vertexPropArray[arrayPosition]) {
      _vertexPropArray[arrayPosition] = new SoVertexProperty;
      _vertexPropArray[arrayPosition]->normalBinding = SoVertexProperty::PER_VERTEX_INDEXED;
      _vertexPropArray[arrayPosition]->materialBinding = SoVertexProperty::PER_VERTEX_INDEXED;
      _vertexPropArray[arrayPosition]->orderedRGBA.set1Value(0, 0x0000FFFF); // normale Farbe
      _vertexPropArray[arrayPosition]->orderedRGBA.set1Value(1, _farDistanceColor.getPackedValue()); // bis _farDistance mm
      _vertexPropArray[arrayPosition]->orderedRGBA.set1Value(2, _nearDistanceColor.getPackedValue()); // bis _nearDistance mm
      //_vertexPropArray[arrayPosition]->orderedRGBA.set1Value(3, 0xFF00FFFF); // test lila opak
      //_vertexPropArray[arrayPosition]->orderedRGBA.set1Value(4, 0x80808080); // test grau transparent
   }

   // nachsehen ob der Punkt bereits vorhanden ist
   it = _vertexSet[arrayPosition].find(vertex);
   if (it != _vertexSet[arrayPosition].end()) {
      // Eckpunkt ist bereits enthalten
      // Vertex zurückgeben
      delete vertex;
      vertex = *it;
   }
   else { 
      // noch nicht enthalten
      ++(_sizeCoordinate3[arrayPosition]);
      int index = _sizeCoordinate3[arrayPosition];
      vertex->vertexIndex = index;
      _vertexSet[arrayPosition].insert(vertex);
      _vertexPropArray[arrayPosition]->vertex.set1Value(index, vector);
      _vertexPropArray[arrayPosition]->normal.set1Value(index, normal);
   }
   return vertex;
}


void SoColorShape::generateITSS() {

   // fuer alle _triangleSets
   for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) {
      // wenn dieser Block Dreiecke enthaelt
      if (!_triangleSet[index].empty()) {

         int count = -1;
         const Edge* edge = 0;
         Triangle* triNew = 0;
         const Vertex* tmpPoint = 0;

         // neues ITSS erzeugen und in den Szenengraphen einhaengen
         SoIndexedTriangleStripSet* itss = new SoIndexedTriangleStripSet;
         addChild(itss);
         itss->vertexProperty = _vertexPropArray[index];

         // solange Dreiecke vorhanden sind
         while (!_triangleSet[index].empty()) {
            // erstes Dreieck einfuegen
            Triangle* triBegin = *_triangleSet[index].begin();
            itss->coordIndex.set1Value(++count, triBegin->vertex1->vertexIndex);
            itss->materialIndex.set1Value(count, triBegin->vertex1->colorIndex);
            itss->coordIndex.set1Value(++count, triBegin->vertex2->vertexIndex);
            itss->materialIndex.set1Value(count, triBegin->vertex2->colorIndex);
            itss->coordIndex.set1Value(++count, triBegin->vertex3->vertexIndex);
            itss->materialIndex.set1Value(count, triBegin->vertex3->colorIndex);

            // weitere Dreiecke anfuegen, anfangen mit beliebiger Kante
            edge = triBegin->edge1;
            // geordneten Richtungswechsel der Dreiecke festlegen (CW, CCW)
            bool direction = true;

            // solange gegenueberliegendes Dreieck und Dreiecke allgemein vorhanden sind
            while (edge != 0 && !_triangleSet[index].empty()) {
               // gegenueberliegendes Dreieck der Kante holen
               triNew = edge->getOppositeTriangle(triBegin);
               // bereits bearbeitetes Dreieck loeschen
               _triangleSet[index].erase(triBegin);
               delete triBegin;
               // wenn gegenueberliegendes Dreieck vorhanden
               if (triNew) {
                  // neuen Punkt besorgen und anhaengen
                  tmpPoint = triNew->getOppositeVertex(edge);
                  if (tmpPoint) {
                     itss->coordIndex.set1Value(++count, tmpPoint->vertexIndex);
                     itss->materialIndex.set1Value(count, tmpPoint->colorIndex);
                     // Drehrichtung wechseln
                     direction = !direction;
                     if (direction) edge = triNew->getCWEdge(tmpPoint);
                     else edge = triNew->getCCWEdge(tmpPoint);

                     // neues Dreieck gleich altes Dreieck
                     triBegin = triNew;
                  }
                  else edge = 0;
               } else edge = 0;
            }
            // jeweiligen Strip abschliessen
            itss->coordIndex.set1Value(++count, -1);
            itss->materialIndex.set1Value(count, -1);
         }
      }
   }
}


void SoColorShape::setMLClass(ml::METKSurfaceDistance3D* surDis) {
   _surDis = surDis;
}


Vertex::Vertex(SbVec3f vertex, short colorIndex) : vertex(vertex), colorIndex(colorIndex) {
}


Edge::Edge() {
   vertex1 = 0;
   vertex2 = 0;
   triangle1 = 0;
   triangle2 = 0;
}


Edge::Edge(Edge* edge) {
   this->vertex1 = edge->vertex1;
   this->vertex2 = edge->vertex2;
   this->triangle1 = edge->triangle1;
   this->triangle2 = edge->triangle2;
}


Edge::~Edge() {
   triangle1 = 0;
   triangle2 = 0;
}


Triangle* Edge::getOppositeTriangle(const Triangle* tri) const {
   if (tri == triangle1) return triangle2;
   if (tri == triangle2) return triangle1;
   return 0;
}


void Edge::unrefTriangle(const Triangle* tri) {
   // Verweis auf bereits geloeschtes Dreieck aufloesen
   if (tri == triangle1) triangle1 = 0;
   else triangle2 = 0;
}


Triangle::Triangle() {
   vertex1 = 0;
   vertex2 = 0;
   vertex3 = 0;
   edge1 = 0;
   edge2 = 0;
   edge3 = 0;
}


Triangle::Triangle(Triangle* triangle) {
   this->vertex1 = triangle->vertex1;
   this->vertex2 = triangle->vertex2;
   this->vertex3 = triangle->vertex3;
   this->edge1 = triangle->edge1;
   this->edge2 = triangle->edge2;
   this->edge3 = triangle->edge3;
}


Triangle::~Triangle() {
   // bei Kanten abmelden
   edge1->unrefTriangle(this);
   edge2->unrefTriangle(this);
   edge3->unrefTriangle(this);
   edge1 = 0;
   edge2 = 0;
   edge3 = 0;
}


const Vertex* Triangle::getOppositeVertex(const Edge* edge) const {
   if (edge == edge1) return vertex1;
   if (edge == edge2) return vertex2;
   if (edge == edge3) return vertex3;
   return 0;
}


const Edge* Triangle::getCWEdge(const Vertex* vertex) const {
   if (vertex == vertex1) return edge2;
   if (vertex == vertex2) return edge3;
   if (vertex == vertex3) return edge1;
   return 0;
}


const Edge* Triangle::getCCWEdge(const Vertex* vertex) const {
   if (vertex == vertex1) return edge3;
   if (vertex == vertex2) return edge1;
   if (vertex == vertex3) return edge2;
   return 0;
}
