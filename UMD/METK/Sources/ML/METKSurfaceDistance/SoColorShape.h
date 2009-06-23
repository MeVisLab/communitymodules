#ifndef _SoColorShape
#define _SoColorShape


#include <Inventor/actions/SoCallbackAction.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFTrigger.h>


#ifdef WIN32
#pragma warning (disable: 4786)
#pragma warning (disable: 4710)
#pragma warning (disable: 4146)
#pragma warning (push,3)
#pragma warning (disable: 4018)
#pragma warning (disable: 4786)
#pragma warning (disable: 4710)
#pragma warning (disable: 4146)
#endif

#include <set>
#include <map>
#include <vector>

#ifdef WIN32
#pragma warning (pop)
#endif

//! size of the hash table pow(HASH_PARTITION, 3)
#define HASH_PARTITION 8


class SoCallbackAction;
class SoMainAxis;
class SoNodeSensor;
class SoPointSet;
class SoPrimitiveVertex;
class SoVertexProperty;
class SoIndexedTriangleStripSet;

class Triangle;
class Edge;
class Vertex;
struct ltVertex;
struct ltEdge;

#include "mlMETKSurfaceDistance3D.h"

class SoColorShape : public SoGroup {
   //! macro that defines extended methods
   SO_NODE_HEADER(SoColorShape);
   
   
public:
   
   //! Constructor
   SoColorShape();
   
   SoSFNode input;
   
   //! must be called first to initialize the class in OpenInventor
   static void initClass();
   void compute();
   void setMLClass(ml::METKSurfaceDistance3D* surDis);

   float _nearDistance, _farDistance;
   SbColor _nearDistanceColor, _farDistanceColor;
   
   
protected:
   
   //! Destructor
   virtual ~SoColorShape();
   
private:
   
   //! internal representation of the point set
   SoVertexProperty** _vertexPropArray;
   int* _sizeCoordinate3;
   //! the transformation matrix
   SbMatrix _worldMatrix;
   
   
   ml::METKSurfaceDistance3D* _surDis;
   
   //! splits the triangle if it has different color indices and stores the unique colored triangles
   void splitTriangle(
      const SbVec3f& vertex1, const SbVec3f& vertex2, const SbVec3f& vertex3, 
      const SbVec3f& normal1, const SbVec3f& normal2, const SbVec3f& normal3,
      const double& distance1, const double& distance2, const double& distance3);
   
   //! splits the triangle if it 2 different color indices and an extension over 2 color levels. Stores the unique colored triangles afterwards
   void splitTriangle2Diff2Level(
      const SbVec3f& vertex1c1, const SbVec3f& vertex2c2, const SbVec3f& vertex3c2, 
      const SbVec3f& normal1c1, const SbVec3f& normal2c2, const SbVec3f& normal3c2,
      const double& distance1c1, const double& distance2c2, const double& distance3c2);
   
   //! stores the given triangle
   void storeTriangle(
      const SbVec3f& vertex1, const SbVec3f& vertex2, const SbVec3f& vertex3, 
      const SbVec3f& normal1, const SbVec3f& normal2, const SbVec3f& normal3,
      const int& colorIndex1);
   
   //! stores a triangle with three different colored vertices
   void storeTriangleAllDifferent(
      const SbVec3f&    point1, const SbVec3f&    point2, const SbVec3f&    point3,
      const SbVec3f&   tpoint1, const SbVec3f&   tpoint2, const SbVec3f&   tpoint3,
      const SbVec3f&    dpoint,
      const SbVec3f&   normal1, const SbVec3f&   normal2, const SbVec3f&   normal3,
      const SbVec3f&  tnormal1, const SbVec3f&  tnormal2, const SbVec3f&  tnormal3,
      const SbVec3f&   dnormal,
      const int& colorIndex1,   const int& colorIndex2,   const int& colorIndex3);

   //! returns a vertex with wanted distance value
   const SbVec3f interpolateVector(const SbVec3f& vertex1, const SbVec3f& vertex2, const double& distance1, const double& distance2, const double& distanceWanted);
   
   //! inserts the points into the hashtable
   const Vertex* insertVertex(const SbVec3f vector, const SbVec3f normal, const int colorIndex, const int arrayPosition);
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
   std::set<Vertex*, ltVertex>* _vertexSet;
   
   //! set containing all edges
   std::set<Edge*, ltEdge>* _edgeSet;
   
   //! vector containing all triangles
   std::set<Triangle*>* _triangleSet;
   
   int _extentX, _extentY, _extentZ;
   SbVec3f _offSet;
};


class Vertex {
public:
   Vertex(SbVec3f vertex, short colorIndex);
   SbVec3f vertex;
   short colorIndex;
   int vertexIndex;
};


class Edge {
public:
   Edge();
   Edge(Edge* edge);
   virtual ~Edge();
   Triangle* getOppositeTriangle(const Triangle* tri) const;
   void unrefTriangle(const Triangle* tri);
   
   const Vertex *vertex1, *vertex2;
   Triangle *triangle1, *triangle2;
};


class Triangle {
public:
   Triangle();
   Triangle(Triangle* triangle);
   virtual ~Triangle();
   const Vertex* getOppositeVertex(const Edge* edge) const;
   const Edge* getCWEdge(const Vertex* vertex) const;
   const Edge* getCCWEdge(const Vertex* vertex) const;
   
   const Vertex *vertex1, *vertex2, *vertex3;
   Edge *edge1, *edge2, *edge3;
};


//! sort criterion for the hashtable set
struct ltVertex {
   
   //! sorts points lexicographical
   //! test for vertex coordinates, if they are the same, test for color index
   bool operator()(const Vertex* v1, const Vertex* v2) const {
      const float* p1 = v1->vertex.getValue();
      const float* p2 = v2->vertex.getValue();
      
      if (*p1 < *p2) return true;
      else {
         if (*p1 > *p2) return false;
         else {
            if (*(++p1) < *(++p2)) return true;
            else {
               if (*p1 > *p2) return false;
               else {
                  if (*(++p1) < *(++p2)) return true;
                  else {
                     if (*p1 > *p2) return false;
                     else {
                        if (v1->colorIndex < v2->colorIndex) return true;
                        else return false;
                     }
                  }
               }
            }
         }
      }
   }
};


//! sort criterion for the edge set
struct ltEdge {
   
   bool operator()(const Edge edge1, const Edge edge2) const {
      const Vertex* e1v1 = edge1.vertex1;
      const Vertex* e2v1 = edge2.vertex1;
      
      if (e1v1 < e2v1) return true;
      else {
         if (e1v1 > e2v1) return false;
         else {
            const Vertex* e1v2 = edge1.vertex2;
            const Vertex* e2v2 = edge2.vertex2;
            if (e1v2 < e2v2) return true;
            else return false;
         }
      }
   }
};

#endif
