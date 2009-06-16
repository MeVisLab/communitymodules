#include "SoBackgroundColor.h"

SO_NODE_SOURCE(SoBackgroundColor);

// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoBackgroundColor::initClass() 
{
  SO_NODE_INIT_CLASS(SoBackgroundColor, SoNode, "Node");
  SO_ENABLE(SoGLRenderAction, SoLazyElement);
}

SoBackgroundColor::SoBackgroundColor() {
  SO_NODE_CONSTRUCTOR(SoBackgroundColor);
//  SO_TRACE_IN("SoBackgroundColor::SoBackgroundColor")  
  color.setValue(1.0,1.0,1.0); //weiß 
  pushMatrix=false;
  popMatrix=false;
}

SoBackgroundColor::SoBackgroundColor(float r, float g, float b) {
  //SO_TRACE_IN("SoBackgroundColor::SoBackgroundColor")
  
  SO_NODE_CONSTRUCTOR(SoBackgroundColor);
  color.setValue(r,g,b);  
}

SoBackgroundColor::~SoBackgroundColor() {
   //SO_TRACE_IN("SoBackgroundColor::~SoBackgroundColor")
	
  // Remove the node sensor.
  //if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}


//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
/*void SoBackgroundColor::nodeChangedCB(void *data, SoSensor* sens)
{
  //SO_TRACE_IN("SoBackgroundColor::nodeChangedCB")

  ((SoBackgroundColor*) data)->nodeChanged((SoNodeSensor*)sens); 
}*/

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
/*void SoBackgroundColor::nodeChanged(SoNodeSensor* sensor)
{
  //SO_TRACE_IN("SoBackgroundColor::nodeChanged")

  // Get the field which caused the notification.
  //SoField* field = sensor->getTriggerField();

  // Handle changed fields here
  // Get the field which caused the notification.
}*/

void SoBackgroundColor::GLRender(SoGLRenderAction* /*action*/)
{
  //glClearColor(0.0, 0.0, 0.0);
	if (pushMatrix) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        //glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
        glPushMatrix();		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		//glLoadIdentity();
		
		//glDisable(GL_COLOR_MATERIAL );
		//glDisable(	GL_BLEND );
	}
  
	
	glClearColor( color.getValue()[0], color.getValue()[1], color.getValue()[2], 0.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);
    

	if (popMatrix) {
        //glEnable( GL_BLEND );
		//glEnable(GL_COLOR_MATERIAL );

		glPopAttrib();
       
		glMatrixMode(GL_PROJECTION);
        glPopMatrix();
       
		glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

		
	}
  
}
