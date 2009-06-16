#include "SoFlatShadingNode.h"

SO_NODE_SOURCE(SoFlatShadingNode);

// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoFlatShadingNode::initClass() 
{
	SO_NODE_INIT_CLASS(SoFlatShadingNode, SoNode, "Node");
	SO_ENABLE(SoGLRenderAction, SoLazyElement);
}

SoFlatShadingNode::SoFlatShadingNode()
{
	SO_NODE_CONSTRUCTOR(SoFlatShadingNode);
	myShading = new SoLightModel();
	myShading->model=SoLightModel::BASE_COLOR;
	this->addChild(myShading);
}

SoFlatShadingNode::~SoFlatShadingNode()
{
}


void SoFlatShadingNode::GLRender(SoGLRenderAction *action)
{
	SoState *state = action->getState();
	SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);//BASE_COLOR= ENUM 0 = FLAT
	//glDisable(GL_COLOR_MATERIAL );
	//glDisable(	GL_BLEND );
	//children->traverse(action); //neuer Versuch    
}
