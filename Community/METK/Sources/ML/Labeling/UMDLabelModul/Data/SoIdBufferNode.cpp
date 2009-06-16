//entspricht in Timos Projekt SoColorCodedNode.cpp
#include "SoIdBufferNode.h"

#include "SoAnnotator.h"

SO_NODE_SOURCE(SoIdBufferNode);

// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoIdBufferNode::initClass() 
{
  SO_NODE_INIT_CLASS(SoIdBufferNode, SoNode, "Node");
  SO_ENABLE(SoGLRenderAction, SoLazyElement);
}

SoIdBufferNode::SoIdBufferNode() {
  SO_NODE_CONSTRUCTOR(SoIdBufferNode);
  bufferID = NULL;
}

SoIdBufferNode::~SoIdBufferNode() {
	if (bufferID != NULL ) {
		free(bufferID);
	}
}


void SoIdBufferNode::GLRender(SoGLRenderAction *action) {
  //SoState *state = action->getState();    

  glGetIntegerv(GL_VIEWPORT, view);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  
  SoIdBufferNode::doAction(action);
}


void SoIdBufferNode::callback(SoCallbackAction *action) {
   SoIdBufferNode::doAction(action);
}

void SoIdBufferNode::doAction(SoAction* /*action*/)
{
   //SoState *state = action->getState();
   SbMatrix projectionMatrix;
   SbMatrix viewingMatrix;

   if (bufferID == NULL ) {
	    bufferID = (unsigned char *) malloc(BUFFER_X_SIZE * BUFFER_Y_SIZE * OBSCURE_FACTOR); //size see Annotater.h
   }
   
   glViewport(view[0], view[1], view[2], view[3]);
   
   glPushMatrix();
	 //projectionMatrix.makeIdentity();
	 //viewingMatrix.makeIdentity();

	  //SoViewingMatrixElement::set(state,this,viewingMatrix);
	  //SoProjectionMatrixElement::set(state,this,projectionMatrix);

		leseBackBuffer(bufferID);	
	glPopMatrix();

	//KONRAD!!!
	//Das clearen bringt nix, weil es nicht transparent löschen kann und immer ne Farbe braucht
/*			glClearColor(0,1,0.4,1);
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);
			glClear(GL_ACCUM_BUFFER_BIT);
			glClear(GL_STENCIL_BUFFER_BIT); */	

   myAnnotator->_bufferID = bufferID;  
}

void SoIdBufferNode::leseBackBuffer(unsigned char *lesePuffer )
{
  GLint origPackAlign, origUnpackAlign;
  glGetIntegerv( GL_PACK_ALIGNMENT, &origPackAlign );
  glGetIntegerv( GL_UNPACK_ALIGNMENT, &origUnpackAlign );

  glReadBuffer(GL_BACK);
  glDisable(GL_CULL_FACE);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);	// sonst gibt's verschiebungen beim Lesen
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//Orig
  glReadPixels(view[0], view[1], view[2], view[3], GL_RGBA, GL_UNSIGNED_BYTE, lesePuffer); //GL_RGBA
  glPixelStorei(GL_PACK_ALIGNMENT, origPackAlign ); //sonst spinnt die spätere SoText2 anzeige
  glPixelStorei(GL_UNPACK_ALIGNMENT, origUnpackAlign );
  glEnable(GL_CULL_FACE); 
}

void SoIdBufferNode::setSoAnnotator( SoAnnotator *_myAnnotator) {
  this->myAnnotator = _myAnnotator;
}

