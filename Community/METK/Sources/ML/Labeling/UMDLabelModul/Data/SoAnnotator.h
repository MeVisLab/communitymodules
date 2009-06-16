#include "LabelingSystem.h"

#include <windows.h>
#include <stdio.h>
#include <time.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/SbLinear.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/SoLists.h>

#include <Inventor/fields/SoSFBool.h>

#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <limits>

#include "image_basic.h"

// forwarding declaration
class SoLabelObject;

class LABELING_EXPORT SoAnnotator : public SoGroup
{ 
	SO_NODE_HEADER(SoAnnotator);

public:   
	SoAnnotator();
	~SoAnnotator();

	static void initClass();

	void init( std::vector<SoLabelObject*> &_myLabelObjects );
	void setAutoUpdate(bool onOff);

	void reLayout();
	//void reFresh();

	unsigned char *_bufferID;

	//weightExternalLength = 75.0;   
	float getWeightLength( void  )       { return _weightExternalLength; };   //großer Wert = sehr lang -> schlecht
	void  setWeightLength( float weight ){  _weightExternalLength = weight; };  
	//weightExternalAngle = 1.0;   
	float getWeightAngle( void )         { return _weightExternalAngle; };    //großer Wert = kleiner Winkel -> gut
	void  setWeightAngle( float weight ) {  _weightExternalAngle = weight; };  
	//weightExternalDelta = 20.0;
	float getWeightDelta( void )         { return _weightExternalDelta; };    //großer wert = große Distanz -> schlecht
	void  setWeightDelta( float weight ) {  _weightExternalDelta = weight; };  
	//weightExternalNeighbor = 75.0;
	float getWeightNeighbor( void )         { return _weightExternalNeighbor; };    //großer wert = großer minimaler Abstand -> schlecht
	void  setWeightNeighbor( float weight ) {  _weightExternalNeighbor = weight; };
	float getWeightIntersection( void )         { return _weight_Intersection; };    //großer wert = großer minimaler Abstand -> schlecht
	void  setWeightIntersection( float weight ) {  _weight_Intersection = weight; };

	//funktionen set/get für gewicht unsichtbarer Objekte
	float getWeightAngleInvisObj( void )         { return _weight_AngleInvisObj; };    //großer Wert = kleiner Winkel -> gut
	void  setWeightAngleInvisObj( float weight ) {  _weight_AngleInvisObj = weight; };
	float getWeightDeltaInvisObj( void )         { return _weight_DeltaInvisObj; };    //großer wert = große Distanz -> schlecht
	void  setWeightDeltaInvisObj( float weight ) {  _weight_DeltaInvisObj = weight; };
	float getWeightLengthInvisObj( void  )       { return _weight_LengthInvisObj; };   //großer Wert = sehr lang -> schlecht
	void  setWeightLengthInvisObj( float weight ){  _weight_LengthInvisObj = weight; };
	float getWeightNeighborInvisObj( void )         { return _weight_NeighborInvisObj; };    //großer wert = großer minimaler Abstand -> schlecht
	void  setWeightNeighborInvisObj( float weight ) {  _weight_NeighborInvisObj = weight; };
	float getWeightIntersectionInvisObj( void )         { return _weight_IntersectionInvisObj; };    //großer wert = großer minimaler Abstand -> schlecht
	void  setWeightIntersectionInvisObj( float weight ) {  _weight_IntersectionInvisObj = weight; };

	void  setDrawOrbitPoints( bool value ){  _drawOrbit=value; };

	int getBufferSkipLevel ( void )      { return _skipLevel; };    //standard = 4
	void  setBufferSkipLevel( int level ){  level>0? _skipLevel = level: _skipLevel=1; };

	int getOrbitDistance(void) { return _orbitDistance; }
	void setOrbitDistance( int distance) { _orbitDistance = distance; }

	bool isUseAgent( void ) { return _useAgents; };
	bool getUseAgent( void ) { return _useAgents; };
	void setUseAgent( bool flag ) { _useAgents=flag; };

	bool isOrthoStyle( void ) { return _useOrthoStyle; };
	bool getOrthoStyle( void ) { return _useOrthoStyle; };
	void setOrthoStyle( bool flag ) { _useOrthoStyle=flag; };

	SbVec2s calcBBCoordinates( SbVec3f );

	void saveBufferAsBMP();

protected:
	virtual void GLRender(SoGLRenderAction *action);
	virtual void callback(SoCallbackAction *action);
	virtual void doAction(SoAction *action);

private:
	std::vector< SoLabelObject* > *myLabelObjects; //Pointer auf das Vectorf

	struct point {
		short int x, y;
		//bool isSet=0; //TEST; um Orbit-Punkt als belegt zu markieren
	};  


	// ------- Kandidaten -----------------------------------------------------
	//#define SKIP_LEVEL 2
#define NUMBER_OF_MAXIMA 10

#define BUFFER_X_SIZE 3000
#define BUFFER_Y_SIZE 3000

	int _skipLevel;
	int _orbitDistance;
	int _obscureFactor;

	void erzeugeDistanzFeld(unsigned char *bilddaten);
	void findeOrbit(int distance);
	void ermittleMaxima();
	void drawOrbitPoints();

	unsigned char *_bufferDT, *_bufferDT1, *_bufferDT2, *_bufferDT3, *_bufferDT4;  //MB: wieso 4??  

	point _maximum[256][NUMBER_OF_MAXIMA];
	unsigned short _maximumWert[256][NUMBER_OF_MAXIMA], _maximumWertAlt[256][NUMBER_OF_MAXIMA];
	unsigned short _maximumIndex[256], _maximumIndexAlt[256];

	void findCandidatesAnnotationBoxes();
	void findCandidatesExtLabel();


	std::vector<point> _orbit; //point orbit[10000]; alte statische datenstruktur
	int _orbitcounter;


	// External -----------------------------------------------------------
	void evalExtLabels();
	bool validExternalPlacement(short x, short y, short sizex, short sizey, bool isLeft);
	bool pointInBox(short int boxPosX, short int boxPosY, short int boxDimX, short int boxDimY, short int pointPosX, short int pointPosY);
	//bool lineIntersectBox(SbVec2s boxStart, SbVec2s boxEnd, SbVec2s lineStart, SbVec2s lineEnd );
	//bool boxInBox(short int box1PosX, short int box1PosY, short int box1DimX, short int box1DimY,
	//  short int box2PosX, short int box2PosY, short int box2DimX, short int box2DimY);
	//bool lineLineIntersection( const SbVec2s l1p1, const SbVec2s l1p2, const SbVec2s l2p1, const SbVec2s l2p2 );
	void resolveLineLineIntersections( void );
	void controllIntersections( void );
	bool isCrossed( SbVec2s a, SbVec2s b, SbVec2s c, SbVec2s d );
	//bool isHS_Cross_VS( SbVec2s a, SbVec2s b, SbVec2s c, SbVec2s d );

	bool _useOrthoStyle;

	int   scoreExternalLabel(int i);
	float getExternalReflineLength(short p1x, short p1y, short p2x, short p2y);
	float getExternalReflineAngle(short p1y, short p2y);
	float getExternalDelta(short oldx, short oldy, short newx, short newy);

	//float getExternalInfoHiding(short p1x, short p1y, short p2x, short p2y);
	float getExternalNeighborDistance(short px, short py, int anzahlNachbarn);
	float getExternalIntersectionCount(const short p1x, const short p1y, const short p2x, const short p2y, const int curElement);

	bool agentExternalAnchorStillValid(int id, short displacementX, short displacementY);
	void agentExternalAnchorCheckNeighborhoodDF(int id);
	void agentExternalTextCheckNeighborhood(int i);
	// --------------------------------------------------------------------

	void ermittleKonvexeHuelle();
	static int punkteSortierKernel(point *p1, point *p2) {
		if (p1->x > p2->x) return(1);	// Tausch
		else if ((p1->x == p2->x) && (p1->y > p2->y)) return(1);	// Tausch 
		else return(-1); // kein Tausch
	}
	int  chainHull_2D(point *P, int n, point *H);
	inline float isLeft( point P0, point P1, point P2 );

	void normalisiere( std::vector<float>& value);
	//void normalisiere(float value[100], int numValues);//alt
	//void normalisiere(float valueIn[100], float valueOut[100], int numValues);//überflüssig
	int  whoIsTop(float value[100], int numValues);
	int  whoIsTop( std::vector<float>& value);  

	int _numberOfBoxes, _numberOfBoxCandidates, _maxCandidates;

	float _weightExternalLength, _weightExternalAngle, _weightExternalDelta, _weightExternalNeighbor, _weight_Intersection;
	float _weight_LengthInvisObj,_weight_AngleInvisObj, _weight_DeltaInvisObj, _weight_NeighborInvisObj, _weight_IntersectionInvisObj; //gewichte für nicht sichtbarelabels    
	float _weightBoxDF, _weightBoxDelta, _weightBoxNeighbors;
	int _agentInternalLifetime, _agentInternalLifetimeLeft, _agentExternalLifetime, _agentExternalLifetimeLeft, _agentBoxLifetime, _agentBoxLifetimeLeft;
	bool _useAgents, _autoUpdate, _layoutUpdate;
	//point extNewpos[255];
	//int extNewOrbitIndex[255];

	bool _debug;
	bool _drawOrbit;

	SbVec2s _origin, _size;
	SoState *_state;
	SoSFBool _redraw;
	//SbMatrix objectToWorldMatrix;

	//bool isInit;
};