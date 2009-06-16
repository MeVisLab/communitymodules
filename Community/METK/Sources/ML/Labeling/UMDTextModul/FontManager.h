#ifndef __FontManager_H
#define __FontManager_H
//----------------------------------------------------------------------------------
//
// Copyright 2004, MeVis gGmbH
// ALL RIGHTS RESERVED
//
// THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
// INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
// DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
// PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
//
//----------------------------------------------------------------------------------
/*!
// \file    FontManager.h
// \author  Florian Link
// \date    3/2004
//
// abstract GL font drawing using FTGL
*/
// Last changed by $Author: wolf $
// $Id: View2DFont.h,v 1.6 2005/04/15 08:24:35 wolf Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoView2D/View2DFont.h,v $
//----------------------------------------------------------------------------------

#include "LabelingSystem.h"

#ifndef __SoView2DSystem_H
#include "SoView2DSystem.h"
#endif

#ifdef WIN32
  #include "windows.h"
  #include "shlobj.h"
#endif
#include <string>
#include <iostream>
#include <math.h>
#include <FTGLTextureFont.h>
#include <FTGLExtrdFont.h>
#include <FTGLOutlineFont.h>
#include <FTGLPolygonFont.h>
#include <FTGLPixmapFont.h>
#include <FTGLBitmapFont.h>

#ifndef __mlUnicode_H
#include <mlUnicode.h>
#endif

//see SoTextModul for indices
#define FTGL_BITMAP 6
#define FTGL_PIXMAP 5
#define FTGL_POLYGON 4
#define FTGL_OUTLINE 3
#define FTGL_EXTRUDE 2
#define FTGL_TEXTURE 1

#define ALIGNMENT_LEFT 1
#define ALIGNMENT_RIGHT 2
#define ALIGNMENT_CENTER 3

class FTFont;


//! a texture font that can be resized to any size
//! internally it uses FTGL Texture fonts of different sizes to give a crisp rendering
class FontManager {

public:
  FontManager();
  FontManager(const char* _filename, int _size, int _mode );
  FontManager(const char* _filename, int _size );
  ~FontManager();

  //! init with given font (either just a font name without .ttf or an absolute path)
  //! requires a valid GL context!
  bool init(const char* fontname, int size);
  bool init(const char* fontname, int size, int mode);

  //! init with given font (either just a font name without .ttf or an absolute path)
  //! additionally you can select how many fonts are created internally and in which sizes
  //! requires a valid GL context!
  bool init(const char* fontname, int sharedContextId, int numFonts, int* sizes);

  //! set Fontsize! Return false if error
  bool setFontSize(int _fontsize );
  //! get Fontsize
  int getFontSize(void){ return (int)fontSize;};

   //! set Textcolor
  void setColor(const float* _color );
  void setColor(float r, float g, float b, float a );
  //! get Textcolor
  void getColor(float& r, float& g, float& b, float& a);

  //! set a flag for textbackground; 1=draw background
  void setBackground(bool _bg) {background = _bg;};
  bool getBackground(void){ return background;};
  
  //! set BackgroundColor
  void setBgColor(const float* _color );
  void setBgColor(float r, float g, float b, float a );
  //! get BackgroundColor
  void getBgColor(float& r, float& g, float& b, float& a);

  //! set Halo color
  void setHaloColor(float r, float g, float b, float a );
  void setHaloColor(const float* _color );
  //! get current color of 
  void getHaloColor(float& r, float& g, float& b, float& a);
  
  void setAlignment( int _alignment ) { alignment=_alignment; };

  //! 0=nothing; 1=shadow; 2=halo
  void setHaloMode(int _status ) { haloStatus=_status;};
  //! 0=nothing; 1=shadow; 2=halo
  int getHaloMode(void){ return haloStatus;};


  //! get the Bounding-BoxKoordinates
  void getTextBox( float& x1, float& y1, float& z1, float& x2, float& y2, float& z2 );
  void getTextBox( float& x1, float& y1, float& x2, float& y2 );
 
  //! get textWidth
  float getTextWidth(void) { return textWidth; };

  //!get textHeight
  float getTextHeight(void) { return textHeight; }; 


  //! draws a multi-line text, lines are separated by \n
  void   drawText(float dx, float dy, float font, float* color, const char* text,int mode,int _shadow);
  void   drawText(float _dx, float _dy, float* _color, const char* _text ,int _align, int _shadow);
  void   drawText( float _dx, float _dy,const char* _text );
  
  //! returns size of the box spanned by text box
  void   getTextSize(const char* _text, int &_width,int &_height);
  //! returns size of the box spanned by text box
  void   getTextSize(float fontsize, const char* text, int &width,int &height);

  
  //! draws a simple string with no linefeeds
  void   drawString(float dx, float dy, float* color, const char* string, int len, int halo);
  void   drawString(float dx, float dy, const char* string, int len ) {
    drawString( dx, dy, color, string, len, haloStatus);
  }
  
  //! get width of simple string
  int    getStringWidth(const char* string, int len);
  
  //! get height of font
  int    getFontHeight(const char* _text);
    
  //! if font set without errors, return true
  bool isValid( void ) { return fontValid; };

  //! return flag for text underscore
  bool isUnderscore(void) {return underscore;};
  void setUnderscore(bool _underscore) { underscore=_underscore; };
  
  void setGlBlend( bool _blend) { glBlend=_blend;};
  bool getGlBlend( void ) { return glBlend;};
  bool isGlBlend( void ) { return glBlend;};

  

  //! if font set without errors, return true
  bool isInit( void ) { return initFont; };
  //! flag if font was initialized
  bool  initFont;

   //! cleanup the fonts (must have current GL context set)
  void cleanup();
 
  //! set the line spacing
  void  setLineSpacing(float spacing) { lineSpacing = spacing; }
  float getLineSpacing(void ) { return lineSpacing; }

  //! set the inset spacing
  void  setInsetSpace(int space) { insetSpace = space; }
  int getInsetSpace(void ) { return insetSpace; }


private:
   //! free old font and load the new one
  bool  loadFont();

  //! true=text underscore; 
  bool underscore;
  //! last cache context id (Inventor)
  //int  _lastContext;

  //! absolute path to ttf font
  std::string fontfile;

  //! fallback ttf font
  std::string _fallback;

  //! font
  FTFont* font;
  FTFont* fontKontur;

  //FTGLPixmapFont* infoFont;
  bool isPixMapFont;

  //! halostatus; 1=shadow, 2=halo
  int haloStatus;
  
  //font rendering type mode 
  int mode;

  //! alignment 1=left, 2=right, 3=center
  int alignment;

  //! start x,y and size in x and y
  float boxSize[6];

  //! textColor
  float color[4];

  //! backgroundColor
  float bgColor[4];

  //! HaloColor (shadow)
  float haloColor[4];

  //! sizes of fonts
  float fontSize;
  //! the line spacing that was set
  float lineSpacing;
  int insetSpace;
  
  //! flag if unicode rendering is possible/enabled
  bool unicodeRendering;
  
  //! flag for textbackground
  bool background;
  //! flag; true, if font loaded without errors
  bool fontValid;

  //! flag for GL_BLEND mode
  bool glBlend;

  float textWidth;
  float textHeight;

  //test debug
  void renderFontmetrics( wchar_t* text ); 
  
 
};

#endif
