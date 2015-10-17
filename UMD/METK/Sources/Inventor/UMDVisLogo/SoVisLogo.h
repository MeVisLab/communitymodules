//----------------------------------------------------------------------------------
//! The Inventor node SoVisLogo derived from SoShape showing the logo of VIS group for 3D viewer.
/*!
// \file    SoVisLogo.h
// \author  Konrad Mühler
// \date    2007-06-12
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SOVISLOGO_H__
#define __SOVISLOGO_H__


// Local includes
#include "UMDVisLogoSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoShape.h>

#include <Inventor/elements/SoViewportRegionElement.h>

// include used fields
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include "XVLeaveScope.h"

//! The Inventor node SoVisLogo derived from SoShape showing the logo of VIS group for 3D viewer.
class UMDVISLOGO_EXPORT SoVisLogo : public SoShape
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoVisLogo);

public:
  //! Constructor
  SoVisLogo();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  //@{! Fields

  SoSFString  fldLocalPath;
  SoSFEnum    fldFilename;
  SoSFEnum    fldCorner;

  //@}

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

protected:
  //! Protected destructor
  virtual            ~SoVisLogo();

  //! Sensor to detect changes in node's fields.
  SoNodeSensor*   _nodeSensor;

  //! Called by \c _nodeSensor when any field in node changes.
  //! Only redirects the call of this static function to the
  //! class method \c nodeChanged.
  static void       nodeChangedCB(void *data, SoSensor* sens);

  //! Called by the static function \c _nodeChangedCB to notify node changes.
  void              nodeChanged(SoNodeSensor* sensor);

  SoFieldSensor*  filenameSensor;
  SoFieldSensor*  localPathSensor;
  static void   filenameChangedCB(void *, SoSensor *);

  enum logoNames {vislogo, otto, otto2, lst, neck}; //Defintion der Filenames in filenameChangedCB
  enum corners {topleft, topright, bottomleft, bottomright};

private:

  unsigned long load_result, img_width, img_height;
  char* png_image;
  char *filename;

  int pngLoad(const char *file, unsigned long &pwidth, unsigned long &pheight, char* &image_data_ptr);

  void activateAttachments();
};

#endif // __SOVISLOGO_H


