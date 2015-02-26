//----------------------------------------------------------------------------------
//! The Inventor module class SoTextureMaskedArea
/*!
// \file    SoTextureMaskedArea.cpp
// \author  Björn Meyer
// \date    2005-07-20
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "SoTextureMaskedArea.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
// TODO: include more inventor things here
#include "XVLeaveScope.h"


#ifndef __SoView2D_H
#include "SoView2D.h"
#endif
#ifndef __View2DSlice_H
#include "View2DSlice.h"
#endif
#ifndef __View2DSliceList_H
#include "View2DSliceList.h"
#endif
#ifndef __mlLUTFunction_H
#include "mlLUTFunction.h"
#endif
#ifndef __mlLUTData_H
#include "mlLUTData.h"
#endif

#include "SoMLLUTElement.h"


SO_NODE_SOURCE(SoTextureMaskedArea)


// --------------------------------------------------------------------------
//! Initializes this class. Called on dll initialization.
// --------------------------------------------------------------------------
void SoTextureMaskedArea::initClass()
{
   SO_NODE_INIT_CLASS(SoTextureMaskedArea, SoView2DExtension, "SoView2DExtension");
}

// --------------------------------------------------------------------------
//! Constructor, creates fields
// --------------------------------------------------------------------------
SoTextureMaskedArea::SoTextureMaskedArea()
{
   // Execute inventor internal stuff for node construction.
   SO_NODE_CONSTRUCTOR(SoTextureMaskedArea);
   
   SO_NODE_ADD_FIELD(inTexture, ());
   SO_NODE_ADD_FIELD(inMask, ());
   SO_NODE_ADD_FIELD(overlayColour, (1,1,1));
   SO_NODE_ADD_FIELD(currentSlice, (0));
   SO_NODE_ADD_FIELD(overlayAlpha, (0.2));
   SO_NODE_ADD_FIELD(showTextureOnly, (false));
   
   SO_NODE_ADD_FIELD(chooseTexture, (0));
   SO_NODE_ADD_FIELD(chooseTextureEnum,(fromFile));
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, fromFile);
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, Checkerboard);
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, Diagonal1);
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, Diagonal2);
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, Horizontal);
   SO_NODE_DEFINE_ENUM_VALUE(textureModes, Vertical);
   SO_NODE_SET_SF_ENUM_TYPE(chooseTextureEnum, textureModes);
   
   //! Set inherited fields to a default state using the default string setting
   //! mechanism available in OpenInventor. E.g. in this case the extension
   //! shall be activated by button 1 and it must be over a valid voxel position
   //! to be activated.
   set("button1 IGNORED button2 IGNORED button3 IGNORED needsValidVoxel TRUE");
   
   
   
   // Create a sensor calling _nodeChangedCB if any field changes. Use a priority 0
   // sensor to be sure that changes are not delayed or collected.
   _nodeSensor = new SoNodeSensor(SoTextureMaskedArea::nodeChangedCB, this);
   _nodeSensor->setPriority(0);
   _nodeSensor->attach(this);
   
   
   //new field sensor for crosshair selection
   _chooseTextureSens = new SoFieldSensor(SoTextureMaskedArea::textureChangedCB, this);
   _chooseTextureSens->attach(&chooseTextureEnum);
   
   tx=0; ty=0; tz=0; mx=0; my=0; mz=0; ox=0; oy=0; oz=0;
   drawCounter = 0;
   textureOK = false;
   oOk = false;
   
   
   _lutData = new ml::LUTData<MLuint8>;
}

// --------------------------------------------------------------------------
//! Protected destructor to cleans up internally created nodes and node sensor.
// --------------------------------------------------------------------------
SoTextureMaskedArea::~SoTextureMaskedArea()
{
   delete _lutData;
   
   // Remove the node sensor.
   if (_nodeSensor){ delete _nodeSensor; _nodeSensor = NULL; }
}

//-----------------------------------------------------------------------------
// Called by nodeSensor when any field in node changes. Only redirect the call
// of this static function to the nodeChanged method.
//-----------------------------------------------------------------------------
void SoTextureMaskedArea::nodeChangedCB(void *data, SoSensor* sens)
{
   ((SoTextureMaskedArea*) data)->nodeChanged((SoNodeSensor*)sens);
}

//-----------------------------------------------------------------------------
//! Called on any change on the node, field might by also NULL
//-----------------------------------------------------------------------------
void SoTextureMaskedArea::nodeChanged(SoNodeSensor* sensor)
{
   // Get the field which caused the notification.
   SoField* field = sensor->getTriggerField();
   
   // Handle changed fields here
   if (field == &inMask)
   {
      if ( (chooseTexture.getValue() == 0) || inMask.isValid()) textureOK = false;
   }
   
}


void SoTextureMaskedArea::textureChangedCB(void* data, SoSensor* sensor)
{
   ((SoTextureMaskedArea*) data)->textureChanged((SoFieldSensor*)sensor);
}


void SoTextureMaskedArea::textureChanged(SoFieldSensor* /*sensor*/)
{
   switch (chooseTextureEnum.getValue())
   {
   case fromFile			: chooseTexture.setValue(0);
      break;
   case Checkerboard	: chooseTexture.setValue(1);
      break;
   case Diagonal1		: chooseTexture.setValue(2);
      break;
   case Diagonal2		: chooseTexture.setValue(3);
      break;
   case Horizontal		: chooseTexture.setValue(4);
      break;
   case Vertical			: chooseTexture.setValue(5);
      break;
   }
   textureOK = false;
}


// -------------------------------------------------
// View2DExtension virtual methods
// -------------------------------------------------
bool SoTextureMaskedArea::evalEvent(SoView2D* /*view2d*/, View2DSliceList* /*slicelist*/,
                                    View2DEvent* /*ec*/, View2DEventPhase /*phase*/)
{
   return FALSE;
}


void SoTextureMaskedArea::draw(View2DSliceList *dsl, View2DSlice *dslice, int /*slice*/)
{
   // get size of the original image
   dsl->getImageSize(ox, oy, oz);
   // get size of the mask
   inMask.getSize(mx, my, mz);
   
   // set texture size for build-in textures
   switch (chooseTexture.getValue())
   {
   case 1 :	tx = 2;	ty = 2; tz = 1; tc = 3;
      break;
   case 2 :	tx = 3;	ty = 3; tz = 1; tc = 3;
      break;
   case 3 :	tx = 3;	ty = 3; tz = 1; tc = 3;
      break;
   case 4 :	tx = 1;	ty = 2; tz = 1; tc = 3;
      break;
   case 5 :	tx = 2;	ty = 1; tz = 1; tc = 3;
      break;
   }
   
   // if no valid texture at input and choice is 'from file' -> don't do anything
   if ( !inTexture.isValid() && (chooseTexture.getValue()==0) )
   {
      tx=0; ty=0,tz=0;  tc=0;
      textureOK = false;
   }
   // else -> get size of input texture
   else if( inTexture.isValid() && (chooseTexture.getValue()==0)) inTexture.getSize(tx, ty, tz, tc);
   
   std::cout << "Texture Size is X=" << tx << "   Y=" << ty << "   Z=" << tz << "   C=" << tc << std::endl;
   
   // allocate memory
   textureData = malloc(tc*tx*ty*MLSizeOf(MLuint8Type));
   textureBigData = malloc(3*ox*oy*MLSizeOf(MLuint8Type));
   maskData = malloc(mx*my*MLSizeOf(MLuint8Type));
   originalData = malloc(ox*oy*MLSizeOf(MLuint8Type));
   imageData = malloc(3*ox*oy*MLSizeOf(MLuint8Type));
   
   
   // check masks size -> only draw, if it's equal to original image size
   if ( (mx != ox) || (my != oy) )
   {
      std::cout << "Maske passt nicht zum Originalbild - bitte korrekte Bilder nutzen!" << std::endl;
   }
   else
   {
      // check if memory was allocated
      if ( (textureData == NULL) || (textureBigData == NULL) || (maskData == NULL) || (imageData == NULL) )
      {
         std::cout << "Insufficient memory for allocation available!" << std::endl;
      }
      // if so ->
      else
      {
         // set pointers to image (mask, texture, ...) memory 
         //_ML_UINT8 *tPointer, *tBPointer, *mPointer, *iPointer, *oPointer;
         tPointer	= (_ML_UINT8 *) textureData;
         tBPointer	= (_ML_UINT8 *) textureBigData;
         mPointer	= (_ML_UINT8 *) maskData;
         iPointer	= (_ML_UINT8 *) imageData;
         oPointer	= (_ML_UINT8 *) originalData;
         
         // get original
         oOk = dsl->getInputImage()->getTile3D(originalData, static_cast<MLDataType>(MLuint8Type), 0, 0, currentSlice.getValue(), ox, oy, 1);
         
         // compute texture, if not done already
         if (!textureOK) computeTexture();
         
         
         // Drawing field enabled, i.e. are we supposed to draw?
         if ( drawingOn.getValue() && textureOK )
         {
            // get pointer to overlay texture
            View2DTexture* overlay = dslice->getOverlayTexture(this, 0);
            
            if (drawCounter > 0)
            {
               // we share the lut mode of the View2D
               //if (chooseTexture.getValue() == 0) dsl->getOwner()->updateLutData(_lutFunction, _lutData, &inTexture);
               //else dsl->getOwner()->updateLutData(_lutFunction, _lutData, &inMask);
               dsl->getOwner()->updateLutData(_lutFunction, _lutData, &inTexture, View2DLutPrecision::VIEW2D_LUT_AUTO);
               
               //_lutFunction = new ml::LUTFunction(ml::LUT_RGB, true);
               //_lutFunction->renderRescaled(_lutData, 0x00, 0xFF);
               
               //_lutData->init(ml::LUT_RGB, 0, 0x00, 0xFF);
               
               //_lutFunction = dsl->getOwner()->getDefaultLut();
               
               //_lutFunction->setRelative(true);
               
               //					_lutTransform.setInputLUT(_lutFunction);
               //					_lutTransform.setVisualType(ml::LUT_RGB);
               
               //					_lutData->clear();
               //					_lutFunction->render(_lutData);
               
               
               // set lut, so that the overlay texture is not affected by lut-changes in the original image
               //overlay->setLut(_lutData, dsl->getCurrentCacheContext());
               
               // pass memory area to overlay texture
               overlay->setData(imageData, ox, oy, MLuint8Type, 3);
               drawCounter--;
            }
            
            SbVec3f v, w;
            float vp1[3], vp2[3], vp3[3];
            
            // transform overlay image to slice device coords
            v = SbVec3f(0, 0, 0);
            inMask.mapVoxelToWorld(v, w);
            dslice->mapWorldToDevice(w, vp1[0], vp1[1]);
            v = SbVec3f(mx, 0, 0);
            inMask.mapVoxelToWorld(v, w);
            dslice->mapWorldToDevice(w, vp2[0], vp2[1]);
            v = SbVec3f(0, my, 0);
            inMask.mapVoxelToWorld(v, w);
            dslice->mapWorldToDevice(w, vp3[0], vp3[1]);
            vp1[2] = vp2[2] = vp3[2] = 0;
            
            
            // compose colour with alpha value
            float colour[4];
            colour[0] = overlayColour.getValue()[0]; colour[1] = overlayColour.getValue()[1]; colour[2] = overlayColour.getValue()[2]; colour[3] = overlayAlpha.getValue();
            
            // draw texture
            //dslice->drawTexture(overlay, vp1, vp2, vp3, BLEND_BLEND, colour, dsl->getCurrentCacheContext());
         }
         
         // free memory
         free(textureData);
         free(textureBigData);
         free(maskData);
         free(originalData);
      } // else (memory was allocated)
   } // else (mask size equal original)
}


void SoTextureMaskedArea::computeTexture()
{
   if (oOk)
   {
      textureOK = true;
      
      /*
      // get texture from file or create build-in textures
      switch (chooseTexture.getValue())
      {
      case 0 :	inTexture.getTile3D(textureData, MLuint8Type, 0, 0, 0, tx, ty, tz);
      break;
      case 1 :	*tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++;
      *tPointer = 0xFF; tPointer++; *tPointer = 0;
      tPointer	= (_ML_UINT8 *) textureData;
      break;
      case 2 :	*tPointer = 0xFF; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
      *tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0; tPointer++;
      *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0xFF;
      tPointer	= (_ML_UINT8 *) textureData;
      break;
      case 3 :	*tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++;
      *tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0; tPointer++;
      *tPointer = 0xFF; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0;
      tPointer	= (_ML_UINT8 *) textureData;
      break;
      case 4 :	*tPointer = 0xFF; tPointer++;
      *tPointer = 0;
      tPointer	= (_ML_UINT8 *) textureData;
      break;
      case 5 :	*tPointer = 0xFF; tPointer++; *tPointer = 0;
      tPointer	= (_ML_UINT8 *) textureData;
      break;
      }
      */
      // TEST FOR COLOURED TEXTURES
      // get texture from file or create build-in textures
      switch (chooseTexture.getValue())
      {
      case 0 :	//inTexture.getTile3D(textureData, MLuint8Type, 0, 0, 0, tx, ty, tz);
         if (tc == 3)
         {
            XVImageSize p1(0, 0, 0, 0, 0, 0);
            XVImageSize p2(tx, ty, tz, 3, 1, 1);
            inTexture.getTile6D(textureData, static_cast<MLDataType>(MLuint8Type), p1, p2);
         }
         else if (tc == 1)
         {
            inTexture.getTile3D(textureData, 
               static_cast<MLDataType>(MLuint8Type), 
               0, 0, 0, 
               tx, ty, tz);
         }
         break;
      case 1 :	*tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         tPointer	= (_ML_UINT8 *) textureData;
         break;
      case 2 :	*tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF;
         tPointer	= (_ML_UINT8 *) textureData;
         break;
      case 3 :	*tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0;
         tPointer	= (_ML_UINT8 *) textureData;
         break;
      case 4 :	*tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0;
         tPointer	= (_ML_UINT8 *) textureData;
         break;
      case 5 :	*tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++; *tPointer = 0xFF; tPointer++;
         *tPointer = 0; tPointer++; *tPointer = 0; tPointer++; *tPointer = 0;
         tPointer	= (_ML_UINT8 *) textureData;
         break;
      }
      
      tPointer = (_ML_UINT8 *) textureData;
      tgPointer = (_ML_UINT8 *) textureData;
      tbPointer = (_ML_UINT8 *) textureData;
	  int setdummy=0;
      for (setdummy; setdummy < tx*ty; setdummy++)
      {
         tgPointer++;
         tbPointer++;
      }
      for (setdummy=0; setdummy < tx*ty; setdummy++) tbPointer++;
      _ML_UINT8 *tPPos, *tgPPos, *tbPPos;
      tPPos = tPointer;
      tgPPos = tgPointer;
      tbPPos = tbPointer;
      
      
      // get mask
      mOk = inMask.getTile3D(maskData, static_cast<MLDataType>(MLuint8Type), 0, 0, currentSlice.getValue(), mx, my, 1);
      
      // tile the small texture onto a big one ...
      //if ( (tx<ox) || (ty<oy) )
      //{
      // handle black-and-white textures
      if (tc == 1){
         for (int cY=0; cY<oy; cY++)
         {
            for (int cX=0; cX<ox; cX++)
            {
               // set 3 values (rgb)
               *tBPointer++ = *tPointer;
               *tBPointer++ = *tPointer;
               *tBPointer++ = *tPointer++;
               
               // check texture x-size and start from beginning
               if ( ((cX+1)%tx)==0 )
               {
                  tPointer = tPPos;
                  
                  //go to current y position in texture
                  for (int row=0; row<((cY+1)%ty); row++)
                  {
                     for (int col=0; col<tx; col++) tPointer++;
                  }
               }// if
            } // for cX
            
            // check texture y-size and start from beginning
            if ( ((cY+1)%ty)==0 ) tPointer = tPPos;
         } // for cY
      }//if
      
      // handle coloured textures
      else if (tc == 3){
         for (int cY=0; cY<oy; cY++)
         {
            for (int cX=0; cX<ox; cX++)
            {
               *tBPointer++ = *tPointer++;
               *tBPointer++ = *tgPointer++;
               *tBPointer++ = *tbPointer++;
               
               // check texture x-size and start from beginning
               if ( ((cX+1)%tx)==0 )
               {
                  tPointer = tPPos;
                  tgPointer = tgPPos;
                  tbPointer = tbPPos;
                  
                  //go to current y position in texture
                  for (int row=0; row<((cY+1)%ty); row++)
                  {
                     for (int col=0; col<tx; col++) 
                     {
                        tPointer++;
                        tgPointer++;
                        tbPointer++;
                     }
                  }
               }// if
            }// cX
            
            // check texture y-size and start from beginning
            if ( ((cY+1)%ty)==0 )
            {
               tPointer = tPPos;
               tgPointer = tgPPos;
               tbPointer = tbPPos;				
            }
         } // for cY
      }//if
      
      // set pointers to start values
      tPointer = tPPos;
      tgPointer = tgPPos;
      tbPointer = tbPPos;
      tBPointer	= (_ML_UINT8 *) textureBigData;
      //} // if (tx<ox)...
      
      //... or choose the original texture if it is big enough
      //else
      //{
      //	tPointer = (_ML_UINT8 *) textureData;
      //	tBPointer = tPointer;
      //}
      
      // combine mask with texture
      if (mOk)
      {
         // for each pixel...
         for ( int i=0; i < ox*oy; i++)
         {
            // set complete texture overlay to texture map (non-masked)
            if (showTextureOnly.getValue())
            {
               *iPointer++ = *tBPointer++;
               *iPointer++ = *tBPointer++;
               *iPointer = *tBPointer;
            }
            
            else 
            {
               _ML_UINT8 *testPointer;
               testPointer= tBPointer;
               // if mask is set and one of the textures rgb values is set --> image = texture
               if ( (*mPointer > 0) && ((*testPointer++ > 0) || (*testPointer++ > 0) || (*testPointer > 0) ) )
               { 
                  *iPointer++ = *tBPointer++;
                  *iPointer++ = *tBPointer++;
                  *iPointer = *tBPointer;
               }
               // if mask is not set  --> image = 0
               else if (*mPointer == 0)
               {
                  *iPointer++ = 0;
                  *iPointer++ = 0;
                  *iPointer = 0;
                  
                  tBPointer++;tBPointer++;
               }
               // else --> set original value
               else
               {
               /*
               *iPointer++ = *oPointer;
               *iPointer++ = *oPointer;
               *iPointer = *oPointer;
                  */
                  *iPointer++ = 0;
                  *iPointer++ = 0;
                  *iPointer = 0;
                  //*iPointer++;*iPointer++;
                  tBPointer++;tBPointer++;
               }
            }
            
            tBPointer++;
            mPointer++;
            iPointer++;
            oPointer++;
         } // for
      } // if (mOk)
      
      drawCounter = 1;
   } // (oOk)
}


void  SoTextureMaskedArea::GLRender(SoGLRenderAction *action)
{
   _lutFunction = SoMLLUTElement::getLut(action->getState());
   SoView2DExtension::GLRender(action);
}
