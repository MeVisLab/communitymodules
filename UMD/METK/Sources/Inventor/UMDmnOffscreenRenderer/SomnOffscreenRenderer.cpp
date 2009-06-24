//----------------------------------------------------------------------------------
//! The Inventor module class SomnOffscreenRenderer
/*!
// \file    SomnOffscreenRenderer.cpp
// \author  Mathias Neugebauer, Roland and Konrad
// \date    2007-03-30
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SomnOffscreenRenderer.h"
#include "mnOGLOffscreen.h"
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/SbViewportRegion.h>
#include "image_basic.h"

#include <iostream>
using namespace std;

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif

// FAKE-Fenster um HDC zu kriegen, das muss auch besser gehen
//--------------------------------------------------------------------------
HDC			hDC=NULL;	// Private GDI Device Context
HWND		hWnd=NULL;	// Holds Our Window Handle
HINSTANCE	hInstance;	// Holds The Instance Of The Application
WNDCLASS	wc;			// Windows Class Structure

LRESULT CALLBACK WndProc(	HWND	hWnd,		// Handle For This Window
							UINT	uMsg,		// Message For This Window
							WPARAM	wParam,		// Additional Message Information
							LPARAM	lParam)		// Additional Message Information
{
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
//--------------------------------------------------------------------------

#include "XVLeaveScope.h"

#include "glext.h"

#define NONE 0
#define FRAMEBUFFER 1
#ifdef USE_WINPBUFFER
  #define PBUFFER 2
  #define SCREENSHOT 3
#else
  #define SCREENSHOT 2
#endif

SO_NODE_SOURCE(SomnOffscreenRenderer)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SomnOffscreenRenderer::initClass()
{
  SO_NODE_INIT_CLASS(SomnOffscreenRenderer, SoSeparator, "Separator");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SomnOffscreenRenderer::SomnOffscreenRenderer()
{
	SO_TRACE_IN("SomnOffscreenRenderer::SomnOffscreenRenderer")

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SomnOffscreenRenderer);
	SO_NODE_ADD_FIELD(m_fldScene, (NULL));
	SO_NODE_ADD_FIELD(bufferSizeX, (400));
	SO_NODE_ADD_FIELD(bufferSizeY, (400));
	SO_NODE_ADD_FIELD(saveImage, (FALSE));
	SO_NODE_ADD_FIELD(saveAVI, (FALSE));
	SO_NODE_ADD_FIELD(fldFilename, ("c:/temp.bmp"));
	SO_NODE_ADD_FIELD(m_flCreateBuffer, ());
	SO_NODE_ADD_FIELD(m_flRender, ());

	SO_NODE_ADD_FIELD(transparencyType, (SORTED_OBJECT_BLEND));
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, SCREEN_DOOR);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, ADD);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, DELAYED_ADD);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, SORTED_OBJECT_ADD);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, BLEND);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, DELAYED_BLEND);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_transparencyType, SORTED_OBJECT_BLEND);  
	SO_NODE_SET_SF_ENUM_TYPE(transparencyType, _enum_transparencyType);

	SO_NODE_ADD_FIELD(readyTrigger, ());
	SO_NODE_ADD_FIELD(finishAVI, ());
	SO_NODE_ADD_FIELD(aviFilename, ("c:/temp.avi"));
	SO_NODE_ADD_FIELD(aviFramerate, (7));

	SO_NODE_ADD_FIELD(bufferType, (BUFFER_AUTO));
	SO_NODE_DEFINE_ENUM_VALUE(_enum_BufferType, BUFFER_AUTO);
#ifdef USE_WINPBUFFER
	SO_NODE_DEFINE_ENUM_VALUE(_enum_BufferType, BUFFER_PBUFFER);
#endif
	SO_NODE_DEFINE_ENUM_VALUE(_enum_BufferType, BUFFER_FRAMEBUFFER);
	SO_NODE_DEFINE_ENUM_VALUE(_enum_BufferType, BUFFER_SCREENSHOT);
	SO_NODE_SET_SF_ENUM_TYPE(bufferType, _enum_BufferType);
	
	// Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
	// sensor to be sure that changes are not delayed or collected.
	_nodeSensor = new SoNodeSensor(SomnOffscreenRenderer::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);
	m_iBufferSizeX = 400;
	m_iBufferSizeY = 400;
	m_flHasBuffer = false;
	_insideGlFunc = NULL;
	_attachmentFunc = NULL;
	m_bufferType = NONE;

	myAviWriter = new ML_NAMESPACE::kAviWriter();
	myAviWriter->getFieldContainer()->getField("outputFilename")->setStringValue("c:/temp.avi");
}

SomnOffscreenRenderer::SomnOffscreenRenderer(int iBufferSizeX, int iBufferSizeY)
{
  SomnOffscreenRenderer();
  m_iBufferSizeX = iBufferSizeX;
  m_iBufferSizeY = iBufferSizeY;
  bufferSizeX.setValue(m_iBufferSizeX);
  bufferSizeY.setValue(m_iBufferSizeY);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SomnOffscreenRenderer::~SomnOffscreenRenderer()
{
  SO_TRACE_IN("SomnOffscreenRenderer::~SomnOffscreenRenderer")

  // Remove the node sensor.
  if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }

  glDeleteFramebuffersEXT(1, &fbo);
  glDeleteRenderbuffersEXT(1, &colorBuffer);
  glDeleteRenderbuffersEXT(1, &depthBuffer);
}

// --------------------------------------------------------------------------
// Looking for extension: EXT_framebuffer_object
// If the required extensions are present, get the addresses for the
// functions that we wish to use
// --------------------------------------------------------------------------
 bool SomnOffscreenRenderer::initFramebufferExtension()
{
  char *ext = (char*)glGetString( GL_EXTENSIONS );

  if( strstr( ext, "EXT_framebuffer_object" ) == NULL )
  {
	cout << "EXT_framebuffer_object extension was not found" << endl;
	return false;
  }
  else
  {
	glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

	if( !glIsRenderbufferEXT || !glBindRenderbufferEXT || !glDeleteRenderbuffersEXT || 
		!glGenRenderbuffersEXT || !glRenderbufferStorageEXT || !glGetRenderbufferParameterivEXT || 
		!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT || 
		!glGenFramebuffersEXT || !glCheckFramebufferStatusEXT || !glFramebufferTexture1DEXT || 
		!glFramebufferTexture2DEXT || !glFramebufferTexture3DEXT || !glFramebufferRenderbufferEXT||  
		!glGetFramebufferAttachmentParameterivEXT || !glGenerateMipmapEXT )
	{
		cout << "One or more EXT_framebuffer_object functions were not found" << endl;
		return false;
	}
	else
	{
		cout << "All EXT_framebuffer_object functions were found" << endl;
		return true;
	}
  }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SomnOffscreenRenderer::nodeChangedCB(void *data, SoSensor* sens)
{
  SO_TRACE_IN("SomnOffscreenRenderer::nodeChangedCB")

  ((SomnOffscreenRenderer*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SomnOffscreenRenderer::nodeChanged(SoNodeSensor* sensor)
{
  SO_TRACE_IN("SomnOffscreenRenderer::nodeChanged")

  // Get the field which caused the notification.
  SoField* field = sensor->getTriggerField();
  
  if ( field == &bufferSizeX )
  {
    m_iBufferSizeX = bufferSizeX.getValue();
  }

  else if ( field == &bufferSizeY )
  {
    m_iBufferSizeY = bufferSizeY.getValue();
  }

  else if ( field == &m_flCreateBuffer )
  {
    createBuffer();
  }

  else if ( field == &m_flRender )
  {
    render();
  }

  else if ( field == &finishAVI)
  {
	if (myAviWriter) myAviWriter->_endRecording();
  }

  else if ( field == &aviFilename)
  {
	  if (myAviWriter) myAviWriter->getFieldContainer()->getField("outputFilename")->setStringValue(aviFilename.getValue().getString());
  }

  else if (field == &aviFramerate)
  {
	  if (myAviWriter) myAviWriter->getFieldContainer()->getField("framesPerSecond")->setStringValue(kBasics::IntToString(aviFramerate.getValue()));
  }

  else if (field == &m_fldScene) //DANN HAT SICH WAS AN EINEM DER KINDER KINDERKNOTEN GEÄNDERT
  {
	  //std::cout << "Changing ... Render" << std::endl;
	  //render();
  }

}

void SomnOffscreenRenderer::setSizeX ( int iX )
{
  m_iBufferSizeX = iX;
}

void SomnOffscreenRenderer::setSizeY ( int iY )
{
  m_iBufferSizeY = iY;
}

int SomnOffscreenRenderer::getSizeX ( )
{
  if (!m_flHasBuffer) return -1;
  return m_iBufferSizeX;
}

int SomnOffscreenRenderer::getSizeY ( )
{
  if (!m_flHasBuffer) return -1;
  return m_iBufferSizeY;
}

void SomnOffscreenRenderer::makeAttachments (void)
{
  glDeleteRenderbuffersEXT(1, &colorBuffer);
  glDeleteRenderbuffersEXT(1, &depthBuffer);

  // Create the render buffer for color	
  glGenRenderbuffersEXT( 1, &colorBuffer );
  glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, colorBuffer );
  glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA, m_iBufferSizeX, m_iBufferSizeY );

  // Create the render buffer for depth	
  glGenRenderbuffersEXT(1, &depthBuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_iBufferSizeX, m_iBufferSizeY);

  // Attach the color render buffer to the FBO as its color attachment
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorBuffer);

  // Attach the depth render buffer to the FBO as its depth attachment
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);
}

void SomnOffscreenRenderer::createBuffer (void)
{	
	if ( (initFramebufferExtension() && bufferType.getValue()==_enum_BufferType(BUFFER_AUTO)) || bufferType.getValue()==_enum_BufferType(BUFFER_FRAMEBUFFER))
	//if (false) //ICH ZWINGE IHN HIER MAL NUR IM PBUFFER ZU RENDERN
	{
		std::cout << "Framebuffer extension initialized" << std::endl;
		m_bufferType = FRAMEBUFFER;
	}
	else if ( bufferType.getValue()==_enum_BufferType(BUFFER_SCREENSHOT) )
	{
		std::cout << "Taking screenshots: " << m_iBufferSizeX << "x" << m_iBufferSizeY << std::endl;
		m_flHasBuffer = true; // doesn't need buffer, so always true
		m_bufferType = SCREENSHOT;
	}
	else //if ( bufferType.getValue()==_enum_BufferType(BUFFER_PBUFFER) )
	{
#ifdef USE_WINPBUFFER
		std::cout << "Framebuffer extension not initialized, using p-buffer instead" << std::endl;
		m_bufferType = PBUFFER;
#else
    std::cout << "Framebuffer and PBUFFER extension not initialized/available, using screenshot instead" << std::endl;
    m_flHasBuffer = true; // doesn't need buffer, so always true
    m_bufferType = SCREENSHOT;
#endif
	}



  switch(m_bufferType) {
	case FRAMEBUFFER:
		GLenum status;
		glDeleteFramebuffersEXT(1, &fbo);

		// Setup our FBO
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

		if(_attachmentFunc != NULL) _attachmentFunc();
		else makeAttachments();

		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			printFrameBufferStatus(status);
			m_flHasBuffer = false;
		}
		else
		{
			cout << "framebuffer successfully created: " << m_iBufferSizeX << "x" << m_iBufferSizeY << endl;
			m_flHasBuffer = true;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	// Unbind the FBO for now
		break;
#ifdef USE_WINPBUFFER
	case PBUFFER:
		// FAKE-Fenster um HDC zu kriegen, das muss auch besser gehen
		//--------------------------------------------------------------------------
		hInstance		  = GetModuleHandle(NULL);
		wc.style		  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	  // Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc	  = (WNDPROC) WndProc;					  // WndProc Handles Messages
		wc.cbClsExtra	  = 0;									  // No Extra Window Data
		wc.cbWndExtra	  = 0;									  // No Extra Window Data
		wc.hInstance	  = hInstance;							  // Set The Instance
		wc.hIcon		  = LoadIcon(NULL, IDI_WINLOGO);		  // Load The Default Icon
		wc.hCursor		  = LoadCursor(NULL, IDC_ARROW);		  // Load The Arrow Pointer
		wc.hbrBackground  = NULL;								  // No Background Required For GL
		wc.lpszMenuName	  = NULL;								  // We Don't Want A Menu
		wc.lpszClassName  = L"OpenGL";							  // Set The Class Name

		RegisterClass(&wc);

		hWnd=CreateWindowEx(WS_EX_APPWINDOW,	// Extended Style For The Window
							L"OpenGL",			// Class Name
							L"bla",				// Window Title
							WS_POPUP |			// Defined Window Style
							WS_CLIPSIBLINGS |	// Required Window Style
							WS_CLIPCHILDREN,	// Required Window Style
							0, 0,				// Window Position
							1,	                // Calculate Window Width
							1,	                // Calculate Window Height
							NULL,				// No Parent Window
							NULL,				// No Menu
							hInstance,			// Instance
							NULL);	

		hDC = GetDC(hWnd);
		//--------------------------------------------------------------------------

		m_poOff = new CmnOGLOffscreen (hDC,m_iBufferSizeX, m_iBufferSizeY);
		m_poOff->init();
		m_flHasBuffer = true;
		break;
#endif
  }

  m_region = new SbViewportRegion( m_iBufferSizeX, m_iBufferSizeY );
  m_renderAction = new SoGLRenderAction(*m_region);
  //m_renderAction->setTransparencyType(SoGLRenderAction::BLEND);       //später funktion  
  m_renderAction->setTransparencyType(SoGLRenderAction::TransparencyType(transparencyType.getValue()));  
}

void SomnOffscreenRenderer::printFrameBufferStatus(GLenum status)
{
  if (status == GL_FRAMEBUFFER_COMPLETE_EXT) {
    cout << "framebuffer complete" << endl;
  } else {
	cout << "problem with framebuffer: ";
	switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			cout << "FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			cout << "FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
			break;
	}
  }
}

void SomnOffscreenRenderer::setTransparency ( SoGLRenderAction::TransparencyType eTrans )
{
  if (!m_flHasBuffer)
  {
    cout<<"create buffer before setting transparency style!"<<endl;
    return;
  }
  m_renderAction->setTransparencyType(eTrans);

}

void SomnOffscreenRenderer::setSceneRoot ( SoNode *scene )
{
  m_fldScene.setValue(scene);
}

void SomnOffscreenRenderer::setInsideGlFunc ( void (*insideGlFunc) (void) )
{
  _insideGlFunc = insideGlFunc;
}

void SomnOffscreenRenderer::setAttachmentFunc ( void (*attachmentFunc) (void) )
{
  _attachmentFunc = attachmentFunc;
}

void SomnOffscreenRenderer::render(void)
{
	if (!m_flHasBuffer)
	{
		cout<<"no buffer to render in!"<<endl;
		return;
	}

	switch(m_bufferType)
	{
		case FRAMEBUFFER:
			if (!m_fldScene.getValue())
			{
				cout << "nothing to render in OffScreenRenderer" << endl;
			}
			else
			{
				//cout << "render FRAMEBUFFER" << std::endl;
				GLenum status;
				// First we bind the FBO so we can render to it
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

				if(_attachmentFunc != NULL) _attachmentFunc();
				else makeAttachments();

				status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
				if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
					printFrameBufferStatus(status);
				} 
				else
				{
					glEnable(GL_DEPTH_TEST);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					m_renderAction->apply(m_fldScene.getValue());
					if ( _insideGlFunc != NULL ) _insideGlFunc();
					if (saveImage.getValue() || saveAVI.getValue()) saveToFile();
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
					glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );
				}
			}
			break;
#ifdef USE_WINPBUFFER
		case PBUFFER:
			if (!m_fldScene.getValue())
			{
				cout << "nothing to render in OffScreenRenderer" << endl;
			}
			else
			{
				//cout << "render PBUFFER" << std::endl;			
				m_poOff->enable();
				glEnable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_renderAction->apply(m_fldScene.getValue());
				if ( _insideGlFunc != NULL ) _insideGlFunc();
				if (saveImage.getValue() || saveAVI.getValue()) saveToFile();
				m_poOff->disable();
			}
			break;
#endif
		case SCREENSHOT:
			if (saveImage.getValue() || saveAVI.getValue()) saveToFile();
			break;
	}
	readyTrigger.touch();
}

void SomnOffscreenRenderer::saveToFile(void)
{
	unsigned char* data;
	unsigned long val;
	int iIter = 0;
	data = new unsigned char[m_iBufferSizeX*m_iBufferSizeY*3];
	//cout << "getting pixel data" << endl;
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadPixels(0, 0, m_iBufferSizeX, m_iBufferSizeY, GL_RGB, GL_UNSIGNED_BYTE, data);		
	if (saveImage.getValue())
	{
		image_basic test(m_iBufferSizeX, m_iBufferSizeY, COLOR_24BPP);
		for (int y = 0; y < m_iBufferSizeY; y++)
		{
			for (int x = 0; x < m_iBufferSizeX; x++)
			{
				val = ((long)data[iIter+2] << 16) | ((long)data[iIter+1] << 8) | ((long)data[iIter]);
				test.set(x,y,val);
				iIter=iIter+3;
			}
		}
		//cout << "saving image" << endl;
		//test.file_write_bitmap(std::string("c:\\test_neu.bmp"));
		test.file_write_bitmap(std::string(fldFilename.getValue().getString()));
	}

	if (saveAVI.getValue())
	{		
		myAviWriter->addImageToMovieByData(data, m_iBufferSizeX, m_iBufferSizeY);
	}

	delete[] data;
}