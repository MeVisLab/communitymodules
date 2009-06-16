//----------------------------------------------------------------------------------
//! The Inventor module class SoTextModul
/*!
// \file    SoTextModul.cpp
// \author  Florian Link
// \author  Last changed by $Author: Maik Beyer $
// \date    2007-01-17
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoTextModul.h"

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
// TODO: include more inventor things here
#include "XVLeaveScope.h"

SO_NODE_SOURCE(SoTextModul)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoTextModul::initClass()
{
	SO_NODE_INIT_CLASS(SoTextModul, SoShape, "Shape");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoTextModul::SoTextModul()
{
	SO_TRACE_IN("SoTextModul::SoTextModul")

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoTextModul);

	SO_NODE_ADD_FIELD(string, (""));

	SO_NODE_ADD_FIELD(fontFile, ("c:/windows/Fonts/arial.ttf"));
	SO_NODE_ADD_FIELD(fontValid, (0));

	SO_NODE_ADD_FIELD(fontsize, (12) );
	SO_NODE_ADD_FIELD(lineSpacing, (1) );
	SO_NODE_ADD_FIELD(insetSpace, (5) );
	SO_NODE_ADD_FIELD(blend, (true) );
	SO_NODE_ADD_FIELD(textSize, (0,0) );
	SO_NODE_ADD_FIELD(underscore, (false));

	SO_NODE_ADD_FIELD(color, (1,1,1));
	SO_NODE_ADD_FIELD(colorAlpha, (1.0)); 
	SO_NODE_ADD_FIELD(haloColor, (0.5, 0.5, 0.5));
	SO_NODE_ADD_FIELD(haloAlpha, (0.8));
	//  SO_NODE_ADD_FIELD(shadow, (FALSE));

	SO_NODE_ADD_FIELD(backgroundAlpha, (0.2f));
	SO_NODE_ADD_FIELD(backgroundColor, (1,1,1));
	//SO_NODE_ADD_FIELD(background,(TRUE));

	SO_NODE_ADD_FIELD(alwaysOnTop,(TRUE));

	// Added by Wolf:Disable clipping planes if off.
	SO_NODE_ADD_FIELD(applyClippingPlanes, (TRUE));

	SO_NODE_ADD_FIELD(justification, (SoTextModul::LEFT));
	SO_NODE_ADD_FIELD(halo, (SoTextModul::NONE));
	SO_NODE_ADD_FIELD(fontRenderType, (SoTextModul::TEXTURE)); 

	SO_NODE_ADD_FIELD(viewportSize, (0,0));
	SO_NODE_ADD_FIELD(freeze, (false));
	SO_NODE_ADD_FIELD(translation, (0,0));
	SO_NODE_ADD_FIELD(currTextBoxPos, (0,0));

	SO_NODE_DEFINE_ENUM_VALUE(Halo, NONE);
	SO_NODE_DEFINE_ENUM_VALUE(Halo, SHADOW);
	SO_NODE_DEFINE_ENUM_VALUE(Halo, HALO);
	SO_NODE_SET_SF_ENUM_TYPE(halo, Halo);

	SO_NODE_DEFINE_ENUM_VALUE(Justification, LEFT);
	SO_NODE_DEFINE_ENUM_VALUE(Justification, RIGHT);
	SO_NODE_DEFINE_ENUM_VALUE(Justification, CENTER);
	SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);

	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, TEXTURE); //Vector-Type
	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, EXTRUDED);//Vector-Type
	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, OUTLINE);//Vector-Type
	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, POLYGON);//Vector-Type
	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, PIXMAP); //Raster-Type; Antialising
	SO_NODE_DEFINE_ENUM_VALUE(FontRenderType, BITMAP); //Raster-Type 
	SO_NODE_SET_SF_ENUM_TYPE(fontRenderType, FontRenderType);

	// Set inherited fields to a default state using the default string setting
	// mechanism available in OpenInventor. E.g. in this case the extension
	// shall be activated by button 1 and it must be over a valid voxel position
	// to be activated.
	// Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
	// sensor to be sure that changes are not delayed or collected.
	_nodeSensor = new SoNodeSensor(SoTextModul::nodeChangedCB, this);
	_nodeSensor->setPriority(0);
	_nodeSensor->attach(this);

	myFontManager = new FontManager();
	checkFilepath();  
	ftglFontInit=false;
}

// --------------------------------------------------------------------------
//! Check the exist of own font-file
// --------------------------------------------------------------------------
void SoTextModul::checkFilepath()
{  
	FTFont* checkFont = new FTGLPixmapFont(fontFile.getValue().getString() );
	if( checkFont->Error() ) fontValid.setValue(0);
		else fontValid.setValue(1);
	
	delete checkFont;
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoTextModul::~SoTextModul(){
	//std::cout << "START DESTRUKTOR SoTextModul" <<std::endl;
	SO_TRACE_IN("SoTextModul::~SoTextModul")

	delete myFontManager; 
	ftglFontInit=false;
	//std::cout << "ENDE DESTRUKTOR SoTextModul" <<std::endl;
}



void SoTextModul::reset()
{
	string.setValue("");
	fontFile.setValue("c:/windows/Fonts/arial.ttf");  
	fontValid.setValue(false);

	fontsize.setValue(12);
	lineSpacing.setValue(1);
	insetSpace.setValue(5);
	blend.setValue(true);
	textSize.setValue(0,0);
	underscore.setValue(false);
	color.setValue(1,1,1); 
	colorAlpha.setValue(1.0); 
	haloColor.setValue(0.5, 0.5, 0.5);
	haloAlpha.setValue(0.8); 
	backgroundAlpha.setValue(0.2f);
	backgroundColor.setValue(1,1,1);
	//background.setValue(true);
	alwaysOnTop.setValue(true);
	applyClippingPlanes.setValue(true);
	justification.setValue(SoTextModul::LEFT);
	halo.setValue(SoTextModul::NONE);
	fontRenderType.setValue(SoTextModul::TEXTURE); 
	freeze.setValue(false);
	viewportSize.setValue( 0.0, 0.0 );
	translation.setValue( 0.0, 0.0 );
	currTextBoxPos.setValue( 0.0, 0.0);

	delete myFontManager; 
	ftglFontInit=false;
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoTextModul::nodeChangedCB(void *data, SoSensor* sens)
{
  SO_TRACE_IN("SoTextModul::nodeChangedCB");
  ((SoTextModul*) data)->nodeChanged((SoNodeSensor*)sens);
}

void SoTextModul::setColor(SbColor col){
	std::cout  << "COLOR:::::" << col[0] << ";" << col[1] << ";" << col[2] << std::endl;
	color=col;
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoTextModul::nodeChanged(SoNodeSensor* sensor)
{
  SO_TRACE_IN("SoTextModul::nodeChanged")

  // Get the field which caused the notification.
  SoField* field = sensor->getTriggerField();

  // Handle changed fields here
	if (field == &fontFile )
	{
        checkFilepath();
	}
	
	else if ( field==&blend || field==&fontsize || field==&lineSpacing || field==&insetSpace || field==&underscore || 
			  field==&color || field==&colorAlpha   ||  field==&halo || field==&haloAlpha || field==&haloColor || 
			  /*field==&background ||*/ field==&backgroundAlpha || field==&backgroundColor || field==&justification )
	{   
		applyFTGLFontParameter();	  
	}
	
	else if (field==&fontRenderType )
	{
		if ( initFTGLFont() ) applyFTGLFontParameter();		
	}
	
	else if ( field==&freeze)
	{
        translation.setValue(currTextBoxPos.getValue()[0], currTextBoxPos.getValue()[1] );   
	}
}


SbVec2f SoTextModul::getTextSize( SbString _text )
{	
	//SbVec2f returnValue;
    int w, h;
	if (ftglFontInit)
	{
	   if ( myFontManager->isInit() )
	   {
           myFontManager->getTextSize( _text.getString(), w, h);   

		   //std::cout << "XX: textSize.setValue w=" << w << "  h=" << h << std::endl;

           textSize.setValue(w,h);
		   return SbVec2f( w, h);
	   } 
	} 	
	else
	{
		//std::cout << "NOT NOT NOT NOT ftglFontInit" << std::endl;
	}
	//std::cout << "SoTextModul::getTextSize  for " << _text.getString() << std::endl;
	return textSize.getValue();	  
}


void SoTextModul::doAction(SoGLRenderAction *action)
{
	GLRender(action);
}


//! called whenever the scene is rendered
void SoTextModul::GLRender(SoGLRenderAction *action)
{
	SO_TRACE_IN("SoTextModul::GLRender")

	// TODO: do your rendering here, beware to restore the GL state correctly if you change it
	if (!this->shouldGLRender(action)) return;

	//int fontsize;
	SoState * state = action->getState();
  
	const SbViewportRegion & vp = SoViewportRegionElement::get(state);
	SbVec2s vpsize = vp.getViewportSizePixels();
	viewportSize.setValue(vpsize[0], vpsize[1]);

	if ( ftglFontInit ) useFTGLFontRendering(action);
		else useView2DFontRendering(action); 
}


bool SoTextModul::initFTGLFont() {
     
	int rendertype = fontRenderType.getValue();
	if (fontValid.getValue())
	{
		return initFTGLFont(fontFile.getValue().getString(), fontsize.getValue(), rendertype );		
	}
	else
	{
        std::cerr << "Font not found or valid!" << std::endl;
		return false;
	};
}

bool SoTextModul::initFTGLFont( const char* _path, int _fontsize, int _renderType ) {
  bool err = false;  
  switch( _renderType )  {
    case BITMAP:  err =  myFontManager->init( _path, _fontsize, BITMAP);					  
		          break;
	case PIXMAP:  err =  myFontManager->init( _path, _fontsize, PIXMAP );				  
			      break;
    case OUTLINE: err =  myFontManager->init( _path, _fontsize, OUTLINE );
		          break;
    case POLYGON: err =  myFontManager->init( _path, _fontsize, POLYGON );
		          break;
    case EXTRUDED:err =  myFontManager->init( _path, _fontsize, EXTRUDED );		
		          break;
    case TEXTURE: err =  myFontManager->init( _path, _fontsize, TEXTURE );		                
			      break;
	}; //end Switch

  if (err==0) {
    ftglFontInit=true;
  } else {
    ftglFontInit=false;
  }
  return ftglFontInit;
}


void SoTextModul::applyFTGLFontParameter(void)
{   
	if (ftglFontInit)
	{
     
		myFontManager->setGlBlend( blend.getValue() );
		myFontManager->setFontSize( fontsize.getValue() );	
		myFontManager->setColor( color.getValue().getValue()[0], 
					color.getValue().getValue()[1], 
					color.getValue().getValue()[2],
					colorAlpha.getValue() );   
		myFontManager->setLineSpacing( lineSpacing.getValue() );
		myFontManager->setInsetSpace( insetSpace.getValue() );
		myFontManager->setUnderscore( underscore.getValue() );
	    
		//myFontManager->setBackground(background.getValue());
		myFontManager->setBgColor( backgroundColor.getValue().getValue()[0], 
					backgroundColor.getValue().getValue()[1], 
					backgroundColor.getValue().getValue()[2], 
 					backgroundAlpha.getValue());    
		myFontManager->setHaloMode( halo.getValue() );
		myFontManager->setHaloColor(haloColor.getValue().getValue()[0], 
						haloColor.getValue().getValue()[1],
						haloColor.getValue().getValue()[2], 
						haloAlpha.getValue() );
		myFontManager->setAlignment( justification.getValue() ); //stimmt überein mit dem AlignEnum von FontManager
	}   
}

void SoTextModul::useFTGLFontRendering( SoGLRenderAction *action )
{  
	SoState * state = action->getState();
	SbVec3f nilpoint(0.0f, 0.0f, 1.0f);
	const SbMatrix & mat = SoModelMatrixElement::get(state);
	mat.multVecMatrix(nilpoint, nilpoint);

	const SbViewVolume & vv = SoViewVolumeElement::get(state);
	// this function will also modify the z-value of nilpoint according to the view matrix
	vv.projectToScreen(nilpoint, nilpoint);//*/

	const SbViewportRegion & vp = SoViewportRegionElement::get(state);
	SbVec2s vpsize = vp.getViewportSizePixels();

	nilpoint[0] = nilpoint[0] * float(vpsize[0]);
	nilpoint[1] = nilpoint[1] * float(vpsize[1]);

	// Set new state.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT | GL_CLIENT_VERTEX_ARRAY_BIT);

 /* // Set new state.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  //glLoadIdentity();
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  //glLoadIdentity();
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glLoadIdentity();
  */

	glDisable(GL_LIGHTING);

/*  if (alwaysOnTop.getValue()) {
    glDisable(GL_DEPTH_TEST);
  } else {
	glEnable(GL_DEPTH_TEST);
  }*/
 
  // Added by Wolf:Disable clipping planes if off.
	if (!applyClippingPlanes.getValue())
	{
		for (int c=0; c < GL_MAX_CLIP_PLANES; c++) glDisable(GL_CLIP_PLANE0+c);
	}
  
	glPushMatrix();
	glLoadIdentity();  
	glOrtho(0, vpsize[0], 0.0, vpsize[1], -1.0f, 1.0f);// -1.0f, 1.0f);

	int w = 0, h;
	if ( myFontManager->isInit() )
	{
		myFontManager->getTextSize( (char*)(string.getValue().getString()), w, h);   
		textSize.setValue(w,h);
	}
 
	//relativer Punkt /translatiert mit
	float xpos = nilpoint[0];
	float ypos = vpsize[1] -1 - nilpoint[1];
     
    switch (justification.getValue())
	{
		case LEFT:
			break;
		case RIGHT:
			xpos -= w;
			break;
		case CENTER:        
			xpos -= ((float)w)/2;
			break;
		default: // nothing to be done			
			break;
	} 
 
	if (freeze.getValue())
	{
		//absoluter wert
		//ypos = vpsize[1]/2; //mitte des bildschirms
		//ypos +=((float)h)/2;//Texthöhe berücksichtigt
		ypos = translation.getValue()[1];
		//xpos = vpsize[0]/2.0;//mitte Bildschirm		 
		xpos = translation.getValue()[0];
	}
	else
	{ 
		currTextBoxPos.setValue( xpos, ypos);
	}
	 
  
	//glTranslatef(0.0, 0.0, nilpoint[2]);
	glPushMatrix();    
		myFontManager->drawText( xpos, ypos, string.getValue().getString() );
	glPopMatrix();
   

  //KONRAD: WURDE OBEN AUSGESCHALTET
  //if (alwaysOnTop.getValue())
//    glEnable(GL_DEPTH_TEST);

   
	// Pop old GL state.
	glPopClientAttrib();
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
     
}

void SoTextModul::useView2DFontRendering( SoGLRenderAction *action) {
  //copy from SoView2DFont; 
  SoState * state = action->getState();

  SbVec3f nilpoint(0.0f, 0.0f, 0.0f);
  const SbMatrix &mat = SoModelMatrixElement::get(state);
  mat.multVecMatrix(nilpoint, nilpoint);

  const SbViewVolume & vv = SoViewVolumeElement::get(state);

  // this function will also modify the z-value of nilpoint according to the view matrix
  vv.projectToScreen(nilpoint, nilpoint);

  const SbViewportRegion & vp = SoViewportRegionElement::get(state);
  SbVec2s vpsize = vp.getViewportSizePixels();
  nilpoint[0] = nilpoint[0] * float(vpsize[0]);
  nilpoint[1] = nilpoint[1] * float(vpsize[1]);

  // Set new state.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_LIGHTING);

/*  if (alwaysOnTop.getValue())
    glDisable(GL_DEPTH_TEST);*/

  // Added by Wolf:Disable clipping planes if off.
  if (!applyClippingPlanes.getValue()){
    for (int c=0; c < GL_MAX_CLIP_PLANES; c++) glDisable(GL_CLIP_PLANE0+c);
  }

  //glPushMatrix();
  //glLoadIdentity();

  glOrtho(0, vpsize[0], vpsize[1], 0, -1.0f, 1.0f);

  float xpos = nilpoint[0];
  float ypos = vpsize[1] -1 - nilpoint[1];

  View2DFont::AlignXMode align = View2DFont::ALIGN_LEFT;
  float rgbColor[4] = {1,1,1,1};
  int w;
  int h;
  float r,g,b;
  color.getValue().getValue(r,g,b);
  rgbColor[0]=r;
  rgbColor[1]=g;
  rgbColor[2]=b;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  View2DFont* v2dfont = SoView2D::globalFont(action->getCacheContext());
  v2dfont->getTextSize(fontsize.getValue(), (char*)(string.getValue().getString()), w,h);
  textSize.setValue(w,h);
  switch (justification.getValue())
  {
  case LEFT:
    align = View2DFont::ALIGN_LEFT;
    break;
  case RIGHT:
    align = View2DFont::ALIGN_RIGHT;
    xpos -= w;
    break;
  case CENTER:
    xpos -= ((float)w)/2;
    align = View2DFont::ALIGN_CENTER;
    break;
  default:
    // nothing to be done
    break;
  }
  ypos -= ((float)h)/2;

  xpos = floor(xpos);
  ypos = floor(ypos);

  
/*  if (background.getValue()) {
    backgroundColor.getValue().getValue(r,g,b);
    glColor4f(r,g,b,backgroundAlpha.getValue());
    glBegin(GL_POLYGON);{
      glVertex3f(xpos-4,ypos-3    ,nilpoint[2]);
      glVertex3f(xpos-4,ypos+h+3  ,nilpoint[2]);
      glVertex3f(xpos+w+4,ypos+h+3,nilpoint[2]);
      glVertex3f(xpos+w+4,ypos-3  ,nilpoint[2]);
    }
    glEnd();

  }*/
   
    v2dfont->drawText(xpos,ypos,fontsize.getValue(),rgbColor,string.getValue().getString(),align, halo.getValue()?1:0);
      
  glDisable(GL_BLEND);

  glPopAttrib();



  //KONRAD: WURDE OBEN AUSGESCHALTET
  //if (alwaysOnTop.getValue())
//    glEnable(GL_DEPTH_TEST);

  //Pop old GL state.
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  delete v2dfont;
}

//! called when an SoEvent is passed through the scene
void SoTextModul::handleEvent(SoHandleEventAction* /*action*/)
{
  SO_TRACE_IN("SoTextModul::handleEvent")

  // TODO
}

//! called when a scene bounding box is computed
void SoTextModul::computeBBox(SoAction* /*action*/, SbBox3f &box, SbVec3f &center)
{
  SO_TRACE_IN("SoTextModul::computeBBox")

  // TODO
  SbVec3f v0, v1, v2, v3;

  // not implemented!
  box.makeEmpty();
  center = box.getCenter();
}

//! called by the generate primitives action
void SoTextModul::generatePrimitives(SoAction* /*action*/)
{
  SO_TRACE_IN("SoTextModul::generatePrimitives")

  // TODO
}

//! called by the ray pick action
void SoTextModul::rayPick(SoRayPickAction* /*action*/)
{
  SO_TRACE_IN("SoTextModul::rayPick")

  // TODO
}



