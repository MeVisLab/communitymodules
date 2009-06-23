//----------------------------------------------------------------------------------
//! The Inventor module class SoTextureMaskedArea derived from SoView2DExtension
/*!
// \file    SoTextureMaskedArea.h
// \author  Björn Meyer
// \date    2005-07-20
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOTEXTUREMASKEDAREA_H__
#define __SOTEXTUREMASKEDAREA_H__


// Local includes
#include "UMDSoView2DExtensionsSystem.h"

#pragma warning( push, 1 )
// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFString.h>

#include <SoView2DExtension.h>
#include <View2DSlice.h>
#include <View2DSliceList.h>
#include <SoSFMLImage.h>
#pragma warning( pop )

#include <mlLUTFColor.h>

#include "XVLeaveScope.h"

ML_START_NAMESPACE

class LUTFunction;
template <typename T> class LUTData;

ML_END_NAMESPACE

class SoView2D;



//!
class UMDSOVIEW2DEXTENSIONS_EXPORT SoTextureMaskedArea : public SoView2DExtension
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoTextureMaskedArea);

public:
   //! Constructor
   SoTextureMaskedArea();

   //! Initializes this class (called on dll initialization).
   static void  initClass();



   //! Handling of events occurring in the viewer.
   virtual bool evalEvent(SoView2D *view2d, View2DSliceList *list,
      View2DEvent* event, View2DEventPhase phase);

   //! Virtual method called by the \c SoView2D node when an image is rendered.
   virtual void draw(View2DSliceList *list, View2DSlice *slice, int z);

   //! reimplemented from SoNode, stores the LUT for later rendering
   void   GLRender(SoGLRenderAction *action);

   // input connectors for texture image and mask image
   SoSFMLImage inTexture, inMask;

   SoSFColor overlayColour;
   SoSFInt32 currentSlice, chooseTexture;
   SoSFFloat overlayAlpha;
   SoSFBool showTextureOnly;
   SoSFEnum chooseTextureEnum;

protected:
   //! Protected destructor
   virtual            ~SoTextureMaskedArea();

   // function for texture computation
   void computeTexture();

   //! Sensor to detect changes in node's fields.
   SoNodeSensor      *_nodeSensor;
   // field sensor for texture choice
   SoFieldSensor      *_chooseTextureSens;

   //! Called by \c _nodeSensor when any field in node changes.
   //! Only redirects the call of this static function to the
   //! class method \c nodeChanged.
   static void       nodeChangedCB(void *data, SoSensor* sens);
   //! Called by the static function \c _nodeChangedCB to notify node changes.
   void              nodeChanged(SoNodeSensor* sensor);


   // texture selection callback routines
   static void        textureChangedCB(void* userData, SoSensor* sensor);
   void              textureChanged(SoFieldSensor* sensor);


   //! the lut data that is rendered
   ml::LUTData<MLuint8>* _lutData;
   //! the lut from the scene, stored by GLRender
   ml::LUTFunction* _lutFunction;

   ml::LUTFColor _lutTransform;


private:

   // your own member functions...


   // your own member variables...
   int tx, ty, tz, tc, mx, my, mz, ox, oy, oz, drawCounter;
   bool textureOK, mOk, oOk;
   void *textureData, *textureBigData, *maskData, *imageData, *originalData;
   _ML_UINT8 *tPointer, *tgPointer, *tbPointer, *tBPointer, *mPointer, *iPointer, *oPointer;

   enum textureModes {fromFile, Checkerboard, Diagonal1, Diagonal2, Horizontal, Vertical};

};

#endif // __SOTEXTUREMASKEDAREA_H


