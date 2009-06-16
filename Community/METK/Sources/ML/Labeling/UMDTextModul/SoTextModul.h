//----------------------------------------------------------------------------------
//! The Inventor module class SoTextModul derived from SoShape
/*!
// \file    SoTextModul.h
// \author  Florian Link
// \author  Last changed by $Author: Maik Beyer $
// \date    2007-01-17
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __SOTEXTMODUL_H__
#define __SOTEXTMODUL_H__


// Local includes
#include "LabelingSystem.h"

// included because of global font sharing
#ifndef __SoView2D_H
#include "SoView2D.h"
#endif

#include <limits.h>

//#ifndef __SoExtText2_H
//#include "SoExtText2.h"
#ifndef __SoTextModul_H
#include "SoTextModul.h"
#endif

#ifndef __View2DFont_H
#include "View2DFont.h"
#endif

#ifndef __FontManager_H
#include "FontManager.h"
#endif

#include <FTGLPixmapFont.h>
#include <string>
// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SoPickedPoint.h>
#include "XVLeaveScope.h"


class FontManager;

//! 
class LABELING_EXPORT SoTextModul : public SoShape
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoTextModul);

public:
	//! Constructor
	SoTextModul();

	~SoTextModul();
  
	//! Initializes this class (called on dll initialization).
	static void  initClass();


	//! Halo modes
	enum Halo {
		NONE=0,
		SHADOW=1,
		HALO=2
	};


	//! Justification modes
	enum Justification {
		LEFT = 1,
		RIGHT,
		CENTER
	};

	//! Avaliable Font types (font as texture, bitmap, extruded....)
	enum FontRenderType {
		TEXTURE = 1,
		EXTRUDED,
		OUTLINE,
		POLYGON,
		PIXMAP,
		BITMAP
	};

	//! text to draw
	SoSFString string;

	//!own fontpaths
	SoSFString fontFile;
	//!if true the file exist
	SoSFBool fontValid;
	SoSFBool underscore;

	SoSFInt32 fontsize;
	SoSFVec2f textSize;
	SoSFVec2f viewportSize;
	SoSFVec2f translation;
	SoSFVec2f currTextBoxPos;
	SoSFBool  freeze;
	SoSFBool  blend;
	SoSFInt32 lineSpacing;
	SoSFInt32 insetSpace;

	//! halo
	SoSFEnum halo;
	//! justification of text
	SoSFEnum justification;
	//! Font render type (see FTGL help)
	SoSFEnum fontRenderType;
	//! text color
	SoSFColor color;
	//! alpha-value of text color
	SoSFFloat  colorAlpha;

	//! Halo Color
	SoSFColor haloColor;
	//! alpha-value of halo color
	SoSFFloat haloAlpha;

	//! draw black shadow
	//SoSFBool shadow;
	//! draw on top (without depth buffer)
	SoSFBool alwaysOnTop;

	// Added by Wolf:Disable clipping planes if off.
	//! apply clipping planes. Default is true (= clipping is enabled)
	SoSFBool   applyClippingPlanes;
	//! draw a background rectangle
	//SoSFBool   background;
	//! color of rectangle
	SoSFColor  backgroundColor;
	//! blending of rectangle
	SoSFFloat  backgroundAlpha;
	//! called whenever the scene is rendered


	virtual void  GLRender(SoGLRenderAction *action);
	//! called when an SoEvent is passed through the scene
	virtual void  handleEvent(SoHandleEventAction *action);
	//! called when a scene bounding box is computed
	virtual void  computeBBox(SoAction * action, SbBox3f &box, SbVec3f &center);
	//! called by the generate primitives action
	virtual void  generatePrimitives(SoAction *action);
	//! called by the ray pick action
	virtual void  rayPick(SoRayPickAction *action);

	//! set all values to default
	void reset(void);
	void applyFTGLFontParameter(void);
	bool initFTGLFont();
	bool initFTGLFont(const char* _path, int _fontsize, int _renderType );
	void doAction(SoGLRenderAction *action);
	void setColor(SbColor col);

	SbVec2f getTextSize( SbString _text );

protected:

	//! Sensor to detect changes in node's fields.
	SoNodeSensor      *_nodeSensor;

	//! Called by \c _nodeSensor when any field in node changes.
	//! Only redirects the call of this static function to the
	//! class method \c nodeChanged.
	static void       nodeChangedCB(void *data, SoSensor* sens);

	//! Called by the static function \c _nodeChangedCB to notify node changes.
	void              nodeChanged(SoNodeSensor* sensor);

private:

	FontManager* myFontManager;

	bool ftglFontInit;  

	void checkFilepath(void); 
	void useView2DFontRendering(SoGLRenderAction *action);
	void useFTGLFontRendering(SoGLRenderAction *action );
  
};

#endif // __SOTEXTMODUL_H


