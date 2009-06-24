
#include "mnOGLOffscreen.h"

#ifdef USE_WINPBUFFER

CmnOGLOffscreen::CmnOGLOffscreen(HDC m_hgldc, int iWidth, int iHeight)
{
  this->m_hgldc = m_hgldc;
  this->iWidth = iWidth;
  this->iHeight = iHeight;
  
  wglGetExtensionsStringARB = NULL;
  wglCreatePbufferARB = NULL;
  wglGetPbufferDCARB = NULL;
  wglReleasePbufferDCARB = NULL;
  wglDestroyPbufferARB = NULL;
  wglQueryPbufferARB = NULL;
  wglChoosePixelFormatARB = NULL;
}

int CmnOGLOffscreen::getWidth() {return iWidth;}
int CmnOGLOffscreen::getHeight() {return iHeight;}

int CmnOGLOffscreen::init(void)
{
  /* Fehlercodes:
  -1 = Unable to get address for wglGetExtensionsStringARB
  -2 = WGL_ARB_pbuffer extension was not found
  -3 = One or more WGL_ARB_pbuffer functions were not found
  -4 = WGL_ARB_pixel_format extension was not found
  -5 = One or more WGL_ARB_pixel_format functions were not found
  -6 = WGL_ARB_render_texture extension was not found
  -7 = One or more WGL_ARB_render_texture functions were not found
  -8 = Could not find an acceptable pixel format
  -9 = Could not create the p-buffer
  */
  wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	char *ext = NULL;
	
	if( wglGetExtensionsStringARB ) ext = (char*)wglGetExtensionsStringARB( wglGetCurrentDC() ); else return -1;
	
  if( strstr( ext, "WGL_ARB_pbuffer" ) == NULL ) return -2;
  
  wglCreatePbufferARB    = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
  wglGetPbufferDCARB     = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
  wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
	wglDestroyPbufferARB   = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
	wglQueryPbufferARB     = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
  
  if( !wglCreatePbufferARB || !wglGetPbufferDCARB || !wglReleasePbufferDCARB ||!wglDestroyPbufferARB || !wglQueryPbufferARB ) return -3;
	
  if( strstr( ext, "WGL_ARB_pixel_format" ) == NULL ) return -4;
	
  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

  if( !wglChoosePixelFormatARB ) return -5;

	g_pbuffer.hPBuffer = NULL;
	g_pbuffer.nWidth   = iWidth;
	g_pbuffer.nHeight  = iHeight;

	int pf_attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
		WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
		WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};

	unsigned int count = 0;
  int pixelFormat;
	wglChoosePixelFormatARB( m_hgldc,(const int*)pf_attr, NULL, 1, &pixelFormat, &count);

  if( count == 0 ) return -8;
	
	g_pbuffer.hPBuffer = wglCreatePbufferARB( m_hgldc, pixelFormat, g_pbuffer.nWidth, g_pbuffer.nHeight, NULL/*pb_attr*/ );
	g_pbuffer.hDC      = wglGetPbufferDCARB( g_pbuffer.hPBuffer );
	g_pbuffer.hRC      = wglCreateContext( g_pbuffer.hDC );
	if( !g_pbuffer.hPBuffer ) return -9;


  enable();

  glShadeModel(GL_SMOOTH);						
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);			
	glClearDepth(1.0f);							
	glEnable(GL_DEPTH_TEST);					
	glDepthFunc(GL_LEQUAL);						
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glViewport(0,0,iWidth,iHeight);	
  glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double m_dAspect = (iHeight == 0) ? (double)iWidth : (double)iWidth/(double)iHeight;
  double m_dNearPlane   = 0.0; 
	double m_dFarPlane    = 15.0; 
  gluPerspective(45.0, m_dAspect, m_dNearPlane, m_dFarPlane); 
	glMatrixMode(GL_MODELVIEW);

  disable();


  return 1;
}

bool CmnOGLOffscreen::enable()
{
	m_hgldc_old = wglGetCurrentDC();
	m_hglrc_old = wglGetCurrentContext();
  if(!wglMakeCurrent( g_pbuffer.hDC, g_pbuffer.hRC)) return 0;
  return 1;
}

void CmnOGLOffscreen::disable()
{
  wglMakeCurrent( m_hgldc_old, m_hglrc_old);
}

void CmnOGLOffscreen::getPixelData (unsigned char *data)
{
 glReadPixels(0,0,iWidth,iHeight,GL_RGBA,GL_UNSIGNED_BYTE,data);
}
#endif