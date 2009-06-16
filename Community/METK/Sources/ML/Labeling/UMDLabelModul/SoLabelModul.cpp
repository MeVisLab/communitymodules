//----------------------------------------------------------------------------------
//! The Inventor module class SoLabelModul
/*!
// \file    SoLabelModul.cpp
// \author  Maik Beyer
// \date    2007-01-31
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoLabelModul.h"

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


SO_NODE_SOURCE(SoLabelModul)

using namespace std;

#define TEXTRENDERMODE PIXMAP //TEXTURE, EXTRUDED, OUTLINE, POLYGON, PIXMAP, BITMAP

// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoLabelModul::initClass()
{
	SO_NODE_INIT_CLASS(SoLabelModul, SoSeparator, "Separator");    
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoLabelModul::SoLabelModul(){
	SO_TRACE_IN("SoLabelModul::SoLabelModul");

		// Execute inventor internal stuff for node construction.
		SO_NODE_CONSTRUCTOR(SoLabelModul);

	SO_NODE_ADD_FIELD(input,(0));
	SO_NODE_ADD_FIELD( outputColorBufferView,(0) );
	SO_NODE_ADD_FIELD( outputMyAnnotatorOutput,(0) );

	SO_NODE_ADD_FIELD( clear,() );
	SO_NODE_ADD_FIELD( start, () );

	//MÜSSEN NOCH AUF LOKALE PFADE UMGEBOGEN WERDEN!!!
	SO_NODE_ADD_FIELD( fontFile, ("c:/windows/Fonts/arial.ttf") );	
	SO_NODE_ADD_FIELD( fontSize, (12) );
	SO_NODE_ADD_FIELD( linespacing, (1) );
	SO_NODE_ADD_FIELD( insetspace, (5) );
	SO_NODE_ADD_FIELD( refLineWidth, (1.0) );
	SO_NODE_ADD_FIELD( fontColor, ( 0.0, 0.0, 0.0 ) );	//fld_labelTextColor

	//SO_NODE_ADD_FIELD( flushLayout, (true) );

	SO_NODE_ADD_FIELD( haloColorInvert, (true) );
	SO_NODE_ADD_FIELD( orthoRefLines, (false) ); 

	SO_NODE_ADD_FIELD(invisObjStyle, (none));	
	SO_NODE_DEFINE_ENUM_VALUE(invisStyles, none);
	SO_NODE_DEFINE_ENUM_VALUE(invisStyles, dashedLine);
	SO_NODE_DEFINE_ENUM_VALUE(invisObjStyle, solidLine);
	SO_NODE_DEFINE_ENUM_VALUE(invisStyles, arrow);	
	SO_NODE_SET_SF_ENUM_TYPE(invisObjStyle, invisStyles);



	SO_NODE_ADD_FIELD( arrowColor, (0.0, 0.0, 0.0) );
	SO_NODE_ADD_FIELD( refLineColor, (0.0, 0.0, 0.0) );
	SO_NODE_ADD_FIELD( refLineHaloColor, (1.0, 1.0, 1.0) );
	SO_NODE_ADD_FIELD( arrowColorHalo, (1.0, 1.0, 1.0) );
	SO_NODE_ADD_FIELD( arrowSize, (10) );
	SO_NODE_ADD_FIELD( arrowThickness, (1.5) );

	SO_NODE_ADD_FIELD( bgColor,(1.0, 1.0, 1.0) );  
	SO_NODE_ADD_FIELD( defaultLabelBgColor,(0.8,0.8,0.8) );
	SO_NODE_ADD_FIELD( labelBGAlpha, (1.0) );
	SO_NODE_ADD_FIELD( labelIfInteraction, (true) );
	//SO_NODE_ADD_FIELD( labelIfInvis, (false) );
	SO_NODE_ADD_FIELD( labelBackground, (false) );
	SO_NODE_ADD_FIELD( motionLabeling, (true) );


	SO_NODE_ADD_FIELD( debug_drawOrbit, (false) );	
	SO_NODE_ADD_FIELD( printLabelObjects, () );

	SO_NODE_ADD_FIELD( skipLevel, (3) );


	SO_NODE_ADD_FIELD( mouseListener, (true) );

	/*	SO_NODE_ADD_FIELD( cso_coordinates, (0.0, 0.0, 0.0) );
	SO_NODE_ADD_FIELD( curObject, (0) );

	SO_NODE_ADD_FIELD( frameCoherenceAgent, (true) );
	SO_NODE_ADD_FIELD( getObjectData, () );	
	SO_NODE_ADD_FIELD( isInteraction, (false) );
	SO_NODE_ADD_FIELD( labelColorID, (0) );
	SO_NODE_ADD_FIELD( labelOn, (true) );
	SO_NODE_ADD_FIELD( labelPriority, (1) );
	SO_NODE_ADD_FIELD( fld_labelText, ("") );
	*/
	SO_NODE_ADD_FIELD( mouseClickLeft, (true) );
	SO_NODE_ADD_FIELD( mouseClickMiddle, (false) );
	SO_NODE_ADD_FIELD( mouseClickObject, (0) ); //CSO
	SO_NODE_ADD_FIELD( mouseClickRight, (false) );
	SO_NODE_ADD_FIELD( mouseOverObject, (0) );
	SO_NODE_ADD_FIELD( mousePos, ( 0.0, 0.0) );

	SO_NODE_ADD_FIELD( orbitDistance, (20) );	

	/*
	SO_NODE_ADD_FIELD( numberShapes,(0));
	SO_NODE_ADD_FIELD( objectIsBackground, (false) );
	SO_NODE_ADD_FIELD( on, (true) );	
	SO_NODE_ADD_FIELD( refLineColor, (0.0, 0.0, 0.0) );
	SO_NODE_ADD_FIELD( refLineHalo, (true) );
	SO_NODE_ADD_FIELD( refLineHaloColor, (1.0, 1.0, 1.0) );	
	SO_NODE_ADD_FIELD( refLineWidth, (1.0) );
	SO_NODE_ADD_FIELD( resetParameter, () );
	SO_NODE_ADD_FIELD( resetParameterForAll, () );
	SO_NODE_ADD_FIELD( resetStyle, () );
	SO_NODE_ADD_FIELD( resetStyleToAll, () );
	SO_NODE_ADD_FIELD( setLabelData, () );
	SO_NODE_ADD_FIELD( setLabelDataToAll, () );	
	SO_NODE_ADD_FIELD( status_label, (false) );
	SO_NODE_ADD_FIELD( status_objectVisible, (false) );
	SO_NODE_ADD_FIELD( status_refPoint, (0.0, 0.0) );
	SO_NODE_ADD_FIELD( status_textPos, (0.0, 0.0) );
	SO_NODE_ADD_FIELD( status_textSize, (0.0, 0.0) );
	SO_NODE_ADD_FIELD( updateTextSize, () );
	SO_NODE_ADD_FIELD( w_angle, (.2) );
	SO_NODE_ADD_FIELD( w_angle_io, (0.0) );
	SO_NODE_ADD_FIELD( w_delta, (0.8) );
	SO_NODE_ADD_FIELD( w_delta_io, (0.8) );
	SO_NODE_ADD_FIELD( w_intersection, (0.8) );
	SO_NODE_ADD_FIELD( w_intersection_io, (0.8) );
	SO_NODE_ADD_FIELD( w_length, (0.9) );
	SO_NODE_ADD_FIELD( w_length_io, (1.0) );
	SO_NODE_ADD_FIELD( w_neighbor, (0.7) );
	SO_NODE_ADD_FIELD( w_neighbor_io, (0.0) );
	*/


	// EventCallback fuer die Mousebutton-Ereignisse
	SoEventCallback* eventCB = new SoEventCallback;
	//eventCB-ref(); //TODO checken, ob notwendig
	addChild(eventCB);
	eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousePressedCB, this);
	eventCB->addEventCallback(SoLocation2Event::getClassTypeId(), mouseMovedCB, this);

	_nodeSensor = new SoNodeSensor(SoLabelModul::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);

	mouseDown=false;

	initLabelModul();
}


// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoLabelModul::~SoLabelModul(){
	//std::cout << "START DESTRUKTOR SoLabelModul" <<std::endl;
	SO_TRACE_IN("SoLabelModul::~SoLabelModul");

		// Remove the node sensor.
		if (_nodeSensor){ delete _nodeSensor;}// _nodeSensor = 0; } 
		cleanUp();

		root->unref();
		//	switchOnOff->unref();
		sceneModel->unref();
		colorIDModel->unref();  
		colorIDModelUpToDate=false;  
		myTextModul->unref();
		delete myTextModul;
		//std::cout << "ENDE DESTRUKTOR SoLabelModul" <<std::endl;
}



// --------------------------------------------------------------------------
//! Init Data Structure
// --------------------------------------------------------------------------
void SoLabelModul::initLabelModul()
{
	kDebug::Debug("initLabelModul",kDebug::DL_NONE);
	colorIDModelUpToDate=false;  

	if(SoTextModul::getClassTypeId() == SoType::badType()) SoTextModul::initClass();
	myTextModul = new SoTextModul();
	myTextModul->ref();

	bool validFonts = initFonts(); //fontPathValid.setValue( initFont() );      
	if (!validFonts) kDebug::Debug("Invalid Font path or something like that", kDebug::DL_NONE);

	if(SoLabelObject::getClassTypeId() == SoType::badType()) SoLabelObject::initClass();

	if(SoAnnotator::getClassTypeId() == SoType::badType()) SoAnnotator::initClass();
	myAnnotator = new SoAnnotator();
	myAnnotator->ref();
	initAnnotator(); 	

	if(SoIdBufferNode::getClassTypeId() == SoType::badType()) SoIdBufferNode::initClass(); 
	idBufferNode = new SoIdBufferNode();
	idBufferNode->ref();
	idBufferNode->setSoAnnotator(myAnnotator);

	if(SoFlatShadingNode::getClassTypeId() == SoType::badType()) SoFlatShadingNode::initClass();
	flatShadingNode = new SoFlatShadingNode(); 
	flatShadingNode->ref();

	if(SoPhongShadingNode::getClassTypeId() == SoType::badType()) SoPhongShadingNode::initClass();
	phongShadingNode = new SoPhongShadingNode();
	phongShadingNode->ref();

	if(SoBackgroundColor::getClassTypeId() == SoType::badType()) SoBackgroundColor::initClass();
	whiteBackground = new SoBackgroundColor(1.0, 1.0, 1.0);
	whiteBackground->ref();	
	curBackground = new SoBackgroundColor( bgColor.getValue()[0], bgColor.getValue()[1], bgColor.getValue()[2] );
	curBackground->ref();

	root = new SoSeparator();
	root->ref();
	sceneModel = new SoSeparator();
	sceneModel->ref();

	colorIDModel = new SoSeparator();	
	colorIDModel->ref();
	this->addChild(root);

	myCube = new SoCube();
	myCube->ref();
	myCube->width=100;myCube->height=100;myCube->depth=100;
	//root->addChild(myCube);
	this->touch();
}


void SoLabelModul::initAnnotator()
{
	kDebug::Debug("SoLabelModul::initAnnotator()");
	myAnnotator->init( myLabelObjects );

	myAnnotator->setOrbitDistance(orbitDistance.getValue());
	myAnnotator->setBufferSkipLevel(skipLevel.getValue());
	//xxx ^^ auf 1 setzen? war 4
	myAnnotator->setWeightAngle(0.2);//myAnnotator->setWeightAngle( w_angle.getValue() );
	myAnnotator->setWeightDelta(0.8);//myAnnotator->setWeightDelta( w_delta.getValue() );
	myAnnotator->setWeightIntersection(0.8);//myAnnotator->setWeightIntersection( w_intersection.getValue() );
	myAnnotator->setWeightLength(0.9);//myAnnotator->setWeightLength( w_length.getValue() );
	myAnnotator->setWeightNeighbor(0.7);//myAnnotator->setWeightNeighbor( w_neighbor.getValue() );
	myAnnotator->setWeightAngleInvisObj(0.0);//myAnnotator->setWeightAngleInvisObj( w_angle_io.getValue() );
	myAnnotator->setWeightDeltaInvisObj(0.8);//myAnnotator->setWeightDeltaInvisObj( w_delta_io.getValue() );
	myAnnotator->setWeightLengthInvisObj(1.0);//myAnnotator->setWeightLengthInvisObj( w_length_io.getValue() );
	myAnnotator->setWeightNeighborInvisObj(0.0);//myAnnotator->setWeightNeighborInvisObj( w_neighbor_io.getValue() );
	myAnnotator->setWeightIntersectionInvisObj(0.8);//myAnnotator->setWeightIntersectionInvisObj( w_intersection_io.getValue() );   
	myAnnotator->setAutoUpdate(true);
}


bool SoLabelModul::initFonts()
{
	kDebug::Debug("Fonts init");
	myTextModul->fontFile.setValue( SoLabelModul::fontFile.getValue() );	

	if ( myTextModul->fontValid.getValue() )
	{
		myTextModul->string.setValue( "Test123" ); //myTextModul->string.setValue( SoLabelModul::fld_labelText.getValue() );
		myTextModul->fontsize.setValue( fontSize.getValue() );		
		myTextModul->lineSpacing.setValue( linespacing.getValue());
		myTextModul->insetSpace.setValue( insetspace.getValue());
		myTextModul->color.setValue( fontColor.getValue() );
		myTextModul->backgroundColor.setValue( fontColor.getValue() );
		myTextModul->halo.setValue( 0 ); //kein schatten; halo später setzen
		myTextModul->haloColor.setValue( SbColor(1.0, 1.0, 1.0) );
		myTextModul->haloAlpha.setValue( 0.0 );
		myTextModul->blend.setValue(true);				
		//SCHWARZE SEITE DER MACHT //myTextModul->fontRenderType.setValue(SoTextModul::TEXTRENDERMODE); //TEXTURE, EXTRUDED, OUTLINE, POLYGON, PIXMAP, BITMAP
		myTextModul->alwaysOnTop.setValue(true);
		//myTextModul->background.setValue(false); //wird vom Modul hier gesetzt		
		myTextModul->underscore.setValue(false);
		myTextModul->freeze.setValue(true);

		myTextModul->initFTGLFont();

		myTextModul->applyFTGLFontParameter();//wichtig, weil sonst keine aktualisierung der Parameter stattfindet!
	}

	return myTextModul->fontValid.getValue();
}



void SoLabelModul::updateFontParameter( SoLabelObject* obj, SoTextModul* textModul)
{	
	textModul->string.setValue(obj->getLabelText().c_str());
	textModul->color.setValue( obj->getLabelTextColor() );
	textModul->colorAlpha.setValue( obj->getTransparency() );
	textModul->fontsize.setValue( obj->getFontSize()  );
	textModul->lineSpacing.setValue( obj->getLinespacing() );
	textModul->insetSpace.setValue( obj->getInsetSpace() );
	textModul->translation.setValue( obj->getPosition().getValue()[0], obj->getPosition().getValue()[1]);
	textModul->applyFTGLFontParameter();
}



void SoLabelModul::nodeChangedCB(void *data, SoSensor* sens)
{
	SO_TRACE_IN("SoLabelModul::nodeChangedCB")
		((SoLabelModul*) data)->nodeChanged((SoNodeSensor*)sens);
}



void SoLabelModul::nodeChanged(SoNodeSensor* sensor)
{
	SO_TRACE_IN("SoLabelModul::nodeChanged")

		// Get the field which caused the notification.
		SoField* field = sensor->getTriggerField();

	if (field==&input)
	{ 
		//std::cout << "Input-Invent" << std::endl;
		if (input.getValue()!= 0) //Input vorhanden?
		{			
			sceneModel->removeAllChildren();
			sceneModel->addChild( input.getValue());
			generateNewData();
		}
		else
		{
			cleanUp();
		}
		//switchOnOff->touch();
		this->touch();
	}

	else if (field==&clear)
	{
		cleanUp();
	}

	else if (field==&start)
	{
		input.touch();
		if (input.getValue())
		{
			//std::cout << "Start pressed and input.getValue()!=NULL " << std::endl;
			generateNewData();
		}
		else
		{
			//std::cout << "Start pressed and NO input.getValue()!!!!" << std::endl;
		}
	}

	/*else if ( field==&updateTextSize) //debug
	{
	for (int i = 0; i< myLabelObjects.size(); i++) myLabelObjects[i]->validTextBoxSize = false;
	}

	else if ( field == &resetParameter)
	{
	resetLabelParameter(curObject.getValue()-1 );
	}

	else if ( field == &resetParameterForAll)
	{
	for (int i=0; i<myLabelObjects.size(); i++) resetLabelParameter(i);
	}

	else if ( field == &setLabelData)
	{
	setLabelParameter( curObject.getValue()-1 );
	}

	else if ( field == &setLabelDataToAll)
	{
	for (int i=0; i<myLabelObjects.size(); i++) setLabelParameter(i);	   
	}
	*/
	else if ( field == &bgColor)
	{
		curBackground->setColor(bgColor.getValue()[0], bgColor.getValue()[1], bgColor.getValue()[2]);
	}

	else if ( field == &fontColor){
		vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
		myTextModul->setColor(fontColor.getValue());
		while(it!=myLabelObjects.end()){
			(*it)->setLabelTextColor(fontColor.getValue()[0],fontColor.getValue()[1],fontColor.getValue()[2]);
			it++;
		}
	}
	else if ( field == &refLineWidth){
		vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
		while(it!=myLabelObjects.end()){
			(*it)->setRefLineWidth(refLineWidth.getValue());
			it++;
		}
	}

	else if ( field == &refLineColor){
		vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
		while(it!=myLabelObjects.end()){
			(*it)->setRefLineColor(refLineColor.getValue()[0],refLineColor.getValue()[1],refLineColor.getValue()[2]);
			it++;
		}
	}

	else if ( field == &refLineHaloColor){
		vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
		while(it!=myLabelObjects.end()){
			(*it)->setRefLineHaloColor(refLineHaloColor.getValue()[0],refLineHaloColor.getValue()[1],refLineHaloColor.getValue()[2]);
			it++;
		}
	}

	else if ( field == &orthoRefLines)
	{
		myAnnotator->setOrthoStyle( orthoRefLines.getValue() );
	}	

	//ANNOTATOR-Parameter
	else if ( field == &orbitDistance)
	{
	myAnnotator->setOrbitDistance( orbitDistance.getValue() );
	}
	else if ( field == &skipLevel)
	{	
		myAnnotator->setBufferSkipLevel( skipLevel.getValue() );
	}
	/*else if ( field == &w_angle)
	{
	myAnnotator->setWeightAngle( w_angle.getValue() );
	}
	else if ( field == &w_delta) {
	myAnnotator->setWeightDelta( w_delta.getValue() );
	}
	else if ( field == &w_length) {		
	myAnnotator->setWeightLength( w_length.getValue() );
	}
	else if ( field == &w_neighbor) {		
	myAnnotator->setWeightNeighbor( w_neighbor.getValue() );
	}
	else if ( field == &w_intersection) {		
	myAnnotator->setWeightIntersection( w_intersection.getValue() );
	}
	else if ( field == &w_angle_io) {
	myAnnotator->setWeightAngleInvisObj( w_angle_io.getValue() );
	}
	else if ( field == &w_delta_io) {
	myAnnotator->setWeightDeltaInvisObj( w_delta_io.getValue() );
	}
	else if ( field == &w_length_io) {		
	myAnnotator->setWeightLengthInvisObj( w_length_io.getValue() );
	}
	else if ( field == &w_neighbor_io) {		
	myAnnotator->setWeightNeighborInvisObj( w_neighbor_io.getValue() );
	}
	else if ( field == &w_intersection_io) {		
	myAnnotator->setWeightIntersectionInvisObj( w_intersection_io.getValue() );
	}
	else if ( field == &frameCoherenceAgent) {
	myAnnotator->setUseAgent(frameCoherenceAgent.getValue() );
	}*/
	else if ( field == &debug_drawOrbit)
	{
		if (debug_drawOrbit.getValue() == 1 )
			myAnnotator->setDrawOrbitPoints(true);
		else  myAnnotator->setDrawOrbitPoints(false);		
	}	
	else if (field == &printLabelObjects)
	{
		if (myLabelObjects.size()==0)
		{
			std::cout << "myLabelObjects.size()==0" << std::endl;
		}
		for (int i=0;i<myLabelObjects.size();i++)
		{
			myLabelObjects[i]->printInfo();
		}
	}
}//end nodeChanged


void SoLabelModul::mousePressedCB(void* u/*userData*/, SoEventCallback* eventCB)
{
	//std::cout << "MOUSEPRESSEDCB" << std::endl;
	SoLabelModul* obj = (SoLabelModul*) u;
	obj->mousePressed(eventCB);
}


void SoLabelModul::mousePressed(SoEventCallback* eventCB)
{
	//std::cout << "MOUSEPRESSED" << std::endl;
	const SoEvent* event = eventCB->getEvent();
	if ( mouseListener.getValue()==1){	
		if (SO_MOUSE_RELEASE_EVENT(event, BUTTON1)){
			mouseButtonPressed=false;
			clear.touch();
			start.touch();
		}else if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)){
			mouseButtonPressed=true;
		}
	}
}



void SoLabelModul::mouseMovedCB(void* /*userData*/, SoEventCallback* /*eventCB*/)
{
	//std::cout << "MOVEMOUSE--------------------" << std::endl;
	//SoLabelModul* obj = (SoLabelModul*) userData;
	//obj->mouseMoved(eventCB);

}


void SoLabelModul::mouseMoved(SoEventCallback* /*eventCB*/)
{
	//mouseButtonPressed=true;
	//if ( mouseListener.getValue()== 1) getMousePosition(eventCB); else eventCB->setHandled();
}


int SoLabelModul::getMousePosition(SoEventCallback* /*eventCB*/)
{
	/*	int returnvalue=0;
	const SoEvent* event = eventCB->getEvent();   
	mousePos.setValue( event->getPosition()[0], event->getPosition()[1] ); 
	returnvalue = getObjectAtMousePos();
	eventCB->setHandled();
	return returnvalue;   */ return true;
}


int SoLabelModul::getObjectAtMousePos(void)
{
	/*	int i;
	float xmin, ymin, xmax, ymax;
	SbBox2f textBox;
	for (i=0; i<myLabelObjects.size(); i++ )
	{
	//position = obere linke ecke
	xmin = myLabelObjects[i]->getPosition().getValue()[0];
	ymin = myLabelObjects[i]->getPosition().getValue()[1] - myLabelObjects[i]->getTextSize().getValue()[1];
	xmax = myLabelObjects[i]->getPosition().getValue()[0] + myLabelObjects[i]->getTextSize().getValue()[0];
	ymax = myLabelObjects[i]->getPosition().getValue()[1];

	textBox.setBounds( xmin, ymin, xmax, ymax);

	if ( textBox.intersect( mousePos.getValue() ) == 1 )
	{
	mouseOverObject.setValue(i+1);
	cso_coordinates.setValue( myLabelObjects[i]->worldCenterBB.getValue() );
	return i+1;//Objektnummerierung 1...n; daher i+1!
	}
	}
	mouseOverObject.setValue(0);	
	return 0;*/ return true;
}



void SoLabelModul::calcTextSize( SoLabelObject* obj){
	myTextModul->string.setValue(obj->getLabelText().c_str());
	myTextModul->fontsize.setValue( obj->getFontSize() );
	myTextModul->lineSpacing.setValue( obj->getLinespacing() );
	myTextModul->insetSpace.setValue( obj->getInsetSpace() );
	myTextModul->applyFTGLFontParameter(); //Font initialiseren mit parametern (hier wichtig: FontSize!)
	obj->setTextSize( myTextModul->getTextSize( obj->getLabelText().c_str()) );
	//std::cout << "calcTextSize   id=" << obj->metkObjId << "  text=" << obj->getLabelText() << "  size=" << obj->getTextSize()[0] << "," << obj->getTextSize()[1] << std::endl;
	obj->validTextBoxSize=true;
}



void SoLabelModul::setStyleToLabel(SoLabelObject* obj)
{
	if ( obj)
	{  
		obj->setFontSize(fontSize.getValue());
		obj->setLinespacing(linespacing.getValue());
		obj->setInsetSpace(insetspace.getValue());
		obj->setLabelTextColor( fontColor.getValue());//.0, 0.0, 0.0, 0.0 );	  
		obj->setBackgroundColor(defaultLabelBgColor.getValue()[0],defaultLabelBgColor.getValue()[1],defaultLabelBgColor.getValue()[2]);		
		obj->setRefLineColor ( refLineColor.getValue() );
		obj->setRefLineHalo( true );
		obj->setRefLineWidth( refLineWidth.getValue() );
		obj->setRefLineHaloColor( refLineHaloColor.getValue() );
		obj->setInvisibleObjectRefLineWidth( 1.0 );
		obj->setArrowThickness( arrowThickness.getValue() );
		obj->setArrowSize( arrowSize.getValue() );
		obj->setArrowColor( arrowColor.getValue() );
		obj->setArrowColorHalo( arrowColorHalo.getValue() );
	}
}



void SoLabelModul::setLabelParameter(SoLabelObject* slo)
{    
	//std::cout << "setLabelParameter(_obj) _obj=" << _obj << std::endl;
	if (!input.getValue()) return;
	if ( colorIDModelUpToDate==false )
	{
		colorIDModel= createColorIDModel( /*input.getValue()*/ );	//colorIDModel= createColorIDModel( sceneModel );	
		buildNode();
	}

	bool tempLabelOn,tempVisibleOn = false,valueExist;
	string tempLabelText;
	SbVec3f tempLabelBG;
	bool exists;
	if ( slo ){
		slo->validTextBoxSize		= false;
		slo->on                    = true;// kann durch wert aus METK noch geändert werden

		if (slo->metkObjId!=""){
			///////////ENTSCHEIDUNG WAS GENAU NUN GETEXTET WIRD
			if (_metkParent){				
				valueExist =_metkParent->getObjMgr()->getObjAttributeBool(slo->metkObjId, LAY_LABEL, INF_LABELON, tempLabelOn);
				valueExist = valueExist && _metkParent->getObjMgr()->getObjAttributeBool(slo->metkObjId, LAY_APPEARANCE, INF_VISIBLE, tempVisibleOn);
				slo->on = tempLabelOn && tempVisibleOn && valueExist;
				kDebug::Debug("on="+kBasics::BoolToString(slo->on)+" fuer "+slo->metkObjId);

				//_metkParent->getObjMgr()->getObjAttributeString(slo->metkObjId, LAY_DESCRIPTION, INF_NAME, tempLabelText);				
				//slo->setLabelText(tempLabelText+"\n"+slo->metkObjId);
				exists = _metkParent->getObjMgr()->getObjAttributeString(slo->metkObjId, LAY_LABEL, INF_LABELTEXT, tempLabelText);
				if (!exists) tempLabelText = "Default Text";
				kDebug::Debug("tempLabelText="+tempLabelText,kDebug::DL_LOW);
				slo->setLabelText(tempLabelText);

				//Get Label-BG Color
				exists = _metkParent->getObjMgr()->getObjAttributeVec3f(slo->metkObjId, LAY_LABEL, INF_LABELBG, tempLabelBG);
				if (exists)
					slo->setBackgroundColor(tempLabelBG[0],tempLabelBG[1],tempLabelBG[2]);
				else
					slo->setBackgroundColor(defaultLabelBgColor.getValue()[0],defaultLabelBgColor.getValue()[1],defaultLabelBgColor.getValue()[2]);
			}
			else
			{
				kDebug::Debug("no _metkParent ... set labelText to "+slo->metkObjId);
				slo->setLabelText(slo->metkObjId+"\n"+slo->metkObjId);
			}			
		}
		else
		{
			slo->setLabelText("Default Label");
		}

		//std::cout << "calcTextSize sofort in setLabelParameter" << std::endl;
		calcTextSize(slo);

		slo->drawIfInvis           = (invisObjStyle.getValue()!=none);
		slo->drawIfInteraction     = labelIfInteraction.getValue();		
		//slo->priority              = 1;//labelPriority.getValue();	       
		slo->objectIsBackground    = false;//objectIsBackground.getValue();
		myTextModul->string.setValue(slo->getLabelText().c_str());		
	}
}



void SoLabelModul::cleanUp(){ 
	//kDebug::Debug("cleanUp",kDebug::DL_NONE);
	//alle Label Objekte löschen
	std::vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
	while(it!=myLabelObjects.end()){
		delete *it;
		it++;
	}
	//for (int i=0; i<myLabelObjects.size(); i++ ) delete myLabelObjects[i];
	myLabelObjects.clear(); 
	sceneModel->removeAllChildren();
	root->removeAllChildren();
	//curObject.setValue(0);
	//numberShapes.setValue(0);
	colorIDModelUpToDate=false;
	//mouseClickObject.setValue(0);
	//mouseOverObject.setValue(0);
}



bool SoLabelModul::generateNewData()
{
	//kDebug::Debug("generateNewData",kDebug::DL_NONE);

	if (!input.getValue())
	{
		cleanUp();
		std::cout << "!input.getValue()  return false" << std::endl; 
		return false; 
	}

	//KONRAD: I hope this helps to force something like ReConnectInventor does	
	outputColorBufferView.touch();

	colorIDModel = createColorIDModel(/*input.getValue()*/);

	//std::cout << "myLabelObjects.size() after createColorIDModel=" << myLabelObjects.size() << std::endl;

	std::vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
	while(it!=myLabelObjects.end())
	{
		setLabelParameter(*it);
		it++;
	}
	buildNode();
	return true;
}



SoSeparator *SoLabelModul::createColorIDModel(/*SoNode* /*model*/)//(SoSeparator *model)
{
	kDebug::Debug("createColorIDModel");
	SoSearchAction sa;
	SoSeparator *rootColorCoded;
	int l, index, shapes;
	SoGetBoundingBoxAction getbbx(viewportRegion); //bbx->viewpoirtRegion
	char beschriftungsTextNr[4];
	////bool newMyLabelObjects=false;

	rootColorCoded = static_cast<SoSeparator*> (input.getValue()->copy() );//rootColorCoded = static_cast<SoSeparator*> (sceneModel->copy() );
	rootColorCoded->ref();

	sa.setType(SoFile::getClassTypeId(), TRUE); // all SoFile nodes
	sa.setInterest(SoSearchAction::ALL);
	sa.apply(rootColorCoded);

	SoPathList shapePathes = sa.getPaths();
	shapes = shapePathes.getLength();

	if ( shapes==0) //suche noch nach Shapes
	{
		//std::cout << "//suche noch nach Shapes" << std::endl;
		sa.setType(SoShape::getClassTypeId(), TRUE); // all SoFile nodes
		sa.setInterest(SoSearchAction::ALL);
		sa.apply(rootColorCoded);
		shapePathes = sa.getPaths();
		shapes =  shapePathes.getLength();
	}
	//numberShapes.setValue(shapes);
	kDebug::Debug("number of shapes: "+kBasics::IntToString(shapes),kDebug::DL_LOW);
	colorIDModelUpToDate=true;

	/*if (myLabelObjects.size()!= shapes ) //update oder Change?
	{
	//std::cout << "myLabelObjects.resize(shapes)" << std::endl;
	myLabelObjects.resize(shapes);
	initAnnotator();
	newMyLabelObjects=true; 
	}*/

	initAnnotator();

	//cleanup myLabelObjects
	std::vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
	while(it!=myLabelObjects.end()){
		delete (*it);
		it++;
	}	
	myLabelObjects.clear(); 
	myLabelObjects.resize(shapes);

	for (int i=0; i<shapes; i++){  // für jedes Teilobjekt
		SoPath *path = (SoPath *) shapePathes.get(i);
		l = path->getLength();
		SoNode *node = path->getTail();
		path->pop();
		SoNode *parent = path->getTail();

		SoLabelObject* tmpobj=new SoLabelObject();
		myLabelObjects[i] = tmpobj;//new SoLabelObject();//neu anlegen, wenn nicht exisitert

		//init label object		
		setStyleToLabel(tmpobj);
		setLabelParameter(tmpobj);

		if (parent->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())){
			SoNode* tryLabelName = ((SoGroup*)parent)->getChild(0);
			if (tryLabelName->getTypeId().isDerivedFrom(SoLabel::getClassTypeId())){
				//Get Label				
				SbString tempID;
				((SoLabel*)tryLabelName)->getField(SbName("label"))->get(tempID);
				kDebug::Debug("SoLabel found: "+string(tempID.getString()),kDebug::DL_LOW);
				tmpobj->metkObjId=kBasics::trimQuotatedStr(tempID.getString(),'"');
			}else{
				kDebug::Debug("No SoLabel node found for a structure");
				tmpobj->metkObjId="";
			}
		} 

		if (parent->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())){	  
			//BOUNDING-BOX
			getbbx.apply(parent);			
			tmpobj->centerBB = getbbx.getBoundingBox().getCenter();
			//SbVec3f test(tmpobj->centerBB);

			SoMaterial *material = new SoMaterial(); 
			material->ref();

			//COLOR-CODE
			SbColor code;
			//im Annotator wird nur Rotwert analysiert aus Performancegründen!			
			if (tmpobj->objectIsBackground)
			{
				code = SbColor(255/255.0f, 1.0, 1.0);
				tmpobj->colorID = 0;
				//tmpobj->on=false; //weil color Code 0
			}else{
				code = SbColor((i+1)/255.0f, (i+1)/255.0f, (i+1)/255.0f); //nur Rot-wert wird letztendlich verwertet
				tmpobj->colorID = i+1; 
			}
			material->ambientColor.setValue(code);
			material->diffuseColor.setValue(code); //wichtig für FlatShading
			material->specularColor.setValue(code);
			material->emissiveColor.setValue(code);
			material->shininess.setValue(1.0f);
			material->transparency.setValue(0.0f); // Original 0.0f

			//neuenKnoten zusammenbauen
			SoGroup *group = (SoGroup *) parent;
			sprintf(beschriftungsTextNr, "_%d", i+1); //"_" sonst fehler, weil inventornamen nicht mit "_" beginnen sollten		
			parent->setName( SbName(beschriftungsTextNr) );
			index = group->findChild(node);
			group->insertChild(material, index);	 
		}
	}  
	return rootColorCoded;
}



void SoLabelModul::buildNode()
{   
	//kDebug::Debug("buildNode",kDebug::DL_NONE);
	root->removeAllChildren();	
	//root->addChild(myCube);
	//WIRD FLATSHADINGNODE WIRKLICH GEBRAUCHT???
	root->addChild(flatShadingNode);
	root->addChild(whiteBackground);	
	root->addChild(colorIDModel); 
	root->addChild(idBufferNode);   //capture Framebuffer //FEHLER drin?

	//WIRD PHONG SHADING WIRKLICH GEBRAUCHT???
	root->addChild(phongShadingNode);
	root->addChild(curBackground);
	//root->addChild(sceneModel);
	root->addChild(myAnnotator); //Annotationsberechnung

	myAnnotator->reLayout();

	//SoSeparator *test = new SoSeparator();
	//test->ref();
	//test->addChild(flatShadingNode);
	//test->addChild(whiteBackground);	
	//test->addChild(colorIDModel); 
	//test->addChild(idBufferNode);   //capture Framebuffer //FEHLER drin?
	//test->addChild(phongShadingNode);
	//test->addChild(myAnnotator); //Annotationsberechnung
	//outputColorBufferView=test;

	//myAnnotator->reLayout();
}



//+++++++++++++++++++++++++++++++++++++++
// Render-wichtige Funktionen
//+++++++++++++++++++++++++++++++++++++++




//KONRAD: not used??
/*void SoLabelModul::drawRefPoint( int x, int y, SoLabelObject* obj)
{
float z = 1.0; 
float r, g, b, a;

if ( obj->drawIfInvis && !(obj->labelState.isObjectVisible)){ 
//Ref-Point für unsichtbare Objekte
/*myLabelObjects[i]->getRefLineColor(r, g, b);
a=myLabelObjects[i]->getRefLineColorAlpha();
glPointSize( myLabelObjects[i]->getRefLineWidth() );
glBegin(GL_POINTS);
glColor4f (r, g, b, a ); // Endpunkt innen	   
glVertex3f(x, y+1, z);     glVertex3f(x, y-1, z);
glVertex3f(x, y, z); 
glVertex3f(x-1, y, z);     glVertex3f(x+1, y, z);

if (myLabelObjects[i]->getRefLineHalo() ) {
myLabelObjects[i]->getRefLineHaloColor(r, g, b);
a=myLabelObjects[i]->getRefLineColorAlpha();
glColor4f (r, g, b, a );			
glVertex3f(x-1, y+2, z);   glVertex3f(x  , y+2, z);
glVertex3f(x+1, y+2, z);   glVertex3f(x-2, y+1, z);
glVertex3f(x-1, y+1, z);   glVertex3f(x+1, y+1, z);
glVertex3f(x+2, y+1, z);   glVertex3f(x-2, y,   z);
glVertex3f(x+2, y  , z);   glVertex3f(x-2, y-1, z);
glVertex3f(x-1, y-1, z);   glVertex3f(x+1, y-1, z);
glVertex3f(x+2, y-1, z);   glVertex3f(x-1, y-2, z);
glVertex3f(x  , y-2, z);   glVertex3f(x+1, y-2, z);
}
glEnd();
*/
/*	}else{
//sichtbare Objekte
obj->getRefLineColor(r, g, b);
a=obj->getRefLineColorAlpha();
glPointSize( obj->getRefLineWidth() );
glBegin(GL_POINTS);
glColor4f (r, g, b, a ); // Endpunkt innen	   
glVertex3f(x, y+1, z);     glVertex3f(x, y-1, z);
glVertex3f(x, y, z); 
glVertex3f(x-1, y, z);     glVertex3f(x+1, y, z);

if (obj->getRefLineHalo()){
a=obj->getRefLineColorAlpha();
if (haloColorInvert.getValue()==1){
r=1.0-r; g=1.0-g; b=1.0-b; 
}else{
obj->getRefLineHaloColor(r, g, b);   
}
glColor4f (r, g, b, a );			
glVertex3f(x-1, y+2, z);   glVertex3f(x  , y+2, z);
glVertex3f(x+1, y+2, z);   glVertex3f(x-2, y+1, z);
glVertex3f(x-1, y+1, z);   glVertex3f(x+1, y+1, z);
glVertex3f(x+2, y+1, z);   glVertex3f(x-2, y,   z);
glVertex3f(x+2, y  , z);   glVertex3f(x-2, y-1, z);
glVertex3f(x-1, y-1, z);   glVertex3f(x+1, y-1, z);
glVertex3f(x+2, y-1, z);   glVertex3f(x-1, y-2, z);
glVertex3f(x  , y-2, z);   glVertex3f(x+1, y-2, z);
}
glEnd();
}
}*/


void SoLabelModul::drawInvisRefLine(SoLabelObject* obj)
{
	float r, g, b, a;
	obj->getRefLineColor(r, g, b);
	a=obj->getRefLineColorAlpha();	   

	if ( invisObjStyle.getValue()==arrow ) //Arrow zeichnen
	{
		SbVec2f labelPos( (float) obj->referenceLine.label.getValue()[0],  (float) obj->referenceLine.label.getValue()[1]);
		SbVec2f objPos(   (float) obj->referenceLine.object.getValue()[0], (float) obj->referenceLine.object.getValue()[1]);
		if ( obj->isJustificationRight() ){
			labelPos.setValue( labelPos.getValue()[0]+6, labelPos.getValue()[1] ); //vergrößert den Abstand der Textbox; sonst klebt die Pfeilspitze so am text
		}else{
			labelPos.setValue( labelPos.getValue()[0]-6, labelPos.getValue()[1] ); //vergrößert den Abstand der Textbox; sonst klebt die Pfeilspitze so am text
		}

		SbVec2f textSize( obj->getTextSize()[0], obj->getTextSize()[1] );
		SbVec2f lo = labelPos-objPos;
		SbVec2f yo( 0.0, 1.0);
		glLineWidth( obj->getArrowThickness() );
		if (obj->getRefLineHalo()==1 ){ //halo zeichnen			 
			if (haloColorInvert.getValue()==1){
				obj->getArrowColor(r, g, b);
				r=1.0-r; g=1.0-g; b=1.0-b; 
			}else{
				obj->getArrowColorHalo( r, g, b);
			}
			glColor4f (r, g, b, a );
			drawArrow( labelPos - SbVec2f(1.0, 1.0), objPos - SbVec2f(1.0, 1.0), obj->getArrowSize()*2 );
			drawArrow( labelPos + SbVec2f(1.0, 1.0), objPos + SbVec2f(1.0, 1.0), obj->getArrowSize()*2 );			 
		}
		obj->getArrowColor(r, g, b);
		glColor4f (r, g, b, a );
		drawArrow(labelPos, objPos, obj->getArrowSize()*2);		 
	}
	else if ( invisObjStyle.getValue()==dashedLine ) //zeichne gestrichelte Linie
	{ 
		glEnable(GL_LINE_STIPPLE);
		glLineWidth( obj->getInvisibleObjectRefLineWidth() );
		glLineStipple(1, 0xF3F3); //repeat, pattern (16Pixel)
		drawRefLineToObj(obj); 
		glDisable(GL_LINE_STIPPLE);   
	}
	else if ( invisObjStyle.getValue()==solidLine) //zeichen normale Linie
	{
		drawRefLineToObj(obj);
	}
}



void SoLabelModul::drawRefSegment(SoLabelObject* obj)
{
	if (!(obj->labelState.isObjectVisible))
	{
		drawInvisRefLine(obj);	   
	}
	else
	{
		drawRefLineToObj(obj);		
	}
}



void SoLabelModul::drawArrow( SbVec2f p1, SbVec2f p2, float dist)
{ 
	//p1=Labelpunkt p2=objektpunkt    
	//SbVec2f p1p2 = p2-p1;
	SbVec2f vertex = (p2-p1); vertex.normalize(); vertex=vertex*dist; 

	SbVec2f arrow1(1,-1), arrow2(-1,1);
	//orthogonale Vectoren bilden
	if ( abs(vertex[0])-1<=0 || abs(vertex[1])-1<=0)
	{
		arrow1[0]=vertex[1]/2.0;
		arrow1[1]=vertex[0]/2.0;	   
		arrow2 = arrow1; arrow2.negate();
		vertex=p1+vertex; 
		arrow1=arrow1+p1+((vertex-p1)/2.0);
		arrow2=arrow2+p1+((vertex-p1)/2.0);
	}
	else
	{
		arrow1[0]= -vertex[1]/2.0;
		arrow1[1]=  vertex[0]/2.0;	
		arrow2 = arrow1; arrow2.negate();
		vertex=p1+vertex;
		arrow1=arrow1+p1+((vertex-p1)/2.0);
		arrow2=arrow2+p1+((vertex-p1)/2.0);	  
	}

	glBegin(GL_LINES);
	glVertex3f(  p1[0], p1[1], 1.0);
	glVertex3f(  vertex[0], vertex[1], 1.0);
	glVertex3f(  arrow1[0], arrow1[1], 1.0); //spitze 1
	glVertex3f(  vertex[0], vertex[1], 1.0);

	glVertex3f(  arrow2[0], arrow2[1], 1.0); //spitze 2
	glVertex3f(  vertex[0], vertex[1], 1.0);
	glEnd();
}



void SoLabelModul::drawRefLineToObj(SoLabelObject* obj)
{
	float r, g, b, a;
	float fracLineWidth = obj->getRefLineWidth()/2.0;
	fracLineWidth -= int (fracLineWidth);

	//hä?
	//float displace =  (fracLineWidth > 0.25 && fracLineWidth < 0.75) ? 0.5 : 0.0; //damit linie genau auf pixellinie liegt und korrekt gerendert wird

	obj->getRefLineColor(r, g, b);   
	glLineWidth( obj->getRefLineWidth() ); 
	a=obj->getRefLineColorAlpha();
	float objvalue0=(float) obj->referenceLine.object.getValue()[0];
	float objvalue1=(float) obj->referenceLine.object.getValue()[1];
	float labvalue0=(float) obj->referenceLine.label.getValue()[0];
	float labvalue1=(float) obj->referenceLine.label.getValue()[1];
	float reflinewidth=obj->getRefLineWidth();
	if (!(orthoRefLines.getValue())){//nicht orthogonale Reference Linien
		if (obj->getRefLineHalo()){//Zeichne Halo
			if (haloColorInvert.getValue()==1){ //halo-farbe auswählen
				r=1.0-r; g=1.0-g; b=1.0-b; 
			}else{
				obj->getRefLineHaloColor(r, g, b);
			}
			glColor4f (r, g, b, a );	 
			glPointSize( reflinewidth+4.0); //Halo-ankerpunkt schön groß machen
			glBegin(GL_POINTS);
			glVertex3f(objvalue0, objvalue1, 1.0);
			////////glVertex3f(objvalue0+displace, objvalue1+displace, 1.0);
			glEnd();

			glLineWidth( reflinewidth+2.0);//Halo Linie 2 pixel größer machen als reflineLinie
			glBegin(GL_LINES); //Halo der referenzlinie referenzlinie 
			if ( obj->isJustificationRight() ) //je nach dem ob rechts oder links den abstand (zur Textbox) aus ästhetischen gründen leicht vergrößern
			{
				glVertex3f(objvalue0, objvalue1, 1.0);
				glVertex3f(labvalue0+1, labvalue1, 1.0);			   
				////////////glVertex3f(objvalue0 +displace, objvalue1+displace, 1.0);
				///////////glVertex3f(labvalue0+1+displace, labvalue1+displace, 1.0);			   
			}
			else
			{
				glVertex3f(objvalue0, objvalue1, 1.0);
				glVertex3f(labvalue0-1, labvalue1, 1.0);			   
				/////////////glVertex3f(objvalue0 +displace, objvalue1+displace, 1.0);
				////////////glVertex3f(labvalue0-1+displace, labvalue1+displace, 1.0);			   
			}
			glEnd();

		}

		//nach dem evt. Halo nun der ankerpunkt und die ReferenceLinie
		obj->getRefLineColor(r, g, b);
		glColor4f(r, g, b, a );	 // schwarze linie  

		glPointSize( reflinewidth+2.0 ); //ankerpunkt etwas größer als linie machen
		glBegin(GL_POINTS);
		glVertex3f(objvalue0, objvalue1, 1.0);
		///////glVertex3f(objvalue0+displace, objvalue1+displace, 1.0);
		glEnd();

		glLineWidth( reflinewidth );
		glBegin(GL_LINES); //referenzlinie 
		if ( obj->isJustificationRight() ) //je nach dem ob rechts oder links den abstand (zur Textbox) aus ästhetischen gründen leicht vergrößern
		{			
			////////////////glVertex3f( objvalue0 +displace,  objvalue1+displace, 1.0);
			////////////////glVertex3f( labvalue0+1+displace,   labvalue1+displace, 1.0 );

			//ohne displace
			glVertex3f( objvalue0,  objvalue1, 1.0);
			glVertex3f( labvalue0+1,   labvalue1, 1.0 );
		}
		else
		{			
			////////////////glVertex3f( objvalue0 +displace,  objvalue1+displace, 1.0);
			////////////////glVertex3f( labvalue0-1+displace,   labvalue1+displace, 1.0 );

			//ohne displace
			glVertex3f( objvalue0 ,  objvalue1, 1.0);
			glVertex3f( labvalue0-1,   labvalue1, 1.0 );

		}
		glEnd();
	}
	else //orthoRefLines
	{ 
		if(obj->getRefLineHalo()==1 ){ //zuerst Halo
			if (haloColorInvert.getValue()==1){
				r=1.0-r; g=1.0-g; b=1.0-b; 
			}else{
				obj->getRefLineHaloColor( r, g, b );
			}

			glPointSize( reflinewidth+4.0 ); //halo-ankerpunkt groß machen
			glColor4f (r, g, b, a );
			glBegin(GL_POINTS); //Halo Ankerkpunkt zeichnen
			//ohne displace
			glVertex3f(objvalue0, objvalue1, 1.0);
			glEnd();
			glPointSize( reflinewidth+2.0); //knickpunktgröße = liniengröße (hier von der halo-linie)
			glBegin(GL_POINTS);//knotenpunkt zeichnen, damit die Linien icht abgebrochen aussieht
			//ohne displace
			glVertex3f( objvalue0,  labvalue1, 1.0);
			glEnd();

			glLineWidth( reflinewidth+2.0 );
			glBegin(GL_LINES);//zeichne dickere Halo-Linie
			//ohne displace
			glVertex3f( objvalue0,  objvalue1, 1.0); //x=ref, y=ref
			glVertex3f( objvalue0,  labvalue1, 1.0);  //x=ref, y=label
			glVertex3f( objvalue0,  labvalue1, 1.0);  //x=ref, y=label
			if ( obj->isJustificationRight() ){ //je nach dem ob rechts oder links den abstand (zur Textbox) aus ästhetischen gründen leicht vergrößern
				//ohne displace
				glVertex3f( labvalue0+1, labvalue1, 1.0 ); //x=label, y=label
			}else{
				//ohne displace
				glVertex3f( labvalue0-1, labvalue1, 1.0 ); //x=label, y=label
			}
			glEnd();

		}
		obj->getRefLineColor( r, g, b );
		glPointSize( reflinewidth+2.0);
		glColor4f (r, g, b, a );		
		glBegin(GL_POINTS); //ankerpunkt
		glVertex3f(objvalue0, objvalue1, 1.0);
		glEnd();

		glPointSize( reflinewidth );
		glBegin(GL_POINTS);//knickpunkt
		glVertex3f( objvalue0,  labvalue1, 1.0);
		glEnd();
		glLineWidth(reflinewidth);
		glBegin(GL_LINES);		
		glVertex3f( objvalue0,  objvalue1, 1.0); //x=ref, y=ref
		glVertex3f( objvalue0,  labvalue1, 1.0);
		glVertex3f( objvalue0,  labvalue1, 1.0);
		if(obj->isJustificationRight()){
			glVertex3f( labvalue0+1, labvalue1, 1.0 ); //x=label, y=label
		}else{
			glVertex3f( labvalue0-1, labvalue1, 1.0 ); //x=label, y=label
		}
		glEnd();
	}
}



void SoLabelModul::drawText(SoLabelObject* obj, SoGLRenderAction *action ){	
	glColor4f(1.0, 0.0, 1.0, 0.5); //wegen bug in FTGL und Pixmaps; testen obs geht
	updateFontParameter(obj, myTextModul);
	myTextModul->doAction(action);
}


void SoLabelModul::drawLabel( SoGLRenderAction *action)
{
	SoState * state = action->getState();
	currentRenderAction = action;

	bool old_gl_lighting = glIsEnabled(GL_LIGHTING);	
	glDisable(GL_LIGHTING);

	const SbViewportRegion & vp = SoViewportRegionElement::get(state);
	SbVec2s vpsize = vp.getViewportSizePixels();

	SbVec3f nilpoint(0.0f, 0.0f, 0.0f);
	const SbMatrix & mat = SoModelMatrixElement::get(state);
	mat.multVecMatrix(nilpoint, nilpoint);

	// Set new state.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glLoadIdentity();  
	glOrtho(0, vpsize[0], 0.0, vpsize[1], -1.0f, 1.0f);// -1.0f, 1.0f);

	glDisable(GL_LIGHTING);
	bool old_gl_blend = glIsEnabled(GL_BLEND);	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int textSizeX, textSizeY;
	float x, y;//, xMin=vpsize[0], xMax=0; 
	SbVec3f dir;

	mouseDown=false;
	if(mouseButtonPressed) mouseDown=true;
	if(motionLabeling.getValue() || !mouseDown)
	{
		std::vector<SoLabelObject*>::iterator it=myLabelObjects.begin();
		while(it!=myLabelObjects.end()){
			//weltkoordinaten für den ObjektPunkt  ermitteln
			mat.multDirMatrix( (*it)->centerBB, dir);
			(*it)->worldCenterBB.setValue( dir.getValue() );	   

			if ( !(*it)->validTextBoxSize )
			{
				//std::cout << "calcTextSize (eigentlich zu spaet)" << std::endl;
				calcTextSize(*it);
			}		

			//sichtbare Labels, die dargestellt werden sollen (und wenn interaktion, dann schauen, ob bei interaktion gezeichnet werden soll)
			if (((*it)->labelState.visible || invisObjStyle.getValue()!=none) && (*it)->on)
			{
				//ggf. TextBoxgrößen neu berechnen

				textSizeX= (*it)->getTextSize().getValue()[0];
				textSizeY= (*it)->getTextSize().getValue()[1];        
				(*it)->setViewportDimensions(vpSize); 
				(*it)->setVisibility(true);        
				(*it)->setTransparency(1.0);
				(*it)->setRefLineColorAlpha( 1.0 );			
				(*it)->referenceLine.label = (*it)->refLineNewPos;

				x = (*it)->referenceLine.label.getValue()[0];
				y = (*it)->referenceLine.label.getValue()[1];

				// wenn rechts vom agent	 
				if ( (*it)->referenceLine.label.getValue()[0] < (*it)->referenceLine.object.getValue()[0])
				{
					(*it)->setJustificationRight();
					myTextModul->justification=SoTextModul::LEFT;				
					x=x-textSizeX;//-5;//5 gleicht eine kleine Überlappung der Linie mit der Textbox aus		
				}else{ // wenn links vom agent
					(*it)->setJustificationLeft();//myExternal[i]->setJustification(SoText2::LEFT); 
					myTextModul ->justification=SoTextModul::RIGHT;				
					//x+=4;//4 gleicht eine kleine Überlappung der Linie mit der Textbox aus		
				}

				y=y+(floor(textSizeY/2.0)); //RefPoint rechts oben
				(*it)->setPosition( x, y ); //y-wert auf mitte der schriftpos setzen

				//Ankerpunkt			
				//refLine
				drawRefSegment(*it);
				drawLabelBackground(x,y-textSizeY,textSizeX,textSizeY,(*it)->getBackgroundColor(),(*it)->getRefLineWidth());

				glLineWidth(1.0); //weil Schrift z.Z. Polygonal				
				drawText(*it, action);
			}
			else //wenn nicht visible
			{ 
				//std::cout << "drawLabel() not(on AND visible) id=" << (*it)->metkObjId << std::endl;
				if ( (*it)->on && !((*it)->labelState.visible)) //sollte gelabelt werden, aber war aus Platzgründen ohne erfolg?
				{
					std::cout << "KEIN PLATZ" << std::endl;
					kDebug::Debug("konnte nicht labeln metk_object = " + (*it)->metkObjId+"   labelState.visible=" + kBasics::BoolToString((*it)->labelState.visible) + "  on=" + kBasics::BoolToString((*it)->on));
				}
				(*it)->setVisibility(false);//markieren als "nicht gelabelt"
			}
			it++;
		}//ende erste while-schleife
	} //Ende IF motion labeling 

	// Pop old GL state.	
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	if (!old_gl_blend) glDisable(GL_BLEND);
	if (old_gl_lighting) glEnable(GL_LIGHTING);
}

void SoLabelModul::drawLabelBackground(int x,int y,int w,int h, SbColor bgColor,int refLineWidth)
{
	//std::cout << "labelBackground.getValue()=" << labelBackground.getValue() << std::endl;
	if(!(labelBackground.getValue()))
		return;
	glEnable( GL_CULL_FACE); //glDisable( GL_CULL_FACE);
	glFrontFace( GL_CCW);//default GL_CCW
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_LIGHTING);  
	glEnable( GL_TEXTURE_2D );
	glEnable(GL_BLEND);

//glRectd(x,y,x+w,y+h);

	glColor4f(bgColor[0],bgColor[1],bgColor[2],labelBGAlpha.getValue());

	glBegin(GL_POLYGON);
	glVertex3f(x,y,1);
	glVertex3f(x+w,y,1);
	glVertex3f(x+w,y+h,1);
	glVertex3f(x,y+h,1);
	glVertex3f(x,y,1);
	glEnd();


	glDisable(GL_BLEND); //für untransparente linien
	glLineWidth( refLineWidth );
	glBegin(GL_LINES);
	glColor4f(0.0,0.0,0.0,1.0);
	glPointSize(2.0);
	glVertex3f(x,y,1);
	glVertex3f(x+w,y,1);

	glVertex3f(x+w,y+h,1);
	glVertex3f(x+w,y,1);

	glVertex3f(x,y+h,1);
	glVertex3f(x,y,1);

	glVertex3f(x,y+h,1);
	glVertex3f(x+w,y+h,1);

	glEnd();

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);
	glDisable( GL_CULL_FACE);
}


void SoLabelModul::GLRender(SoGLRenderAction *action)
{
	//std::cout << "doRender(action) GLRender" << std::endl;
	doRender(action);
	//SoSeparator::GLRender(action);	
}

void SoLabelModul::GLRenderBelowPath(SoGLRenderAction *action)
{
	//	std::cout << "doRender(action) GLRenderBelowPath" << std::endl;	
	doRender(action);
	//SoSeparator::GLRenderBelowPath(action);	
}


void SoLabelModul::doAction(SoGLRenderAction* /*action*/)
{
	//	std::cout << "doAction" << std::endl;
	//doRender(action);

	//SoSeparator::doAction(action);
}


void SoLabelModul::doRender(SoGLRenderAction *action)
{
	//std::cout << "SoLabelModul::doRender(SoGLRenderAction *action)" << std::endl; 
	SbMatrix projectionMatrix;
	SbMatrix viewingMatrix;
	const SbViewportRegion vp = action->getViewportRegion();
	vpSize = vp.getViewportSizePixels();

	myAnnotator->reLayout();  
	this->children->traverse(action);  

	//if ( on.getValue()==1 ) //wichtig, weil sonst unten links beschriftungen gesetzt werden 	 
	//{
	drawLabel(action);
	//} //else wird nur scene durchgeschleift
}



void SoLabelModul::setMLClass(ml::METKLabels* metkParent)
{
	_metkParent = metkParent;
}
