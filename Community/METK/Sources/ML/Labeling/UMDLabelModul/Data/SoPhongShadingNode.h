#ifndef _SO_PHONG_SHADING_NODE_H
#define _SO_PHONG_SHADING_NODE_H

#include "LabelingSystem.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/misc/SoState.h>
#include "XVLeaveScope.h"
//#include <GL/gl.h>


class SoPhongShadingNode : public SoGroup {
	SO_NODE_HEADER(SoPhongShadingNode);

	public:
    SoPhongShadingNode();
	static void initClass();
	
	protected:
		virtual void   GLRender(SoGLRenderAction *action);

	private:
		virtual ~SoPhongShadingNode();
		SoState *state;
		SoLightModel *myShading;
		
};

#endif _SO_PHONG_SHADING_NODE_H
