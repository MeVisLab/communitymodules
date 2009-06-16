#ifndef _SO_FLAT_SHADING_NODE_H
#define _SO_FLAT_SHADING_NODE_H

#include "LabelingSystem.h"

#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoBaseColor.h>
//#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/misc/SoState.h>

#include <Inventor/SoPath.h>

#include "XVLeaveScope.h"
//#include <GL/gl.h>

//#include "../So3dLabel01.h"
//class So3dLabel01;

class SoFlatShadingNode : public SoGroup {
	SO_NODE_HEADER(SoFlatShadingNode);

	public:
    SoFlatShadingNode();
	static void initClass();
	
	protected:
		virtual void   GLRender(SoGLRenderAction *action);

	private:
		virtual ~SoFlatShadingNode();
		SoState *state;
		SoLightModel *myShading; //Test, weil LazyElement nicht geht		
};

#endif _SO_FLAT_SHADING_NODE_H
