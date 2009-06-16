//----------------------------------------------------------------------------------
//! The Inventor module class SoUMDViewerTrigger
/*!
// \file    SoUMDViewerTrigger.cpp
// \author  Konrad Mühler
// \date    2005-07-06
//
// 
*/
//----------------------------------------------------------------------------------

#include "SoUMDViewerTrigger.h"

SO_NODE_SOURCE(SoUMDViewerTrigger)


//! Initializes this class (called on dll initialization).
void SoUMDViewerTrigger::initClass()
{
	SO_NODE_INIT_CLASS(SoUMDViewerTrigger, SoSeparator, "Separator");
	SO_ENABLE(SoGLRenderAction, SoViewVolumeElement); //wozu dies?
}



//!  Constructor
SoUMDViewerTrigger::SoUMDViewerTrigger()
{
	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoUMDViewerTrigger);

	//SO_NODE_ADD_FIELD(ViewerNameFld, ("UndefindedName"));
	SO_NODE_ADD_FIELD(StatusFld, ("Ready"));
	//SO_NODE_ADD_FIELD(PongFld, ("Ready"));
	//SO_NODE_ADD_FIELD(PositionFld, (0,0,0));	
	//SO_NODE_ADD_FIELD(OrientationFld, (0,0,0,0));		
	//SO_NODE_ADD_FIELD(ForceRedrawFld, ());		


    timerSensor = new SoTimerSensor((SoSensorCB*)SoUMDViewerTrigger::timerEvent, this);
    timerSensor->setInterval(SbTime(1.0/50.0));
	//timerSensor->setInterval(SbTime(1.0/1.0)); //!!!!ACHTUNG NUR ZUM VERLANGSAMEN FÜR EUROVISPAPER
    timerSensor->unschedule();

	currentlyLeft = false;
	frameRendered = true;


    // Create a sensor calling nodeChangedCB if any field changes.
    //_nodeSensor = new SoNodeSensor((SoSensorCB*)SoUMDViewerTrigger::_nodeChangedCB, this);
    //_nodeSensor->attach(this);
    
    // Set priority of sensor. Priority 0 forces direct call which is needed for correct field detection.
    //_nodeSensor->setPriority(0);

}



//! Protected destructor to cleans up internally created nodes and node sensor.
SoUMDViewerTrigger::~SoUMDViewerTrigger()
{
}



/*void SoUMDViewerTrigger::_nodeChangedCB(void* data, SoDataSensor* sensor)
{
    ((SoUMDViewerTrigger*)data)->_nodeChanged(sensor);
}

void SoUMDViewerTrigger::_nodeChanged(SoDataSensor* sensor)
{    
    SoField* field = sensor->getTriggerField();
    
    if (field == &ForceRedrawFld)
	{
	}
}
        
*/



//! Called on new rendering the scene
/*! Wird eine Renderaktion ausgelöst, so wird zunächst ein Timer gestertet. Da jedoch noch in dieser Funktion hier die Renderaktion mittels
 *  SoSeparator::GLRenderBelowPath(action) weitergeleitet wird, kann das erste Timer-Ereignis erst ausgelöst werden, wenn aus dieser Renderfunktion zurückgekehrt wird, also das Rendern abgeschlossen ist.
 *  Daher kann beim ersten Timer-Ereignis auch gleich der Zustand wieder auf "Ready" gesetzt werden.
 *  Wie er es so schnell schafft, die Stati "Rendering" und "Ready" von außen zu erkennen bleibt ein Rätsel und das Funktionieren ein Glücksspiel.
 *  Und der ganze Quatsch nur, weil die Viewer in MeVisLab keine Renderstatusinformation nach draußen liefern.
 */
void SoUMDViewerTrigger::GLRenderBelowPath(SoGLRenderAction* action)
{   
	if (frameRendered && !currentlyLeft) //er soll nur neu rendern, wenn er nicht grade schon am Rendern ist
	{
		frameRendered = false;
		
		StatusFld.setValue("Rendering");
		//SoDebugError::postInfo("Trigger","RENDERING");				
	}
	else
	{
		if (currentlyLeft) currentlyLeft=false;
	}
	//SoDebugError::postInfo("Trigger","#####################################################################");	
	if (frameRendered) StatusFld.setValue("Ready");
	timerSensor->schedule();
	SoSeparator::GLRenderBelowPath(action);	
}



//! Called by timerSensor;
void SoUMDViewerTrigger::timerEvent(void* data, SoDataSensor* a)
{	
    ((SoUMDViewerTrigger*)data)->render(a);
}



//! Reset the Status to Ready if rendering process is ready
void SoUMDViewerTrigger::render(SoDataSensor* sensor)
{
	timerSensor->unschedule();
	if (!frameRendered)
	{		
		StatusFld.setValue("Ready");
		//PongFld.setValue("Ready");
		//SoDebugError::postInfo("Trigger","READY");
		currentlyLeft=true;
		frameRendered = true;
	}
}