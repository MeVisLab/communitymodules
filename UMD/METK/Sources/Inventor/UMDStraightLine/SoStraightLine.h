//----------------------------------------------------------------------------------
//! Inventor node SoStraightLine for a straight line with a start and an endpoint derived from SoSeparator.
/*!
// \file    SoStraightLine.h
// \author  kmuehler
// \date    2008-08-06
//
// A single line with a start and an endpoint
*/
//----------------------------------------------------------------------------------

#ifndef __SOSTRAIGHTLINE_H__
#define __SOSTRAIGHTLINE_H__


// Local includes
#include "UMDStraightLineSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
// include used fields
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SbLinear.h>
#include "XVLeaveScope.h"


class SoTranslation;
class SoRotation;
class SoCylinder;

//! Inventor node SoStraightLine for a straight line with a start and an endpoint derived from SoSeparator.
class UMDSTRAIGHTLINE_EXPORT SoStraightLine : public SoSeparator
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoStraightLine);

public:
  //! Constructor
  SoStraightLine();

  virtual ~SoStraightLine();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  //@{! Fields

  SoSFVec3f startPoint;

  SoSFVec3f endPoint;

  SoSFColor color;

  SoSFFloat thickness;

  SoSFFloat transparency;

  //@}


  SbVec3f getDirectionVector() { return _dirVec; };



protected:
  static void startPointChangedCB(void *data, SoSensor* sens);
  static void endPointChangedCB(void *data, SoSensor* sens);
  static void colorChangedCB(void *data, SoSensor* sens);
  static void thicknessChangedCB(void *data, SoSensor* sens);
  static void transparencyChangedCB(void *data, SoSensor* sens);

private:
  SoFieldSensor* _startPointSensor;
  SoFieldSensor* _endPointSensor;
  SoFieldSensor* _colorSensor;
  SoFieldSensor* _thicknessSensor;
  SoFieldSensor* _transparencySensor;

  SoCylinder* _cylinder;
  SoMaterial* _material;
  SoTranslation* _translation1;
  SoTranslation* _translation2;
  SoRotation* _rotation1;

  SbVec3f _dirVec;

  void calculate();
};

#endif // __SOSTRAIGHTLINE_H


