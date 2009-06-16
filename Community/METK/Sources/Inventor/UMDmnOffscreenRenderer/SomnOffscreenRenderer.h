//----------------------------------------------------------------------------------
//! The Inventor module class SomnOffscreenRenderer derived from SoSeparator
/*!
// \file    SomnOffscreenRenderer.h
// \author  Mathias Neugebauer
// \date    2007-03-30
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SOMNOFFSCREENRENDERER_H__
#define __SOMNOFFSCREENRENDERER_H__


// Local includes
#include "UMDmnOffscreenRendererSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include "XVLeaveScope.h"

#include <mlkAviWriter.h>
#include <kBasics.h>

class CmnOGLOffscreen;
class SbViewportRegion;


//! 
class UMDMNOFFSCREENRENDERER_EXPORT SomnOffscreenRenderer : public SoSeparator
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SomnOffscreenRenderer);

public:
  //! Constructor
  SomnOffscreenRenderer();
  SomnOffscreenRenderer(int iBufferSizeX, int iBufferSizeY);

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  void setTransparency	 ( SoGLRenderAction::TransparencyType eTrans );
  void setSceneRoot		 ( SoNode *scene );
  SoNode* getSceneRoot() { return m_fldScene.getValue(); };
  void setInsideGlFunc	 ( void (*insideGlFunc) (void) );  
  void setAttachmentFunc ( void (*attachmentFunc) (void) );  
  void createBuffer		 ( void );
  void render			 ( void );
  void setSizeX			 ( int iX );
  void setSizeY			 ( int iY );
  int  getSizeX			 ( void );
  int  getSizeY			 ( void );


protected:
  //! Protected destructor
  virtual            ~SomnOffscreenRenderer();

  //! Sensor to detect changes in node's fields.
  SoNodeSensor      *_nodeSensor;

  //! Called by \c _nodeSensor when any field in node changes.
  //! Only redirects the call of this static function to the
  //! class method \c nodeChanged.
  static void       nodeChangedCB(void *data, SoSensor* sens);

  //! Called by the static function \c _nodeChangedCB to notify node changes.
  void              nodeChanged(SoNodeSensor* sensor);


private:

  void (*_attachmentFunc) (void);
  void (*_insideGlFunc) (void);
  void makeAttachments();
  bool initFramebufferExtension();
  void printFrameBufferStatus(GLenum status);
  void saveToFile();
  
  // handles to the frame buffer and render buffer objects
  GLuint			fbo;
  GLuint			colorBuffer;
  GLuint			depthBuffer;

  CmnOGLOffscreen*  m_poOff;
  int				m_bufferType;
  int				m_iBufferSizeX, m_iBufferSizeY;
  SoSFNode			m_fldScene;
  SoSFShort			bufferSizeX, bufferSizeY;
  SoSFTrigger		m_flCreateBuffer, m_flRender;
  bool				m_flHasBuffer;
  SoSFBool			saveImage;
  SoSFBool			saveAVI;
  SoSFString		fldFilename;
  SoSFEnum			transparencyType;
  enum _enum_transparencyType { SCREEN_DOOR = SoGLRenderAction::SCREEN_DOOR,
								ADD = SoGLRenderAction::ADD,
								DELAYED_ADD = SoGLRenderAction::DELAYED_ADD,
								SORTED_OBJECT_ADD = SoGLRenderAction::SORTED_OBJECT_ADD,
								BLEND = SoGLRenderAction::BLEND,
								DELAYED_BLEND = SoGLRenderAction::DELAYED_BLEND,
								SORTED_OBJECT_BLEND = SoGLRenderAction::SORTED_OBJECT_BLEND };

  SoSFTrigger		readyTrigger;
  SoSFTrigger		finishAVI;
  SoSFString		aviFilename;
  SoSFInt32			aviFramerate;
  SoSFEnum			bufferType;
  enum _enum_BufferType { BUFFER_AUTO=0, BUFFER_PBUFFER=1, BUFFER_FRAMEBUFFER=2, BUFFER_SCREENSHOT=3 };
  

  SoGLRenderAction* m_renderAction;
  const SbViewportRegion* m_region;

  ml::kAviWriter*  myAviWriter;

  // EXT_framebuffer_object - http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
  PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
  PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
  PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
  PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
  PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
  PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
  PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
  PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
  PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
  PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
  PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
  PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
  PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
  PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;
};

#endif // __SOMNOFFSCREENRENDERER_H


