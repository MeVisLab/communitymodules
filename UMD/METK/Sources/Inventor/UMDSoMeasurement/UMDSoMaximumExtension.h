/*! Inventor node class UMDSoMaximumExtension calculating a maximum extension.
// \file UMDSoMaximumExtension.h
*/

#ifndef _UMDSoMaximumExtension
#define _UMDSoMaximumExtension

#include "UMDMeasurementInit.h"
#include "UMDSoAutoMeasureTool.h"

class SoFieldSensor;
class SoQHull3D;
class UMDSoAngleLines;
class SoComposeRotationFromTo;
class UMDSoDistanceLine;
class SoLineSet;
class UMDSoShapeToPointSet;

//! Inventor node class UMDSoMaximumExtension calculating a maximum extension.
class SO_MEASUREMENT_CLASS_SPEC UMDSoMaximumExtension : public SoSeparator{
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoMaximumExtension);


public:

  //! Constructor
  UMDSoMaximumExtension();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  // the extensions
  SoSFFloat diameter; // Z-axis
  SoSFFloat depth;  // Y-axis
  SoSFFloat width;  // X-axis

  SoSFFloat midPoint;
  SoSFInt32 SizeOfPointSet;

  // input
  SoSFNode inObjectNode;

  // decides whether the display modes will be displayed or not
  SoSFBool displayInput;
  SoSFBool displayDistanceLine;
  SoSFBool displayUnit;
  SoSFBool displayBBox;
  SoSFBool displayMainAxes;
  SoSFBool largestValueOnly;

  //! the entity, in which the measure of the diameters is shown
  SoSFEnum unit;

  //! points with the maximum distance of the point set for drawing a line between them
  //------------------
  //- main_objectAxis ---
  //------------------
  SoSFVec3f pDiameter_1;
  SoSFVec3f pDiameter_2;
  //------------------
  //- x_objectAxis = depth ---
  //------------------
  SoSFVec3f pDepth_1; // depth point 1
  SoSFVec3f pDepth_2; // depth point 2

  //------------------
  //- y_objectAxis = width ---
  //------------------
  SoSFVec3f pWidth_1; // width point 3
  SoSFVec3f pWidth_2; // width point 4

protected:

  //! Destructor
  virtual ~UMDSoMaximumExtension();

private:

  enum eUnit {deg, mm, cmm, cm, m, ml, l};
  SoFieldSensor* _computeMaximumExtensionSens;
  // Sensor für Darstellungsveränderungen
  SoFieldSensor* _lineColorSens;
  SoFieldSensor* _displayDistLineSens;
  SoFieldSensor* _displayInputSens;
  SoFieldSensor* _displayUnitSens;
  SoFieldSensor* _displayMainAxes;
  SoFieldSensor* _displayLargValueOnly;
  SoFieldSensor* _displayBBox;

  UMDSoShapeToPointSet* _shapeToPointSet;

  //! convex hull of the point set
  SoQHull3D* _convexHull;
  SoGroup* group;

  static void computeMaximumExtensionCB(void *userData, SoSensor*);
  static void displayChanged(void* userData, SoSensor*);
  void getPointSet(SoGroup* inputGroup, float* &pointSet, int &size);
  bool _computeMaximumExtension();
  UMDSoDistanceLine* insertDistanceLine();
  // calculates the euclidean distance between two points
  float computeDistance(float* p_fPoint1, float* p_fPoint2);
  float computeDistance(SbVec3f vec1, SbVec3f vec2);

  // computes the diameter
  float computeMaxExtensionBruteForce(float* p_fPointSet, int lSize);
  void computeWidthExtension(int &size);

  // computes the (orthogonal)projection point q of an point p on a line
  SbVec3f computeHessianPoint(SbVec3f startPos, SbVec3f endPos, float x, float y, float z);
  float computeHessianPointLength(SbVec3f startPos, SbVec3f endPos, float x, float y, float z);

  // display mode -> MainAxes or BBox
  void insertMainAxes();
  void insertExtentPoints();
  void insertBBox();

  void clear();//bisher nur eine Fkt. enthalten, wenn es nicht mehr werden clear() aufgeben
  void OnChange();
  void display();

  float* pointSet;

  // orthogonal distances from the widthpoints to the objectaxis; zur Skalierung der 3. Main Axis
  float fDistPt1tooZAxis, fDistPt2tooZAxis, fDistPt3tooZAxis, fDistPt4tooZAxis;

  // nur für zwischenanzeige
  float* pointSet2; // Translated pointSet
  float* pointSet3; // Rotated pointSet2 to z-Axis
  float* pointSet4; // Rotated pointSet3 to y-z-Face

  SoSFVec3f vWidthPt1_bb; // transformed point for main axis and bbox
  SoSFVec3f vWidthPt1_ortho; // corresponding point to the first width point on object x-axis
  SoSFVec3f vWidthPt2_bb; // transformed point for main axis and bbox
  SoSFVec3f vWidthPt2_ortho; // corresponding point to the second width point on object x-axis
  SoSFVec3f vWidthPt3_bb; // transformed point for main axis and bbox
  SoSFVec3f vWidthPt3_ortho; // corresponding point to the first width point on object z-axis
  SoSFVec3f vWidthPt4_bb; // transformed point for main axis and bbox
  SoSFVec3f vWidthPt4_ortho; // corresponding point to the first width point on object z-axis


  // width of the bounding box line
  SoSFFloat lineWidthBBox;
  SoSFColor colBBox;

  // line for dislaying the maximum and the width distance
  UMDSoDistanceLine *_distanceLine;
  SoSFFloat fScaleLine;
  SoSFFloat lrgSide;

};

#endif