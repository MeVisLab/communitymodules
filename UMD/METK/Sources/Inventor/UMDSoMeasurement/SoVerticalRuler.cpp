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
//! SoView2D extension for drawing annotations
/*!
// \file    SoView2DAnnotation.cpp
// \author  Florian Link
// \date    7/2001
//
*/
// Last changed by $Author: tboskamp $
// $Id: SoView2DAnnotation.cpp 47181 2006-12-05 15:05:47Z tboskamp $
// $Source$
//----------------------------------------------------------------------------------

#include "SoVerticalRuler.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"

#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/misc/SoState.h>

#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoCacheElement.h>

#include "XVLeaveScope.h"

#include <mlGL2DFont.h>


SO_NODE_SOURCE(SoVerticalRuler)

void SoVerticalRuler::initClass()
{
   SO_NODE_INIT_CLASS(SoVerticalRuler, SoSeparator, "Separator");
   SO_ENABLE(SoGLRenderAction, SoViewingMatrixElement);
   SO_ENABLE(SoGLRenderAction, SoLazyElement);
   SO_ENABLE(SoGLRenderAction, SoModelMatrixElement);
   SO_ENABLE(SoGLRenderAction, SoViewVolumeElement);
}

SoVerticalRuler::SoVerticalRuler()
{
   SO_NODE_CONSTRUCTOR(SoVerticalRuler);

   SO_NODE_ADD_FIELD(color,       (0, 0, 0));
}


SoVerticalRuler::~SoVerticalRuler()
{
}


void SoVerticalRuler::GLRenderBelowPath(SoGLRenderAction* action)
{ 
   GLRender(action);
}

void SoVerticalRuler::GLRender(SoGLRenderAction* action)
{ 
   SoState* state = action->getState();
   if (SoViewVolumeElement::get(state).getProjectionType() == SbViewVolume::ORTHOGRAPHIC) {
      char string[256];

      SbVec3f t, s;
      SbRotation r, so;
      SoProjectionMatrixElement::get(state).getTransform(t, r, s, so);
      SbVec2s size = action->getViewportRegion().getWindowSize();

      int gauge = 1.0f / -s[1];
      float oneUnit = (-s[1] * size[1] / 2.0f) / (float)gauge;
      int mark;

      // Skala auf vielfaches von 5 runden wenn Maßstab > 5
      if (gauge > 5) {
         gauge = gauge - gauge % 5;
         mark = 5;
      } else {
         mark = 1;
      }

      // Skala auf vielfaches von 50 runden wenn Maßstab > 100 (für cm)
      if (gauge > 100) {
         gauge = gauge - gauge % 50;
         mark = 50;
      } else {
         mark = 10;
      }

      // wenn > 20 umschalten von mm auf cm
      if (gauge < 100) {
         sprintf(string, "%dmm", (int)(gauge));
      } else {
         sprintf(string, "%dcm", (int)(gauge / 10));
      }

      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE);
      glDisable(GL_ALPHA_TEST);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();

      glOrtho(0, size[0], 0, size[1], -1, 1);

      glColor3fv(color.getValue().getValue());

      // Senkrechte Linie zeichnen
      glBegin(GL_LINES);
      glVertex2f(2, (size[1] / 2) - ((gauge * oneUnit) / 2) * gauge);
      glVertex2f(2, (size[1] / 2) + ((gauge * oneUnit) / 2) * gauge);

      // Waagerechte Linien zeichnen
      glVertex2f(2, (size[1] / 2) - ((gauge * oneUnit) / 2) * gauge);
      glVertex2f(7, (size[1] / 2) - ((gauge * oneUnit) / 2) * gauge);
      glVertex2f(2, (size[1] / 2) + ((gauge * oneUnit) / 2) * gauge);
      glVertex2f(7, (size[1] / 2) + ((gauge * oneUnit) / 2) * gauge);

      glEnd();
      // Maßzahl zeichnen
      float fcolor[4] = {color.getValue()[0],color.getValue()[1],color.getValue()[2],1};
      ML_OPENGL_NAMESPACE::GL2DFont* font = ML_OPENGL_NAMESPACE::GL2DFont().getDefaultSystemFont(); 
      font->drawString(10, (size[1] / 2), 12, fcolor, string, 0, true, false);

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopAttrib();
   }
}