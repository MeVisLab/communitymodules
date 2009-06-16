
//----------------------------------------------------------------------------------
//! The Inventor module class SoStippling
/*!
// \file    SoStippling.cpp
// \author  Alexandra Baer
// \date    2006-07-26
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoStippling.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
//#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

// TODO: include more inventor things here
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTextureImageElement.h>
#include <Inventor/elements/SoTextureQualityElement.h>

#include <Inventor/errors/SoReadError.h>
#include <Inventor/errors/SoDebugError.h>

#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
//#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoLocalBBoxMatrixElement.h>

// RP: neue Headerdateien
#include <Inventor/nodes/SoVertexShader.h>
#include <Inventor/nodes/SoFragmentShader.h>
#include <Inventor/nodes/SoShaderParameter3f.h>
#include <Inventor/nodes/SoShaderParameterColor.h>

#include "XVLeaveScope.h"

#include <Math.h>
#include <stdio.h>
#include <stdlib.h>

/*! \brief round()
* 
* This function rounds the input parameter to the nearest integer. 
*/
#define round(x)(x<0?ceil((x)-0.55):floor((x)+0.55))
#define distance(x,y) (sqrt(pow((y[0]-x[0]),2.0f)+pow((y[1]-x[1]),2.0f)+pow((y[2]-x[2]),2.0f)))

#include <GL/gl.h>
#include <GL/glew.h>
//#include <GL/glaux.h>

// for loading the shader files
#include <cstdlib>
#include <string.h>

SO_NODE_SOURCE(SoStippling)


/// Initializes this class. Called on dll initialization.

void SoStippling::initClass()
{
   SO_NODE_INIT_CLASS(SoStippling, SoGroup, "Group");
   
   SO_ENABLE(SoGLRenderAction, SoViewingMatrixElement);
   SO_ENABLE(SoGLRenderAction, SoLazyElement);
   SO_ENABLE(SoGLRenderAction, SoModelMatrixElement);
   SO_ENABLE(SoGLRenderAction, SoViewVolumeElement);
   SO_ENABLE(SoGLRenderAction, SoLocalBBoxMatrixElement);
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoStippling::SoStippling()
{
	SO_TRACE_IN("SoStippling::SoStippling");

	// Execute inventor internal stuff for node construction.
	SO_NODE_CONSTRUCTOR(SoStippling);

	// RP: Feld zum Anschluss des Eingangsgraphen
	SO_NODE_ADD_FIELD(input, (0));

	// Set up field for model properties used for stippling visualization
	SO_NODE_ADD_FIELD(transparency, (1.0));
	SO_NODE_ADD_FIELD(color, (SbColor(1,1,1)));
	SO_NODE_ADD_FIELD(pointColor, (SbColor(0,0,0)));

	// Set up field for texture-mapping and surface illumination changes (correction)
	SO_NODE_ADD_FIELD(gamma,  (1.5));
	SO_NODE_ADD_FIELD(textureSize, (16));
	SO_NODE_ADD_FIELD(showCubes,  (FALSE));

	// Set up field for texture loading
    SO_NODE_ADD_FIELD(filename, (""));
    SO_NODE_ADD_FIELD(image, (SbVec2s(0, 0), 0, 0));
    SO_NODE_ADD_FIELD(wrapS, (REPEAT));
    SO_NODE_ADD_FIELD(wrapT, (REPEAT));
    SO_NODE_ADD_FIELD(model, (MODULATE));
    SO_NODE_ADD_FIELD(blendColor, (SbColor(0,0,0)));

    // Set up enumerations for texture model, which specifies how texture values 
	// are combined with the color values of the fragment being processed
    SO_NODE_DEFINE_ENUM_VALUE(Model, MODULATE);
    SO_NODE_DEFINE_ENUM_VALUE(Model, DECAL);
    SO_NODE_DEFINE_ENUM_VALUE(Model, BLEND);

    SO_NODE_DEFINE_ENUM_VALUE(Wrap, REPEAT);
    SO_NODE_DEFINE_ENUM_VALUE(Wrap, CLAMP);
    
    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(model, Model);
    SO_NODE_SET_SF_ENUM_TYPE(wrapS, Wrap);
    SO_NODE_SET_SF_ENUM_TYPE(wrapT, Wrap);
   
	
	// Create a sensor calling nodeChangedCB if any field changes. Use a priority 0
	// sensor to be sure that changes are not delayed or collected.	
	nodeSensor = new SoNodeSensor(nodeChangedCB, this);
    nodeSensor->setPriority(0);
    nodeSensor->attach(this);

    // Set up sensors to keep the image/filename fields agreeing.
    // Sensors are used instead of field to field connections or raw
    // notification so that the fields can still be attached to/from
    // other fields.
    filenameSensor = new SoFieldSensor(SoStippling::filenameChangedCB, this);
    filenameSensor->setPriority(0);
    filenameSensor->attach(&filename);

	imageSensor = new SoFieldSensor(imageChangedCB, this);
    imageSensor->setPriority(0);
    imageSensor->attach(&image);

    renderList = NULL;  // Display list used for rendering

    isBuiltIn = TRUE;


	// init values 
	// RP: Shader-Handle nicht mehr nötig
	//_shader = 0;
	_transpValue = 1.0;
	_texSize = 16;
	_gammaValue = 1.5;
	_initCube = false;
   
	/* RP: Aufbau des internen Graphen aus diversen SoShaderParametern,
	 * SoVertexShader, SoFragmentShader und SoShaderProgram
	 */
	// create uniform variable nodes and connect to fields
	SoShaderParameterColor *_pointColorUniform  = new SoShaderParameterColor;
	_pointColorUniform->name   = "PointColor";
	_pointColorUniform->value.connectFrom(&pointColor);

	SoShaderParameterColor *_colorUniform  = new SoShaderParameterColor;
	_colorUniform->name   = "Color";
	_colorUniform->value.connectFrom(&color);

	SoShaderParameter1f    *_transparencyUniform   = new SoShaderParameter1f;
	_transparencyUniform->name    = "Transparency";
	_transparencyUniform->value.connectFrom(&transparency);

	SoShaderParameter1f    *_gammaUniform   = new SoShaderParameter1f;
	_gammaUniform->name    = "Gamma";
	_gammaUniform->value.connectFrom(&gamma);

	_cubesizeUniform   = new SoShaderParameter1f;
	_cubesizeUniform->name    = "Cubesize";

	// Create vertex and fragment shader
	SbString fragPath = SbString(getenv("UserProjectsPath"));
	fragPath += SbString("\\sources\\Inventor\\UMDStippling\\fragment.glsl");

	SbString vertPath = SbString(getenv("UserProjectsPath"));
	vertPath += SbString("\\sources\\Inventor\\UMDStippling\\vertex.glsl");

	SoVertexShader   *_vertexShader   = new SoVertexShader;
	_vertexShader->sourceType      = SoShaderObject::FILENAME;
	_vertexShader->sourceProgram   = vertPath;

	SoFragmentShader *_fragmentShader = new SoFragmentShader;
	_fragmentShader->sourceType    = SoShaderObject::FILENAME;
	_fragmentShader->sourceProgram = fragPath;

	// Create shader program
	_shaderProgram = new SoShaderProgram;

	// add all shader nodes to internal network
	addChild(_pointColorUniform);
	addChild(_colorUniform);
	addChild(_transparencyUniform);
	addChild(_gammaUniform);
	addChild(_cubesizeUniform);
	addChild(_vertexShader);
	addChild(_fragmentShader);
	addChild(_shaderProgram);
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoStippling::~SoStippling()
{
   SO_TRACE_IN("SoStippling::~SoStippling");
   
   // Remove the node sensor.
   if (nodeSensor){ delete nodeSensor; nodeSensor = NULL; }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads into instance of SoStippling. Returns FALSE on error.
//
// Use: protected

SbBool SoStippling::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    // Detach filename sensor temporarily
    filenameSensor->detach();

    // Read field info as usual.
    SbBool readOK = SoNode::readInstance(in, flags);

    if (readOK && !filename.isDefault()) {
	// See the comment in SoFile::readInstance for why we do this
	// and don't just let the FieldSensor take care of reading in
	// the image.
	setReadStatus(readOK);
	(*(filenameSensor->getFunction()))(filenameSensor->getData(), NULL);

	// Don't set readOK, because not being able to read the image
	// isn't a fatal error.  But do issue a read error:
	if (getReadStatus() == FALSE)
	    SoReadError::post(in, "Could not read texture file %s",
			      filename.getValue().getString());
    }

    // Reattach sensor
    filenameSensor->attach(&filename);

    return readOK;
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoStippling::nodeChangedCB(void *data, SoSensor* sens)
{
	SO_TRACE_IN("SoStippling::nodeChangedCB")
		
		((SoStippling*) data)->nodeChanged((SoNodeSensor*)sens);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback that sets the filename to "" whenever the image field
//    is changed.
//
// Use: static, internal

void SoStippling::imageChangedCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoStippling *tex = (SoStippling *)data;

    if (tex->image.isIgnored()) return;

    tex->filenameSensor->detach();
    tex->filename.setValue("");
    tex->filename.setDefault(TRUE);
    tex->filenameSensor->attach(&tex->filename);

    if (tex->renderList) {
	tex->renderList->unref();
	tex->renderList = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback that reads in the given file and stuffs it into the
//    image field whenever the filename field changes.
//
// Use: static, internal

void SoStippling::filenameChangedCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoStippling *tex = (SoStippling *)data;

    if (tex->filename.isIgnored()) {
	tex->setReadStatus(FALSE);
	return;
    }

    // Read in image file right away...
    int nx, ny, nc;
    AUX_RGBImageRec *texture;
	unsigned char *bytes = NULL;

    SbBool result = readTexture(tex->filename.getValue(), nx, ny, nc, texture);
    
	if (!result) {

		// Read error is taken care of by readImage() call
		nx = ny = nc = 0;
		texture = NULL;

	}else{
		bytes = texture->data;
	}

    // Detach the image sensor temporarily...
    tex->imageSensor->detach();

    // Set the image to the right value:
    tex->image.setValue(SbVec2s(nx, ny), nc, bytes);

    // And set its default bit so it isn't written out
    tex->image.setDefault(TRUE);

    if (bytes != NULL) delete [] bytes;

    if (tex->renderList) {
	tex->renderList->unref();
	tex->renderList = NULL;
    }
    tex->imageSensor->attach(&tex->image);

    tex->setReadStatus(result);
	free(texture);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs typical action
//
// Use: extender

void SoStippling::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *	state = action->getState();

    if (image.isIgnored() ||
	SoTextureOverrideElement::getImageOverride(state))
	return; // Texture being overriden or this node ignored
    if (isOverride()) {
	SoTextureOverrideElement::setImageOverride(state, TRUE);
    }

    SbVec2s size;
    int nc;
    const unsigned char *bytes = image.getValue(size, nc);
//    int numBytes = size[0]*size[1]*nc;

    SoTextureImageElement::set(state, this, size, nc, bytes,
			       wrapS.getValue(), wrapT.getValue(),
			       model.getValue(), blendColor.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs callback action
//
// Use: extender

void SoStippling::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoStippling::doAction(action);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoStippling::nodeChanged(SoNodeSensor* sensor)
{
	SO_TRACE_IN("SoStippling::nodeChanged");
   
	// Get the field which caused the notification.
	SoField* field = sensor->getTriggerField();

	/* RP: Behandlung des input-Feldes:
	 * angeschlossenen Graphen als letztes Kind in internen Graphen einfügen
	 * oder wieder entfernen
	 */
	if (field == &input) {

		if (input.getValue() != NULL) {
			addChild(input.getValue());
		} else {
			if (getChild(getNumChildren()-1) != _shaderProgram)
				removeChild(getNumChildren()-1);
		}

	}else if(field == &color){

		_colorValue = color.getValue();

	}else if(field == &pointColor){

		_pointColorValue = pointColor.getValue();
		float r,g,b;
		r=0.0;
		g=0.0;
		b=0.0;
		_pointColorValue.getValue(r,g,b);

	}else if(field == &transparency){ 

		_transpValue = transparency.getValue();

	}else if(field == &gamma){

		_gammaValue = gamma.getValue();

	}else if(field == &textureSize){

		_texSize = textureSize.getValue();
	}
}

//-----------------------------------------------------------------------------
//! Called on starting a Viewer
//-----------------------------------------------------------------------------
void SoStippling::GLRenderBelowPath(SoGLRenderAction* action)
{ 
	// RP: Shader Program muss nicht mehr erzeugt werden
	// create shader program and locate position for uniform variables just for the first run
	/*if (!_shader) {
	
		glewInit();
		_shader = createShaderProg();

      // locate Cubesize, gamma value and the desired transparency and color value as uniform variable once
	  // default color is white for stippling
		if (_shader){
			_shaderLoc			= glGetUniformLocationARB(_shader,"Cubesize");
			_shaderLocGamma		= glGetUniformLocationARB(_shader,"Gamma");
			_shaderLocTransp	= glGetUniformLocationARB(_shader,"Transparency");
			_shaderLocColor		= glGetUniformLocationARB(_shader,"Color");
			_shaderLocPointColor		= glGetUniformLocationARB(_shader,"PointColor");

		} else {

			// error occured
			std::cout<<"Shader program and uniform variables have not been created !!"<<std::endl;
		}
   }*/
   // RP: nur rendern, wenn Graph an Input angeschlossen ist
   //if(_shader)
   if(input.getValue() != NULL)
   {

      SoState* state = action->getState();

      // Get the matrices from the state to convert from object to screen space
      _objToScreen = (SoViewingMatrixElement::get(state) * SoProjectionMatrixElement::get(state));
      _screenToObj = _objToScreen.inverse();

      // Get the size of the window from the state _winSize[0] = xSize  _winSize[1]=ySize
      if (SbVec2s(_winSize[0], _winSize[1]) != SoViewportRegionElement::get(state).getWindowSize()) 
      {
         _winSize[0] = SoViewportRegionElement::get(state).getWindowSize()[0];
         _winSize[1] = SoViewportRegionElement::get(state).getWindowSize()[1];
      }
     
      SoGroup::GLRenderBelowPath(action);

      // glUseProgramObjectARB - use the shader
	  // RP: nicht mehr nötig
      //glUseProgramObjectARB(_shader);
	  float cubeSize = specifyCubeSize();
      
	//-------------------------------------------------------------------------------------------------
	/*! glUniform1fARB(loc, value); passes an uniform parameter to the shader program.
	*
	* \param loc
	* the reguired location of the uniform parameter, which can then be used to assign the value to it.
	*
	*\param value
	* for the uniform parameter.
	*/
	//-------------------------------------------------------------------------------------------------
	  // RP: nicht mehr nötig
	  /*glUniform1fARB(_shaderLoc, 0);
      glUniform1fARB(_shaderLoc, cubeSize);
	  glUniform1fARB(_shaderLocGamma, _gammaValue);
	  glUniform1fARB(_shaderLocTransp, _transpValue);

	  float r = 1.0, g = 1.0, b = 1.0;
	  _colorValue.getValue(r,g,b);

	  glUniform3fARB(_shaderLocColor, r,g,b);

	  float pr = 0.0, pg = 0.0, pb = 0.0;
	  _pointColorValue.getValue(pr,pg,pb);

	  glUniform3fARB(_shaderLocPointColor, pr,pg,pb);*/

	  // tranparent objects will be visualize transparent
	  if(_transpValue != 1.0){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  }
	  // RP: cubesize uniform muss noch gesetzt werden
	  _cubesizeUniform->value = cubeSize;

      SoGroup::GLRender(action);
      glDisable(GL_BLEND);
	  // RP: nicht mehr nötig
      //glUseProgramObjectARB(0);
   }
}

void SoStippling::GLRender(SoGLRenderAction* action)
{ 
	SoState* state = action->getState();

  //Textureigenschaften
	 if (image.isIgnored() ||
		SoTextureOverrideElement::getImageOverride(state))
		return; // Texture being overriden or this node ignored
		if (isOverride()) {
		SoTextureOverrideElement::setImageOverride(state, TRUE);
		}

		SbVec2s size;
		int nc;
		const unsigned char *bytes = image.getValue(size, nc);
		int numBytes = size[0]*size[1]*nc;

		float texQuality = SoTextureQualityElement::get(state);
		if (texQuality == 0 || numBytes == 0 || image.isIgnored()) {
		SoGLTextureEnabledElement::set(state, FALSE);
		return;
		}
		else {
		SoGLTextureEnabledElement::set(state, TRUE);
		}	

		// Check for special cases of 1/2 component texture and model
		// DECAL or 3/4 component texture and model BLEND; print out
		// errors in these cases:
		
		int m = model.getValue();
/*		if (nc < 3 && m == DECAL) {
	#ifdef DEBUG
		SoDebugError::post("SoTexture2::GLRender",
				   "Texture model is DECAL, but texture image"
				   " has only %d components (must be 3 or 4).  "
				   "Use imgcopy to convert the image.", nc);
	#endif	    
		SoGLTextureEnabledElement::set(state, FALSE);
		}
		else */
		if (nc > 2 && m == BLEND) {
	#ifdef DEBUG
		SoDebugError::post("SoTexture2::GLRender",
				   "Texture model is BLEND, but texture image"
				   " has %d components (must be 1 or 2).  "
				   "Use imgcopy to convert the image.", nc);
	#endif	    
		SoGLTextureEnabledElement::set(state, FALSE);
		} else {
		// This is kind of weird-- the element builds and uses the
		// display list (which is why we pass it in and assign
		// it) because it sends the GL calls, and needs to know
		// the list if the state is popped.  But this node must
		// manage storage and deletion of the display list, since
		// the list must go away if the node is deleted or the
		// image is changed.

		// See if renderList is valid (in the right context, with
		// the right texture quality):
		int context = SoGLCacheContextElement::get(state);
		if (renderList && renderList->getContext() == context &&
			texQuality == renderListQuality) {
			SoGLTextureImageElement::set(
			state, this, size, nc, bytes, texQuality,
			wrapS.getValue(), wrapT.getValue(),
			m, blendColor.getValue(), renderList);
		}  // Not valid, try to build
		else {
			// Free up old list, if necessary:
			if (renderList) {
			renderList->unref(state);
			renderList = NULL;
			}
			renderList = SoGLTextureImageElement::set(
			state, this, size, nc, bytes, texQuality,
			wrapS.getValue(), wrapT.getValue(),
			m, blendColor.getValue(), NULL);
			if (renderList)
			renderList->ref();
			renderListQuality = texQuality;
		}
		}
}

//-------------------------------------------------------------------------------------------------
//! This function computes the cube size required for this texture mapping method (multi-cube).
//
// the cube size will be determined by using the current texture size 
// this texture size will be projected to object space and used as the initially cube size
//-------------------------------------------------------------------------------------------------

float SoStippling::specifyCubeSize(){
   
   
   //getObjOrigin to determine z for back-projection of texture size
   SbVec3f box3DCenter, objOrigin2D, minVal, maxVal;
   
   //determine z
   box3DCenter		= getObjOrigin();
   _objToScreen.multVecMatrix(SbVec3f(box3DCenter[0], box3DCenter[1], box3DCenter[2]), objOrigin2D);
   
   //project length of texture to objectspace and determine length there
   float texWidth	= (_texSize * 2.0)/float (_winSize[0]);
   float z			= objOrigin2D[2];
   
   _screenToObj.multVecMatrix(SbVec3f(0.0,0.0,z),minVal);
   _screenToObj.multVecMatrix(SbVec3f(texWidth, 0.0,z),maxVal);
   
   float size = distance(minVal,maxVal);
   
   float newSize;
   
   if(_initCube != 1)
   {
      //init CubeSize found
      _initCube = true;
      _currentSize = size;
      
      return _currentSize;
      
   }else{
      
      int n = 0;
      newSize = size;
      
      //test if new size is pow(2.0,n) of current size
      //if true set new size to current size
      while(n<9)
      {
         //increased cubes
         if(size >= _currentSize)
         {
            if(size < (_currentSize * pow(2.0,n)))
            {
               break;	
               
            }else{
               
               newSize = _currentSize * pow(2.0,n);
            }
            //decreased cubes
         }else{
            
            //smaller cube than current cube
            if(size > (_currentSize * (1.0/pow(2.0,n))))
            {
               break;
               
            }else{
               
               newSize = _currentSize * (1.0/pow(2.0,n));
            }
         }
         n++;
      }
   }
   return newSize;
}

//-------------------------------------------------------------------------------------------------
//! Calculation to get the coordinates of the object center.
//-------------------------------------------------------------------------------------------------

SbVec3f SoStippling::getObjOrigin(){
   
   //determine center of box
   SbVec3f bBoxMin, bBoxMax, center;
   getBoundingBox3D().getBounds(bBoxMin,bBoxMax);
   
   //save coordinates of object origin to SbVec3f center
   center[0] = (bBoxMin[0] + bBoxMax[0])/2.0;
   center[1] = (bBoxMin[1] + bBoxMax[1])/2.0;
   center[2] = (bBoxMin[2] + bBoxMax[2])/2.0;
   
   return center;
}

//-------------------------------------------------------------------------------------------------
//3D bounding box
//-------------------------------------------------------------------------------------------------

SbBox3f SoStippling::getBoundingBox3D(){
   
   // Create and add GetBoundingBoxAction 
   SoGetBoundingBoxAction bBoxAction(_viewRegion);
   bBoxAction.apply(this);
   
   return bBoxAction.getBoundingBox();
}

//-------------------------------------------------------------------------------------------------
/*! This function creates and compiles a shader program based on the text file read 
* with the \c loadShaderFile method.
*/
// furthermore definition of uniform parameter (Cubesize)
//-------------------------------------------------------------------------------------------------

GLhandleARB SoStippling::createShaderProg() {
   
   GLhandleARB vertexSh,fragmentSh, progSh = 0;
   
   char *vs = NULL,*fs = NULL;
   
   //GlhandleARB v,f (object that will act as a shader container)
   //glCreateShaderObjectARB - allocate the data structures necessary for storing an OpenGL Shader
   vertexSh	= glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
   fragmentSh	= glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   
   // glShaderSourceARB - provide the shader source in 'text' form (done in subroutine loadShaderFile)
   SbString fragPath = SbString(getenv("UserProjectsPath"));
   fragPath += SbString("\\sources\\Inventor\\UMDStippling\\fragment.glsl");

   SbString vertPath = SbString(getenv("UserProjectsPath"));
   vertPath += SbString("\\sources\\Inventor\\UMDStippling\\vertex.glsl");
   vs = loadShaderFile(vertPath.getString());
   fs = loadShaderFile(fragPath.getString());
   
   if (vs && fs) {
      const char * ff = fs;
      const char * vv = vs;
      
      //add source code (string or pointer to string)
      //glShaderSource(GLuint shader, int numOfStrings, const char **strings, int *lenOfStrings);
      //shader - the handler to the shader
      //numOfStrings - the number of strings in the array
      //strings - the array of strings
      //lenOfStrings - an array with the length of each string, or NULL, meaning that the strings are NULL terminated.
      glShaderSourceARB(vertexSh, 1, &vv,NULL);
      glShaderSourceARB(fragmentSh, 1, &ff,NULL);
      
      free(vs);free(fs);
      
      //compile the shader
      glCompileShaderARB(vertexSh);
      glCompileShaderARB(fragmentSh);
      
      // glCreateProgramObjectARB - shaders must be wrapped within so-called Program Objects.
      // Program Objects are used by OpenGL to manage shaders
      progSh = glCreateProgramObjectARB();
      
      // glAttachObjectARB - shaders must be attached to a program object
      glAttachObjectARB(progSh,fragmentSh);
      glAttachObjectARB(progSh,vertexSh);
      
      // glLinkProgramARB - link together the compiled shaders
      glLinkProgramARB(progSh);
   }
   return progSh;
}

//-------------------------------------------------------------------------------------------------
// This function loads the shader file.

/*! \param *fn
* text file, that will be transformed into shader code.
*/
//-------------------------------------------------------------------------------------------------
char * SoStippling::loadShaderFile(const char *fn) {
   
   FILE *fp;
   char *content = NULL;
   
   int	count=0;
   
   
   if (fn != NULL) 
   {
      fp = fopen(fn,"rt");
      
      if (fp != NULL) 
      {
         fseek(fp, 0, SEEK_END);
         count = ftell(fp);
         rewind(fp);
         
         if (count > 0) 
         {
            content = (char *)malloc(sizeof(char) * (count+1));
            count = fread(content,sizeof(char),count,fp);
            content[count] = '\0';
         }
         fclose(fp);
      } else {
         
         std::cerr << "could not open file: " << fn << std::endl;
      }
   }
   return content;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    read passed image file
//
// Use: static, protected
//
////////////////////////////////////////////////////////////////////////
SbBool SoStippling::readTexture(const SbString& fname, int &w, int &h, int &nc, AUX_RGBImageRec *&texture){

	// if there is no image loaded return texture = NULL for used for image field;
    texture = NULL;

    SoInput in;

    if (!in.openFile(fname.getString(), TRUE)) {

		#ifdef DEBUG
			SoDebugError::postInfo("SoStippling::readTexture",
				"Reading texture image %s failed",fname.getString());
		#endif 

		return FALSE;
    }

#ifdef DEBUG
    SoDebugError::postInfo("SoStippling::readTexture","Reading texture image %s",fname.getString());
#endif

	FILE *fp = in.getCurFile();
    //fseek(fp, 0, SEEK_SET);
    
    if (fp == NULL) return FALSE;

	// load texture wihle using ausDIBImageLoad function and save width and height 
	texture = auxDIBImageLoad((LPCWSTR)fname.getString());

	/* RP: eingefügt, damit das Modul nicht abstürzt,
	 * wenn ein nicht unterstütztes Bildformat ausgewählt wurde
	 */
	if (texture == NULL) return FALSE;

	w = texture->sizeX;
	h = texture->sizeY;
	nc = 3;

    return TRUE;
}



