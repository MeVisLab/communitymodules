//! Class CmnOGLOffscreen and struct PBUFFER.
//! \file mnOGLOffscreen.h

#ifndef MNOGLOFFSCREEN__H
#define MNOGLOFFSCREEN__H

#include "UMDmnOffscreenRendererSystem.h"

#ifdef USE_WINPBUFFER

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "wglext.h"

//! Struct PBUFFER.
typedef struct {

    HPBUFFERARB hPBuffer;
    HDC         hDC;
    HGLRC       hRC;
    int         nWidth;
    int         nHeight;

} PBUFFER;

//! Class CmnOGLOffscreen
class CmnOGLOffscreen
{
  public:
    CmnOGLOffscreen(HDC m_hgldc, int iWidth, int iHeight);
    int init(void);
    bool enable(void);
    void disable(void);
    int getWidth(void);
    int getHeight(void);
    void getPixelData (unsigned char *data);

  private:
    int iWidth, iHeight;
//    HGLRC m_hglrc;
    HDC   m_hgldc;
    HGLRC m_hglrc_old;
    HDC   m_hgldc_old;

    PBUFFER g_pbuffer;

    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
    PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
    PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
    PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
    PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
    PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
};
#endif

#endif