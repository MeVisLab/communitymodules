/////////////////////////////////////////////////////////////////////
//
//  SoClearGLBuffers.cpp
//
//  (c) 2001  Nicolas Halper  ISG
//
/////////////////////////////////////////////////////////////////////

#include <SoClearGLBuffers.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/SbLinear.h>
#include <Inventor/sensors/SoNodeSensor.h>

#include <mlOpenGL.h>

SO_NODE_SOURCE(SoClearGLBuffers);


SoClearGLBuffers::SoClearGLBuffers()
{
  SO_NODE_CONSTRUCTOR(SoClearGLBuffers);

	SO_NODE_ADD_FIELD(clearColorBuffer, (FALSE));
	SO_NODE_ADD_FIELD(clearDepthBuffer, (FALSE));
	SO_NODE_ADD_FIELD(clearAccumBuffer, (FALSE));
	SO_NODE_ADD_FIELD(clearStencilBuffer, (FALSE));

  SO_NODE_ADD_FIELD(whichGLbuffers, (0));

  _nodeSensor= new SoNodeSensor(&_nodeChangedCB, this);
  _nodeSensor->attach(this);
  _nodeSensor->setPriority(0);
}

void
SoClearGLBuffers::initClass()
{
    SO_NODE_INIT_CLASS(SoClearGLBuffers, SoNode, "SoNode");
}

SoClearGLBuffers::~SoClearGLBuffers()
{
  if (_nodeSensor) { delete _nodeSensor; _nodeSensor = 0; }
}

void
SoClearGLBuffers::callback(SoCallbackAction *action)
{
	return;
}

void
SoClearGLBuffers::GLRender(SoGLRenderAction *action)
{
	glClear(whichGLbuffers.getValue());
}

void
SoClearGLBuffers::doAction(SoAction *action)
{
	return;
}


////////////////////////////
//
//  C++ Callback wrapper
//

void
SoClearGLBuffers::_nodeChangedCB(void *clientData, SoSensor * /*sensor*/)
{
   SoField *field= ((SoClearGLBuffers *)clientData)->_nodeSensor->getTriggerField();
   ((SoClearGLBuffers *)clientData)->_nodeSensor->detach();
   ((SoClearGLBuffers *)clientData)->_nodeChanged(field);
   ((SoClearGLBuffers *)clientData)->_nodeSensor->attach((SoNode *)clientData);
}


void
SoClearGLBuffers::_nodeChanged(SoField * triggerField)
{
  if(triggerField == &whichGLbuffers) {
    clearColorBuffer.setValue((whichGLbuffers.getValue() & GL_COLOR_BUFFER_BIT) ? TRUE : FALSE);
    clearDepthBuffer.setValue((whichGLbuffers.getValue() & GL_DEPTH_BUFFER_BIT) ? TRUE : FALSE);
    clearAccumBuffer.setValue((whichGLbuffers.getValue() & GL_ACCUM_BUFFER_BIT) ? TRUE : FALSE);
    clearStencilBuffer.setValue((whichGLbuffers.getValue() & GL_STENCIL_BUFFER_BIT) ? TRUE : FALSE);
  }
  else {
    whichGLbuffers.setValue(
      (clearColorBuffer.getValue() ? GL_COLOR_BUFFER_BIT : 0) |
      (clearDepthBuffer.getValue() ? GL_DEPTH_BUFFER_BIT : 0) |
      (clearAccumBuffer.getValue() ? GL_ACCUM_BUFFER_BIT : 0) |
      (clearStencilBuffer.getValue() ? GL_STENCIL_BUFFER_BIT : 0 ));
  }
}
