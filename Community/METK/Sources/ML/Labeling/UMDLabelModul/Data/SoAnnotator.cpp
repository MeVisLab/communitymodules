#define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#include <windows.h>
#include "SoAnnotator.h"

#include "SoLabelObject.h"

SO_NODE_SOURCE(SoAnnotator);

void SoAnnotator::initClass()
{
	SO_NODE_INIT_CLASS(SoAnnotator, SoGroup, "Group"); //Original
}


SoAnnotator::SoAnnotator()
{	
	SO_NODE_CONSTRUCTOR(SoAnnotator);   
	SO_NODE_ADD_FIELD(_redraw,(TRUE));

	//isInit=false;

	_bufferDT = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE); // auch nötig, da rand bei ermittleMaxima sonst schwierigkeiten macht
	_bufferDT1 = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE);
	_bufferDT2 = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE);
	_bufferDT3 = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE);
	_bufferDT4 = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE);

	_weightExternalLength = 0.9; 
	_weightExternalAngle = 1.0; 
	_weightExternalDelta = 0.8;
	_weightExternalNeighbor = 0.7; 
	_weight_LengthInvisObj = 1.0;
	_weight_AngleInvisObj = 0.0;
	_weight_DeltaInvisObj = 0.8;
	_weight_NeighborInvisObj=0.0;

	_skipLevel=1;//original 4;
	_orbitDistance=20;//original 20
	_agentExternalLifetime = 5; //original 5

	_useAgents = true;
	_useOrthoStyle=false;
	_debug = false;
	_drawOrbit=false;
	_autoUpdate = false;

	_obscureFactor = 4;
}


SoAnnotator::~SoAnnotator() { }


void SoAnnotator::init( std::vector<SoLabelObject*> &_myLabelObjects )
{     
	myLabelObjects = &_myLabelObjects;
}


void SoAnnotator::setAutoUpdate(bool onOff) {
  this->_autoUpdate = onOff;
}

void SoAnnotator::reLayout() {
  this->_layoutUpdate = true;
}

void SoAnnotator::GLRender(SoGLRenderAction *action)
{
	int i;
	SbMatrix projectionMatrix;
	SbMatrix viewingMatrix;
	const SbViewportRegion vp = action->getViewportRegion();

	_state = action->getState();
	_origin = vp.getViewportOriginPixels();
	_size = vp.getViewportSizePixels();

	const SbViewVolume &vv = SoViewVolumeElement::get(_state);

	glPushMatrix();
	projectionMatrix.makeIdentity();
	viewingMatrix.makeIdentity();

	SoViewingMatrixElement::set(_state,this,viewingMatrix); 
	SoProjectionMatrixElement::set(_state,this,projectionMatrix);

	SbVec3f dir;  

	if (_autoUpdate || _layoutUpdate) // Komplettes Relayout
	{ 
		erzeugeDistanzFeld(_bufferID);
		ermittleMaxima();

		findCandidatesExtLabel(); // orbit

		for (int i=0; i<(int)myLabelObjects->size(); i++)
		{
			if ((*myLabelObjects)[i]->on)
			{
				//weltkoordinaten ermitteln
				//Centerbox auf ScreenKoordinaten umrechnen
				vv.projectToScreen( (*myLabelObjects)[i]->centerBB, dir);
				//mit VP-Größe Multiplizieren
				dir.setValue( dir.getValue()[0]*_size[0], dir.getValue()[1]*_size[1], dir.getValue()[2] );
				(*myLabelObjects)[i]->screenCenterBB.setValue( dir.getValue()  );

				if (_maximumWert[i+1][0] == 0)
				{ 
					(*myLabelObjects)[i]->labelState.visible  = false; //wird später erst berücksichtigt, ob versteckt gelabelt werden soll
					(*myLabelObjects)[i]->referenceLine.comment5="--invisible weil _maximumWert[i+1][0]=0--";
					(*myLabelObjects)[i]->labelState.isObjectVisible = false;
				}
				else
				{
					(*myLabelObjects)[i]->labelState.visible  = true; //potentiell setzbar sichtbar
					(*myLabelObjects)[i]->labelState.isObjectVisible = true;
				}
				(*myLabelObjects)[i]->labelState.onConvexHull = false;

				if ( (*myLabelObjects)[i]->on ) //wenn label gar nicht gezeichnet werden soll, dann als behandelt markieren
				{
					(*myLabelObjects)[i]->labelState.alreadyProcessed = false;
				}
				else
				{
					(*myLabelObjects)[i]->labelState.alreadyProcessed = true;
				}

				(*myLabelObjects)[i]->setViewportDimensions( _size[0], _size[1] );
			}
		}
		evalExtLabels();

		_agentExternalLifetimeLeft = _agentExternalLifetime;

		_layoutUpdate = false;
	}
	else
	{ 
		//else zweig wird zur zeit nicht benutzt, da auch noch fehlerhaft
		//Individuelles Relayout durch Agenten (ist auch intern ist in evalIntLabels-> mehr Trägheit)
		if (_useAgents)
		{      
			if (_agentExternalLifetimeLeft)
			{
				for (i=0; i<(int)myLabelObjects->size(); i++)
				{
					if ((*myLabelObjects)[i]->labelState.visible && (*myLabelObjects)[i]->on ) // nur behandeln, wenn Objekt momentan sichtbar
					{
						// erstmal um Ankerpunkt kümmern
						agentExternalAnchorCheckNeighborhoodDF(i);
						// jetzt um die Texte kümmern, damit die sich von ihren direkten Nachbarn auf dem Orbit abstoßen
						agentExternalTextCheckNeighborhood(i);
						resolveLineLineIntersections(); //
					}
				}  
				_agentExternalLifetimeLeft--;
			}
		}
	}


	if (_drawOrbit)  drawOrbitPoints();
	glPopMatrix();

	//KONRAD
	//glClear(GL_COLOR_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_STENCIL_BUFFER_BIT);
	//glClear(GL_ACCUM_BUFFER_BIT);
	

	SoAnnotator::doAction(action);

//	SoGroup::GLRender(action);
}



void SoAnnotator::callback(SoCallbackAction *action)
{
	SoAnnotator::doAction(action);
	
	
//	SoGroup::callback(action);
}

void SoAnnotator::doAction(SoAction* /*action*/)
{ 
  // nothing left to do here...
	
	
//	SoGroup::doAction(action);
}

void SoAnnotator::drawOrbitPoints()
{
	glColor3f(1.0, 0.0, 0.0); 
	glPointSize(1.0);
	float ox, oy;
	glBegin(GL_POINTS);		
	for (int i=0; i<(int)_orbit.size(); i++)
	{
		ox=_orbit[i].x;
		oy=_orbit[i].y;
		ox=((ox/_size[0])*2.0)-1.0;
		oy=((oy/_size[1])*2.0)-1.0;
		glVertex3f( ox, oy, 1.0 );		
	}
	glEnd();
}


// -------------------------------------------------------------------------------


void SoAnnotator::erzeugeDistanzFeld(unsigned char *bilddaten)
{
	int x,y, treffer;
	unsigned char min, pixelWert, farbindex, wert[4], wertTop=0, wertTopIndex = 0;

	memset(_bufferDT, 0, BUFFER_X_SIZE*BUFFER_Y_SIZE / _skipLevel);  
	memset(_bufferDT1, 0, BUFFER_X_SIZE*BUFFER_Y_SIZE / _skipLevel);  
	memset(_bufferDT2, 0, BUFFER_X_SIZE*BUFFER_Y_SIZE / _skipLevel); 
	memset(_bufferDT3, 0, BUFFER_X_SIZE*BUFFER_Y_SIZE / _skipLevel); 
	memset(_bufferDT4, 0, BUFFER_X_SIZE*BUFFER_Y_SIZE / _skipLevel);

	//TESTEN OB size>BUFFER_X_SIZE
	/*memset(_bufferDT, 0, _size[0]*_size[1] / _skipLevel);  
	memset(_bufferDT1, 0, _size[0]*_size[1] / _skipLevel);  
	memset(_bufferDT2, 0, _size[0]*_size[1] / _skipLevel); 
	memset(_bufferDT3, 0, _size[0]*_size[1] / _skipLevel); 
	memset(_bufferDT4, 0, _size[0]*_size[1] / _skipLevel);*/


	// von unten links nach oben rechts
	for (x=_skipLevel; x<_size[0]/_skipLevel-1; x++) // jedes Pixel X, Achtung Rand nicht berücksichtigt
	{
		for (y=_skipLevel; y<_size[1]/_skipLevel-1; y++) // jedes Pixel Y, Achtung Rand nicht berücksichtigt
		{
			treffer = 0; // neue Suche	
			farbindex = bilddaten[y*_skipLevel*_size[0]*_obscureFactor + x*_skipLevel*_obscureFactor];
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++; 
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;

			if (treffer == 0) // wenn alle Nachbarn gleich
			{
				min = 255;
				pixelWert = _bufferDT1[(y-1)*_size[0] + (x-1)]; // bottom-left
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT1[(y-1)*_size[0] + x]; // bottom
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT1[y*_size[0] + (x-1)]; // left
				if (pixelWert < min) min = pixelWert;

				if (min<255) _bufferDT1[y*_size[0] + x]     = min+1;
				else _bufferDT1[y*_size[0] + x]     = 255;
			}
		}
	}

	// von oben rechts nach unten links
	for (x=(_size[0]/_skipLevel)-_skipLevel; x>=_skipLevel; x--) {			// jedes Pixel X, Achtung Rand nicht berücksichtigt
		for (y=(_size[1]/_skipLevel)-_skipLevel; y>=_skipLevel; y--) {				// jedes Pixel Y, Achtung Rand nicht berücksichtigt
			treffer = 0;												// neue Suche	
			farbindex = bilddaten[y*_skipLevel*_size[0]*_obscureFactor + x*_skipLevel*_obscureFactor];
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++; 
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;

			if (treffer == 0) {									// wenn alle Nachbarn gleich
				min = 255;
				pixelWert = _bufferDT2[(y+1)*_size[0] + (x+1)]; // top-right
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT2[(y+1)*_size[0] + x]; // top
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT2[y*_size[0] + (x+1)]; // right
				if (pixelWert < min) min = pixelWert;

				if (min<255) _bufferDT2[y*_size[0] + x]     = min+1;
				else _bufferDT2[y*_size[0] + x]     = 255;
			}
		}
	}

	// von unten rechts nach oben links
	for (x=(_size[0]/_skipLevel)-_skipLevel; x>=_skipLevel; x--) {			// jedes Pixel X, Achtung Rand nicht berücksichtigt
		for (y=_skipLevel; y<_size[1]/_skipLevel-1; y++) {				// jedes Pixel Y, Achtung Rand nicht berücksichtigt
			treffer = 0;												// neue Suche	
			farbindex = bilddaten[y*_skipLevel*_size[0]*_obscureFactor + x*_skipLevel*_obscureFactor];
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++; 
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;

			if (treffer == 0) {									// wenn alle Nachbarn gleich
				min = 255;
				pixelWert = _bufferDT3[(y-1)*_size[0] + (x+1)]; // bottom-right
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT3[(y-1)*_size[0] + x]; // bottom
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT3[y*_size[0] + (x+1)]; // right
				if (pixelWert < min) min = pixelWert;

				if (min<255) _bufferDT3[y*_size[0] + x]     = min+1;
				else _bufferDT3[y*_size[0] + x]     = 255;
			}
		}
	}

	// von oben rechts nach unten links
	for (x=_skipLevel; x<_size[0]/_skipLevel-1; x++) {			// jedes Pixel X, Achtung Rand nicht berücksichtigt
		for (y=(_size[1]/_skipLevel)-_skipLevel; y>=_skipLevel; y--) {				// jedes Pixel Y, Achtung Rand nicht berücksichtigt
			treffer = 0;												// neue Suche	
			farbindex = bilddaten[y*_skipLevel*_size[0]*_obscureFactor + x*_skipLevel*_obscureFactor];
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++; 
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x-1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y+1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;
			if (bilddaten[(y-1)*_skipLevel*_size[0]*_obscureFactor + (x+1)*_skipLevel*_obscureFactor] != farbindex) treffer++;

			if (treffer == 0) {									// wenn alle Nachbarn gleich
				min = 255;
				pixelWert = _bufferDT4[(y+1)*_size[0] + (x-1)]; // top-left
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT4[(y+1)*_size[0] + x]; // top
				if (pixelWert < min) min = pixelWert;
				pixelWert = _bufferDT4[y*_size[0] + (x-1)]; // left
				if (pixelWert < min) min = pixelWert;

				if (min<255) _bufferDT4[y*_size[0] + x]     = min+1;
				else _bufferDT4[y*_size[0] + x]     = 255;
			}
		}
	}

	// nun die MIN(a,b,c,d) ermitteln und in bildaten speichern
	for (x=_skipLevel; x<_size[0]/_skipLevel-1; x++) {			// jedes Pixel X, Achtung Rand nicht berücksichtigt
		for (y=_skipLevel; y<_size[1]/_skipLevel-1; y++) {
			wert[0] = _bufferDT1[y*_size[0] + x];
			wert[1] = _bufferDT2[y*_size[0] + x];
			wert[2] = _bufferDT3[y*_size[0] + x];
			wert[3] = _bufferDT4[y*_size[0] + x];

			wertTop = 255;
			for (int i=0; i<4; i++) {  // minimum
				if (wert[i] <= wertTop) {
					wertTop = wert[i];
					wertTopIndex = i;
				}
			}
			_bufferDT[y*_size[0] + x]     = wert[wertTopIndex];
		}
	}
}

void SoAnnotator::ermittleMaxima()
{
	// Maß:   sammle pro Farbe die n größten Werte
	// FixMe: aus Performancegründen wird nur der R-wert getrachtet
	int x, y, currentIndex;
	unsigned char farbindex, distanzWert; //MB: ggf. farbwert 0 rausnehmen

	memcpy(_maximumWertAlt, _maximumWert, sizeof(_maximumWertAlt));
	memset(_maximumWert, 0, sizeof(_maximumWert)); // init
	memset(_maximum, 0, sizeof(_maximum)); 
	memset(_maximumIndex, 0, sizeof(_maximumIndex));
	memset(_maximumIndexAlt, 0, sizeof(_maximumIndexAlt));

	for (x=1; x<_size[0]/_skipLevel; x++) {			// jedes Pixel X, Achtung Rand nicht berücksichtigt //MB: TEST _skipLevel -1
		for (y=1; y<_size[1]/_skipLevel; y++) {				// jedes Pixel Y, Achtung Rand nicht berücksichtigt  //MB: TEST _skipLevel -1
			farbindex = _bufferID[(y*_skipLevel)*_size[0]*_obscureFactor+(x*_skipLevel)*_obscureFactor]; // Farbindex der position
			distanzWert = _bufferDT[y*_size[0]+x];
			currentIndex = _maximumIndex[farbindex];

			if (distanzWert >= _maximumWert[farbindex][_maximumIndexAlt[farbindex]]) {
				_maximum[farbindex][currentIndex].x = x * _skipLevel;
				_maximum[farbindex][currentIndex].y = y * _skipLevel;
				_maximumWert[farbindex][currentIndex] = distanzWert;
				_maximumIndexAlt[farbindex] = currentIndex;
				_maximumIndex[farbindex] = (currentIndex+1) % NUMBER_OF_MAXIMA;
			} 
		}
	}
}

void SoAnnotator::findeOrbit(int distance)
{
	int x,y;

	_orbitcounter = 0; 
	_orbit.clear();
	//orbit.resize( (_size[0]/_skipLevel * _size[1]/_skipLevel) );
	for (x=0; x<_size[0]/_skipLevel; x++) {			
		for (y=0; y<_size[1]/_skipLevel; y++) {
			if (_bufferDT[y*_size[0] + x] == distance) {
				if (_bufferID[(y*_skipLevel)*_size[0]*_obscureFactor + (x*_skipLevel)*_obscureFactor] == 255) {  //<<< BG-Value
					if (x != distance+(_skipLevel-1) && x != _size[0]/_skipLevel-distance-(_skipLevel-1) &&  // kein rand!
						y != distance+(_skipLevel-1) && y != _size[1]/_skipLevel-distance-(_skipLevel-1)) {             
							point a; a.x=x * _skipLevel; a.y = y * _skipLevel;				
							_orbit.push_back(a);
							//_orbit[orbitcounter].x = x * _skipLevel;
							//_orbit[orbitcounter].y = y * _skipLevel;
							_orbitcounter++;
					}
				}
			}
		}
	}
}

void SoAnnotator::findCandidatesExtLabel()
{
	findeOrbit(_orbitDistance / _skipLevel); // finde orbit im abstand 20
}

bool lineIntersectBox(SbVec2s B1, SbVec2s B2, SbVec2s L1, SbVec2s L2 ) {
	// guter quellcode unter: http://www.3dkingdoms.com/weekly/weekly.php?a=3
	if (L2[0] < B1[0] && L1[0] < B1[0] ) return false;
	if (L2[0] > B2[0] && L1[0] > B2[0] ) return false;
	if (L2[1] < B1[1] && L1[1] < B1[1] ) return false;
	if (L2[1] > B2[1] && L1[1] > B2[1] ) return false;	
	
	if (L1[0] > B1[0] && L1[0] < B2[0] &&
	    L1[1] > B1[1] && L1[1] < B2[1] ) return true;
	else return false;

}

bool SoAnnotator::pointInBox(short int boxPosX, short int boxPosY, short int boxDimX, short int boxDimY, short int pointPosX, short int pointPosY) {
  	if ( pointPosX >= boxPosX - boxDimX &&  
	     pointPosX <= boxPosX + boxDimX &&
         pointPosY >= boxPosY - boxDimY &&  //
	     pointPosY <= boxPosY + boxDimY ) {
        return true;   
    }  
    return false;
} 


//*********************** External Labeling ******************************
//************************************************************************ 
void SoAnnotator::evalExtLabels() {
	int i, topmostIndex;

	//legt eine Prioritätsliste fest, weil im nachhinein greedyalgorithmus
	ermittleKonvexeHuelle();	

	for (i=0; i<(int)myLabelObjects->size(); i++) { //Enthält die Indizes aller Objekte die gelabelt werden sollen sortiert nach kriterien		

if ((*myLabelObjects)[i]->on)
{
		//----SETZEN DER ANCHORPOINTS IM OBJEKT
		//Erst unsichtbare Objekte, die dennoch belabelt werden sollen
		if ((*myLabelObjects)[i]->drawIfInvis==true && (*myLabelObjects)[i]->labelState.isObjectVisible==false)
		{
			(*myLabelObjects)[i]->labelState.visible=true;
			SbVec2s screenCenterBB( floor( (*myLabelObjects)[i]->screenCenterBB.getValue()[0]), floor((*myLabelObjects)[i]->screenCenterBB.getValue()[1]) );
			//liegt Objekt außerhalb des Bildschirms?
			////std::cout << "evalExtLabels(): is invisible!  object=" << (*myLabelObjects)[i]->metkObjId << "  screenCenterBB=" << screenCenterBB.getValue()[0] << "," << screenCenterBB.getValue()[1] << "  ";
			if ( screenCenterBB.getValue()[0] < 0 ||  screenCenterBB.getValue()[0] > _size[0] ||
				screenCenterBB.getValue()[1] < 0 ||  screenCenterBB.getValue()[1] > _size[1])
			{
				std::cout << "visible set to FALSE ";
				(*myLabelObjects)[i]->labelState.visible = false; //wenn außerhalb des fensters nicht berechnen
				(*myLabelObjects)[i]->referenceLine.comment6="--invisible weil centerBB der Structure außerhalb des Screens--";
				continue;
			}
			(*myLabelObjects)[i]->referenceLine.object.setValue(  screenCenterBB.getValue() );	
			(*myLabelObjects)[i]->referenceLine.anchorType = "screenCenterBB";

		} 
		else {			
			if (agentExternalAnchorStillValid(i, 0, 0) && _useAgents) { // wenn ankerpunkt noch auf objekt
				agentExternalAnchorCheckNeighborhoodDF(i);
			} else { // andernfalls setze ihn auf ein neues Maximum      
				(*myLabelObjects)[i]->referenceLine.object.setValue( _maximum[i+1][0].x, _maximum[i+1][0].y ); //_maximum[0]=hintergrund
				(*myLabelObjects)[i]->referenceLine.anchorType = "_maximum";
			}
			////std::cout << "evalExtLabels(): is visible object=" << (*myLabelObjects)[i]->metkObjId << "  referenclineXY=" << (*myLabelObjects)[i]->referenceLine.object.getValue()[0] <<","<< (*myLabelObjects)[i]->referenceLine.object.getValue()[1];
		}						

		int bestIdx_labelObject = i;

		topmostIndex = scoreExternalLabel(i);
		if (topmostIndex == -1)
		{			
			(*myLabelObjects)[bestIdx_labelObject]->labelState.visible = false; //wenn kein platz für label
			(*myLabelObjects)[bestIdx_labelObject]->referenceLine.comment1 = "--topMostIndex==-1 daher angeblich kein Platz für Label--";
			continue;
		}
		int y;
		y=_orbit[topmostIndex].y;
		(*myLabelObjects)[i]->refLineNewPos.setValue( _orbit[topmostIndex].x, y );// jetzt noch das äußere Ende der Refline eintragen
		(*myLabelObjects)[i]->labelState.alreadyProcessed = true;
		(*myLabelObjects)[i]->referenceLine.orbitIndex = topmostIndex; // achtung nur fix wegen bug
		(*myLabelObjects)[i]->extNewOrbitIndex = topmostIndex;
		(*myLabelObjects)[i]->referenceLine.label.setValue( _orbit[topmostIndex].x, y );
}
	}//ende durchlauf alle objekte

	resolveLineLineIntersections(); 
	controllIntersections();	
}



int SoAnnotator::scoreExternalLabel(int nr ) {
	SbVec2s labelSize;
	//float scoreSum[1000], scoreLength[1000], scoreAngle[1000], scoreDelta[1000], scoreNeighbors[1000];// scoreInfoHiding[1000], scoreNeighbors[1000];
	std::vector<float> scoreSum, scoreLength, scoreAngle, scoreDelta, scoreNeighbors, scoreIntersection;
	int j, topmostIndex;
	short orbitx, orbity, objectx, objecty;
	bool isLeft;  

	if ( nr<0 || nr >= (int)myLabelObjects->size() ) return -1;

	for (j=0; j<_orbitcounter; j++) {//initialisieren
		scoreSum.push_back(0);
		scoreLength.push_back(0);
		scoreAngle.push_back(0);
		scoreDelta.push_back(0);
		scoreNeighbors.push_back(0);
		scoreIntersection.push_back(0);
	}

	//memset(scoreSum, 0, sizeof(scoreSum));

	if (!(*myLabelObjects)[nr]->validTextBoxSize)
	{
		std::cout << "Invalid TextBoxSize ... recalc?? ... " << (*myLabelObjects)[nr]->metkObjId;
		std::cout << "   Text waere = " << (*myLabelObjects)[nr]->getLabelText() << std::endl;
	}
	labelSize = (*myLabelObjects)[nr]->getTextSize();//myExternal[i]->getSize();
	//std::cout << "labelSize=" << labelSize[0] << "," << labelSize[1] << std::endl;

	objectx = (*myLabelObjects)[nr]->referenceLine.object.getValue()[0];
	objecty = (*myLabelObjects)[nr]->referenceLine.object.getValue()[1];
	if (labelSize[0] <= 0)
	{
		(*myLabelObjects)[nr]->labelState.visible = false;
		(*myLabelObjects)[nr]->referenceLine.comment7="--invisible weil labelSize<=0--";
		std::cout << "XXX  " << (*myLabelObjects)[nr]->metkObjId << " --invisible weil labelSize<=0-- " << std::endl;
		return -1; // momentan invalid, erst wieder beim nächsten Pass betrachten
	}
	for (j=0; j<_orbitcounter; j++) {
		orbitx = _orbit[j].x;
		orbity = _orbit[j].y;
		// wie verläuft potentielle Referenzlinie?
		if (orbitx >= objectx) isLeft = false; // wenn rechts
		else isLeft = true; // wenn links
		// prüfe erstmal die Platzierbarkeit: Label<->Object (left/right), Label<->processed Labels, Label<->Boxes
		if (validExternalPlacement(orbitx, orbity, labelSize[0], labelSize[1], isLeft))
		{
			scoreLength[j]       = getExternalReflineLength(objectx, objecty, orbitx, orbity);
			scoreAngle[j]        = getExternalReflineAngle(objecty, orbity);
			scoreDelta[j]        = getExternalDelta((*myLabelObjects)[nr]->referenceLine.label.getValue()[0], (*myLabelObjects)[nr]->referenceLine.label.getValue()[1], orbitx, orbity); // alter wert			
			scoreNeighbors[j]    = getExternalNeighborDistance( orbitx, orbity, nr);
			scoreIntersection[j] = getExternalIntersectionCount( orbitx, orbity, objectx, objecty, nr);
		} 
		else
		{
			scoreAngle[j] = 0.0; // kleiner wert als schlecht -> ( 1/differenz der beiden y-werte
			scoreLength[j] = scoreDelta[j] = scoreNeighbors[j] = scoreIntersection[j] = 1000.0; // hoher wert als schlecht -> siehe summe
		}
	}

	// normalisieren
	normalisiere(scoreLength);
	normalisiere(scoreAngle);
	normalisiere(scoreDelta);
	normalisiere(scoreNeighbors);
	normalisiere(scoreIntersection);

	// Summe bilden und wichten!
	for (j=0; j<_orbitcounter; j++)
	{
		if ( (*myLabelObjects)[nr]->labelState.isObjectVisible==false && (*myLabelObjects)[nr]->drawIfInvis==true )
		{
			scoreSum[j] = (1.0 - scoreLength[j]) * _weight_LengthInvisObj +        // großer Wert = sehr lang -> schlecht
				scoreAngle[j]   * _weight_AngleInvisObj +         // großer Wert = kleiner Winkel -> gut
				(1.0 - scoreDelta[j])  * _weight_DeltaInvisObj+          // großer wert = große Distanz -> schlecht
				(1.0 - scoreNeighbors[j]) * _weight_NeighborInvisObj     // großer Wert = großer Abstand (minimale Abstand) -> schlecht					  
				+(1.0 - scoreIntersection[j]) * _weight_IntersectionInvisObj;         
		}
		else
		{
			scoreSum[j] = (1.0 - scoreLength[j]) * _weightExternalLength +         // großer Wert = sehr lang -> schlecht
				scoreAngle[j]   * _weightExternalAngle +                 // großer Wert = kleiner Winkel -> gut
				(1.0 - scoreDelta[j])  * _weightExternalDelta +          // großer wert = große Distanz -> schlecht
				(1.0 - scoreNeighbors[j]) * _weightExternalNeighbor +    // großer Wert = großer Abstand (minimale Abstand) -> schlecht
				(1.0 - scoreIntersection[j]) * _weight_Intersection;  // großer Wert = großer Abstand (minimale Abstand) -> schlecht				
		}	
	}


	for (j=0; j<nr-1; j++ ) { // alle schon benutzten Stellen auf score=0 setzen; aber nur die bisher besuchten Knoten einbeziehen
		if ( (*myLabelObjects)[ j ]->labelState.alreadyProcessed ) {
			scoreSum[ (*myLabelObjects)[ j ]->referenceLine.orbitIndex] = 0.0;
		}
	} // timo: achtung, noch nicht überprüft ob es wirkt

	// Wer hat den besten Score?
	//topmostIndex = whoIsTop( scoreSum, orbitcounter);
	topmostIndex = whoIsTop( scoreSum );
	if ( scoreSum.size() == 0)
	{ 
		//std::cout << "ScoreSum nicht gefuellt!" << std::endl;
		return -1; //dann wurde der vector nicht gefüllt
	}
	if (scoreSum[topmostIndex] > 0.001) return topmostIndex;
	else return -1; // wenn es total unmöglich ist, eine Position aufzufinden
}

bool SoAnnotator::validExternalPlacement(short x, short y, short sizex, short sizey, bool isLeft ) {
  point p1, p2, p3, p4; // oben rechts, unten, rechts, unten links, oben links
  SbVec2s labelSize;
  SbBox2s labelBB, otherLabelBB;
  unsigned int i;
  int epsilon=5;
  sizex += epsilon;//20; //MB ggf. Timo fragen warum
  sizey += epsilon;//Sicherheitsabstand

  if (isLeft) { // wenn rechts vom label verläuft
    p1.x = x - sizex;  p1.y = y + sizey / 2; // oben links
    p2.x = x - sizex;  p2.y = y - sizey / 2; // unten links
    p3.x = x;          p3.y = y - sizey / 2; // unten rechts
    p4.x = x;          p4.y = y + sizey / 2; // oben rechts
	labelBB.setBounds( p2.x, p2.y, p4.x, p4.y); 
  } else {      // wenn links vom label die linie verläuft
    p1.x = x;          p1.y = y + sizey / 2; // oben rechts
    p2.x = x;          p2.y = y - sizey / 2; // unten rechts
    p3.x = x + sizex;  p3.y = y - sizey / 2; // unten links
    p4.x = x + sizex;  p4.y = y + sizey / 2; // oben links
	labelBB.setBounds( p2.x, p2.y, p4.x, p4.y); 
  }

  // Ist das Label überhaupt noch innerhalb des Bildschirms?
  if (p1.x < 0 || p1.x > _size[0] || p1.y < 0 || p1.y > _size[1]) return false;
  if (p2.x < 0 || p2.x > _size[0] || p2.y < 0 || p2.y > _size[1]) return false;
  if (p3.x < 0 || p3.x > _size[0] || p3.y < 0 || p3.y > _size[1]) return false;
  if (p4.x < 0 || p4.x > _size[0] || p4.y < 0 || p4.y > _size[1]) return false;

  // Ist da eine Kollision Objekt<->Label? 
  if (_bufferID[p1.y*_size[0]*_obscureFactor+p1.x*_obscureFactor] != 255) return false;
  if (_bufferID[p2.y*_size[0]*_obscureFactor+p2.x*_obscureFactor] != 255) return false;
  if (_bufferID[p3.y*_size[0]*_obscureFactor+p3.x*_obscureFactor] != 255) return false;
  if (_bufferID[p4.y*_size[0]*_obscureFactor+p4.x*_obscureFactor] != 255) return false;

  // Kollision Label<->Label? Wenn Orbitpunkt in naher Nachbarschaft belegt, dann Kollision
  for (i=0; i<(*myLabelObjects).size(); i++) {
    if ((*myLabelObjects)[i]->labelState.alreadyProcessed == true) {
	  labelSize = (*myLabelObjects)[i]->getTextSize(); 	  
	  SbVec2s pkt = (*myLabelObjects)[i]->refLineNewPos.getValue();
	  SbVec2s objPkt = (*myLabelObjects)[i]->referenceLine.object.getValue();
	  if ( pkt[0] >= objPkt[0] ){ //isLeft=false
		 otherLabelBB.setBounds( pkt[0]-epsilon, pkt[1]-(labelSize[1]/2.0)-epsilon, pkt[0]+labelSize[0]+epsilon, pkt[1]+(labelSize[1]/2.0)+epsilon); //+-1 wegen rundungsfehler beim teilen
	  } else { //isLeft=true
		 otherLabelBB.setBounds( pkt[0]-labelSize[0]-epsilon, pkt[1]-(labelSize[1] /2.0)-epsilon, pkt[0]+epsilon,  pkt[1]+(labelSize[1]/2.0)+epsilon );
	  }
	  if ( labelBB.intersect(otherLabelBB) ) {
		  return false;
	  }
	  

      if (pointInBox( pkt.getValue()[0], pkt.getValue()[1], labelSize[0], labelSize[1], p1.x, p1.y) ) return false;
      if (pointInBox( pkt.getValue()[0], pkt.getValue()[1], labelSize[0], labelSize[1], p2.x, p2.y) ) return false;
      if (pointInBox( pkt.getValue()[0], pkt.getValue()[1], labelSize[0], labelSize[1], p3.x, p3.y) ) return false;
      if (pointInBox( pkt.getValue()[0], pkt.getValue()[1], labelSize[0], labelSize[1], p4.x, p4.y) ) return false;
      if (pointInBox( pkt.getValue()[0], pkt.getValue()[1], labelSize[0], labelSize[1], (p2.x + p4.x) / 2, (p2.y + p4.y) / 2)) return false;
	 
    }
  }


  return true;
}

float SoAnnotator::getExternalReflineLength(short p1x, short p1y, short p2x, short p2y) {
  return sqrt((float) ((p1x - p2x)*(p1x - p2x) + (p1y - p2y)*(p1y - p2y)));  // e-Dist
}

float SoAnnotator::getExternalReflineAngle(short p1y, short p2y) {
  // je kleiner der y-Unterschied, desto kleiner der Winkel... nicht optimal, aber einfach
  int diff;
  float score;
  
  diff = abs(p1y - p2y);
  if (diff > 0) score = 1.0 / (float) diff;
  else score = 1.00; // bester Score für gerade Linie
  //else score = 0.00; // bester Score für gerade Linie

  return score;
}

float SoAnnotator::getExternalDelta(short oldx, short oldy, short newx, short newy) {
  if (oldx == 0) return 0.0; // wenn vorher auf null (war nicht sichtbar) dann immer 0 zurückliefern
  return sqrt((float) ((oldx - newx)*(oldx - newx) + (oldy - newy)*(oldy - newy)));  // e-Dist  
}


float SoAnnotator::getExternalNeighborDistance(short px, short py, int nr) {
	float minDistance=1000;//initalwert 
	SbVec2s labelPos;
	   for (int i=0; i<=nr-1; i++ ) {	 
          labelPos = (*myLabelObjects)[ i ]->refLineNewPos.getValue();
		  minDistance=MIN( minDistance, sqrt( (float) ((px - labelPos.getValue()[0])*(px - labelPos.getValue()[0]) + (py - labelPos.getValue()[1])*(py - labelPos.getValue()[1]))));
	   }

	//}
	return minDistance;
}


float SoAnnotator::getExternalIntersectionCount( const short orbitx, const short orbity, const short objectx, const short objecty, const int curElem ){
	int i, intersect=0; 
	SbVec2s l1p1(orbitx, orbity);
	SbVec2s l1p2(objectx, objecty);
    SbVec2s l2p1, l2p2; 

	//objeckte werden von 0..n abgearbeitet, daher sind alle elemente von 0..curElem-1 bereits zugewiesen
	for ( i=0; i<curElem; i++ ) {	 //nur bisher behandelte Elemente betrachten, die schon einen orbit zugewiesen haben
        l2p1 = (*myLabelObjects)[ i ]->refLineNewPos.getValue();
		l2p2 = (*myLabelObjects)[ i ]->referenceLine.object.getValue();
		//nur objekte betrachten, die sichtbar sind und gelabelt werden konnten
		if ((*myLabelObjects)[ i ]->labelState.visible && (*myLabelObjects)[ i ]->labelState.isObjectVisible )
		{
		   if ( isCrossed(l1p1,l1p2, l2p1,l2p2) ) intersect++;
		}
	}	
	return static_cast<float> (intersect);
}


void SoAnnotator::controllIntersections( void ) {
	
	//ALLE LABELS AUSBLENDEN DIE SICH ÜBERSCHNEIDEN
	SbVec2s labelSize, labelPos, objPos, b1, b2; 
//	int idx;
//	bool isLeft;
	//evt. neu sortieren, damit nicht sichtbare zuerst rausgeschmissen werden
	
	resolveLineLineIntersections();

	//nach dem LineLineIntersection kann es durch änderung des isLeft Status zu überschneidungen kommen
	//außerdem wird nun nach lineBoxIntersection geschaut
	for (int i=0; i<(int)myLabelObjects->size(); i++ )
	{		
		if ( (*myLabelObjects)[i]->labelState.visible && (*myLabelObjects)[i]->on ) {
		   labelSize=(*myLabelObjects)[i]->getTextSize();
		   labelPos=(*myLabelObjects)[i]->refLineNewPos.getValue();//
		   //b1= (*myLabelObjects)[i]->referenceLine.label.getValue();//debugTest
		   objPos=(*myLabelObjects)[i]->referenceLine.object.getValue();
		   //textbox bestimmen, für spätere intersectionAbfrage		
		   if ( labelPos[0] > objPos[0] ){ //isLeft=false
    		   b1[0]= labelPos[0];               b1[1]= labelPos[1]-(labelSize[1] /2.0)-1;
		       b2[0]= labelPos[0]+labelSize[0];  b2[1]= labelPos[1]+ (labelSize[1]/2.0)+1;
		   } else { //isLeft=true
     		   b1[0]= labelPos[0]-labelSize[0];  b1[1]= labelPos[1]-(labelSize[1] /2.0)-1;
		       b2[0]= labelPos[0];               b2[1]= labelPos[1]+ (labelSize[1]/2.0)+1;		   
		   }
				
    		//TextBox überhaupt im Bildschirm?  kann durch tausch durch resolveLineLineIntersections() auftreten
		   if (b1[0] < 0 || b2[0] > _size[0] || b1[1] < 0 || b2[1] > _size[1])
		   {
			   //std::cout << "visible=false in controllIntersections for i=" << i << std::endl;
			   (*myLabelObjects)[i]->labelState.visible=false;
			   (*myLabelObjects)[i]->referenceLine.comment2="--bei controllIntersections auf invisible gestellt da außerhalb des Bildschirms--";
		   }
		
		   for (int j=(int)myLabelObjects->size()-1; j>=0; j--) { //rückwärts durchlaufen, damit die unwichtigen Labels zuerst ausgeblendet werden bei überschneidungen
			   if ( (*myLabelObjects)[j]->labelState.visible && j!=i && (*myLabelObjects)[j]->on &&
					labelPos != (*myLabelObjects)[j]->refLineNewPos.getValue() ) {	//gleich positionierte Labels bei multiref dürfen/müssen überlappen			
					SbVec2s l1, l2;
					l1=(*myLabelObjects)[j]->referenceLine.object.getValue();
					l2=(*myLabelObjects)[j]->refLineNewPos.getValue();
					
					if ( lineIntersectBox(b1,b2,l1,l2 ) ) //schnittpunkte refLine <-> TextBox?
					{
						//std::cout << "visible=false in controllIntersections2 for j=" << j << std::endl;
					   (*myLabelObjects)[j]->labelState.visible=false;	
					   (*myLabelObjects)[j]->referenceLine.comment3="-- ausgeblendet weil refLine<->TextBox intersection wohl angeblich--";
					}
					else //noch checken ob Label sich mit Label schneidet TextBox <-> TextBox
					{ 						
						SbVec2s ts_j = (*myLabelObjects)[j]->getTextSize();	
						SbBox2s objI(b1, b2);
						SbBox2s objJ;
						//l2 = labelPos, l1=objektPos
						if ( l2[0] > l1[0] ){ //isLeft=false
							objJ.setBounds( SbVec2s(l2[0],l2[1]-(ts_j[1]/2.0)-1), SbVec2s(l2[0]+ts_j[0], l2[1]+ (ts_j[1]/2.0)+1) ); 											
						} else {
							objJ.setBounds( SbVec2s(l2[0]-ts_j[0],l2[1]-(ts_j[1]/2.0)-1), SbVec2s(l2[0], l2[1]+ (ts_j[1]/2.0)+1) ); 		               
						} 
						
						if (objI.intersect(objJ) )
						{
							//std::cout << "visible=false in controllIntersections3 for j=" << j << std::endl;
						   (*myLabelObjects)[j]->labelState.visible=false;
						   (*myLabelObjects)[j]->referenceLine.comment4="-- ausgeblendet weil TextBox <-> TextBox intersection--";
						}

					}//end else					
				}
			}
		}
	}	
}

void SoAnnotator::resolveLineLineIntersections() { // löst Schnitte von Ref.-Linien untereinander auf
	unsigned int i,j;
	SbVec2s help;
	//std::cout << "resolveLineLineIntersections" << std::endl;
	if( _useOrthoStyle==false )
	{
		for (i=0; i<myLabelObjects->size(); i++) {
			if ( (*myLabelObjects)[i]->labelState.visible && (*myLabelObjects)[i]->on ) {
				for (j=0; j<myLabelObjects->size(); j++) {
					if ( (*myLabelObjects)[j]->labelState.visible &&  //i auf j geändert			   
						(*myLabelObjects)[j]->on && i != j) { //

							if (isCrossed((*myLabelObjects)[i]->referenceLine.object.getValue(), (*myLabelObjects)[i]->refLineNewPos.getValue(), 
								(*myLabelObjects)[j]->referenceLine.object.getValue(), (*myLabelObjects)[j]->refLineNewPos.getValue() )) 
							{				   
								help           = (*myLabelObjects)[i]->refLineNewPos.getValue(); // x UND y vertauschen (im gegensatz zu labelLine
								(*myLabelObjects)[i]->refLineNewPos.setValue( (*myLabelObjects)[j]->refLineNewPos.getValue()   );
								(*myLabelObjects)[j]->refLineNewPos = help;              
							}			
					}
				}
			}
		}
	}
	else
	{	
		SbVec2s hs1, hs2, vs1, vs2;	
		for (i=0; i<myLabelObjects->size(); i++) {
			if ( (*myLabelObjects)[i]->labelState.visible && (*myLabelObjects)[i]->on ) {
				for (j=0; j<myLabelObjects->size(); j++) {
					if ( (*myLabelObjects)[j]->labelState.visible && (*myLabelObjects)[j]->on && i != j) {  //i auf j geändert				   
						hs1.setValue( (*myLabelObjects)[i]->referenceLine.object.getValue()[0], (*myLabelObjects)[i]->refLineNewPos.getValue()[1] );
						hs2=(*myLabelObjects)[i]->refLineNewPos.getValue();
						vs1.setValue( (*myLabelObjects)[j]->referenceLine.object.getValue()[0], (*myLabelObjects)[j]->refLineNewPos.getValue()[1] );
						vs2=(*myLabelObjects)[j]->referenceLine.object.getValue();
						if (isCrossed( hs1, hs2, vs1, vs2)) {
							//tausche LabelPosition
							help           = (*myLabelObjects)[i]->refLineNewPos.getValue(); // x UND y vertauschen (im gegensatz zu labelLine
							(*myLabelObjects)[i]->refLineNewPos.setValue( (*myLabelObjects)[j]->refLineNewPos.getValue()   );
							(*myLabelObjects)[j]->refLineNewPos = help;  
						}
					}
				}
			}
		}

	} 
}

bool SoAnnotator::agentExternalAnchorStillValid(int id, short displacementX, short displacementY) {
  short posx, posy;

  posx = (*myLabelObjects)[id]->referenceLine.object.getValue()[0] + displacementX;
  posy = (*myLabelObjects)[id]->referenceLine.object.getValue()[1] + displacementY;
  
  if (posx < 0 || posx >= _size[0] || posy < 0 || posy >= _size[1]) return false;
  if (_bufferID[posy*_size[0]*_obscureFactor+posx*_obscureFactor] != id+1) return false;  //...vorher != id)
  return true;
}

void SoAnnotator::agentExternalAnchorCheckNeighborhoodDF(int id) {    
  point pos[9];
  float score[9];
  int i, topIndex;
  short posx, posy;

  pos[0].x =  0;  pos[0].y =  0;   // mitte, alternative für gehen
  pos[1].x = -1;  pos[1].y =  0;   // links 
  pos[2].x = +1;  pos[2].y =  0;   // rechts  
  pos[3].x =  0;  pos[3].y = -1;   // unten 
  pos[4].x =  0;  pos[4].y = +1;   // oben  
  pos[5].x = -1;  pos[5].y = +1;   // oben links
  pos[6].x = +1;  pos[6].y = +1;   // oben rechts
  pos[7].x = -1;  pos[7].y = -1;   // unten links
  pos[8].x = +1;  pos[8].y = -1;   // unten rechts

  for (i=0; i<9; i++) {
    if (agentExternalAnchorStillValid(id, pos[i].x * _skipLevel, pos[i].y * _skipLevel)) { // ist Beschriftung hier überhaupt möglich?
      posx = (*myLabelObjects)[id]->referenceLine.object.getValue()[0] + pos[i].x * _skipLevel;
      posy = (*myLabelObjects)[id]->referenceLine.object.getValue()[1]+ pos[i].y * _skipLevel;
      score[i] = (float) _bufferDT[posy/_skipLevel*_size[0] + posx/_skipLevel];
    }
    else score[i] = 0.0; 
  }
  topIndex = whoIsTop(score, 9); // wer hat den größen wert

  // Position angleichen
  (*myLabelObjects)[id]->referenceLine.object.setValue( (*myLabelObjects)[id]->referenceLine.object.getValue()[0] + (pos[topIndex].x * _skipLevel), 
	                                                 (*myLabelObjects)[id]->referenceLine.object.getValue()[1] + (pos[topIndex].y * _skipLevel));
}

void SoAnnotator::agentExternalTextCheckNeighborhood(int id)
{
	short posx, posy, p2x, p2y;
	float score[255], scoreOrbit1, scoreOrbit2;
	int i, nearestNeighborLabel, nearestOrbit1, nearestOrbit2, orbitStart, orbitEnd, n;

	// erstmal nächstgelegenen Nachbarn finden
	score[0] = 0.0;
	posx = (*myLabelObjects)[id]->referenceLine.label.getValue()[0];
	posy = (*myLabelObjects)[id]->referenceLine.label.getValue()[1];

	for (i=0; i<(int)myLabelObjects->size(); i++) {
		if (i != id && (*myLabelObjects)[i]->labelState.visible && (*myLabelObjects)[i]->on) {
			p2x = (*myLabelObjects)[i]->referenceLine.label.getValue()[0];
			p2y = (*myLabelObjects)[i]->referenceLine.label.getValue()[1];
			score[i] = 1000.0 - sqrt((float) ((posx - p2x)*(posx - p2x) + 
				(posy - p2y)*(posy - p2y)));  // e-Dist
		}
		else score[i] = 0.0;
	}
	nearestNeighborLabel = whoIsTop(score, (*myLabelObjects).size()); // das mit der kleinsten Distanz


	// nun die nähesten beiden Orbitpunkte zum eigentlichen Label ermitteln
	orbitStart = (*myLabelObjects)[id]->referenceLine.orbitIndex - 5; // Optimierung: nur die nächsten paar Orbitwerte untersuchen

	if (orbitStart < 0) orbitStart = 0;
	orbitEnd   = (*myLabelObjects)[id]->referenceLine.orbitIndex + 5;  
	if (orbitEnd >= _orbitcounter) orbitStart = _orbitcounter - 1;
	posx = (*myLabelObjects)[id]->referenceLine.label.getValue()[0];
	posy = (*myLabelObjects)[id]->referenceLine.label.getValue()[1];
	for (i=orbitStart, n=0; i<=orbitEnd; i++, n++)
	{
		if (i != id)
		{
			score[n] = 1000.0 - sqrt((float) ((posx - _orbit[i].x)*(posx - _orbit[i].x) + 
				(posy - _orbit[i].y)*(posy - _orbit[i].y)));  // e-Dist
		}
		else score[n] = 0.0;
	}  
	nearestOrbit1 = whoIsTop(score, 11) + orbitStart;
	score[nearestOrbit1 - orbitStart] = 0.0; // der ist aus dem Rennen
	nearestOrbit2 = whoIsTop(score, 11) + orbitStart;

	// nun den Orbitpunkt wählen der die größere Distanz zum Nachbarlabel hat
	posx = (*myLabelObjects)[nearestNeighborLabel]->referenceLine.label.getValue()[0]; //referenceLine[nearestNeighborLabel].label.x;
	posy = (*myLabelObjects)[nearestNeighborLabel]->referenceLine.label.getValue()[1]; //referenceLine[nearestNeighborLabel].label.y;
	p2x = _orbit[nearestOrbit1].x; 
	p2y = _orbit[nearestOrbit1].y; 
	scoreOrbit1 = sqrt((float) ((posx - p2x)*(posx - p2x) + (posy - p2y)*(posy - p2y)));
	p2x = _orbit[nearestOrbit2].x; 
	p2y = _orbit[nearestOrbit2].y; 
	scoreOrbit2 = sqrt((float) ((posx - p2x)*(posx - p2x) + (posy - p2y)*(posy - p2y)));

	if (scoreOrbit1 > scoreOrbit2) { // wenn scoreOrbit1 größer -> geh zu diesem
		SbVec2s labelSize = (*myLabelObjects)[id]->getTextSize();
		//(*myLabelObjects)[id]->labelState.alreadyProcessed = false;//DEBUG-GUCKEN obs geht
		if (validExternalPlacement(_orbit[nearestOrbit1].x, _orbit[nearestOrbit1].y, labelSize[0], labelSize[1], (*myLabelObjects)[id]->isJustificationLeft() )) { 
			(*myLabelObjects)[id]->referenceLine.label.setValue( _orbit[nearestOrbit1].x, _orbit[nearestOrbit1].y );
			(*myLabelObjects)[id]->referenceLine.orbitIndex = nearestOrbit1;
			//printf("Agent");
		}
		//(*myLabelObjects)[id]->labelState.alreadyProcessed = true; //DEBUG gucken obs geht
	}
	else { // wenn nicht, geh zum andern
		SbVec2s labelSize = (*myLabelObjects)[id]->getTextSize();
		//(*myLabelObjects)[id]->labelState.alreadyProcessed = false; //DEBUG gucken obs geht
		if (validExternalPlacement(_orbit[nearestOrbit2].x, _orbit[nearestOrbit2].y, labelSize[0], labelSize[1],  (*myLabelObjects)[id]->isJustificationLeft() )) { 
			(*myLabelObjects)[id]->referenceLine.label.setValue( _orbit[nearestOrbit2].x, _orbit[nearestOrbit2].y );
			(*myLabelObjects)[id]->referenceLine.orbitIndex = nearestOrbit2;			
		}
		//(*myLabelObjects)[id]->labelState.alreadyProcessed = true; //DEBUG, gucken obs geht
	}
}


bool SoAnnotator::isCrossed( SbVec2s a, SbVec2s b, SbVec2s c, SbVec2s d) {
  /* Check whether line segment [a,b] crosses segment [c,d]. Fast method due to Amie Wilkinson. */
  double determinant, b1, b2, ax, ay, bx, by, cx, cy, dx, dy;

  ax = a.getValue()[0]; ay = a.getValue()[1];
  bx = b.getValue()[0]; by = b.getValue()[1];
  cx = c.getValue()[0]; cy = c.getValue()[1];
  dx = d.getValue()[0]; dy = d.getValue()[1];

  bx -= ax; by -= ay; cx -= ax; cy -= ay; dx -= ax; dy -= ay;

  determinant = dx*cy - dy*cx;
  if (determinant == 0.) return false;
  b1 = (cy*bx - cx*by) / determinant;
  if (b1 <= 0.) return false;
  b2 = (dx*by - dy*bx) / determinant;
  if (b2 <= 0) return false;
  if (b1+b2 <= 1.) return false;
  return true;
}   

void SoAnnotator::ermittleKonvexeHuelle() { //zur einer priorisierung 
	point temp[255], hull[255];
	unsigned int j;
	int i,zaehler, hullZaehler;

	for (i=0, zaehler=0; i<(int)myLabelObjects->size(); i++) {
		//if ( (*myLabelObjects)[i]->labelState.visible) {
		if ( ( (*myLabelObjects)[i]->labelState.visible || (*myLabelObjects)[i]->drawIfInvis )
			&& (*myLabelObjects)[i]->on )
		{
			temp[zaehler].x = (*myLabelObjects)[i]->referenceLine.object.getValue()[0];
			temp[zaehler].y = (*myLabelObjects)[i]->referenceLine.object.getValue()[1];
			zaehler++;
		}
	}
	qsort(temp, zaehler, sizeof(point), (int (__cdecl *)(const void *, const void *)) punkteSortierKernel); // sortiere nach x und y
	hullZaehler = chainHull_2D(temp, zaehler, hull) - 1;
	//std::cout << "HULLS:" << hullZaehler << std::endl;
	// nun eintragen in labelState
	for (i=0; i<hullZaehler; i++) { // für jedes Element der hülle
		for (j=0; j<myLabelObjects->size(); j++) { // jedes Label auf gleiche Position prüfen
			if ((*myLabelObjects)[j]->on)
			{
				if (hull[i].x == (*myLabelObjects)[j]->referenceLine.object.getValue()[0] && 
					hull[i].y == (*myLabelObjects)[j]->referenceLine.object.getValue()[1]) 
					(*myLabelObjects)[j]->labelState.onConvexHull = true;
			}
		}
	}
}


int SoAnnotator::chainHull_2D(point *P, int n, point *H) {
  // chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
  //     Input:  P[] = an array of 2D points
  //                   presorted by increasing x- and y-coordinates
  //             n = the number of points in P[]
  //     Output: H[] = an array of the convex hull vertices (max is n)
  //     Return: the number of points in H[]
  int    bot=0, top=(-1);  
  int    i;              

  int minmin = 0, minmax;
  float xmin = P[0].x;
  for (i=1; i<n; i++) if (P[i].x != xmin) break; 
  minmax = i-1;
  if (minmax == n-1) {      
    H[++top] = P[minmin];
    if (P[minmax].y != P[minmin].y) H[++top] = P[minmax];
    H[++top] = P[minmin];          
    return top+1;
  }

  int maxmin, maxmax = n-1;
  float xmax = P[n-1].x;
  for (i=n-2; i>=0; i--) if (P[i].x != xmax) break;
  maxmin = i+1;
  H[++top] = P[minmin];    
  i = minmax;
  while (++i <= maxmin) {
    if (isLeft( P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin) continue;     
    while (top > 0) {
      if (isLeft( H[top-1], H[top], P[i]) > 0) break;       
      else top--;      
    }
    H[++top] = P[i];   
  }
  if (maxmax != maxmin) H[++top] = P[maxmax]; 
  bot = top;                
  i = maxmin;
  while (--i >= minmax) {
    if (isLeft( P[maxmax], P[minmax], P[i]) >= 0 && i > minmax) continue;        
    while (top > bot) {
      if (isLeft( H[top-1], H[top], P[i]) > 0) break;      
      else top--;       
    }
    H[++top] = P[i];     
  }
  if (minmax != minmin) H[++top] = P[minmin];
  return top+1;
}

inline float SoAnnotator::isLeft(point P0, point P1, point P2) {
  // isLeft(): tests if a point is Left|On|Right of an infinite line.
  //    Input:  three points P0, P1, and P2
  //    Return: >0 for P2 left of the line through P0 and P1
  //            =0 for P2 on the line
  //            <0 for P2 right of the line
  //    See: the January 2001 Algorithm on Area of Triangles
  return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}

//************************************************************************
//************************************************************************ 



//************************************************************************
//************************************************************************
/*void SoAnnotator::normalisiere(float value[100], int numValues) { // normalisiert und werte spannen komplett zwischen 0.0 und 1.0
  // Ausnahme: wenn alle eingangswerte gleich, dann werden alle ausgangswerte 0.0
  // ein value mit -1000.0 bedeutet, dass dies auf keinen fall zu wählen ist... bsp. rand
  float minValue = +1000.0, maxValue = -1000.0, normaliser;
  int i;

  for (i=0; i<numValues; i++) { // unteres Limit
    if (value[i] < minValue && value[i] > -999.0) minValue = value[i];
  }
  for (i=0; i<numValues; i++) { // oberes Limit
    if (value[i] > maxValue && value[i] > -999.0) maxValue = value[i];
  }


  for (i=0; i<numValues; i++) { // kleinster Wert ist nun 0.0
    if (value[i] > -999.0) value[i] -= minValue;
  }  
  if (fabs(maxValue - minValue) > 0.0) { // keine Teilung durch 0m wenn alle gleich
    normaliser = 1.0 / (maxValue - minValue); // max. Länge ist nun 1.0
    for (i=0; i<numValues; i++) {
      if (value[i] > -999.0) value[i] *= normaliser;
    }
  }
}*/

void SoAnnotator::normalisiere(std::vector<float>& value) { // normalisiert und werte spannen komplett zwischen 0.0 und 1.0
  // Ausnahme: wenn alle eingangswerte gleich, dann werden alle ausgangswerte 0.0
  // ein value mit -1000.0 bedeutet, dass dies auf keinen fall zu wählen ist... bsp. rand
  float minValue = +1000.0, maxValue = -1000.0, normaliser;
  unsigned int i;
  int defaultNorm;
/*
  for (i=0; i<value.size(); i++) { // unteres Limit	  
    if (value[i] < minValue && value[i] > -999.0) minValue = value[i];	
  }
  for (i=0; i<value.size(); i++) { // oberes Limit	
    if (value[i] > maxValue && value[i] > -999.0) maxValue = value[i];
  }
  
  for (i=0; i<value.size(); i++) { // kleinster Wert ist nun 0.0
    if (value[i] > -999.0) value[i] -= minValue;
  }  
  if (fabs(maxValue - minValue) > 0.0) { // keine Teilung durch 0m wenn alle gleich
    normaliser = 1.0 / (maxValue - minValue); // max. Länge ist nun 1.0
    for (i=0; i<value.size(); i++) {
      if (value[i] > -999.0) value[i] *= normaliser;
    }
  }*/

  /*for (i=0; i<value.size(); i++) {
	float test = value[i];
  }*/

  for (i=0; i<value.size(); i++ ) {
	if ( value[i] < -1000 ) value[i]=-1000;
	if ( value[i] >  1000 ) value[i]= 1000;
	if ( value[i] < minValue ) minValue = value[i];
	if ( value[i] > maxValue ) maxValue = value[i];
	value[i]+=1000; //wert ist jetzt definitv positiv
  }
  
  if (minValue==maxValue) {
	  if (minValue==0.0) defaultNorm=0.0;
	  else defaultNorm=1.0;
	  for (i=0; i<value.size(); i++) value[i]=defaultNorm;
  }else {
	  normaliser = 1.0 / (maxValue - minValue); // max. Länge wird nun 1.0
      for (i=0; i<value.size(); i++) {
	    value[i]-=(1000-minValue); // kleinster Wert ist nun 0.0
       value[i] *= normaliser;
	  }
  }

  //DEBUG
  /*for (i=0; i<value.size(); i++) {
	float test = value[i];
  }*/

}

/*
void SoAnnotator::normalisiere(float valueIn[100], float valueOut[100], int numValues) { // normalisiert und werte spannen komplett zwischen 0.0 und 1.0
  // Ausnahme: wenn alle eingangswerte gleich, dann werden alle ausgangswerte 0.0
  // ein value mit -1000.0 bedeutet, dass dies auf keinen fall zu wählen ist... bsp. rand
  float minValue = +1000.0, maxValue = -1000.0, normaliser;
  int i;

  memcpy(valueOut, valueIn, sizeof(float) * numValues);
  for (i=0; i<numValues; i++) { // unteres Limit
    if (valueOut[i] < minValue && valueOut[i] > -999.0) minValue = valueOut[i];
  }
  for (i=0; i<numValues; i++) { // oberes Limit
    if (valueOut[i] > maxValue && valueOut[i] > -999.0) maxValue = valueOut[i];
  }


  for (i=0; i<numValues; i++) { // kleinster Wert ist nun 0.0
    if (valueOut[i] > -999.0) valueOut[i] -= minValue;
  }  
  if (fabs(maxValue - minValue) > 0.0) { // keine Teilung durch 0m wenn alle gleich
    normaliser = 1.0 / (maxValue - minValue); // max. Länge ist nun 1.0
    for (i=0; i<numValues; i++) {
      if (valueOut[i] > -999.0) valueOut[i] *= normaliser;
    }
  }
}
*/

int  SoAnnotator::whoIsTop( std::vector<float>& value ) {
  float maxValue = -1000.0;
  unsigned int i;
  int maxIndex = 0;

  for (i=0; i<value.size(); i++) {
    if (value[i] > maxValue) {
      maxValue = value[i];
      maxIndex = i;
    }
  }
  return(maxIndex);
}

int  SoAnnotator::whoIsTop(float value[100], int numValues) {
  float maxValue = -1000.0;
  int i, maxIndex = 0;

  for (i=0; i<numValues; i++) {
    if (value[i] > maxValue) {
      maxValue = value[i];
      maxIndex = i;
    }
  }
  return(maxIndex);
}


void SoAnnotator::saveBufferAsBMP()
{

	unsigned long val, val1;
	image_basic test(_size[0], _size[1],COLOR_24BPP);
	image_basic test1(_size[0], _size[1],COLOR_24BPP);
	unsigned long bufferVal, bufferVal1;
	for (int y = 0;y<_size[1]; y++)
	{
		for (int x = 0;x<_size[0]; x++)
		{
			bufferVal = _bufferDT[y*_size[0] + x];
			bufferVal1 = _bufferDT1[y*_size[0] + x];
			val = ((long)bufferVal << 16) | ((long)bufferVal << 8) | ((long)bufferVal);
			val1 = ((long)bufferVal1 << 16) | ((long)bufferVal1 << 8) | ((long)bufferVal1);
			test.set(x,y,val);
			test1.set(x,y,val1);
		}
	}
	std::cout<<"saving image"<<std::endl;
	test.file_write_bitmap("c:\\bufferDT.bmp");
	test1.file_write_bitmap("c:\\bufferDT1.bmp");
}