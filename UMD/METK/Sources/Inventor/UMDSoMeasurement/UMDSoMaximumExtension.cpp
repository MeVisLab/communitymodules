// #############################
// ####
// ####  UMDSoMaximumExtension.cpp
// ####
// #############################


#include <Inventor/errors/SoDebugError.h>
#include "UMDSoDistanceLine.h"
#include "UMDSoShapeToPointSet.h"
#include "UMDSoMaximumExtension.h"
#include "SoQHull3D.h"
#include <stdio.h>

#ifdef WIN32
  #define MyMessageBox(A, B, C, D) MessageBox(A, B, C, D) 
  #define My_MB_OK                 MB_OK
#else
  #define MyMessageBox(A, B, C, D) SoDebugError::post("File %s, line %d: %d %s %s", __FILE__, __LINE__, A, B, C );
  #define My_MB_OK
#endif

// Define a local MyMin instead of min, because there we have platform dependent collisions in headers.
#define MyMin(A,B) ((A)<(B) ? (A) : (B))


SO_NODE_SOURCE(UMDSoMaximumExtension);

void UMDSoMaximumExtension::initClass() { 
	// muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
	SO_NODE_INIT_CLASS(UMDSoMaximumExtension, SoSeparator, "Separator");
}


UMDSoMaximumExtension::UMDSoMaximumExtension() {
	SO_NODE_CONSTRUCTOR(UMDSoMaximumExtension);

	// extensions
	SO_NODE_ADD_FIELD(diameter, (0.0f));
	SO_NODE_ADD_FIELD(depth, (0.0f));
	SO_NODE_ADD_FIELD(width, (0.0f));

	// extension points
	SO_NODE_ADD_FIELD(pDiameter_1, (0, 0, 0));
	SO_NODE_ADD_FIELD(pDiameter_2, (0, 0, 0));
	SO_NODE_ADD_FIELD(pDepth_1, (0, 0, 0));
	SO_NODE_ADD_FIELD(pDepth_2, (0, 0, 0));
	SO_NODE_ADD_FIELD(pWidth_1, (0, 0, 0));
	SO_NODE_ADD_FIELD(pWidth_2, (0, 0, 0));

	SO_NODE_ADD_FIELD(inObjectNode, (NULL));

	// display mode
	SO_NODE_ADD_FIELD(displayInput, (TRUE));
	SO_NODE_ADD_FIELD(colBBox, (1,1,1));
	// bounding box display mode
	SO_NODE_ADD_FIELD(displayBBox, (TRUE));
	SO_NODE_ADD_FIELD(lineWidthBBox, (1));
	// result display mode
	SO_NODE_ADD_FIELD(displayMainAxes, (TRUE));
	SO_NODE_ADD_FIELD(displayDistanceLine, (FALSE));
	SO_NODE_ADD_FIELD(largestValueOnly, (FALSE));
	SO_NODE_ADD_FIELD(displayUnit, (TRUE));

	// choice of the distance line unit
	SO_NODE_ADD_FIELD(unit,(mm));
	SO_NODE_DEFINE_ENUM_VALUE(eUnit, mm);
	SO_NODE_DEFINE_ENUM_VALUE(eUnit, cm);
	SO_NODE_DEFINE_ENUM_VALUE(eUnit, m);
	SO_NODE_SET_SF_ENUM_TYPE(unit, eUnit);

	group = new SoGroup();
	group->ref();

	_shapeToPointSet = new UMDSoShapeToPointSet();
	_convexHull = new SoQHull3D();
	_convexHull->ref();

	// die Feldsensoren
	_computeMaximumExtensionSens = new SoFieldSensor(computeMaximumExtensionCB, this);
	_computeMaximumExtensionSens->attach(&inObjectNode);
	_displayDistLineSens = new SoFieldSensor(displayChanged, this);
	_displayDistLineSens->attach(&displayDistanceLine);
	_displayInputSens = new SoFieldSensor(displayChanged, this);
	_displayInputSens->attach(&displayInput);
	_displayUnitSens = new SoFieldSensor(displayChanged, this);
	_displayUnitSens->attach(&displayUnit);
	_displayMainAxes = new SoFieldSensor(displayChanged, this);
	_displayMainAxes->attach(&displayMainAxes);
	_displayLargValueOnly = new SoFieldSensor(displayChanged, this);
	_displayLargValueOnly->attach(&largestValueOnly);
	_displayBBox = new SoFieldSensor(displayChanged, this);
	_displayBBox->attach(&displayBBox);
}

UMDSoMaximumExtension::~UMDSoMaximumExtension() {
	// transformierte punktemengen
	if (pointSet) { delete[] pointSet; pointSet = NULL; }
	if (pointSet2) { delete[] pointSet2; pointSet2 = NULL; }
	if (pointSet3) { delete[] pointSet3; pointSet3 = NULL; }
	if (pointSet4) { delete[] pointSet4; pointSet4 = NULL; }
	if (_computeMaximumExtensionSens) { delete _computeMaximumExtensionSens; _computeMaximumExtensionSens = NULL; }

	_convexHull->unref();
	group->unref();
}
void UMDSoMaximumExtension::computeMaximumExtensionCB(void* userData, SoSensor*) {
	UMDSoMaximumExtension* obj = (UMDSoMaximumExtension*) userData;
	obj->_computeMaximumExtension();
}
void UMDSoMaximumExtension::displayChanged(void* userData, SoSensor*) {
  UMDSoMaximumExtension* obj = (UMDSoMaximumExtension*) userData;
  if (obj->inObjectNode.getValue() != NULL){
		obj->display();
  }
}
bool UMDSoMaximumExtension::_computeMaximumExtension() {
	clear();
	if (inObjectNode.getValue() != NULL) {
		int size=0;

		UMDSoShapeToPointSet* shapeToPointSet = new UMDSoShapeToPointSet;
		shapeToPointSet->ref();

		shapeToPointSet->rootInput.setValue(inObjectNode.getValue());
		shapeToPointSet->calculate();

		// convex hull of the input point set
		_convexHull->removeAllChildren();
		_convexHull->addChild(inObjectNode.getValue());
		_convexHull->showPoints.setValue(TRUE);
		_convexHull->touch();

		group->removeAllChildren();
		group->addChild(_convexHull->out_qhull.getValue());
		getPointSet(group,pointSet,size);
		// sind Punkte vorhanden, tue folgendes
		if(size>0){
			float fTmp = 0.0;
			fTmp = computeMaxExtensionBruteForce(pointSet, size);
			SizeOfPointSet.setValue(size);
			diameter.setValue(fTmp);
			computeWidthExtension(/*group, */size);	
			display();
			return true;
		}
		shapeToPointSet->unref();
	}else{
		clear();
		SizeOfPointSet.setValue(0);
		diameter.setValue(0.0);
		depth.setValue(0.0);
		width.setValue(0.0);
		pDiameter_1.setValue(0.0f,0.0f,0.0f);
		pDiameter_2.setValue(0.0f,0.0f,0.0f);
		pDepth_1.setValue(0.0f,0.0f,0.0f);
		pDepth_2.setValue(0.0f,0.0f,0.0f);
		pWidth_1.setValue(0.0f,0.0f,0.0f);
		pWidth_2.setValue(0.0f,0.0f,0.0f);
		vWidthPt1_ortho.setValue(0.0f,0.0f,0.0f);
		vWidthPt2_ortho.setValue(0.0f,0.0f,0.0f);
		vWidthPt3_ortho.setValue(0.0f,0.0f,0.0f);
		vWidthPt4_ortho.setValue(0.0f,0.0f,0.0f);
	}
	return false;
}
/*
**	Berechnet das Punktepaar mit maximalen Durchmesser-> Brute Force Algorithmus O(n^2)
*/
float UMDSoMaximumExtension::computeMaxExtensionBruteForce(float* p_fPointSet, int lSize){
	// berechnet den abstand zwischen allen konvexen punkten und gibt den größten zurück
	float fMaxExtension = -1.0;
	if(lSize>0){
		float tmp = 0.0;
		float point_i[3], point_j[3];
		for(int i=0; i<=3*lSize-2; i+=3){
			point_i[0] = p_fPointSet[i];
			point_i[1] = p_fPointSet[i+1];
			point_i[2] = p_fPointSet[i+2];
			for(int j=i+3; j<=3*lSize-2; j+=3){		
				point_j[0] = p_fPointSet[j];
				point_j[1] = p_fPointSet[j+1];
				point_j[2] = p_fPointSet[j+2];
				tmp = computeDistance(point_i, point_j);				
				if(tmp>fMaxExtension){
					fMaxExtension = tmp;
					pDiameter_1.setValue(point_i);
					pDiameter_2.setValue(point_j);
				}
			}
		}
	}
	else{
		fMaxExtension = 0.0;
		float defaultPoint[3];
		defaultPoint[0]=0.0;
		defaultPoint[1]=0.0;
		defaultPoint[2]=0.0;
		clear();
		SizeOfPointSet.setValue(0);
		diameter.setValue(0.0);
		depth.setValue(0.0);
		width.setValue(0.0);
		pDiameter_1.setValue(defaultPoint);
		pDiameter_2.setValue(defaultPoint);
		pDepth_1.setValue(defaultPoint);
		pDepth_2.setValue(defaultPoint);
		pWidth_1.setValue(defaultPoint);
		pWidth_2.setValue(defaultPoint);

		MyMessageBox (NULL, L"No Points available for computing the maximum extension!", L"UMDSoMaximumExtension::computeMaxExtensionBruteForce", My_MB_OK);
	}
	//display();
	return fMaxExtension;
}

void UMDSoMaximumExtension::display() {
	insertMainAxes();
	removeAllChildren();
	// display input
	if(displayInput.getValue()==TRUE){
		addChild(inObjectNode.getValue());
	}
	if(displayDistanceLine.getValue()==TRUE){
		insertExtentPoints();
	}
	if(displayMainAxes.getValue()==TRUE){
		insertMainAxes();
	}
	if(largestValueOnly.getValue()==TRUE){
		UMDSoDistanceLine* _largestLine;
		_largestLine = insertDistanceLine();
		_largestLine->endPos.setValue(pDiameter_2.getValue());
		_largestLine->startPos.setValue(pDiameter_1.getValue());
		_largestLine->color.setValue(1,0,0);
		addChild(_largestLine);
	}
	if(displayBBox.getValue()==TRUE){
		// Knoten zum einstellen der Liniendicke der BoundingBox
		SoDrawStyle* drawStyle = new SoDrawStyle;
		addChild(drawStyle);
		drawStyle->lineWidth.connectFrom(&lineWidthBBox);

		 // Farbe der BoundingBox
		SoMaterial* colorBBox = new SoMaterial;
		addChild(colorBBox);
		colorBBox->diffuseColor.connectFrom(&colBBox);

		insertBBox();
	}
}

void UMDSoMaximumExtension::clear() {
	removeAllChildren();
}

UMDSoDistanceLine* UMDSoMaximumExtension::insertDistanceLine() { 
	UMDSoDistanceLine *line = new UMDSoDistanceLine;
	line->ref();
	line->unitFlag.connectFrom(&displayUnit);
	line->unit.connectFrom(&unit);
	line->minDistance.setValue(10000);
	line->unrefNoDelete();
	return line;
} //insertDistanceLine

void UMDSoMaximumExtension::getPointSet(SoGroup* inputGroup, float* &pointSet, int &size) {
  
  // Punktmenge für beide Eingänge einlesen
  if (inputGroup->getNumChildren() != 0) {
    UMDSoShapeToPointSet* shapeToPointSet = new UMDSoShapeToPointSet;
    shapeToPointSet->ref();
    shapeToPointSet->rootInput.setValue(inputGroup);
    shapeToPointSet->calculate();
    size = shapeToPointSet->vertexProperty->vertex.getNum();
    if (size != 0) {
      pointSet = new float[size * 3];
      int counter2 = 0;
      const SbVec3f* pSbVec3f = shapeToPointSet->vertexProperty->vertex.getValues(0);
      for (int counter = 0; counter < size; counter++) {
        pointSet[counter2++] = pSbVec3f->getValue()[0];
        pointSet[counter2++] = pSbVec3f->getValue()[1];
        pointSet[counter2++] = pSbVec3f->getValue()[2];
        pSbVec3f++;
      }
    }
    shapeToPointSet->unref();
  }
}
/*
**	Berechnet den euklidischen Abstand zwischen zwei Punkten
*/
float UMDSoMaximumExtension::computeDistance(float* p_fPoint1, float* p_fPoint2){
	float fDistance = -1.0;

	if(p_fPoint1 != p_fPoint2){
		fDistance = sqrtf(powf(p_fPoint1[0]-p_fPoint2[0],2)+powf(p_fPoint1[1]-p_fPoint2[1],2)+powf(p_fPoint1[2]-p_fPoint2[2],2));
	}
	else{
		fDistance = 0.0;
		MyMessageBox (NULL, L"Points are identical, there is no distance between them!", L"UMDSoMaximumExtension::computeDistance", My_MB_OK);
	}
	return fDistance;
}
float UMDSoMaximumExtension::computeDistance(SbVec3f vec1, SbVec3f vec2){
	float fDistance = -1.0;

	if(vec1 != vec2){
		fDistance = sqrtf(powf(vec1[0]-vec2[0],2)+powf(vec1[1]-vec2[1],2)+powf(vec1[2]-vec2[2],2));
	}
	else{
		fDistance = 0.0;
		MyMessageBox (NULL, L"Points are identical, there is no distance between them!", L"UMDSoMaximumExtension::computeDistance", My_MB_OK);
	}
	return fDistance;
}
/*
** Transformiert die maximalen Punkte auf die z-Achse, dann auf y-z-Ebene um die WidthPoints zu berechnen 
*/
void UMDSoMaximumExtension::computeWidthExtension(int &size){
	
	// Zunächst wird der dem Koordinatenursprung am nächsten liegende Punkt (einer der beiden maximalen Punkte) bestimmt
	float fDist_1=0;
	float fDist_2=0;

	SbVec3f vec0(0, 0, 0);
	float pointOrigin[3];
	pointOrigin[0]=vec0.getValue()[0];
	pointOrigin[1]=vec0.getValue()[1];
	pointOrigin[2]=vec0.getValue()[2];

	float point_1[3], point_2[3];
	point_1[0] = pDiameter_1.getValue()[0];
	point_1[1] = pDiameter_1.getValue()[1];
	point_1[2] = pDiameter_1.getValue()[2];
	point_2[0] = pDiameter_2.getValue()[0];
	point_2[1] = pDiameter_2.getValue()[1];
	point_2[2] = pDiameter_2.getValue()[2];
	
	// Abstand der maximalen Punkte zum Koordinatenursprung
	fDist_1= computeDistance(pointOrigin, point_1);
	fDist_2= computeDistance(pointOrigin, point_2);

	SbMatrix _sbMatrix;	// Transformationsmatrix

	// suche nach diameter punkt, welcher am nächsten zum Koordinatenursprung liegt
	// dieser wird in den Koordinatenursprung transliert, alle anderen werden entsprechend 
	// mit transliert
	if(MyMin(fDist_1,fDist_2)==fDist_1){
		SbVec3f translateVec3f(-1*point_1[0],-1*point_1[1],-1*point_1[2]);
		// Bestimmen des Translationsvektors
		_sbMatrix.setTranslate(translateVec3f);
	}
	else if(MyMin(fDist_1,fDist_2)==fDist_2){
		SbVec3f translateVec3f(-1*point_2[0],-1*point_2[1],-1*point_2[2]);
		// Bestimmen des Translationsvektors
		_sbMatrix.setTranslate(translateVec3f);
	}
	if(size>0){
		int cnt=0;
		// Translation
		pointSet2 = new float[size * 3]; //translierte punktemenge
		SbVec3f tmp, tmp2, tmpExtPoint1, tmpExtPoint2, ExtPoint;
		for(int i=0; i<3*SizeOfPointSet.getValue()-2;i+=3){
			tmp[0]=pointSet[i];
			tmp[1]=pointSet[i+1];
			tmp[2]=pointSet[i+2];
			_sbMatrix.multVecMatrix(tmp, tmp2);
			pointSet2[i]=tmp2[0];
			pointSet2[i+1]=tmp2[1];
			pointSet2[i+2]=tmp2[2];
			if((tmp.getValue()[0]==pDiameter_1.getValue()[0]) & (tmp.getValue()[1]==pDiameter_1.getValue()[1]) & (tmp.getValue()[2]==pDiameter_1.getValue()[2])){
				tmpExtPoint1.setValue(tmp2[0],tmp2[1],tmp2[2]);
			}
			else if((tmp.getValue()[0]==pDiameter_2.getValue()[0]) & (tmp.getValue()[1]==pDiameter_2.getValue()[1]) & (tmp.getValue()[2]==pDiameter_2.getValue()[2])){
				tmpExtPoint2.setValue(tmp2[0],tmp2[1],tmp2[2]);
			}
			cnt++;
		}		
		SbMatrix _sbMatrixRotZ, _sbMatrixRotYFace;
		SbRotation sbRotate_1, sbRotate_2; 
		if(tmpExtPoint1==SbVec3f(0.0f,0.0f,0.0f)){
			ExtPoint=tmpExtPoint2;
			// for the orientation
			SoSFVec3f tmp1;
			tmp1.setValue(pDiameter_1.getValue());
			pDiameter_1.setValue(pDiameter_2.getValue());
			pDiameter_2.setValue(tmp1.getValue());
		}
		else if(tmpExtPoint2==SbVec3f(0.0f,0.0f,0.0f)){
			ExtPoint=tmpExtPoint1;
		}
		SbVec3f vecObjectAxis(ExtPoint.getValue()[0], ExtPoint.getValue()[1], ExtPoint.getValue()[2]);
		SbVec3f vecYAxis(0.0f,0.0f,0.0f);
		SbVec3f vecZAxis(0.0f,0.0f,1.0f);
		SbVec3f vecWidthAxis;
		
		// Rotation auf z-Achse
		sbRotate_1.setValue(vecObjectAxis, vecZAxis);
		_sbMatrixRotZ.setRotate(sbRotate_1);
		pointSet3 = new float[size * 3]; //translierte punktemenge
		SbVec3f tmp3(0.0f,0.0f,0.0f), tmp4(0.0f,0.0f,0.0f);
		int idx=0; // speichert index des am weitesten entfernten punktes von aktueller objektachse
		float fHessianPointDist=0;
		float fHessianPointDistTmp;
		for(int i=0; i<3*SizeOfPointSet.getValue()-2;i+=3){
			fHessianPointDistTmp=0;
			tmp3[0]=pointSet2[i];
			tmp3[1]=pointSet2[i+1];
			tmp3[2]=pointSet2[i+2];
			_sbMatrixRotZ.multVecMatrix(tmp3,tmp4);
			pointSet3[i]=tmp4[0];
			pointSet3[i+1]=tmp4[1];
			pointSet3[i+2]=tmp4[2];
			fHessianPointDistTmp = computeHessianPointLength(SbVec3f(0.0f, 0.0f, 0.0f),vecZAxis,tmp4[0],tmp4[1],tmp4[2]);
			if(fHessianPointDistTmp > fHessianPointDist){
				fHessianPointDist = fHessianPointDistTmp;
				idx=i;
			}
		}
		vecWidthAxis.setValue(pointSet3[idx], pointSet3[idx+1], 0);
		bool bYPos;
		if(vecWidthAxis[1]>=0){
			vecYAxis[0]=0.0f;
			vecYAxis[1]=1.0f;
			vecYAxis[2]=0.0f;
			bYPos=true;
		}
		else{
			vecYAxis[0]=0.0f;
			vecYAxis[1]=-1.0f;
			vecYAxis[2]=0.0f;
			bYPos=false;
		}

		//Rotation auf y-z-Ebene
		sbRotate_2.setValue(vecWidthAxis, vecYAxis);
		_sbMatrixRotYFace.setRotate(sbRotate_2);
		pointSet4 = new float[size * 3]; //translierte punktemenge
		SbVec3f tmp5(0.0f,0.0f,0.0f), tmp6(0.0f,0.0f,0.0f);
		for(int i=0; i<3*SizeOfPointSet.getValue()-2;i+=3){
			tmp5[0]=pointSet3[i];
			tmp5[1]=pointSet3[i+1];
			tmp5[2]=pointSet3[i+2];
			_sbMatrixRotYFace.multVecMatrix(tmp5,tmp6);
			pointSet4[i]=tmp6[0];
			pointSet4[i+1]=tmp6[1];
			pointSet4[i+2]=tmp6[2];
		}
		//suche nach Punkten, die die Breite und Tiefe der Punktewolke bestimmen (y-und z-Koordinaten suchen)
		float fWidthExtension,fWidthExtension2;
		float fMaxX=pointSet4[0];
		float fMinX=pointSet4[0];
		int maxIdx_x=0;
		int minIdx_x=0;
		float fMaxY=pointSet4[1];
		float fMinY=pointSet4[1];
		int maxIdx_y=0;
		int minIdx_y=0;
		
		for(int j=0;j<3*SizeOfPointSet.getValue()-2; j+=3){
			if(pointSet4[j]>=fMaxX){
				fMaxX=pointSet4[j];
				maxIdx_x=j;
			}
			else if(pointSet4[j]<=fMinX){
				fMinX=pointSet4[j];
				minIdx_x=j;
			}
			if(pointSet4[j+1]>=fMaxY){
				fMaxY=pointSet4[j+1];
				maxIdx_y=j;
			}
			else if(pointSet4[j+1]<=fMinY){
				fMinY=pointSet4[j+1];
				minIdx_y=j;
			}
		}
		// Index  idx in pointSet4, entspricht gleichem Punkt in pointSet an der Stelle idx
		
		// Distanz zwischen minimalen und maximalen Punkt (entlang der x und y-Achse)
		fWidthExtension = abs(fMaxY-fMinY);
		fWidthExtension2 = abs(fMaxX-fMinX);

		// ungerade Widthpunkte liegen am weitesten von der Hauptachse (Diameter Achse)
		pDepth_1.setValue(pointSet[maxIdx_y], pointSet[maxIdx_y+1], pointSet[maxIdx_y+2]);
		pDepth_2.setValue(pointSet[minIdx_y], pointSet[minIdx_y+1], pointSet[minIdx_y+2]);
		pWidth_1.setValue(pointSet[maxIdx_x], pointSet[maxIdx_x+1], pointSet[maxIdx_x+2]);
		pWidth_2.setValue(pointSet[minIdx_x], pointSet[minIdx_x+1], pointSet[minIdx_x+2]);
		fDistPt1tooZAxis=abs(fMaxY);// zur Skalierung der 1. Width Axis
		fDistPt2tooZAxis=abs(fMinY);// zur Skalierung der 1. Width Axis
		fDistPt3tooZAxis=abs(fMaxX);// zur Skalierung der 2. Width Axis
		fDistPt4tooZAxis=abs(fMinX);// zur Skalierung der 2. Width Axis
		depth.setValue(fWidthExtension);
		width.setValue(fWidthExtension2);
	}
}
// berechnet den Punkt auf der Objektachse, zum dem der übergebene Punkt orthogonal ist
SbVec3f UMDSoMaximumExtension::computeHessianPoint(SbVec3f startPos, SbVec3f endPos, float x, float y, float z){
	
	// Line-Vector between pDiameter_1 and pDiameter_2 -> Objektachse
	float a[3];
	a[0]=startPos[0]-endPos[0];
	a[1]=startPos[1]-endPos[1];
	a[2]=startPos[2]-endPos[2];
	
	// beliebiger Punkt von der Objektachse (hier Startpunkt)
	float p0[3];
	p0[0]=startPos[0];
	p0[1]=startPos[1];
	p0[2]=startPos[2];

	float hessianPoint[3];
	// vector between the current point and the chosen point p0 -->(hessianPoint-p0)
	float q_p0[3];
	// vector hessianPoint-p0 multiplied with the direction vector of the distance line
	float q_p0_a=0.0;
	float aPow=0.0;
	aPow=pow(a[0],2)+pow(a[1],2)+pow(a[2],2);

	q_p0[0]=x-p0[0];
	q_p0[1]=y-p0[1];
	q_p0[2]=z-p0[2];

	q_p0_a=q_p0[0]*a[0]+q_p0[1]*a[1]+q_p0[2]*a[2];	

	// projected (longitudinal) point q on the distance line
	// F=p0+(a*(q_p0*a)/|a|) <-- hessian form
	hessianPoint[0]=p0[0]+(a[0]*(q_p0_a/aPow));
	hessianPoint[1]=p0[1]+(a[1]*(q_p0_a/aPow));
	hessianPoint[2]=p0[2]+(a[2]*(q_p0_a/aPow));

	return SbVec3f(hessianPoint[0], hessianPoint[1], hessianPoint[2]);
}
// berechnet den kürzesten Abstand eines Punktes zur Objektachse
float UMDSoMaximumExtension::computeHessianPointLength(SbVec3f startPos, SbVec3f endPos, float x, float y, float z){
	
	float fDistFromInputLine; // defined by startPos and endPos

	// Line-Vector between pDiameter_1 and pDiameter_2 -> Objektachse
	float a[3];
	a[0]=startPos[0]-endPos[0];
	a[1]=startPos[1]-endPos[1];
	a[2]=startPos[2]-endPos[2];
	
	// beliebiger Punkt von der Objektachse (hier Startpunkt)
	float p0[3];
	p0[0]=startPos[0];
	p0[1]=startPos[1];
	p0[2]=startPos[2];

	float hessianPoint[3];
	// vector between the current point and the chosen point p0 -->(hessianPoint-p0)
	float q_p0[3];
	// vector hessianPoint-p0 multiplied with the direction vector of the distance line
	float q_p0_a=0.0;
	float aPow=0.0;
	aPow=pow(a[0],2)+pow(a[1],2)+pow(a[2],2);

	q_p0[0]=x-p0[0];
	q_p0[1]=y-p0[1];
	q_p0[2]=z-p0[2];

	q_p0_a=q_p0[0]*a[0]+q_p0[1]*a[1]+q_p0[2]*a[2];	

	// projected (longitudinal) point q on the distance line
	// F=p0+(a*(q_p0*a)/|a|) <-- hessian form
	hessianPoint[0]=p0[0]+(a[0]*(q_p0_a/aPow));
	hessianPoint[1]=p0[1]+(a[1]*(q_p0_a/aPow));
	hessianPoint[2]=p0[2]+(a[2]*(q_p0_a/aPow));

	fDistFromInputLine = sqrt(	pow((hessianPoint[0]-x),2)+
								pow((hessianPoint[1]-y),2)+
								pow((hessianPoint[2]-z),2));

	return fDistFromInputLine;
}
void UMDSoMaximumExtension::insertExtentPoints(){
	// Die maximale Ausdehung wird als Linie zwischen den maxima Punkten dargestellt = Objektachse/Diameter
	_distanceLine = insertDistanceLine();
	_distanceLine->endPos.setValue(pDiameter_2.getValue());
	_distanceLine->startPos.setValue(pDiameter_1.getValue());
	_distanceLine->color.setValue(1,0,0);
	addChild(_distanceLine);

	// Ortohogonale Linie vom ersten WidthPoint zur Objektachse
	UMDSoDistanceLine* _distanceLineWidth1;
	_distanceLineWidth1 = insertDistanceLine();
	SbVec3f hessianPoint1; 
	// HessianPoint ist der orthogonal projezierter WidthPoint auf Objektachse
	hessianPoint1 = computeHessianPoint(	pDiameter_1.getValue(),
											pDiameter_2.getValue(),
											pDepth_1.getValue()[0], 
											pDepth_1.getValue()[1], 
											pDepth_1.getValue()[2]);
	vWidthPt1_ortho.setValue(hessianPoint1[0], hessianPoint1[1], hessianPoint1[2]);
	_distanceLineWidth1->endPos.setValue(vWidthPt1_ortho.getValue());
	_distanceLineWidth1->startPos.setValue(pDepth_1.getValue());
	_distanceLineWidth1->color.setValue(0,1,0);
	addChild(_distanceLineWidth1);

	// Ortohogonale Linie vom zweiten WidthPoint zur Objektachse
	UMDSoDistanceLine* _distanceLineWidth2;
	_distanceLineWidth2 = insertDistanceLine();
	SbVec3f hessianPoint2;
	// HessianPoint ist der orthogonal projezierte WidthPoint auf Objektachse
	hessianPoint2 = computeHessianPoint(	pDiameter_1.getValue(),
											pDiameter_2.getValue(),
											pDepth_2.getValue()[0], 
											pDepth_2.getValue()[1], 
											pDepth_2.getValue()[2]);
	vWidthPt2_ortho.setValue(hessianPoint2[0], hessianPoint2[1], hessianPoint2[2]);
	_distanceLineWidth2->endPos.setValue(vWidthPt2_ortho.getValue());
	_distanceLineWidth2->startPos.setValue(pDepth_2.getValue());
	_distanceLineWidth2->color.setValue(0,1,0);
	addChild(_distanceLineWidth2);

	// Ortohogonale Linie vom dritten WidthPoint zur Objektachse
	UMDSoDistanceLine* _distanceLineWidth3;
	_distanceLineWidth3 = insertDistanceLine();
	SbVec3f hessianPoint3; 
	//HessianPoint ist der orthogonal projezierter WidthPoint auf Objektachse
	hessianPoint3 = computeHessianPoint(	pDiameter_1.getValue(),
											pDiameter_2.getValue(),
											pWidth_1.getValue()[0], 
											pWidth_1.getValue()[1], 
											pWidth_1.getValue()[2]);
	vWidthPt3_ortho.setValue(hessianPoint3[0], hessianPoint3[1], hessianPoint3[2]);
	_distanceLineWidth3->endPos.setValue(vWidthPt3_ortho.getValue());
	_distanceLineWidth3->startPos.setValue(pWidth_1.getValue());
	_distanceLineWidth3->color.setValue(0,0,1);
	//_distanceLineWidth1->enableText = FALSE;
	addChild(_distanceLineWidth3);

	UMDSoDistanceLine* _distanceLineWidth4;
	_distanceLineWidth4 = insertDistanceLine();
	SbVec3f hessianPoint4;
	// HessianPoint ist der orthogonal projezierte WidthPoint auf Objektachse
	hessianPoint4 = computeHessianPoint(	pDiameter_1.getValue(),
											pDiameter_2.getValue(),
											pWidth_2.getValue()[0], 
											pWidth_2.getValue()[1], 
											pWidth_2.getValue()[2]);
	vWidthPt4_ortho.setValue(hessianPoint4[0], hessianPoint4[1], hessianPoint4[2]);
	_distanceLineWidth4->endPos.setValue(vWidthPt4_ortho.getValue());
	_distanceLineWidth4->startPos.setValue(pWidth_2.getValue());
	_distanceLineWidth4->color.setValue(0,0,1);
	addChild(_distanceLineWidth4);
}
void UMDSoMaximumExtension::insertMainAxes(){
	// Die maximale Ausdehung wird als Linie zwischen den maxima Punkten dargestellt = Objektachse/Diameter
	_distanceLine = insertDistanceLine();
	_distanceLine->endPos.setValue(pDiameter_2.getValue());
	_distanceLine->startPos.setValue(pDiameter_1.getValue());
	_distanceLine->color.setValue(1,0,0);
	addChild(_distanceLine);

	// maximale Punkte und Mittelpunkt in SbVec3f abspeichern
	
	SbVec3f SbMidPoint;
	SbMidPoint[0]=(pDiameter_2.getValue()[0]+pDiameter_1.getValue()[0])/2;
	SbMidPoint[1]=(pDiameter_2.getValue()[1]+pDiameter_1.getValue()[1])/2;
	SbMidPoint[2]=(pDiameter_2.getValue()[2]+pDiameter_1.getValue()[2])/2;
	//midPoint.setValue(SbMidPoint.getValue());

	SbVec3f SbDepthPoint1;
	SbDepthPoint1 = pDepth_1.getValue();

	SbVec3f vHessianPoint1; 
	// HessianPoints ist der orthogonal projezierter depth point auf Objektachse
	vHessianPoint1 = computeHessianPoint(	pDiameter_1.getValue(),
											pDiameter_2.getValue(),
											pDepth_1.getValue()[0], 
											pDepth_1.getValue()[1], 
											pDepth_1.getValue()[2]);
	
	
	//erstmal nur pDiameter_1 nach MidPoint, normalerweise näheren Pkt. zu MidPoint wählen
	SbVec3f SbVecTranslate;
	SbVecTranslate[0]=-1*(vHessianPoint1.getValue()[0]-SbMidPoint.getValue()[0]);
	SbVecTranslate[1]=-1*(vHessianPoint1.getValue()[1]-SbMidPoint.getValue()[1]);
	SbVecTranslate[2]=-1*(vHessianPoint1.getValue()[2]-SbMidPoint.getValue()[2]);

	//Translate first depth axis to origin
	SbMatrix sbTranslateMatrix, sbRotMatrix;
	sbTranslateMatrix.setTranslate(SbVecTranslate);
	sbTranslateMatrix.multVecMatrix(vHessianPoint1, vHessianPoint1);
	sbTranslateMatrix.multVecMatrix(SbDepthPoint1, SbDepthPoint1);

	// display depth as axis (green)
	SbVec3f sbVecWidthLine1;
	sbVecWidthLine1 =SbDepthPoint1-vHessianPoint1;
	sbVecWidthLine1 = sbVecWidthLine1/sbVecWidthLine1.length();
	
	// Ortohogonale Linie vom ersten WidthPoint zur Objektachse
	UMDSoDistanceLine* _distanceLineAxes2;
	_distanceLineAxes2 = insertDistanceLine();
	_distanceLineAxes2->endPos.setValue(SbMidPoint-(fDistPt2tooZAxis*sbVecWidthLine1));
	_distanceLineAxes2->startPos.setValue(SbDepthPoint1);
	_distanceLineAxes2->color.setValue(0,1,0);
	addChild(_distanceLineAxes2);

	// display width as axis (blue)
	SbVec3f SbVecExtPoint3ortho(pWidth_1.getValue());
	SbVec3f SbVecExtPoint4ortho(pWidth_2.getValue());
	SbVec3f sbVec1 = SbMidPoint-SbDepthPoint1;
	SbVec3f sbVec2 = SbMidPoint-pDiameter_2.getValue();// Fehler in der Matrix->hier pos. ExtPt. wegen Orientierung!!!
	// Kreuzprodukt
	SbVec3f sbVecWidth2=sbVec2.cross(sbVec1);
	SbVec3f sbVecWidth3=-1*sbVec2.cross(sbVec1);
	sbVecWidth2 = sbVecWidth2/sbVecWidth2.length();
	sbVecWidth3 = sbVecWidth3/sbVecWidth3.length();
	sbVecWidth2 = sbVecWidth2*fDistPt3tooZAxis+SbMidPoint;
	sbVecWidth3 = sbVecWidth3*fDistPt4tooZAxis+SbMidPoint;

	UMDSoDistanceLine* _distanceLineAxes3;
	_distanceLineAxes3 = insertDistanceLine();
	_distanceLineAxes3->startPos.setValue(sbVecWidth3);
	_distanceLineAxes3->endPos.setValue(sbVecWidth2);
	_distanceLineAxes3->color.setValue(0,0,1);
	addChild(_distanceLineAxes3);

	vWidthPt1_bb = SbDepthPoint1;
	vWidthPt2_bb = SbMidPoint-(fDistPt2tooZAxis*sbVecWidthLine1);
	vWidthPt3_bb = sbVecWidth2;
	vWidthPt4_bb = sbVecWidth3;
}
void UMDSoMaximumExtension::insertBBox(){
	SbVec3f halfXAxis((pDiameter_1.getValue()-pDiameter_2.getValue()) / 2 );
	SbVec3f halfYAxis((vWidthPt2_bb.getValue()-vWidthPt1_bb.getValue()) / 2);
	SbVec3f halfZAxis((vWidthPt4_bb.getValue()-vWidthPt3_bb.getValue()) / 2);
	
	SbVec3f SbMidPoint;
	SbMidPoint[0]=(pDiameter_2.getValue()[0]+pDiameter_1.getValue()[0])/2;
	SbMidPoint[1]=(pDiameter_2.getValue()[1]+pDiameter_1.getValue()[1])/2;
	SbMidPoint[2]=(pDiameter_2.getValue()[2]+pDiameter_1.getValue()[2])/2;

	SbVec3f halfYAxisPos(SbMidPoint.getValue()-vWidthPt1_bb.getValue());
	SbVec3f halfYAxisNeg(vWidthPt2_bb.getValue()-SbMidPoint.getValue());
	SbVec3f halfZAxisPos(SbMidPoint.getValue()-vWidthPt4_bb.getValue());
	SbVec3f halfZAxisNeg(vWidthPt3_bb.getValue()-SbMidPoint.getValue());

	SoMFVec3f* vertices = new SoMFVec3f;
	vertices->set1Value(0, SbMidPoint - halfXAxis - halfYAxisPos - halfZAxisPos);
	vertices->set1Value(1, SbMidPoint - halfXAxis - halfYAxisPos + halfZAxisNeg);
	vertices->set1Value(2, SbMidPoint - halfXAxis + halfYAxisNeg - halfZAxisPos);
	vertices->set1Value(3, SbMidPoint - halfXAxis + halfYAxisNeg + halfZAxisNeg);
	vertices->set1Value(4, SbMidPoint + halfXAxis - halfYAxisPos - halfZAxisPos);
	vertices->set1Value(5, SbMidPoint + halfXAxis - halfYAxisPos + halfZAxisNeg);
	vertices->set1Value(6, SbMidPoint + halfXAxis + halfYAxisNeg - halfZAxisPos);
	vertices->set1Value(7, SbMidPoint + halfXAxis + halfYAxisNeg + halfZAxisNeg);
		   
	int vertexNum[24] = {0, 1, 3, 2, 0, -1, 
		4, 5, 7, 6, 4, -1,
		0, 4, -1,
		1, 5, -1,
		2, 6, -1,
		3, 7, -1};
		   
	SoVertexProperty* vertexProp = new SoVertexProperty;
	vertexProp->vertex = *vertices;
	SoIndexedLineSet* lineSet = new SoIndexedLineSet;
	lineSet->ref();
	lineSet->vertexProperty = vertexProp;
	lineSet->coordIndex.setValues(0, 24, vertexNum);
	lineSet->unrefNoDelete();
	addChild(lineSet);
}
