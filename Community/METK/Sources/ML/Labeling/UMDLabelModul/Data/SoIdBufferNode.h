#ifndef _SO_ID_BUFFER_NODE_H
#define _SO_ID_BUFFER_NODE_H

#include "LabelingSystem.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
//#include <Inventor/fields/SoSFImage.h>
#include <Inventor/SoPath.h>

#include "XVLeaveScope.h"

// forwarding declaration
class SoAnnotator;
//class SoFrameBufferNode;
//class So3DLabel01;

class SoIdBufferNode : public SoNode {
	SO_NODE_HEADER(SoIdBufferNode);

	public:
    SoIdBufferNode();
	static void initClass();
		GLint view[4];
		GLfloat projection[16], modelview[16];
		SoAnnotator *myAnnotator;
		void setSoAnnotator(SoAnnotator *_myAnnotator);
		//SoFrameBufferNode *myFrameBuffer;
		//void setSoFrameBufferNode (SoFrameBufferNode *_myFrameBuffer);
		//! ID-Buffer
		unsigned char *bufferID;
		unsigned char* getBufferID( void ) {
			return bufferID;
		}	
	
	protected:
		virtual void   GLRender(SoGLRenderAction *action);
		virtual void   callback(SoCallbackAction *action);
		virtual void   doAction(SoAction *action);
	void leseBackBuffer(unsigned char*);	

	private:
		virtual ~SoIdBufferNode();    
		SoState *state;	

#define BUFFER_X_SIZE 3000
#define BUFFER_Y_SIZE 3000
#define OBSCURE_FACTOR 4


};

#endif _SO_ID_BUFFER_NODE_H
