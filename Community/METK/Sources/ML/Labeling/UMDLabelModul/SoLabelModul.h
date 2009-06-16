//----------------------------------------------------------------------------------
//! The Inventor module class SoLabelModul derived from SoSeparator
/*!
// \file    SoLabelModul.h
// \author  Maik Beyer
// \date    2007-01-31
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SOLABELMODUL_H__
#define __SOLABELMODUL_H__


// Local includes
#include "LabelingSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include "Data/SoLabelObject.h"
#include "Data/SoAnnotator.h"
#include "Data/SoBackgroundColor.h"
#include "Data/SoPhongShadingNode.h"
#include "Data/SoFlatShadingNode.h"
#include "Data/SoIDBufferNode.h"

#ifndef __UMDTEXTMODUL_H
#include "UMDTextModul/SoTextModul.h"
#endif


#include "kBasics.h"
#include "METKLabels/mlMETKLabels.h"
#include "METK.h"
#include "kDebug.h"

#define PI 3,14159265358979

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


class LABELING_EXPORT SoLabelModul : public SoSeparator
{
	//! Implements the runtime type system interface of this new node.
	SO_NODE_HEADER(SoLabelModul);

public:
	//! Constructor
	SoLabelModul();

	//! Initializes this class (called on dll initialization).
	static void  initClass();

	SoSFNode       outputColorBufferView;
	SoSFNode       outputMyAnnotatorOutput;  
	SoSFNode       input;

	SoSFTrigger    start;
	SoSFTrigger    clear;

	SoSFString     fontFile;
	SoSFFloat      fontSize;
	SoSFFloat	   linespacing;
	SoSFInt32	   insetspace;
	SoSFFloat      refLineWidth;
	SoSFColor      fontColor;  

	SoSFBool       haloColorInvert;
	SoSFBool       orthoRefLines;

	SoSFEnum	   invisObjStyle;
	enum invisStyles {none, dashedLine, solidLine, arrow};


	SoSFColor      arrowColor;
	SoSFColor      arrowColorHalo;
	SoSFFloat      arrowSize;
	SoSFFloat      arrowThickness;
	SoSFColor      refLineColor; 
	SoSFColor      refLineHaloColor; 

	SoSFColor      bgColor;
	SoSFColor	   defaultLabelBgColor;
	SoSFFloat	   labelBGAlpha;

	SoSFBool       labelIfInteraction;
	//SoSFBool       labelIfInvis;
	SoSFBool       labelBackground;
	SoSFBool       motionLabeling;


	SoSFBool	   debug_drawOrbit;

	SoSFTrigger	printLabelObjects;

	SoSFInt32	     skipLevel;

	SoSFBool       mouseListener;

	/*  
	SoSFVec3f      cso_coordinates; 

	SoSFBool       frameCoherenceAgent; //agent für Framecoherenz
	SoSFTrigger    getObjectData;

	SoSFBool       isInteraction;
	SoSFInt32      labelColorID;
	SoSFBool       labelOn;
	SoSFInt32		 labelPriority;
	SoSFString     fld_labelText;*/

	SoSFBool       mouseClickLeft;
	SoSFBool       mouseClickMiddle;
	SoSFInt32      mouseClickObject;
	SoSFBool       mouseClickRight;
	SoSFInt32      mouseOverObject;
	SoSFVec2f      mousePos;

	SoSFInt32		 orbitDistance;  

	/*SoSFInt32      numberShapes;  
	SoSFBool       objectIsBackground;
	SoSFBool       on;
	
	SoSFNode       output;  

	SoSFColor      refLineColor;
	SoSFBool       refLineHalo;   
	SoSFColor      refLineHaloColor;    
	SoSFFloat      refLineWidth;

	SoSFTrigger    setLabelData;  
	SoSFTrigger    setLabelDataToAll;	
	SoSFString     status;

	SoSFBool       status_label;
	SoSFBool       status_objectVisible;  
	SoSFVec2f      status_refPoint;
	SoSFVec2f      status_textSize;
	SoSFVec2f      status_textPos;   

	SoSFTrigger	 updateTextSize; //DEBUG-funktion zur zeit

	SoSFFloat      w_angle;    //winkel referencelinie; kleine winkel werden bevorzugt.. je höher der wert desto kleiner werden die winkel gehalten
	SoSFFloat      w_angle_io; //gewichte für unsichtbare Labels

	SoSFFloat      w_delta;    //abstand von vorherigen Wert
	SoSFFloat      w_delta_io;

	SoSFFloat      w_intersection; //anzahl der potentiellen Schnittpunkte
	SoSFFloat      w_intersection_io;

	SoSFFloat      w_length;   //länge referenzlinie
	SoSFFloat      w_length_io;

	SoSFFloat      w_neighbor; //min. abstand zum nächsten label
	SoSFFloat      w_neighbor_io;*/

	virtual void	GLRender(SoGLRenderAction *action);
	virtual void	GLRenderBelowPath(SoGLRenderAction *action);
	virtual void  doAction(SoGLRenderAction *action);
	void doRender(SoGLRenderAction *action);
	void drawLabel( SoGLRenderAction *action);


	/*  float getWeightLength( void  );         //großer Wert = sehr lang -> schlecht 
	void setWeightLength( float weight ); //weightExternalLength = 75.0;  

	float getWeightAngle( void ); //weightExternalAngle = 1.0;       //großer Wert = kleiner Winkel -> gut
	void setWeightAngle( float weight );

	float getWeightDelta( void );//weightExternalDelta = 20.0;  //großer wert = große Distanz -> schlecht
	void setWeightDelta( float weight );

	bool getUseAgent( void );*/  

	int getObjectAtMousePos( void ); 
	bool mouseDown;
	void setMLClass(ml::METKLabels* metkParent);

	SoAnnotator *myAnnotator; //public, um vom METKLabels drauf zugreifen zu können

protected:
	//! Protected destructor
	virtual            ~SoLabelModul();
	//! Sensor to detect changes in node's fields.
	SoNodeSensor      *_nodeSensor;

	//! forward CB when field \c objectContainerConnection changed
	static void objectContainerConnectionChangedCB(void *data, SoSensor* sens);
	static void       nodeChangedCB(void *data, SoSensor* sens);

	//! Called by the static function \c _nodeChangedCB to notify node changes.
	void              nodeChanged(SoNodeSensor* sensor);

	//! Callback for Mousebutton-Events
	static void mousePressedCB(void* userData, SoEventCallback* eventCB);
	void        mousePressed(SoEventCallback* eventCB);

	//! Callback for Location2-Events
	static void mouseMovedCB(void* userData, SoEventCallback* eventCB);
	void        mouseMoved(SoEventCallback* eventCB);   

	int getMousePosition(SoEventCallback* eventCB);   

private:

	// your own member variables... 
	void initAnnotator( void );
	bool initFonts();
	void initLabelModul( void );
	void updateFontParameter(SoLabelObject*, SoTextModul* textModul);
	void calcTextSize(SoLabelObject*);
	void cleanUp();
	SoSeparator* createColorIDModel(/*SoNode* model*/);
	bool generateNewData();
	void buildNode( void );
	void setLabelParameter( SoLabelObject* );
	void setStyleToLabel( SoLabelObject* );
	void drawArrow(SbVec2f start, SbVec2f ziel, float laenge );
	void drawRefLineToObj(SoLabelObject*);
	void drawInvisRefLine(SoLabelObject*);
	//void drawRefPoint(int x, int y, SoLabelObject*); //KONRAD: not used?
	void drawRefSegment(SoLabelObject*);
	void drawText(SoLabelObject*, SoGLRenderAction *action );
	void drawLabelBackground(int x,int y,int w,int h, SbColor bgColor,int refLineWidth);
	std::vector<SoLabelObject*> myLabelObjects;

	SoTextModul* myTextModul;  
	SbViewportRegion viewportRegion;
	SoSwitch    *switchOnOff;
	SoSeparator *root;

	SoSeparator *sceneModel;

	SoCube* myCube;

	//SoSeparator *sceneModel;
	SoSeparator *colorIDModel;
	SoBackgroundColor *whiteBackground;
	SoBackgroundColor *curBackground;
	SoIdBufferNode *idBufferNode;
	SoFlatShadingNode *flatShadingNode;
	SoPhongShadingNode *phongShadingNode;

	bool colorIDModelUpToDate;    
	bool mouseButtonPressed;
	SbVec2s vpSize;

	SoGLRenderAction* currentRenderAction; //KONRAD: Wofür das genau ist, weiß ich nicht


	ml::METKLabels* _metkParent;
};

#endif // __SOLABELMODUL_H