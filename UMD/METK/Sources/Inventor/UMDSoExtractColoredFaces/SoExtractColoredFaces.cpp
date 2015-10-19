//----------------------------------------------------------------------------------
//! The Inventor module class SoExtractColoredFaces
/*!
// \file    SoExtractColoredFaces.cpp
// \author  Konrad Mühler
// \date    2008-02-01
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoExtractColoredFaces.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
// TODO: include more inventor things here
#include "XVLeaveScope.h"

#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

SO_NODE_SOURCE(SoExtractColoredFaces)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoExtractColoredFaces::initClass()
{
	SO_NODE_INIT_CLASS(SoExtractColoredFaces, SoSeparator, "Separator");
}


// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoExtractColoredFaces::SoExtractColoredFaces()
{
	SO_TRACE_IN("SoExtractColoredFaces::SoExtractColoredFaces");

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoExtractColoredFaces);

	SO_NODE_ADD_FIELD(input, (NULL));
	SO_NODE_ADD_FIELD(outShape, (NULL));
	SO_NODE_ADD_FIELD(update, ());
	SO_NODE_ADD_FIELD(selectedColor, (1,1,1));
	SO_NODE_ADD_FIELD(valid, (false));
	SO_NODE_ADD_FIELD(printColors, (false));

	// internen Szenengraphen aufbauen
	_itssHolder =  new SoSeparator;
	_itssHolder->ref();
	outShape = _itssHolder;
	_vertexProp = 0;
	_sizeCoordinate3 = 0;

	// Callback zum einsammeln der Koordinaten
	_callbackAction = new SoCallbackAction;
	_callbackAction->addPreCallback        (SoShape::getClassTypeId(), preCB, this);
	_callbackAction->addTriangleCallback   (SoShape::getClassTypeId(), triangleCB, this);

	_hashTable = new std::set<Vertex*, ltVertex>;
	_edgeSet = 0;
	_triangleSet = 0;

	_nodeSensor = new SoNodeSensor(SoExtractColoredFaces::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);
}


// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoExtractColoredFaces::~SoExtractColoredFaces()
{
	SO_TRACE_IN("SoExtractColoredFaces::~SoExtractColoredFaces");

	// Remove the node sensor.
	if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
	_itssHolder->unref();
	if (_nodeSensor) { delete _nodeSensor; _nodeSensor = 0; }
	if (_callbackAction)   { delete _callbackAction;   _callbackAction   = 0; }

	if (_triangleSet)
	{ 		
		_triangleSet->clear(); 
		delete _triangleSet; 
		_triangleSet = 0; 
	}
	if (_edgeSet)     
	{ 		
		_edgeSet->clear();     
		delete _edgeSet;     
		_edgeSet = 0; 
	}
	if (_hashTable)   {

		_hashTable->clear();
		delete _hashTable;     
		_hashTable = 0; 
	}
}


//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoExtractColoredFaces::nodeChangedCB(void *data, SoSensor* sens)
{
	SO_TRACE_IN("SoExtractColoredFaces::nodeChangedCB");

	((SoExtractColoredFaces*) data)->nodeChanged((SoNodeSensor*)sens);
}


//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoExtractColoredFaces::nodeChanged(SoNodeSensor* sensor)
{
	SO_TRACE_IN("SoExtractColoredFaces::nodeChanged");

	// Get the field which caused the notification.
	SoField* field = sensor->getTriggerField();

	if (field==&update)
	{
		enableNotify(false);
		// alles schoen auf nichts zurücksetzen
		_hashTable->clear();
		if (_triangleSet) 
		{ 			
			_triangleSet->clear(); 
			delete _triangleSet; 
			_triangleSet = 0; 
		}
		if (_edgeSet)     
		{ 			
			_edgeSet->clear();     
			delete _edgeSet;     
			_edgeSet = 0; 
		}
		_itssHolder->removeAllChildren();

		if (input.getValue() != 0)
		{
			_edgeSet = new std::set<Edge*, ltEdge>;
			_triangleSet = new std::set<Triangle*>;

			_vertexProp = new SoVertexProperty;
			
			//Von Konrad:
			//_vertexProp->ref();

			_sizeCoordinate3 = -1;

			// Dreiecke aufsammeln
			_callbackAction->apply(input.getValue());

			// SoIndexedTriangleStripSet erstellen
			generateITSS();
		}
		valid.setValue(true);
		enableNotify(true);
	}
	else if (field==&input)
	{
		//std::cout << "input" << std::endl;
	}
}


SoCallbackAction::Response SoExtractColoredFaces::preCB(void* userData, SoCallbackAction* /*action*/, const SoNode* /*node*/)
{
	SoExtractColoredFaces* thisp = (SoExtractColoredFaces*) userData;
	// aktuelle Transformationsmatrix besorgen
	thisp->_worldMatrix = thisp->_callbackAction->getModelMatrix();
	return SoCallbackAction::CONTINUE;
}



void SoExtractColoredFaces::triangleCB(void* usrData, SoCallbackAction* action, const SoPrimitiveVertex* v1, const SoPrimitiveVertex* v2, const SoPrimitiveVertex* v3)
{
	SoExtractColoredFaces* thisp = (SoExtractColoredFaces*)usrData;

	SbVec3f vertex1, vertex2, vertex3, normal1, normal2, normal3;
	const Vertex *vertIndex1, *vertIndex2, *vertIndex3;
	Edge *edge1, *edge2, *edge3;

	// Dreieckseckpunkte besorgen und in Weltkoordinaten umwandeln
	thisp->_worldMatrix.multVecMatrix(v1->getPoint(), vertex1);
	thisp->_worldMatrix.multVecMatrix(v2->getPoint(), vertex2);
	thisp->_worldMatrix.multVecMatrix(v3->getPoint(), vertex3);

	SbColor foo;
	float   bar=0;
	SbColor tempColor;
	float tempTransp;
	int matIndex = v1->getMaterialIndex();
	action->getMaterial(foo, tempColor, foo, foo, bar, tempTransp, matIndex);

	float r,g,b;
	tempColor.getValue(r,g,b);

	std::vector<SbColor>::const_iterator iter;
	bool insert=true;
	float r2,g2,b2;
	for (iter=thisp->colorSearchList.begin();iter!=thisp->colorSearchList.end();iter++)
	{
		SbColor tempColor = (*iter);
		tempColor.getValue(r2,g2,b2);
		if (r==r2 && g==g2 && b==b2) insert=false;
	}

	if (insert)
	{
		thisp->colorSearchList.push_back(tempColor);
		if (thisp->printColors.getValue()) std::cout << r << "," << g << "," << b << std::endl;
	}

	thisp->selectedColor.getValue().getValue(r2,g2,b2);
	if (epsilon(0.01,r,r2) && epsilon(0.01,g,g2) && epsilon(0.01,b,b2))
	{
		// Eckpunkte abspeichern und Indices besorgen
		vertIndex1 = thisp->insertVertex(vertex1, v1->getNormal(), SbVec2f(v1->getTextureCoords()[0], v1->getTextureCoords()[1]));
		vertIndex2 = thisp->insertVertex(vertex2, v2->getNormal(), SbVec2f(v2->getTextureCoords()[0], v2->getTextureCoords()[1]));
		vertIndex3 = thisp->insertVertex(vertex3, v3->getNormal(), SbVec2f(v3->getTextureCoords()[0], v3->getTextureCoords()[1]));

		// Kanten finden bzw. anlegen
		edge3 = thisp->generateEdge(vertIndex1, vertIndex2);
		edge1 = thisp->generateEdge(vertIndex2, vertIndex3);
		edge2 = thisp->generateEdge(vertIndex3, vertIndex1);

		// Dreieck ablegen
		if (edge1 && edge2 && edge3)
		{
			try {
				Triangle* tri = new Triangle(vertIndex1, vertIndex2, vertIndex3, edge1, edge2, edge3);
				thisp->_triangleSet->insert(tri);

				// Den Kanten die Dreiecke zuweisen
				if (edge1)
				{
					if (edge1->getTriangle1() == 0) edge1->setTriangle1(tri);
					else edge1->setTriangle2(tri);
				}
				if (edge2)
				{
					if (edge2->getTriangle1() == 0) edge2->setTriangle1(tri);
					else edge2->setTriangle2(tri);
				}
				if (edge3)
				{
					if (edge3->getTriangle1() == 0) edge3->setTriangle1(tri);
					else edge3->setTriangle2(tri);
				}
			}
			catch (SomeException* error)
			{
				std::cerr << "Exception raised: " << error->details << '\n';
			}
		}
	}
}


bool SoExtractColoredFaces::epsilon(double sour, double value1, double value2)
{
	bool equal = true;
	if (value1<(value2-sour)) equal=false;
	if (value1>(value2+sour)) equal=false;
	return equal;
}


Edge* SoExtractColoredFaces::generateEdge(const Vertex* p1, const Vertex* p2)
{
	Edge* edge = new Edge;
	std::set<Edge*, ltEdge>::iterator it;
	try {
		// wenn Eckpunkte identisch sind, keine Edge anlegen
		if (p1 == p2)
		{
			return 0;
		}
		// zur Identifizierung immer den kleineren Vertex zuerst ablegen
		if (p1 < p2)
		{
			edge->setVertex1(p1);
			edge->setVertex2(p2);
		} else {
			edge->setVertex1(p2);
			edge->setVertex2(p1);
		}
		it = _edgeSet->find(edge);
		if (it != _edgeSet->end())
		{
			// Kante ist bereits enthalten, Kante aus set zurückgeben
			delete edge;
			edge = *it;
		}
		else
		{
			// Kante ist noch nicht enthalten, ins set einfuegen
			_edgeSet->insert(edge);
		}
		// wenn eine der beiden Seiten der Edge noch nicht belegt ist, Edge zurückgeben, sonst NULL
		if (edge->getTriangle1() == 0 || edge->getTriangle2() == 0)
			return edge;
		else
			return 0;
	}
	catch (SomeException* error)
	{
		std::cerr << "Exception raised: " << error->details << '\n';
	}
	return 0;
}


const Vertex* SoExtractColoredFaces::insertVertex(const SbVec3f vector, const SbVec3f normal, const SbVec2f texCoord)
{
	// alle gesammelten Punkte werden in einer hash map abgelegt
	std::set<Vertex*, ltVertex>::iterator it;
	Vertex* vertex = new Vertex(vector, normal, texCoord); 

	try
	{
		// VertexProperty bereits erstellt?
		if (!_vertexProp)
		{
			_vertexProp = new SoVertexProperty;
			_vertexProp->normalBinding = SoVertexProperty::PER_VERTEX_INDEXED;
		}

		// nachsehen ob der Punkt bereits vorhanden ist
		it = _hashTable->find(vertex);
		if (it != _hashTable->end())
		{
			// Eckpunkt ist bereits enthalten
			// Index zurückgeben
			vertex = *it;
		}
		else
		{ 
			// noch nicht enthalten

			++(_sizeCoordinate3);
			//if (_sizeCoordinate3%500==0) std::cout << _sizeCoordinate3 << std::endl;
			//int index = _sizeCoordinate3;
			vertex->vertexIndex = _sizeCoordinate3;
			_hashTable->insert(vertex);			
			_vertexProp->vertex.set1Value(_sizeCoordinate3, vector);
			_vertexProp->normal.set1Value(_sizeCoordinate3, normal);
			//_vertexProp->texCoord.set1Value(_sizeCoordinate3, texCoord);
		}
		return vertex;
	}
	catch (SomeException* error)
	{
		std::cerr << "Exception raised: " << error->details << '\n';
	}
	return 0;
}



void SoExtractColoredFaces::generateITSS()
{
	try
	{
		// wenn dieser Block Dreiecke enthaelt
		if (!_triangleSet->empty())
		{
			int count = -1;
			const Edge* edge = 0;
			Triangle* triNew = 0;
			const Vertex* tmpPoint = 0;

			// neues ITSS erzeugen und in den Szenengraphen einhaengen
			SoIndexedTriangleStripSet* itss = new SoIndexedTriangleStripSet;
			_itssHolder->addChild(itss);
			itss->vertexProperty = _vertexProp;

			// solange Dreiecke vorhanden sind
			while (!_triangleSet->empty())
			{
				// erstes Dreieck einfuegen
				Triangle* triBegin = *_triangleSet->begin();
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
				while (edge != 0 && !_triangleSet->empty()) {
					// gegenueberliegendes Dreieck der Kante holen
					try {
						triNew = edge->getOppositeTriangle(triBegin);
						// bereits bearbeitetes Dreieck loeschen
					}
					catch (SomeException* error) {
						std::cerr << "Exception raised: " << error->details << '\n';
						// das Dreieck gibts wohl doch nicht mehr (nicht zwei-mannigfaltig?)
						triNew = 0;
					}
					if (triBegin)
					{
						_triangleSet->erase(triBegin);
						try {
							delete triBegin;
						}
						catch (SomeException* error) {
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
		//}
	}
	catch (SomeException* error) {
		std::cerr << "Exception raised: " << error->details << '\n';
	}

}

Vertex::Vertex(SbVec3f vertex, SbVec3f normal, SbVec2f texCoord) : vertex(vertex), normal(normal), texCoord(texCoord) { }

Edge::Edge()
{
	_vertex1 = 0;
	_vertex2 = 0;
	_triangle1 = 0;
	_triangle2 = 0;
}


Edge::Edge(Edge* edge)
{
	this->_vertex1 = edge->_vertex1;
	this->_vertex2 = edge->_vertex2;
	this->_triangle1 = edge->_triangle1;
	this->_triangle2 = edge->_triangle2;
	if (this->_vertex1 && this->_vertex1 == this->_vertex2)
		throw SomeException("SoShapePartition::Edge::Edge: vertices with same ID!", 200); 
	if (this->_triangle1 && this->_triangle1 == this->_triangle2)
		throw SomeException("SoShapePartition::Edge::Edge: triangles with same ID!", 200);
}


Edge::~Edge()
{
	_triangle1 = 0;
	_triangle2 = 0;
}


Triangle* Edge::getOppositeTriangle(const Triangle* tri) const
{
	if (tri == _triangle1) return _triangle2;
	if (tri == _triangle2) return _triangle1;
	throw SomeException("SoShapePartition::Triangle::getOppositeTriangle: triangle is not aligned!", 23); 
	return 0;
}


void Edge::unrefTriangle(const Triangle* tri)
{
	// Verweis auf bereits geloeschtes Dreieck aufloesen
	if (tri != _triangle1 && tri != _triangle2)
		throw SomeException("SoShapePartition::Edge::unrefTriangle: tried to delete a non-linked triangle!", 20); 
	if (tri == _triangle1) _triangle1 = 0;
	if (tri == _triangle2) _triangle2 = 0;
}


const Vertex* Edge::getVertex1() const { return _vertex1; }

const Vertex* Edge::getVertex2() const { return _vertex2; }

const Triangle* Edge::getTriangle1() const { return _triangle1; }

const Triangle* Edge::getTriangle2() const { return _triangle2; }

void Edge::setVertex1(const Vertex* v)
{
	_vertex1 = v;
	if (_vertex1 && _vertex1 == _vertex2)
		throw SomeException("SoShapePartition::Edge::Edge: vertices with same ID!", 200);
}


void Edge::setVertex2(const Vertex* v)
{
	_vertex2 = v;
	if (_vertex1 && _vertex1 == _vertex2)
		throw SomeException("SoShapePartition::Edge::Edge: vertices with same ID!", 200);
}


void Edge::setTriangle1(Triangle* tri)
{
	_triangle1 = tri;
	if (_triangle1 && _triangle1 == _triangle2)
		throw SomeException("SoShapePartition::Edge::Edge: triangles with same ID!", 200); 
}


void Edge::setTriangle2(Triangle* tri)
{
	_triangle2 = tri;
	if (_triangle1 && _triangle1 == _triangle2)
		throw SomeException("SoShapePartition::Edge::Edge: triangles with same ID!", 200); 
}


Triangle::Triangle()
{
	_vertex1 = 0;
	_vertex2 = 0;
	_vertex3 = 0;
	_edge1 = 0;
	_edge2 = 0;
	_edge3 = 0;
}


Triangle::Triangle(const Vertex* _vertex1, const Vertex *_vertex2, const Vertex *_vertex3, Edge *_edge1, Edge *_edge2, Edge *_edge3) : _vertex1(_vertex1), _vertex2(_vertex2), _vertex3(_vertex3), _edge1(_edge1), _edge2(_edge2), _edge3(_edge3)
{
	if (_vertex1 == _vertex2 || _vertex1 == _vertex3 || _vertex2 == _vertex3)
		throw SomeException("SoShapePartition::Triangle::Triangle: vertices with same ID or NULL!", 300); 
	if (_edge1 == _edge2 || _edge1 == _edge3 || _edge2 == _edge3)
		throw SomeException("SoShapePartition::Triangle::Triangle: edges with same ID or NULL!", 300); 
}


Triangle::Triangle(Triangle* triangle)
{
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
	catch (SomeException* error) {
		std::cerr << "Exception raised: " << error->details << '\n';
	}
}

const Vertex* Triangle::getVertex1() const { return _vertex1; }

const Vertex* Triangle::getVertex2() const { return _vertex2; }

const Vertex* Triangle::getVertex3() const { return _vertex3; }

const Edge* Triangle::getEdge1() const { return _edge1; }

const Edge* Triangle::getEdge2() const { return _edge2; }

const Edge* Triangle::getEdge3() const { return _edge3; }

const Vertex* Triangle::getOppositeVertex(const Edge* edge) const
{
	if (edge == _edge1) return _vertex1;
	if (edge == _edge2) return _vertex2;
	if (edge == _edge3) return _vertex3;
	throw SomeException("SoShapePartition::Triangle::getOppositeVertex: edge is not aligned!", 31); 
	return 0;
}


const Edge* Triangle::getCWEdge(const Vertex* vertex) const
{
	if (vertex == _vertex1) return _edge2;
	if (vertex == _vertex2) return _edge3;
	if (vertex == _vertex3) return _edge1;
	throw SomeException("SoShapePartition::Triangle::getCWEdge: vertex is not aligned!", 320); 
	return 0;
}


const Edge* Triangle::getCCWEdge(const Vertex* vertex) const
{
	if (vertex == _vertex1) return _edge3;
	if (vertex == _vertex2) return _edge1;
	if (vertex == _vertex3) return _edge2;
	throw SomeException("SoShapePartition::Triangle::getCCWEdge: vertex is not aligned!", 321); 
	return 0;
}
