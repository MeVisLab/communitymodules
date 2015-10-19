// #############################
// ####
// ####  SoConvertShapeENT.cpp
// ####
// #############################

#include "SoShapePartition.h"

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/SbViewportRegion.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/engines/SoComputeBoundingBox.h>
#include <Inventor/fields/SoSFMatrix.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

#include <UMDMainAxis.h>

#include <Inventor/sensors/SoNodeSensor.h>

SO_NODE_SOURCE(SoShapePartition);

void SoShapePartition::initClass() { 
   // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
   SO_NODE_INIT_CLASS(SoShapePartition, SoSeparator, "SoSeparator");
}


SoShapePartition::SoShapePartition() {
   SO_NODE_CONSTRUCTOR(SoShapePartition);

   SO_NODE_ADD_FIELD(shapePartitioning, (false));
   SO_NODE_ADD_FIELD(update, ());
   SO_NODE_ADD_FIELD(cubeSize, (20));
   SO_NODE_ADD_FIELD(input, (0));
   SO_NODE_ADD_FIELD(outConvertedShape, (0));

   SO_NODE_ADD_FIELD(valid, (false));
   SO_NODE_ADD_FIELD(autoUpdate, (false));

   // internen Szenengraphen aufbauen
   _itssHolder =  new SoSeparator;
   _itssHolder->ref();
   outConvertedShape = _itssHolder;

   _extentX = _extentY = _extentZ = 0;
   _vertexPropArray = 0;
   _sizeCoordinate3 = 0;

   // Callback zum einsammeln der Koordinaten
   _myAction = new SoCallbackAction;
   _myAction->addPreCallback        (SoShape::getClassTypeId(), preCB, this);
   _myAction->addTriangleCallback   (SoShape::getClassTypeId(), triangleCB, this);

   _nodeSensor = new SoNodeSensor(_nodeSensorCB, this);
   _nodeSensor->setPriority(0);
   _nodeSensor->attach(this);

   int size= (int)pow((double)cubeSize.getValue(), 3);
   _hashTable = new std::set<Vertex*, ltVertex>[size];
   _edgeSet = 0;
   _triangleSet = 0;
}


SoShapePartition::~SoShapePartition() {
   // ein bischen aufraeumen
   _itssHolder->unref();
   if (_nodeSensor) { delete _nodeSensor; _nodeSensor = 0; }
   if (_myAction)   { delete _myAction;   _myAction   = 0; }

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
   if (_hashTable)   {
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) 
         _hashTable[index].clear();
      delete[] _hashTable;     
      _hashTable = 0; 
   }
   if (_vertexPropArray) { delete[] _vertexPropArray; _vertexPropArray = 0; }
   if (_sizeCoordinate3) { delete[] _sizeCoordinate3; _sizeCoordinate3 = 0; }
}


void SoShapePartition::_nodeSensorCB(void* clientData, SoSensor* /*sensor*/) {
   SoField *field= ((SoShapePartition *)clientData)->_nodeSensor->getTriggerField();
   ((SoShapePartition *)clientData)->_nodeSensor->detach();
   ((SoShapePartition *)clientData)->_nodeChanged(field);
   ((SoShapePartition *)clientData)->_nodeSensor->attach((SoNode *)clientData);
}


void SoShapePartition::_nodeChanged(SoField* triggerField) {
   if (triggerField != &valid && triggerField != &autoUpdate) {
      valid.setValue(false);
   }

   if(triggerField != 0 && (triggerField == &update || autoUpdate.getValue() == true)) {
      compute();
   }
}

void SoShapePartition::compute() {
   enableNotify(false);
   // alles schoen auf nichts zurücksetzen
   if (_vertexPropArray) { delete[] _vertexPropArray; _vertexPropArray = 0; }
   if (_sizeCoordinate3) { delete[] _sizeCoordinate3; _sizeCoordinate3 = 0; }

   for (int index = 0; index < (_extentX * _extentY * _extentZ); index++)
      _hashTable[index].clear();
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
   _itssHolder->removeAllChildren();

   // Wertebereich des Wuerfels eingrenzen
   if (cubeSize.getValue() < 10) cubeSize = 10;
   if (cubeSize.getValue() > 30) cubeSize = 30;

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

         if (shapePartitioning.getValue()) {
            _extentX = xSize / (float) cubeSize.getValue() + 1; 
            _extentY = ySize / (float) cubeSize.getValue() + 1; 
            _extentZ = zSize / (float) cubeSize.getValue() + 1;
         } else {
            _extentX = _extentY = _extentZ = 1;
         }

         _edgeSet = new std::set<Edge*, ltEdge>[_extentX * _extentY * _extentZ];
         _triangleSet = new std::set<Triangle*>[_extentX * _extentY * _extentZ];

         _vertexPropArray = new SoVertexProperty*[_extentX * _extentY * _extentZ];

         int index;
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
   valid.setValue(true);
   enableNotify(true);
}


SoCallbackAction::Response SoShapePartition::preCB(void* userData, SoCallbackAction* /*action*/, const SoNode* /*node*/) {
   SoShapePartition* obj = (SoShapePartition*) userData;

   // aktuelle Transformationsmatrix besorgen
   obj->_worldMatrix = obj->_myAction->getModelMatrix();

   return SoCallbackAction::CONTINUE;
}


void SoShapePartition::triangleCB(void* userData, SoCallbackAction* /*action*/,
                                  const SoPrimitiveVertex* v1,
                                  const SoPrimitiveVertex* v2,
                                  const SoPrimitiveVertex* v3) 
{
   SoShapePartition* obj = (SoShapePartition*) userData;

   SbVec3f vertex1, vertex2, vertex3, normal1, normal2, normal3;
   const Vertex *vertIndex1, *vertIndex2, *vertIndex3;
   Edge *edge1, *edge2, *edge3;
   SbVec3f barycenter;
   float baryX, baryY, baryZ;
   int arrayPosition;

   // Dreieckseckpunkte besorgen und in Weltkoordinaten umwandeln
   obj->_worldMatrix.multVecMatrix(v1->getPoint(), vertex1);
   obj->_worldMatrix.multVecMatrix(v2->getPoint(), vertex2);
   obj->_worldMatrix.multVecMatrix(v3->getPoint(), vertex3);

   if (obj->shapePartitioning.getValue()) {
      // Schwerpunkt und Position im Array bestimmen
      barycenter = vertex1 - obj->_offSet;
      barycenter += vertex2 - obj->_offSet;
      barycenter += vertex3 - obj->_offSet;

      barycenter = barycenter / 3.0;
      barycenter = barycenter / (float) obj->cubeSize.getValue();
      barycenter.getValue(baryX, baryY, baryZ);
      arrayPosition = ((int)fabs(baryX)) +
         ((int)fabs(baryY)) * obj->_extentX + 
         ((int)fabs(baryZ)) * obj->_extentX * obj->_extentY;
   } else
      arrayPosition = 0;

   // Eckpunkte abspeichern und Indices besorgen
   vertIndex1 = obj->insertVertex(vertex1, v1->getNormal(), SbVec2f(v1->getTextureCoords()[0], v1->getTextureCoords()[1]), arrayPosition);
   vertIndex2 = obj->insertVertex(vertex2, v2->getNormal(), SbVec2f(v2->getTextureCoords()[0], v2->getTextureCoords()[1]), arrayPosition);
   vertIndex3 = obj->insertVertex(vertex3, v3->getNormal(), SbVec2f(v3->getTextureCoords()[0], v3->getTextureCoords()[1]), arrayPosition);

   // Kanten finden bzw. anlegen
   edge3 = obj->generateEdge(vertIndex1, vertIndex2, arrayPosition);
   edge1 = obj->generateEdge(vertIndex2, vertIndex3, arrayPosition);
   edge2 = obj->generateEdge(vertIndex3, vertIndex1, arrayPosition);

   // Dreieck ablegen
   if (edge1 && edge2 && edge3) {
      try {
         Triangle* tri = new Triangle(vertIndex1, vertIndex2, vertIndex3, edge1, edge2, edge3);
         obj->_triangleSet[arrayPosition].insert(tri);

         // Den Kanten die Dreiecke zuweisen
         if (edge1) {
            if (edge1->getTriangle1() == 0) edge1->setTriangle1(tri);
            else edge1->setTriangle2(tri);
         }
         if (edge2) {
            if (edge2->getTriangle1() == 0) edge2->setTriangle1(tri);
            else edge2->setTriangle2(tri);
         }
         if (edge3) {
            if (edge3->getTriangle1() == 0) edge3->setTriangle1(tri);
            else edge3->setTriangle2(tri);
         }
      }
      catch (Puckception* error) {
         std::cerr << "Exception raised: " << error->details << '\n';
      }
   }
}


Edge* SoShapePartition::generateEdge(const Vertex* p1, const Vertex* p2, const int arrayPosition) {
   Edge* edge = new Edge;
   std::set<Edge*, ltEdge>::iterator it;
   try {
      // wenn Eckpunkte identisch sind, keine Edge anlegen
      if (p1 == p2) {
         return 0;
      }
      // zur Identifizierung immer den kleineren Vertex zuerst ablegen
      if (p1 < p2) {
         edge->setVertex1(p1);
         edge->setVertex2(p2);
      } else {
         edge->setVertex1(p2);
         edge->setVertex2(p1);
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
      // wenn eine der beiden Seiten der Edge noch nicht belegt ist, Edge zurückgeben, sonst NULL
      if (edge->getTriangle1() == 0 || edge->getTriangle2() == 0)
         return edge;
      else
         return 0;
   }
   catch (Puckception* error) {
      std::cerr << "Exception raised: " << error->details << '\n';
   }
   return 0;
}


const Vertex* SoShapePartition::insertVertex(const SbVec3f vector, const SbVec3f normal, const SbVec2f texCoord, const int arrayPosition) {
   // alle gesammelten Punkte werden in einer hash map abgelegt
   std::set<Vertex*, ltVertex>::iterator it;
   Vertex* vertex = new Vertex(vector, normal, texCoord); 

   try {
      // VertexProperty bereits erstellt?
      if (!_vertexPropArray[arrayPosition]) {
         _vertexPropArray[arrayPosition] = new SoVertexProperty;
         _vertexPropArray[arrayPosition]->normalBinding = SoVertexProperty::PER_VERTEX_INDEXED;
      }


      // nachsehen ob der Punkt bereits vorhanden ist
      it = _hashTable[arrayPosition].find(vertex);
      if (it != _hashTable[arrayPosition].end()) {
         // Eckpunkt ist bereits enthalten
         // Index zurückgeben
         vertex = *it;
      }
      else { 
         // noch nicht enthalten
         ++(_sizeCoordinate3[arrayPosition]);
         int index = _sizeCoordinate3[arrayPosition];
         vertex->vertexIndex = index;
         _hashTable[arrayPosition].insert(vertex);
         //_hashTable[arrayPosition][vector] = _sizeCoordinate3[arrayPosition];
         _vertexPropArray[arrayPosition]->vertex.set1Value(index, vector);
         _vertexPropArray[arrayPosition]->normal.set1Value(index, normal);
         _vertexPropArray[arrayPosition]->texCoord.set1Value(index, texCoord);
      }
      return vertex;
   }
   catch (Puckception* error) {
      std::cerr << "Exception raised: " << error->details << '\n';
   }
   return 0;
}


void SoShapePartition::generateITSS() {

   // fuer alle _triangleSets
   try {
      for (int index = 0; index < (_extentX * _extentY * _extentZ); index++) {
         // wenn dieser Block Dreiecke enthaelt
         if (!_triangleSet[index].empty()) {

            int count = -1;
            const Edge* edge = 0;
            Triangle* triNew = 0;
            const Vertex* tmpPoint = 0;

            // neues ITSS erzeugen und in den Szenengraphen einhaengen
            SoIndexedTriangleStripSet* itss = new SoIndexedTriangleStripSet;
            _itssHolder->addChild(itss);
            itss->vertexProperty = _vertexPropArray[index];

            // solange Dreiecke vorhanden sind
            while (!_triangleSet[index].empty()) {
               // erstes Dreieck einfuegen
               Triangle* triBegin = *_triangleSet[index].begin();
               itss->coordIndex.set1Value(++count, triBegin->getVertex1()->vertexIndex);
               itss->coordIndex.set1Value(++count, triBegin->getVertex2()->vertexIndex);
               itss->coordIndex.set1Value(++count, triBegin->getVertex3()->vertexIndex);

               // weitere Dreiecke anfuegen, anfangen mit beliebiger Kante
               // wenn die NULL, nächste Kante nehmen. Wenn alle NULL sind, gibts ein Problem
               edge = triBegin->getEdge1();
               if (!edge) edge = triBegin->getEdge2();
               if (!edge) edge = triBegin->getEdge3();
               // geordneten Richtungswechsel der Dreiecke festlegen (CW, CCW)
               bool direction = true;

               // solange gegenueberliegendes Dreieck und Dreiecke allgemein vorhanden sind
               while (edge != 0 && !_triangleSet[index].empty()) {
                  // gegenueberliegendes Dreieck der Kante holen
                  try {
                     triNew = edge->getOppositeTriangle(triBegin);
                     // bereits bearbeitetes Dreieck loeschen
                  }
                  catch (Puckception* error) {
                     std::cerr << "Exception raised: " << error->details << '\n';
                     // das Dreieck gibts wohl doch nicht mehr (nicht zwei-mannigfaltig?)
                     triNew = 0;
                  }
                  if (triBegin) {
                     _triangleSet[index].erase(triBegin);
                     try {
                        delete triBegin;
                     }
                     catch (Puckception* error) {
                        std::cerr << "Exception raised: " << error->details << '\n';
                     }
                  }
                  // wenn gegenueberliegendes Dreieck vorhanden
                  if (triNew) {
                     // neuen Punkt besorgen und anhaengen
                     tmpPoint = triNew->getOppositeVertex(edge);
                     if (tmpPoint) {
                        itss->coordIndex.set1Value(++count, tmpPoint->vertexIndex);
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
            }
         }
      }
   }
   catch (Puckception* error) {
      std::cerr << "Exception raised: " << error->details << '\n';
   }

}


Vertex::Vertex(SbVec3f vertex, SbVec3f normal, SbVec2f texCoord) : vertex(vertex), normal(normal), texCoord(texCoord) {
}


Edge::Edge() {
   _vertex1 = 0;
   _vertex2 = 0;
   _triangle1 = 0;
   _triangle2 = 0;
}


Edge::Edge(Edge* edge) {
   this->_vertex1 = edge->_vertex1;
   this->_vertex2 = edge->_vertex2;
   this->_triangle1 = edge->_triangle1;
   this->_triangle2 = edge->_triangle2;
   if (this->_vertex1 && this->_vertex1 == this->_vertex2)
      throw Puckception("SoShapePartition::Edge::Edge: vertices with same ID!", 200); 
   if (this->_triangle1 && this->_triangle1 == this->_triangle2)
      throw Puckception("SoShapePartition::Edge::Edge: triangles with same ID!", 200); 
}


Edge::~Edge() {
   _triangle1 = 0;
   _triangle2 = 0;
}


Triangle* Edge::getOppositeTriangle(const Triangle* tri) const {
   if (tri == _triangle1) return _triangle2;
   if (tri == _triangle2) return _triangle1;
   throw Puckception("SoShapePartition::Triangle::getOppositeTriangle: triangle is not aligned!", 23); 
   return 0;
}


void Edge::unrefTriangle(const Triangle* tri) {
   // Verweis auf bereits geloeschtes Dreieck aufloesen
   if (tri != _triangle1 && tri != _triangle2)
      throw Puckception("SoShapePartition::Edge::unrefTriangle: tried to delete a non-linked triangle!", 20); 
   if (tri == _triangle1) _triangle1 = 0;
   if (tri == _triangle2) _triangle2 = 0;
}


const Vertex* Edge::getVertex1() const {
   return _vertex1;
}


const Vertex* Edge::getVertex2() const {
   return _vertex2;
}


const Triangle* Edge::getTriangle1() const {
   return _triangle1;
}


const Triangle* Edge::getTriangle2() const {
   return _triangle2;
}


void Edge::setVertex1(const Vertex* v) {
   _vertex1 = v;
   if (_vertex1 && _vertex1 == _vertex2)
      throw Puckception("SoShapePartition::Edge::Edge: vertices with same ID!", 200); 
}


void Edge::setVertex2(const Vertex* v) {
   _vertex2 = v;
   if (_vertex1 && _vertex1 == _vertex2)
      throw Puckception("SoShapePartition::Edge::Edge: vertices with same ID!", 200); 
}


void Edge::setTriangle1(Triangle* tri) {
   _triangle1 = tri;
   if (_triangle1 && _triangle1 == _triangle2)
      throw Puckception("SoShapePartition::Edge::Edge: triangles with same ID!", 200); 
}


void Edge::setTriangle2(Triangle* tri) {
   _triangle2 = tri;
   if (_triangle1 && _triangle1 == _triangle2)
      throw Puckception("SoShapePartition::Edge::Edge: triangles with same ID!", 200); 
}


Triangle::Triangle() {
   _vertex1 = 0;
   _vertex2 = 0;
   _vertex3 = 0;
   _edge1 = 0;
   _edge2 = 0;
   _edge3 = 0;
}


Triangle::Triangle(const Vertex* _vertex1, const Vertex *_vertex2, const Vertex *_vertex3, Edge *_edge1, Edge *_edge2, Edge *_edge3) : _vertex1(_vertex1), _vertex2(_vertex2), _vertex3(_vertex3), _edge1(_edge1), _edge2(_edge2), _edge3(_edge3) {
   if (_vertex1 == _vertex2 || _vertex1 == _vertex3 || _vertex2 == _vertex3)
      throw Puckception("SoShapePartition::Triangle::Triangle: vertices with same ID or NULL!", 300); 
   if (_edge1 == _edge2 || _edge1 == _edge3 || _edge2 == _edge3)
      throw Puckception("SoShapePartition::Triangle::Triangle: edges with same ID or NULL!", 300); 
}


Triangle::Triangle(Triangle* triangle) {
   this->_vertex1 = triangle->_vertex1;
   this->_vertex2 = triangle->_vertex2;
   this->_vertex3 = triangle->_vertex3;
   this->_edge1 = triangle->_edge1;
   this->_edge2 = triangle->_edge2;
   this->_edge3 = triangle->_edge3;
}


Triangle::~Triangle() {
   // bei Kanten abmelden
   try {
      _edge1->unrefTriangle(this);
      _edge2->unrefTriangle(this);
      _edge3->unrefTriangle(this);
      _edge1 = 0;
      _edge2 = 0;
      _edge3 = 0;
   }
   catch (Puckception* error) {
      std::cerr << "Exception raised: " << error->details << '\n';
   }
}

const Vertex* Triangle::getVertex1() const {
   return _vertex1;
}


const Vertex* Triangle::getVertex2() const {
   return _vertex2;
}


const Vertex* Triangle::getVertex3() const {
   return _vertex3;
}


const Edge* Triangle::getEdge1() const {
   return _edge1;
}


const Edge* Triangle::getEdge2() const {
   return _edge2;
}


const Edge* Triangle::getEdge3() const {
   return _edge3;
}


const Vertex* Triangle::getOppositeVertex(const Edge* edge) const {
   if (edge == _edge1) return _vertex1;
   if (edge == _edge2) return _vertex2;
   if (edge == _edge3) return _vertex3;
   throw Puckception("SoShapePartition::Triangle::getOppositeVertex: edge is not aligned!", 31); 
   return 0;
}


const Edge* Triangle::getCWEdge(const Vertex* vertex) const {
   if (vertex == _vertex1) return _edge2;
   if (vertex == _vertex2) return _edge3;
   if (vertex == _vertex3) return _edge1;
   throw Puckception("SoShapePartition::Triangle::getCWEdge: vertex is not aligned!", 320); 
   return 0;
}


const Edge* Triangle::getCCWEdge(const Vertex* vertex) const {
   if (vertex == _vertex1) return _edge3;
   if (vertex == _vertex2) return _edge1;
   if (vertex == _vertex3) return _edge2;
   throw Puckception("SoShapePartition::Triangle::getCCWEdge: vertex is not aligned!", 321); 
   return 0;
}
