#ifndef _SOSHAPEPARTITION
#define _SOSHAPEPARTITION


#include "UMDToolsSystem.h"

#include <Inventor/actions/SoCallbackAction.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFTrigger.h>

//! size of the buffer for fast caching of the point set
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10000
#endif


class SoCallbackAction;
class SoNodeSensor;
class SoPointSet;
class SoPrimitiveVertex;
class SoSFMatrix;
class SoVertexProperty;

class Vertex;
class Edge;
class Triangle;
struct ltVertex;
struct ltEdge;


class UMDTOOLS_EXPORT SoShapePartition : public SoSeparator {
   //! macro that defines extended methods
   SO_NODE_HEADER(SoShapePartition);
   
   
public:
   
   //! Constructor
   SoShapePartition();
   
   SoSFNode outConvertedShape;
   SoSFNode input;

   SoSFBool shapePartitioning;
   SoSFTrigger update;
   SoSFInt32 cubeSize;

   SoSFBool valid;
   SoSFBool autoUpdate;

   
   //! must be called first to initialize the class in OpenInventor
   static void initClass();
   
   
protected:
   
   //! Destructor
   virtual ~SoShapePartition();
   
   
private:
   
   //! internal representation of the point set
   SoVertexProperty** _vertexPropArray;
   SoGroup* _itssHolder;
   int* _sizeCoordinate3;
   //! the transformation matrix
   SbMatrix _worldMatrix;

   //! compute the new shape on update
   SoNodeSensor* _nodeSensor;
   static void _nodeSensorCB(void *userData, SoSensor*);
   void _nodeChanged  (SoField *field);

   void compute();
   
   //! inserts the points into the hashtable
   const Vertex* insertVertex(const SbVec3f vector, const SbVec3f normal, const SbVec2f texCoord, const int arrayPosition);
   Edge* generateEdge(const Vertex* p1, const Vertex* p2, const int arrayPosition);

   //! generate TriangleStripSets out of the collected triangles
   void generateITSS();
   
   //! callback action to collect the points
   SoCallbackAction* _myAction;
   //! called, if a new child in the scenegraph will be traversed
   static SoCallbackAction::Response  preCB(void *userData, SoCallbackAction *action, const SoNode *node);
   static void triangleCB(void* userData, SoCallbackAction* action,
      const SoPrimitiveVertex* v1,
      const SoPrimitiveVertex* v2,
      const SoPrimitiveVertex* v3);
   
   //! map hashtable containing all vertices
   std::set<Vertex*, ltVertex>* _hashTable;
   
   //! set containing all edges
   std::set<Edge*, ltEdge>* _edgeSet;
   
   //! vector containing all triangles
   std::set<Triangle*>* _triangleSet;

   int _extentX, _extentY, _extentZ;
   SbVec3f _offSet;
};


class UMDTOOLS_EXPORT Puckception {
public:
   Puckception(char* details, int code) : details(details), code(code) {std::cerr << "Exception raised: " << details << '\n';};
   int code;
   char* details;
};


class Vertex {
public:
   Vertex(SbVec3f vertex, SbVec3f normal, SbVec2f texCoord);
   SbVec3f vertex;
   SbVec3f normal;
   SbVec2f texCoord;
   int vertexIndex;
};


class Edge {
public:
   Edge();
   Edge(Edge* edge);
   virtual ~Edge();
   Triangle* getOppositeTriangle(const Triangle* tri) const;
   void unrefTriangle(const Triangle* tri);
   const Vertex* getVertex1() const;
   const Vertex* getVertex2() const;
   const Triangle* getTriangle1() const;
   const Triangle* getTriangle2() const;
   void setVertex1(const Vertex* v);
   void setVertex2(const Vertex* v);
   void setTriangle1(Triangle* tri);
   void setTriangle2(Triangle* tri);
   
private:
   const Vertex *_vertex1, *_vertex2;
   Triangle *_triangle1, *_triangle2;
};


class Triangle {
public:
   Triangle();
   Triangle(Triangle* triangle);
   Triangle(const Vertex *vertex1, const Vertex *vertex2, const Vertex *vertex3, Edge *edge1, Edge *edge2, Edge *edge3);
   virtual ~Triangle();
   const Vertex* getOppositeVertex(const Edge* edge) const;
   const Edge* getCWEdge(const Vertex* vertex) const;
   const Edge* getCCWEdge(const Vertex* vertex) const;
   const Vertex* getVertex1() const;
   const Vertex* getVertex2() const;
   const Vertex* getVertex3() const;
   const Edge* getEdge1() const;
   const Edge* getEdge2() const;
   const Edge* getEdge3() const;
   
private:
   const Vertex *_vertex1, *_vertex2, *_vertex3;
   Edge *_edge1, *_edge2, *_edge3;
};


//! sort criterion for the hashtable set
struct ltVertex {
   
   //! sorts points lexicographical
   //! test for vertex coordinates, if they are the same, test for color index
   bool operator()(const Vertex* v1, const Vertex* v2) const {
      const float* p1 = v1->vertex.getValue();
      const float* p2 = v2->vertex.getValue();
      
      if (*p1 < *p2) return true;
      if (*p1 > *p2) return false;
      if (*(++p1) < *(++p2)) return true;
      if (*p1 > *p2) return false;
      if (*(++p1) < *(++p2)) return true;
      if (*p1 > *p2) return false;


      p1 = v1->normal.getValue();
      p2 = v2->normal.getValue();
      
      if (*p1 < *p2) return true;
      if (*p1 > *p2) return false;
      if (*(++p1) < *(++p2)) return true;
      if (*p1 > *p2) return false;
      if (*(++p1) < *(++p2)) return true;
      if (*p1 > *p2) return false;


      p1 = v1->texCoord.getValue();
      p2 = v2->texCoord.getValue();
      
      if (*p1 < *p2) return true;
      if (*p1 > *p2) return false;
      if (*(++p1) < *(++p2)) return true;
      if (*p1 > *p2) return false;
      return false;
   }
};


//! sort criterion for the edge set
struct ltEdge {
   
   bool operator()(const Edge edge1, const Edge edge2) const {
      const Vertex* e1v1 = edge1.getVertex1();
      const Vertex* e2v1 = edge2.getVertex1();
      
      if (e1v1 < e2v1) return true;
      else {
         if (e1v1 > e2v1) return false;
         else {
            const Vertex* e1v2 = edge1.getVertex2();
            const Vertex* e2v2 = edge2.getVertex2();
            if (e1v2 < e2v2) return true;
            else return false;
         }
      }
   }
};


#endif // _SOSHAPEPARTITION
