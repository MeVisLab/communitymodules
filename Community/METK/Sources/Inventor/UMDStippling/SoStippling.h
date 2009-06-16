//-------------------------------------------------------------------------------------------------
//! The Inventor module class SoStippling derived from SoSeparator
/*!
// \file    SoStippling.h
// \author  Alexandra Baer
// \date    2006-07-26
//
// 
*/
//-------------------------------------------------------------------------------------------------

#ifndef __SOSTIPPLING_H__
#define __SOSTIPPLING_H__

// Local includes
#include "UMDStipplingSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodes/SoFile.h>

#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbLinear.h>

// include used fields
#include <Inventor/fields/SoSFBool.h>
//#include <Inventor/SbBasic.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFFloat.h>

#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFImage.h>
//#include <Inventor/fields/SoSFString.h>
#include <Inventor/SbString.h>

#include <Inventor/SbBox.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Math.h>
#include <GL/glew.h>

// RP: neue Headerdateien
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShaderProgram.h>
#include <Inventor/nodes/SoShaderParameter1f.h>

#include "XVLeaveScope.h"

class SoGLDisplayList;

//! 
class UMDSTIPPLING_EXPORT SoStippling : public SoGroup
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoStippling);

public:
	//! Constructor
	/*! Constructor that sets default values for the global parameters
	*/
	SoStippling();

	//! Shows cube boundaries of multi-cube in addition to the stippling texture.
	SoSFBool showCubes;

	//! Allows texture size changes.
	SoSFInt32 textureSize;

	//! Enables gamma value changes.
	SoSFFloat gamma;

	//! Enables transparency value changes.
	SoSFFloat transparency;

	//! Enables color value changes.
	SoSFColor		color;

	//! Enables color of stippling points changes.
	SoSFColor		pointColor;

	SoSFString		filename;	// file to read texture from
	SoSFImage		image;		// The texture

	// RP: nur noch ein Input für das Modul, sonst funktioniert interne Struktur nicht
	SoSFNode		input;

	//! Initializes this class (called on dll initialization).
	static void  initClass(void);
  //! Initializes this class (called on dll initialization).
    enum Model {			// Texture model
	MODULATE		= GL_MODULATE,
	DECAL			= GL_DECAL,
	BLEND			= GL_BLEND
    };

    enum Wrap {				// Texture wrap type
	REPEAT			= GL_REPEAT,
	CLAMP			= GL_CLAMP
    };

    // Fields.
    SoSFEnum		wrapS;
    SoSFEnum		wrapT;
    SoSFEnum		model;

	//! Enables blendColor value changes.
    SoSFColor		blendColor;

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
	virtual void	GLRenderBelowPath(SoGLRenderAction* action);
    virtual void	callback(SoCallbackAction *action);


protected:
	//! Protected destructor
	virtual            ~SoStippling();

    // Reads stuff into instance. Returns FALSE on error.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

	//! Sensor to detect changes in node's fields.
	SoNodeSensor	*nodeSensor;
    SoFieldSensor *	filenameSensor;			// These keep the filename fields in sync.
	SoFieldSensor *	imageSensor;


	//! Field callback for changes 
	static void		nodeChangedCB(void *data, SoSensor* sens);

    static void		filenameChangedCB(void *, SoSensor *);
	static void		imageChangedCB(void *, SoSensor *);


	//! Called to notify node changes.
	void			nodeChanged(SoNodeSensor* sens);
	void			filenameChanged(SoFieldSensor* sens);

    int				getReadStatus()		{ return readStatus; }
    void			setReadStatus(int s)	{ readStatus = s; }	



private:

   
  // your own member variables...
    static SbBool   readTexture(const SbString& fname, int &w, int &h, int &nc, AUX_RGBImageRec *&texture);

	//! Computes the required cube size for texture mapping.
	float specifyCubeSize();

	//! Computes the center of an input object.
	SbVec3f getObjOrigin();

	//! Computes the 3 D bounding box of an input object.
	SbBox3f getBoundingBox3D();

	//! Generate a shader program.
	GLhandleARB createShaderProg();

	//! Loads shader text file.
	char * loadShaderFile(const char *fn);

    //own member variables...
	int			_texSize;
	int			_readStatus;
	int			_showCubes;
	int			readStatus;

	float		_transpValue;
	float		_gammaValue;
	float		_currentSize;

	SbColor		_colorValue;
	SbColor		_pointColorValue;
	SbBool		_initCube;
   
	SbMatrix	_objToScreen;
	SbMatrix	_screenToObj;
	SbVec2s		_winSize;
	SbVec2f		_draw2DbBox[4];

	SbViewportRegion _viewRegion;
	SbViewVolume	_viewVolume;

	/* RP: globale Module SoShaderProgram und SoShaderParameter
	 * für den internen Szenegraphen
	 */
	SoShaderProgram *_shaderProgram;
	SoShaderParameter1f *_cubesizeUniform;

	// RP: einzelne GL-Handles nicht mehr benötigt
	/*GLhandleARB		_shader;
	GLint			_shaderLoc;
	GLint			_shaderLocColor;
	GLint			_shaderLocPointColor;
	GLint			_shaderLocTransp;
	GLint			_shaderLocGamma;*/
    
    // Display list info for this texture:
    SoGLDisplayList *renderList;
    float			renderListQuality;
};

#endif // __SOSTIPPLING_H


