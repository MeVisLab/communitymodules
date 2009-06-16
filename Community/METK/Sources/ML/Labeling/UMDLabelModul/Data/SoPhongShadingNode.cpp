#include "SoPhongShadingNode.h"
SO_NODE_SOURCE(SoPhongShadingNode);

// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoPhongShadingNode::initClass() 
{
  SO_NODE_INIT_CLASS(SoPhongShadingNode, SoNode, "Node");
  SO_ENABLE(SoGLRenderAction, SoLazyElement);
}

SoPhongShadingNode::SoPhongShadingNode() {
  SO_NODE_CONSTRUCTOR(SoPhongShadingNode);
  myShading = new SoLightModel();
  myShading->model=SoLightModel::PHONG;
  this->addChild(myShading);
}

SoPhongShadingNode::~SoPhongShadingNode() {
}


void SoPhongShadingNode::GLRender(SoGLRenderAction *action)
{
  SoState *state = action->getState();
  //glEnable(GL_BLEND);
  SoLazyElement::setLightModel(state, SoLazyElement::PHONG);
  SoLazyElement::setTransparencyType(state, SoLazyElement::ALL_MASK); //SoLazyElement::ALL_MASK);//BLENDING_MASK); 
  

	//KONRAD
  //Das clearen bringt nix, weil es nicht transparent löschen kann und immer ne Farbe braucht
/*			glClearColor(0,0,1,1);
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);
			glClear(GL_ACCUM_BUFFER_BIT);
			glClear(GL_STENCIL_BUFFER_BIT); */
}
