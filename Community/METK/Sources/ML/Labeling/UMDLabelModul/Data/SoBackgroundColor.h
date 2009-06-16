#ifndef _SO_BACKGROUND_COLOR_H
#define _SO_BACKGROUND_COLOR_H

#include "LabelingSystem.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbColor.h>
#include <Inventor/misc/SoState.h>
#include "XVLeaveScope.h"

class SoBackgroundColor : public SoGroup {
	SO_NODE_HEADER(SoBackgroundColor);

	public:
        SoBackgroundColor();
	SoBackgroundColor(float r, float g, float b);
	static void initClass();
	
	SoSFColor color;
	
	void setColor(float r, float g, float b){
		color.setValue( r,g,b );
	}
	
	SbColor getColor(void){
		return color.getValue();
	}

    bool getPopMatrixState(void) {return popMatrix; };
	void setPopMatrixState( bool state) { popMatrix=state; };
	bool getPushMatrixState(void) {return pushMatrix; };
	void setPushMatrixState( bool state) { pushMatrix=state; };
	

	protected:
		//! Sensor to detect changes in node's fields.
		//SoNodeSensor      *_nodeSensor;
		virtual void   GLRender(SoGLRenderAction *action);
	//! Called by \c _nodeSensor when any field in node changes.
  //! Only redirects the call of this static function to the
  //! class method \c nodeChanged.
  //static void       nodeChangedCB(void *data, SoSensor* sens);

  //! Called by the static function \c _nodeChangedCB to notify node changes.
  //void              nodeChanged(SoNodeSensor* sensor);

  
	
private:

  // your own member variables...
  	SoState *state;
	
	bool popMatrix;
	bool pushMatrix;
	private:
		virtual ~SoBackgroundColor();
    
	
};

#endif _SO_BACKGROUND_COLOR_H
