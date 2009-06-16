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
// \file    FontManager.cpp
// \author  Florian Link (see View2DFont)
// \author  Last changed by $Author: Maik Beyer $
// \date    2007-01-17
*/
//----------------------------------------------------------------------------------

#ifndef __FontManager_H
#include "FontManager.h"
#endif



#define FontManager_MAXLINEBUF 32000



// buffer for string rendering
static char buf[FontManager_MAXLINEBUF];

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

FontManager::FontManager() {
  
  unicodeRendering=true;//false;
  isPixMapFont=false;
  lineSpacing =1.0;// 0.5;
  insetSpace = 5;
  alignment=ALIGNMENT_LEFT; //alignment
  mode = FTGL_TEXTURE;
  glBlend=true;
  initFont = false;
  underscore = false;
  //_lastContext = -1;
  font = NULL;
  fontKontur = NULL;
  fontSize = -1;
  fontValid = false;
  color[0]=0.0; color[1]=0.0; color[2]=0.0; color[3]=0.0;
  haloStatus=0;
  haloColor[0]=0.5f; haloColor[1]=0.5f; haloColor[2]=0.5f; haloColor[3]=0.2f;
  background=false;
  bgColor[0]=1.0; bgColor[1]=1.0; bgColor[2]=1.0; bgColor[3]=0.0;
  boxSize[0]=0.0;  boxSize[1]=0.0;   boxSize[2]=0.0;  boxSize[3]=0.0;  boxSize[4]=0.0;   boxSize[5]=0.0;
  textWidth=0;
  textHeight=0;

}

FontManager::FontManager(const char* /*_filename*/, int /*_size*/, int /*_mode*/ ) {
  FontManager();
  //bool ok = init(_filename, _size, _mode);
}

FontManager::FontManager(const char* /*_filename*/, int /*_size*/ ) {
  FontManager();
  //bool ok = init(_filename, _size);
}


FontManager::~FontManager() {
  cleanup();
  delete font;
  delete fontKontur;
}


void FontManager::cleanup() {
  fontValid = false;
  initFont = false;
  unicodeRendering=true;
  isPixMapFont=false;
  underscore=false;
  lineSpacing =1.0;// 0.5;
  insetSpace = 5;
  alignment = ALIGNMENT_LEFT;//l=left, 2=right, 3=center
  mode = FTGL_TEXTURE;
  glBlend=true;
  font = NULL;
  fontKontur = NULL;
  fontSize = -1;
  fontValid = false;
  color[0]=0.0; color[1]=0.0; color[2]=0.0; color[3]=0.0;
  haloStatus=0;
  haloColor[0]=0.5f; haloColor[1]=0.5f; haloColor[2]=0.5f; haloColor[3]=0.2f;
  background=false;
  bgColor[0]=1.0; bgColor[1]=1.0; bgColor[2]=1.0; bgColor[3]=0.0;
  boxSize[0]=0.0;  boxSize[1]=0.0;   boxSize[2]=0.0;  boxSize[3]=0.0;  boxSize[4]=0.0;   boxSize[5]=0.0;
  textWidth=0;
  textHeight=0;
}

bool FontManager::init(const char* _filename, int _size, int _mode) {
  //if (initFont) {
    cleanup();
  //}
  fontValid=false;
  initFont = true;
  isPixMapFont=false;
  fontSize = (float)_size;
  fontfile = _filename;
  mode = _mode;  
  return loadFont();  
}

bool FontManager::init(const char* _filename, int _size) {
  return init( _filename, _size, 0);
}


bool FontManager::loadFont(){
  
  delete font; 
  delete fontKontur;
  //unicodeRendering = true;
  bool err = false;
  isPixMapFont=false;
  switch (mode) {	
	case FTGL_TEXTURE: //index 1:			
			font = new FTGLTextureFont( fontfile.c_str() );		
			break;
	case FTGL_EXTRUDE: //index 2
			font = new FTGLExtrdFont( fontfile.c_str() );
			break;
	case FTGL_POLYGON: //index 3
			font = new FTGLPolygonFont( fontfile.c_str() );
			break;
	case FTGL_OUTLINE: //index 4
			font = new FTGLOutlineFont( fontfile.c_str() );
			break;
	case FTGL_PIXMAP: //index 5
			font = new FTGLPixmapFont( fontfile.c_str() );
			isPixMapFont=true;
			break;
	case FTGL_BITMAP: //index 6
			font = new FTGLBitmapFont( fontfile.c_str() );
			break;
	default: //TextureRendering	
		    font = new FTGLTextureFont( fontfile.c_str() );	
  }//end Switch
  
  //font für Kontur anlegen
	fontKontur = new FTGLOutlineFont( fontfile.c_str() );	

  //font->FaceSize(fontSize);
  if ( font->Error()!=0 || fontKontur->Error()!=0 ) {
      std::cerr << "Font could not open" << fontfile << std::endl;
      err |= true;
    } else if (!font->FaceSize((int)fontSize) || !fontKontur->FaceSize((int)fontSize)) {
      std::cerr << "FontManager unable to set font size " << fontSize << std::endl;
      err |= true;
    } else {
		if ( unicodeRendering ) { 
			font->CharMap(ft_encoding_unicode);    
	        fontKontur->CharMap(ft_encoding_unicode);
		}
  };
	if (err==false) {
		fontValid=true;
	} else {
		fontValid=false; initFont=false;
	}
  return err;
}

bool FontManager::setFontSize(int _fontsize ) {

	if (initFont && fontValid) {	  
	  if (!font->FaceSize(_fontsize) || !fontKontur->FaceSize(_fontsize) ) {
		  std::cerr << "FontManager unable to set font size " << fontSize << std::endl;
		  return false;
	  }
	  fontSize=(float)_fontsize;
	  return true;
	} else {
	  std::cerr << "No valid Font!" << fontSize << std::endl;
      return false;
	}
	    
}

void FontManager::setColor(const float* _color ){
	color[0]=_color[0]; color[1]=_color[1]; color[2]=_color[2]; color[3]=_color[3];
	if (isPixMapFont) {//leider wird die änderung nicht gemerkt (z.b. wenn fenster geschlossen wurde und neu aufgemacht->wieder weiß)
		glPixelTransferf( GL_RED_SCALE, color[0] );
		glPixelTransferf( GL_GREEN_SCALE, color[1] );
		glPixelTransferf( GL_BLUE_SCALE, color[2] );
		glPixelTransferf( GL_ALPHA_SCALE, color[3] );
	}
}
void FontManager::setColor(float r, float g, float b, float a ){
	color[0]=r; color[1]=g; color[2]=b; color[3]=a; 
	if (isPixMapFont) { //leider wird die änderung nicht gemerkt (z.b. wenn fenster geschlossen wurde und neu aufgemacht->wieder weiß)
		glPixelTransferf( GL_RED_SCALE, color[0] );
		glPixelTransferf( GL_GREEN_SCALE, color[1] );
		glPixelTransferf( GL_BLUE_SCALE, color[2] );
		glPixelTransferf( GL_ALPHA_SCALE, color[3] );
	}
}
void FontManager::getColor(float& r, float& g, float& b, float& a) {
	r=color[0]; g=color[1]; b=color[2]; a=color[3];
}
void FontManager::setBgColor(const float* _color ){
	bgColor[0]=_color[0]; bgColor[1]=_color[1]; bgColor[2]=_color[2]; bgColor[3]=_color[3]; 
}
void FontManager::setBgColor(float r, float g, float b, float a ){
	bgColor[0]=r; bgColor[1]=g; bgColor[2]=b; bgColor[3]=a; 
}
void FontManager::getBgColor(float& r, float& g, float& b, float& a) {
	r=bgColor[0]; g=bgColor[1]; b=bgColor[2]; a=bgColor[3];
}

void FontManager::setHaloColor(const float* _color ){
	haloColor[0]=_color[0]; haloColor[1]=_color[1]; haloColor[2]=_color[2]; haloColor[3]=_color[3]; 
}
void FontManager::setHaloColor(float r, float g, float b, float a ){
	haloColor[0]=r; haloColor[1]=g; haloColor[2]=b; haloColor[3]=a; 
}
void FontManager::getHaloColor(float& r, float& g, float& b, float& a) {
	r=haloColor[0]; g=haloColor[1]; b=haloColor[2]; a=haloColor[3];
}



void FontManager::renderFontmetrics(wchar_t* myString) {
	float x1, y1, z1, x2, y2, z2;
	font->BBox( myString, x1, y1, z1, x2, y2, z2);
	
	// Draw the bounding box
	glDisable( GL_LIGHTING);
	glDisable( GL_TEXTURE_2D);
    glEnable( GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA

	glColor3f( 0.0, 1.0, 0.0);
	// Draw the front face
	glBegin( GL_LINE_LOOP);
		glVertex3f( x1, y1, z1);
		glVertex3f( x1, y2, z1);
		glVertex3f( x2, y2, z1);
		glVertex3f( x2, y1, z1);
	glEnd();
	// Draw the back face
	if( mode == FTGL_EXTRUDE && z1 != z2)
	{
		glBegin( GL_LINE_LOOP);
			glVertex3f( x1, y1, z2);
			glVertex3f( x1, y2, z2);
			glVertex3f( x2, y2, z2);
			glVertex3f( x2, y1, z2);
		glEnd();
	// Join the faces
		glBegin( GL_LINES);
			glVertex3f( x1, y1, z1);
			glVertex3f( x1, y1, z2);
			
			glVertex3f( x1, y2, z1);
			glVertex3f( x1, y2, z2);
			
			glVertex3f( x2, y2, z1);
			glVertex3f( x2, y2, z2);
			
			glVertex3f( x2, y1, z1);
			glVertex3f( x2, y1, z2);
		glEnd();
	}
		
		// Draw the baseline, Ascender and Descender
	glBegin( GL_LINES);
		glColor3f( 0.0, 0.0, 1.0);
		glVertex3f( 0.0, 0.0, 1.0);
		glVertex3f( font->Advance( myString), 0.0, 1.0);
		glVertex3f( 0.0, font->Ascender(), 1.0);
		glVertex3f( 0.0, font->Descender(), 1.0);
		
	glEnd();
	
	// Draw the origin
	glColor3f( 1.0, 0.0, 0.0);
	glPointSize( 5.0);
	glBegin( GL_POINTS);
		glVertex3f( 0.0, 0.0, 1.0);
	glEnd();
}


void FontManager::drawString(float _dx, float _dy, float* /*_color*/, const char* _string, int _len, int _halo ) {
  
  int buflen = MIN(FontManager_MAXLINEBUF-1, _len);
  strncpy(buf,_string,buflen);
  buf[buflen]=0;

  wchar_t *text = NULL;
  if (unicodeRendering) {
    if (sizeof(wchar_t)==2) {
      text = (wchar_t*)MLConvertUTF8ToUTF16(buf);
    } else if (sizeof(wchar_t)==4) {
      text = (wchar_t*)MLConvertUTF8ToUTF32(buf);
    }
    if (!text) return;
  }
  
  
  if (mode==FTGL_BITMAP || mode==FTGL_PIXMAP ){
        glRasterPos2f( _dx, _dy );	
  }
    glTranslatef( (GLfloat) _dx, (GLfloat) _dy, 1.0);
    
	if (haloStatus==1) {//shadow	  	  
	  float trans = (float)((fontSize/100)*3.0);//3% verschoben für Schatten
	  //zuerst schatten zeichen, dafür schräg nach unten bewegen	    
	  glTranslatef(trans,-trans,0);		  
	  
	  glColor4fv(haloColor);
	  if ( unicodeRendering ) {
        font->Render(text);	  
	  } else {
        font->Render(buf);
	  }
      //und wieder zurück translatieren zum malen des eigentlichen fonts	  
	  glTranslatef( -trans,trans,0);
	  
      
	} else if (_halo==2) {
		
        glColor4fv(haloColor);
		glLineWidth((GLfloat)(fontSize/15.0)); //dicker machen
        if ( unicodeRendering ) {
          fontKontur->Render(text);	  
		} else {
          fontKontur->Render(buf); 
		}
		glLineWidth( 1.0);
		
    } 
		
	if (isPixMapFont) { //langsame, notwendige massnahme; pixmaps werden beim initialisieren generiert und farbänderungen sonst nicht beachtet
		glPixelTransferf( GL_RED_SCALE, color[0] );
		glPixelTransferf( GL_GREEN_SCALE, color[1] );
		glPixelTransferf( GL_BLUE_SCALE, color[2] );
		glPixelTransferf( GL_ALPHA_SCALE, color[3] );
	}else {
		glColor4fv(color);
	}
	if ( unicodeRendering ) {
        font->Render(text);	  
	} else {
        font->Render(buf);	  
	}
	
	//ggf noch unterstreichen
	if (underscore) {
      glColor4fv(color);	  
	  glLineWidth((GLfloat)(fontSize/10.0));
	  //glEnable(GL_LINE_SMOOTH);
	  glBegin(GL_LINES);
	    glVertex3f( boxSize[0], 0.0, 1.0);
	    if ( unicodeRendering ) {
           glVertex3f( font->Advance(text),0.0, 1.0);
		} else {
		   glVertex3f( font->Advance(buf), 0.0, 1.0);
        }		
	  glEnd();
	  glLineWidth(1.0);
	  //glDisable(GL_LINE_SMOOTH);
    }//end isUnderscored
//	glPushMatrix();	
//	renderFontmetrics(text);//debug
//    glPopMatrix();  

  if (text) {
    MLFree(text);
  }

  //zurücktransformieren  
  //glTranslatef( -(int) _dx, -(int) _dy, 0);  
  

}

void FontManager::getTextBox(float& _x1, float& _y1, float& _z1, float& _x2, float& _y2, float& /*_z2*/) {
	_x1=boxSize[0];  _y1=boxSize[1]; _z1=boxSize[2];
	_x2=boxSize[3];  _y2=boxSize[4]; _z1=boxSize[5];
}

void FontManager::getTextBox(float& _x1, float& _y1, float& _x2, float& _y2) {
	_x1=boxSize[0];  _y1=boxSize[1]; 
	_x2=boxSize[3];  _y2=boxSize[4]; 
}


int  FontManager::getStringWidth( const char* string, int len)
{
  float x1, y1, z1, x2, y2, z2;
  int buflen = MIN(FontManager_MAXLINEBUF-1, len);
  strncpy(buf,string,buflen);
  buf[buflen]=0;
  float advance;
  if ( unicodeRendering ) {
    wchar_t *text = NULL;
    if (sizeof(wchar_t)==2) {
      text = (wchar_t*)MLConvertUTF8ToUTF16(buf);
    } else if (sizeof(wchar_t)==4) {
      text = (wchar_t*)MLConvertUTF8ToUTF32(buf);
    }
    if (!text) return 0;
    
	advance = font->Advance(text); //funktion ist buggy
	font->BBox( text, x1, y1, z1, x2, y2, z2);
    MLFree(text);
  } else {
    advance = font->Advance(buf);
	font->BBox( buf, x1, y1, z1, x2, y2, z2);
  }

  int width = (int)advance;//*_fontSize/fontSize; ungenau und passt nicht immer
   
  //return width;
  if (haloStatus==1) {
      width+=(int)ceil(((fontSize/100)*3.0));
	  //width=ceil((x2-x1)+((fontSize/100)*3.0)); 
  }
  return width;
}

int FontManager::getFontHeight(const char* _text)
{
    float x1, y1, z1, x2, y2, z2;
    font->BBox( _text, x1, y1, z1, x2, y2, z2);
	//float test = font->Ascender()+ font->Descender();//*_scale/fontSize;  
    return (int)(font->Ascender()+ font->Descender());//*_scale/fontSize;  
  //if (haloStatus==1) {
   // return ( ceil((y2-y1)+((fontSize/100)*3.0)) ); 
  //}
  
  //return ceil(y2-y1);
	
}


void FontManager::getTextSize(const char* _text, int &_width,int &_height)
{
	if (fontSize < 0 ) setFontSize(12); //defaultwert
	getTextSize(fontSize, _text, _width, _height);
}


void FontManager::getTextSize(float /*_fontSize*/, const char* _text, int &_width,int &_height)
{
	// calculate max width first
	char* p = (char*) _text;
	char* p1 = p;
	int maxWidth = 0;  
	_height = 0;
	while (p1)
	{
		p1 = strchr(p,'\n');
		int len = p1?p1-p:strlen(p);
		int sw = getStringWidth(p,len);
		maxWidth = MAX(maxWidth,sw);
		_height+=(int)(getFontHeight(p)+lineSpacing);//*fontSize;
		p = p1+1;    
	}
	// added one too often, so subtract again   
	_height -=(int)lineSpacing;//*fontSize;	
	_width= maxWidth; 

	float x1, y1, z1, x2, y2, z2;
	font->BBox( _text, x1, y1, z1, x2, y2, z2);

	textWidth=(float)maxWidth;	
	textHeight=(float)_height;
	if (haloStatus==1)
	{
		boxSize[0]=(float)x1;
		boxSize[1]=(float)(y1-((fontSize/100)*3.0));
		boxSize[2]=(float)z1;
		boxSize[3]=(float)(x2+((fontSize/100)*3.0));
		boxSize[4]=(float)y2;
		boxSize[5]=(float)z2;


		textWidth+=(float)ceil((fontSize/100)*3.0);
		textHeight+=(float)ceil((fontSize/100)*3.0);
	}
	else
	{
		boxSize[0]=x1;  boxSize[1]=y1;   boxSize[2]=z1;   boxSize[3]=x2;  boxSize[4]=y2;   boxSize[5]=z2;	 
	}
	//std::cout << "FontManager::getTextSize _width=" << _width << "  _height=" << _height << std::endl;

	_height+=insetSpace;
	_width+=insetSpace;
}

void FontManager::drawText( float _dx, float _dy, const char* _text ){
   drawText(_dx, _dy, fontSize, color, _text, alignment, haloStatus );
}

void FontManager::drawText(float _dx, float _dy, float* _color, const char* _text, int _align, int _shadow) {
    if (fontSize <0 ) {
		fontSize=12; //defaultwert
	}
    drawText(_dx, _dy, fontSize, _color, _text, _align, _shadow);
}

void FontManager::drawText(float _dx, float _dy, float _fontSize, float* /*_color*/, const char* _text, int _align, int /*_shadow*/)
{
	int maxWidth;
	int height;
	getTextSize( _fontSize, _text,maxWidth,height);

	glEnable( GL_CULL_FACE); //glDisable( GL_CULL_FACE);
	glFrontFace( GL_CCW);//default GL_CCW
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_LIGHTING);  
	glEnable( GL_TEXTURE_2D );
	//glPushMatrix();

	if (glBlend) {
		glEnable(GL_BLEND);  
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //glBlendFunc( GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA
	};

	if (mode==FTGL_TEXTURE ) {
		glPolygonMode(GL_FRONT, GL_FILL); //GL_FILL//für korrektes blending der Unterstreichungslinie
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	};

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//debug

	//glPushMatrix();
	if (background)
	{
		int offset = (int)ceil((fontSize/10))+1;
		glColor4fv(bgColor);//
		glBegin(GL_POLYGON); //CCW      
		glVertex3f( _dx-offset, _dy+offset+1, 1.0 );
		glVertex3f( _dx-offset, _dy-textHeight-offset-1, 1.0 );
		glVertex3f( _dx+textWidth+offset+2, _dy-textHeight-offset-1, 1.0 );
		glVertex3f( _dx+textWidth+offset+2, _dy+offset+1, 1.0 );
		glEnd();
	}  
	//glPopMatrix();	

	float dy1=_dy-insetSpace/2; //sutrahieren, weil y-0 unten liegt?
	//dy1+=insetSpace;
	// draw strings
	char* p = (char*) _text;
	char* p1 = p;
	while (p1)
	{    
		p1 = strchr(p,'\n');
		dy1-=getFontHeight(p1);
		int len = p1?p1-p:strlen(p);
		float dx1;
		switch (_align)
		{
		case ALIGNMENT_LEFT:
			dx1=_dx+insetSpace/2;
			break;
		case ALIGNMENT_RIGHT:
			dx1=_dx+maxWidth-getStringWidth(p,len)-insetSpace/2;
			break;
		case ALIGNMENT_CENTER:
			dx1=_dx+maxWidth/2 - getStringWidth(p,len)/2;
			break;
		default:
			std::cerr << "unknown alignment choosen" << std::endl;
			dx1=_dx+insetSpace/2;  //Warum auch immer hier der insetspace halbiert werden muss . anders sieht es nicht aus
			break;
		}
		//dx1+=insetSpace;
		glPushMatrix();	   
		drawString(dx1,dy1, p,len);
		glPopMatrix();     
		p = p1+1;	
		dy1-=lineSpacing;//zeilenabstand noch hinzufügen 
	}
	//glPopMatrix();
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable( GL_CULL_FACE);
}
