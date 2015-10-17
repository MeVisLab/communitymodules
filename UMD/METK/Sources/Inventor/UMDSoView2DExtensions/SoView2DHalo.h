//----------------------------------------------------------------------------------
//! The SoView2DExtension SoView2DHalo derived from SoView2DExtension.
/*!
// \file    SoView2DHalo.h
// \author  Christian Tietjen, Björn Meyer
// \date    2005-06-27
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOVIEW2DHALO_H__
#define __SOVIEW2DHALO_H__


// Local includes
#include "UMDSoView2DExtensionsSystem.h"

#pragma warning( push, 1 )

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <SoView2DExtension.h>
#include <View2DSlice.h>
#include <View2DSliceList.h>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFVec3f.h>
#include "XVLeaveScope.h"

#pragma warning( pop )


//! The SoView2DExtension SoView2DHalo derived from SoView2DExtension.
class UMDSOVIEW2DEXTENSIONS_EXPORT SoView2DHalo : public SoView2DExtension
{
   //! Implements the runtime type system interface of this new node.
   SO_NODE_HEADER(SoView2DHalo);

public:
   //! Constructor
   SoView2DHalo();

   SoSFColor upColor, downColor;
   SoSFVec3f worldPosition;
   SoSFBool  useLineStyle, useColor;

   //! Initializes this class (called on dll initialization).
   static void  initClass();

   //! Handling of events occurring in the viewer.
   virtual bool evalEvent(SoView2D *view2d, View2DSliceList *list, View2DEvent* event, View2DEventPhase phase);

   //! Virtual method called by the \c SoView2D node when an image is rendered.
   virtual void draw(View2DSliceList *list, View2DSlice *slice, int z);

protected:
   //! Protected destructor
   virtual ~SoView2DHalo();
};

#endif // __SODRAWMETAOBJECTS_H


