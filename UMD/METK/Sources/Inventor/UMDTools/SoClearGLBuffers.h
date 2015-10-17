//! Inventor node class derived from SoNode that clears specific GL buffers.
//! \file    UMDStraightLineSystem.h
//! \author  Nicolas Halper

/////////////////////////////////////////////////////////////////////
//
//  SoClearGLBuffers
//
//  A very simple node that clears the specified GL buffers at the
//  point in which this node is hung into the scene graph.
//
//  Why would we want this - for various reasons and effects, believe
//  me! For instance, clearing the z-buffer or the stencil buffer,
//  reading the pixels at a certain point and processing the subsequent
//  rendering on that, etc.
//
//  (c) 2001  Nicolas Halper  ISG
//
/////////////////////////////////////////////////////////////////////
#ifndef  SO_CLEARGLBUFFERS_H
#define  SO_CLEARGLBUFFERS_H

#include "UMDToolsSystem.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>

class SoNodeSensor;
class SoSensor;

//! Inventor node class derived from SoNode that clears specific GL buffers.
class UMDTOOLS_EXPORT SoClearGLBuffers : public SoNode {

    SO_NODE_HEADER(SoClearGLBuffers);

  public:

  SoClearGLBuffers();
  SoSFBool clearColorBuffer;   // GL_COLOR_BUFFER_BIT The buffers currently enabled for color writing.
  SoSFBool clearDepthBuffer;   // GL_DEPTH_BUFFER_BIT The depth buffer.
  SoSFBool clearAccumBuffer;   // GL_ACCUM_BUFFER_BIT The accumulation buffer.
  SoSFBool clearStencilBuffer; // GL_STENCIL_BUFFER_BIT The stencil buffer.

  SoSFInt32 whichGLbuffers;

  SoEXTENDER public:
    virtual void  doAction(SoAction *action);
    virtual void  GLRender(SoGLRenderAction *action);
    virtual void  callback(SoCallbackAction *action);

  SoINTERNAL public:
    static void   initClass();

  protected:
    virtual ~SoClearGLBuffers();

  private:

  //! the sensor listening to the node, for all fields, including changing childs
  SoNodeSensor *_nodeSensor;
  //! called if the ouput should be changed, calls the non-static method \c _nodeChanged
  static void _nodeChangedCB(void *clientData, SoSensor *sensor);
  void _nodeChanged  (SoField *field);

};

#endif
