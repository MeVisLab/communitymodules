//----------------------------------------------------------------------------------
//! The Inventor module class SoQHull3D
/*!
// \file    SoQHull3D.cpp
// \author  Jens Haase
// \date    2005-02-01
//
// convex hull of invetor scene using qhull
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoQHull3D.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

// TODO: include more inventor things here
#include "XVLeaveScope.h"

#include <list>
#include <iostream>
#include <algorithm>

SO_NODE_SOURCE(SoQHull3D)


//debug
bool debug = 0;


///////////////////////////////////////////////////////////////////////////
// INIT CLASS
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::initClass()
{
	SO_NODE_INIT_CLASS(SoQHull3D, SoSeparator, "Separator");
}

///////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
///////////////////////////////////////////////////////////////////////////
SoQHull3D::SoQHull3D()
{

  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoQHull3D);


  SO_NODE_ADD_FIELD(numInput,     (0));
  SO_NODE_ADD_FIELD(numOutput,    (0));
  SO_NODE_ADD_FIELD(numTriangles, (0));
	SO_NODE_ADD_FIELD(ch_surface,   (0));
  SO_NODE_ADD_FIELD(ch_volume,    (0));

  SO_NODE_ADD_FIELD(showPoints, (true));
  SO_NODE_ADD_FIELD(showFaces,  (true));
  SO_NODE_ADD_FIELD(pointColor, (0.5,0.5,1.0));
  SO_NODE_ADD_FIELD(faceColor,  (0.7,0.7,0.4));
  SO_NODE_ADD_FIELD(useColors,  (false));
  SO_NODE_ADD_FIELD(state,      ("no input"));
  SO_NODE_ADD_FIELD(out_qhull,  (NULL));

	//hidden in automatic panel
	SO_NODE_ADD_FIELD(showNormals,    (false));
	SO_NODE_ADD_FIELD(useFaceNormals, (true));


  // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
  // sensor to be sure that changes are not delayed or collected.
  nodeSensor = new SoNodeSensor(SoQHull3D::nodeChangedCB, this);
  nodeSensor->setPriority(0);
  nodeSensor->attach(this);

	//callbacks
	callbackAction = new SoCallbackAction();
  callbackAction->addPreCallback         (SoShape::getClassTypeId(), preCB,         this);
	callbackAction->addTriangleCallback    (SoShape::getClassTypeId(), triangleCB,    this);
	callbackAction->addLineSegmentCallback (SoShape::getClassTypeId(), lineSegmentCB, this);
  callbackAction->addPointCallback       (SoShape::getClassTypeId(), pointCB,       this);

	//separators/group
	output    = new SoGroup();
	chPoints  = new SoSeparator();
	chFaces   = new SoSeparator();
	chNormals = new SoSeparator();

	//output
	out_qhull.setValue(output);
	output->addChild(chPoints);
	output->addChild(chFaces);
	output->addChild(chNormals);

	//flag
	convexHullFound = 0;

	//clear vectors
	inputPointList.clear();
	convexHullPoints.clear();
	convexHullFaceIndexList.clear();
	convexHullIndexLUT.clear();
	convexHullFaceNormals.clear();
	convexHullVertexNormals.clear();

}

///////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
///////////////////////////////////////////////////////////////////////////
SoQHull3D::~SoQHull3D()
{
  // remove the node sensor.
  if (nodeSensor){ delete nodeSensor; nodeSensor = NULL; }

	//clear vectors
	inputPointList.clear();
	convexHullPoints.clear();
	convexHullFaceIndexList.clear();
	convexHullIndexLUT.clear();
	convexHullFaceNormals.clear();
	convexHullVertexNormals.clear();
}

///////////////////////////////////////////////////////////////////////////
// COLLECT PRIMITIVES
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::collectPrimitives()
{
	callbackAction->apply(this);
}

///////////////////////////////////////////////////////////////////////////
// CALL QHULL (see www.qhull.org for data structures, functions and macros)
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::callQHull()
{
		int dim = 3;  	            // dimension of points
		boolT ismalloc = 1;         // True if qhull should free points in qh_freeqhull() or reallocation
		char flags[]= "qhull Qt ";  // option flags for qhull, see qh_opt.htm
		int exitcode;               // 0 if no error from qhull
		vertexT *vertex, **vertexp; // set by FORALLvertices
		facetT *facet;							// set by FORALLfacets
		facetT *neighbor, **neighborp; // set by FOREACHneighbor_
		int curlong, totlong;	      // memory remaining after qh_memfreeshort
		unsigned int i = 0;

		//for vertex normals
		std::list <SbVec3f> normalList;
		std::list <SbVec3f>::iterator iN;
		normalList.clear();
		SbVec3f tmp_normal = SbVec3f(0, 0, 0);

		//create array of coordinates for each point
		coordT *qhullInput = (coordT *) calloc(inputPointList.size(), sizeof (realT));

		//transfer pointlist to format qhull understands
		for ( i=0; i<inputPointList.size(); i++ ) qhullInput[i] = inputPointList[i];

		// number of points
		int numPoints = (int) inputPointList.size()/dim;

	 	//call qhull for results
		exitcode = qh_new_qhull (dim, numPoints, qhullInput, ismalloc, flags, NULL, stderr);

		// if no error
		if (!exitcode) {

			convexHullFound = 1;

			//triangulate facets of convex hull
			qh_triangulate();

			//call qhull macro to get all POINTS of the convex hull
			FORALLvertices {
				convexHullPoints.push_back(SbVec3f(vertex->point[0], vertex->point[1], vertex->point[2]));
				convexHullIndexLUT.push_back(vertex->id);

				//vertex normals
				FOREACHneighbor_(vertex){
					//get all normals of neighboring facets
					normalList.push_back(SbVec3f(neighbor->normal[0], neighbor->normal[1], neighbor->normal[2]));
				}
				//all normals just once
				normalList.unique();

				//get the average normals as vertex normal
				for ( iN = normalList.begin(); iN != normalList.end(); iN++) tmp_normal += *iN;
				tmp_normal /= normalList.size();
				tmp_normal.normalize();

				//add vertex normal to vector
				convexHullVertexNormals.push_back(tmp_normal);

				//clear tmps
				normalList.clear();
				tmp_normal = SbVec3f(0, 0, 0);
			}

			//index-stuff -> create a lookup table 'lut'
			IndexVector::iterator iL;
			iL = std::max_element(convexHullIndexLUT.begin(), convexHullIndexLUT.end());
			int maxID = *iL;
			int32_t* lut = new int32_t[maxID];
			for (iL = convexHullIndexLUT.begin(), i=0; iL != convexHullIndexLUT.end(); iL++, i++)	lut[*iL] = i;



			//call qhull macro to get all triangulated FACETS of the convex hull
			FORALLfacets {

				//add face normal for actual face to vector
				convexHullFaceNormals.push_back(SbVec3f(facet->normal[0], facet->normal[1], facet->normal[2]));

				//add indices for actual face
				FOREACHvertex_(facet->vertices){
					convexHullFaceIndexList.push_back(lut[vertex->id]);
				}
				//add end index for actual face
				convexHullFaceIndexList.push_back(SO_END_FACE_INDEX);

			}


			//todo: get volume and surface
			qh_getarea (qh facet_list);
			ch_surface.setValue(qh totarea);
			ch_volume.setValue(qh totvol);


			//set state
			state.setValue("convex hull finished");

		} else {
			//no hull :(
			convexHullFound = 0;

			//set statistics to 0
			ch_surface.setValue(0);
			ch_volume.setValue(0);

		}

		//cleanup
		qh_freeqhull(!qh_ALL);
		qh_memfreeshort (&curlong, &totlong);
		if (curlong || totlong)	fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);

}


///////////////////////////////////////////////////////////////////////////
// DRAW POINTS
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::drawPoints()
{
	//clear separator from old stuff
	chPoints->removeAllChildren();

	//enough points?
	if (convexHullPoints.size() >= 1 && showPoints.getValue()){

		int i = 0, numPoints = convexHullPoints.size();
		PointVector::iterator iP;
		SbVec3f* points         = new SbVec3f[numPoints];
		SoVertexProperty *vProp = new SoVertexProperty();
		SoPointSet *pointSet    = new SoPointSet();

		//rewrite points
		for (iP = convexHullPoints.begin(), i=0; iP != convexHullPoints.end(); iP++, i++)	points[i] = *iP;

		//color/material
		if (useColors.getValue()){
			SoMaterial *material   = new SoMaterial();
			material->diffuseColor = pointColor.getValue();
			material->ambientColor = pointColor.getValue();
			chPoints->addChild(material);
		}

		//set up vertex property and point set
		vProp->vertex.setValues(0, numPoints, points);
		pointSet->vertexProperty = vProp;

		//add point set to scene
		chPoints->addChild(pointSet);

		//cleanup
		delete[] points; points = NULL;

	} else {
		if (debug) std::cout << " not enough points to render or input scene disconnected" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////
// DRAW FACES
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::drawFaces()
{
	//clear separator from old stuff
	chFaces->removeAllChildren();

	//can we render a face?
	if (convexHullPoints.size() >= 3  && showFaces.getValue()){

		int	i,
			numPoints        = convexHullPoints.size(),
			numIndices       = convexHullFaceIndexList.size(),
			numFaceNormals   = convexHullFaceNormals.size(),
			numVertexNormals = convexHullVertexNormals.size();

		PointVector::iterator iP;
		IndexVector::iterator iV;

		SoIndexedFaceSet *faceSet = new SoIndexedFaceSet();
		SoVertexProperty *vProp   = new SoVertexProperty();
		SbVec3f *points           = new SbVec3f[numPoints];
		SbVec3f *faceNormals      = new SbVec3f[numFaceNormals];
		SbVec3f *vertexNormals    = new SbVec3f[numVertexNormals];
		int32_t *indices          = new int32_t[numIndices];

		//rewrite points
		for (iP = convexHullPoints.begin(), i=0; iP != convexHullPoints.end(); iP++, i++)	points[i] = *iP;

		//rewrite normals
		for (iP = convexHullFaceNormals.begin(), i=0; iP != convexHullFaceNormals.end(); iP++, i++)	faceNormals[i] = *iP;
		for (iP = convexHullVertexNormals.begin(), i=0; iP != convexHullVertexNormals.end(); iP++, i++)	vertexNormals[i] = *iP;

		//rewrite indices
		for (iV = convexHullFaceIndexList.begin(), i=0; iV != convexHullFaceIndexList.end(); iV++, i++)	indices[i] = *iV;

		//set up vertex property
		vProp->vertex.setValues(0, numPoints, points);

		//set up normals
		if (useFaceNormals.getValue()){
			vProp->normal.setValues(0, numFaceNormals, faceNormals);
			vProp->normalBinding.setValue(SoVertexProperty::PER_FACE);
		} else {
			vProp->normal.setValues(0, numVertexNormals, vertexNormals);
			vProp->normalBinding.setValue(SoVertexProperty::PER_VERTEX_INDEXED);
		}

		//rewrite colors
		if (useColors.getValue()) {

			uint32_t *colors = new uint32_t[numPoints];
			SbColor col;
			SoMFUInt32 colorField;

			for (i=0; i<numPoints; i++){
				 col       = faceColor.getValue();
				 colors[i] = col.getPackedValue(0.0f);
			}

			colorField.setValues(0, numPoints, colors);
			vProp->materialBinding.setValue(SoVertexProperty::PER_VERTEX_INDEXED);
			vProp->orderedRGBA = colorField;

			delete[] colors; colors = NULL;
		}


		//set up face set
		faceSet->vertexProperty = vProp;
		faceSet->coordIndex.setValues(0, numIndices, indices);

		//add face set to scene
		chFaces->addChild(faceSet);

		//cleanup
		delete[] points;        points        = NULL;
		delete[] faceNormals;   faceNormals   = NULL;
		delete[] vertexNormals; vertexNormals = NULL;
		delete[] indices;       indices       = NULL;

	} else {
		if (debug) std::cout << "not enough points to render faces or input scene disconnected" << std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////
// DRAW NORMALS
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::drawNormals()
{
	//clear separator from old stuff
	chNormals->removeAllChildren();

	//can we render a face normal?
	if (convexHullPoints.size() >= 3  && showNormals.getValue()){

		int	i, j,
			numPoints        = convexHullPoints.size(),
			numFaceIndices   = convexHullFaceNormals.size()*3,
			numFaceNormals   = convexHullFaceNormals.size()*2,
			numVertexIndices = convexHullVertexNormals.size()*3,
			numVertexNormals = convexHullVertexNormals.size()*2;

		PointVector::iterator iP, iN;
		IndexVector::iterator iV;

		SoIndexedLineSet *lineSet  = new SoIndexedLineSet();
		SoVertexProperty *vProp    = new SoVertexProperty();
		SbVec3f *points            = new SbVec3f[numPoints];
		int32_t *faceIndices       = new int32_t[numFaceIndices];
		SbVec3f *faceNorm_points   = new SbVec3f[numFaceNormals];
		int32_t *vertexIndices     = new int32_t[numVertexIndices];
		SbVec3f *vertexNorm_points = new SbVec3f[numVertexNormals];
		SbVec3f middle;


		//face normals
		if (useFaceNormals.getValue()){
			//calculate points for normals
			for (iV = convexHullFaceIndexList.begin(), i=0, iN = convexHullFaceNormals.begin(); iV != convexHullFaceIndexList.end(); iV+=4, iN++){
					middle = (convexHullPoints[*iV] + convexHullPoints[*(iV+1)] + convexHullPoints[*(iV+2)])/3;
					faceNorm_points[i]   = middle;
					faceNorm_points[i+1] = middle + *iN;
					i+=2;
			}
			//write face indices
			for (i=0, j=0; i<numFaceIndices; i+=3, j++){
				faceIndices[i]   = i-j;
				faceIndices[i+1] = i-j+1;
				faceIndices[i+2] = SO_END_LINE_INDEX;
			}

			//set up vertex property
			vProp->vertex.setValues(0, numFaceNormals, faceNorm_points);
			//set up line set
			lineSet->coordIndex.setValues(0, numFaceIndices, faceIndices);



		//vertex normals
		} else {
			//rewrite points
			for (iP = convexHullPoints.begin(), iN = convexHullVertexNormals.begin(), i=0; iP != convexHullPoints.end(); iP++, i+=2, iN++){
				vertexNorm_points[i]   = *iP;
				vertexNorm_points[i+1] = (*iP)+(*iN);
			}
			//write vertex indices
			for (i=0, j=0; i<numVertexIndices; i+=3, j++){
				vertexIndices[i]   = i-j;
				vertexIndices[i+1] = i-j+1;
				vertexIndices[i+2] = SO_END_LINE_INDEX;
			}

			//set up vertex property
			vProp->vertex.setValues(0, numVertexNormals, vertexNorm_points);
			//set up line set
			lineSet->coordIndex.setValues(0, numVertexIndices, vertexIndices);
		}


		//add vertex property and line set to scene
		lineSet->vertexProperty = vProp;
		chNormals->addChild(lineSet);

		//cleanup
		delete[] points;            points            = NULL;
		delete[] faceIndices;       faceIndices       = NULL;
		delete[] vertexIndices;     vertexIndices     = NULL;
		delete[] faceNorm_points;   faceNorm_points   = NULL;
		delete[] vertexNorm_points; vertexNorm_points = NULL;

	} else {
		if (debug) std::cout << "not enough points to render normals or input scene disconnected" << std::endl;
	}

}

//CALLBACK FUNCTIONS

///////////////////////////////////////////////////////////////////////////
// PRE CALLBACK
///////////////////////////////////////////////////////////////////////////
SoCallbackAction::Response SoQHull3D::preCB(void *userData, SoCallbackAction *action, const SoNode*)
{
    SoQHull3D *helpMe = (SoQHull3D*) userData;

    // set world transformation matrix in object to
    // transform collected vertices correctly
    helpMe->transMatrix = action->getModelMatrix();

    return SoCallbackAction::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////
// POINT CALLBACK
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::pointCB(void *userData, SoCallbackAction *action, const SoPrimitiveVertex *v)
{
		SoQHull3D *helpMe = (SoQHull3D*)userData;

		// get the position of the point
		SbVec3f p(v->getPoint());

		//apply transformation matrix
		helpMe->transMatrix.multVecMatrix(p,p);

		//add to vector for QHull
		inputPointList.push_back(p[0]);
		inputPointList.push_back(p[1]);
		inputPointList.push_back(p[2]);

}


///////////////////////////////////////////////////////////////////////////
// LINE SEGMENT CALLBACK
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::lineSegmentCB(void *userData, SoCallbackAction *action, const SoPrimitiveVertex *v1, const SoPrimitiveVertex *v2)
{

		SoQHull3D *helpMe = (SoQHull3D*)userData;

		// get the position of the point
		SbVec3f p1(v1->getPoint());
		SbVec3f p2(v2->getPoint());

		//apply transformation matrix
		helpMe->transMatrix.multVecMatrix(p1,p1);
		helpMe->transMatrix.multVecMatrix(p2,p2);

		//add to vector for QHull
		//QHull automaticly removes duplicates - so there is
		//no need to ged rid of them
		inputPointList.push_back(p1[0]);
		inputPointList.push_back(p1[1]);
		inputPointList.push_back(p1[2]);

		inputPointList.push_back(p2[0]);
		inputPointList.push_back(p2[1]);
		inputPointList.push_back(p2[2]);

}

///////////////////////////////////////////////////////////////////////////
// TRIANGLE CALLBACK
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::triangleCB(void *userData,
													 SoCallbackAction *action,
													 const SoPrimitiveVertex *v1,
													 const SoPrimitiveVertex *v2,
													 const SoPrimitiveVertex *v3)
{

    SoQHull3D *helpMe = (SoQHull3D*)userData;

	  // get the positions of the primitive vertices
    SbVec3f p1(v1->getPoint());
    SbVec3f p2(v2->getPoint());
    SbVec3f p3(v3->getPoint());

		//apply transformation matrix
    helpMe->transMatrix.multVecMatrix(p1,p1);
    helpMe->transMatrix.multVecMatrix(p2,p2);
    helpMe->transMatrix.multVecMatrix(p3,p3);

		//add to vector for QHull
		//QHull automaticly removes duplicates - so there is
		//no need to ged rid of them
		inputPointList.push_back(p1[0]);
		inputPointList.push_back(p1[1]);
		inputPointList.push_back(p1[2]);

		inputPointList.push_back(p2[0]);
		inputPointList.push_back(p2[1]);
		inputPointList.push_back(p2[2]);

		inputPointList.push_back(p3[0]);
		inputPointList.push_back(p3[1]);
		inputPointList.push_back(p3[2]);
}



///////////////////////////////////////////////////////////////////////////
// NODE CHANGED CALLBACK
///////////////////////////////////////////////////////////////////////////
void SoQHull3D::nodeChangedCB(void *data, SoSensor* sens)
{
  ((SoQHull3D*) data)->nodeChanged((SoNodeSensor*)sens);
}

void SoQHull3D::nodeChanged(SoNodeSensor* sensor)
{

	//get the node which caused the notification.
  SoField *field = sensor->getTriggerField();

	//get the node which caused the notification.
  SoNode *node = sensor->getTriggerNode();


	if (field == 0 && node != NULL){

		chPoints->removeAllChildren();
		chFaces->removeAllChildren();
		chNormals->removeAllChildren();

		if ( getNumChildren() >= 1 ){

			//clear vectors
			inputPointList.clear();
			convexHullPoints.clear();
			convexHullFaceIndexList.clear();
			convexHullIndexLUT.clear();
			convexHullFaceNormals.clear();
			convexHullVertexNormals.clear();

			//collect all point values from input scene
			state.setValue("collecting primitives");
			collectPrimitives(); //get points and stuff ...

			//send points to QHull to compute the convex hull
			state.setValue("computing convex hull");
			callQHull();

			//do we have a hull?
			if (convexHullFound){

				//set statistical information
				numInput.setValue(inputPointList.size()/3);
				numOutput.setValue(convexHullPoints.size());
				numTriangles.setValue(convexHullFaceIndexList.size()/4);

				//call draw functions
				drawPoints();
				drawFaces();
				drawNormals();


			//no hull was found
			} else {
				state.setValue("no convex hull found");
				if (debug) std::cout << " no convex hull found" << std::endl;
			}

		//no input
		} else {
			if (getNumChildren() < 1) state.setValue("no input");
				numInput.setValue(0);
				numOutput.setValue(0);
				numTriangles.setValue(0);
				ch_surface.setValue(0);
				ch_volume.setValue(0);
		}
	}

	//handle changed fields
	if (field == &showPoints || field == &pointColor){
		drawPoints();
	}

	if (field == &showFaces || field == &faceColor){
		drawFaces();
	}

	if (field == &useColors){
		drawPoints();
		drawFaces();
	}

	//'hidden' fields in automatic panel
	if (field == &showNormals){
		drawNormals();
	}
	if (field == &useFaceNormals){
		drawFaces();
		drawNormals();
	}
}

