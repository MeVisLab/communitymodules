//----------------------------------------------------------------------------------
//! Inventor node SoDraw2DLabels derived from SoView2DExtension.
/*!
// \file    SoDraw2DLabels.h
// \author  Konrad Mühler
// \date    2008-05-08
//
//
*/
//----------------------------------------------------------------------------------

#ifndef __SODRAW2DLABELS_H__
#define __SODRAW2DLABELS_H__


// Local includes
#include "UMDDraw2DLabelsSystem.h"

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/nodes/SoSubNode.h>
#include <View2DFont.h>
#include <SoView2D.h>
#include <SoView2DExtension.h>
#include <View2DSlice.h>
#include <View2DSliceList.h>
// include used fields
#include <Inventor/fields/SoSFString.h>
#include "XVLeaveScope.h"


#include "kBasics.h"


//! Inventor node SoDraw2DLabels derived from SoView2DExtension.
class UMDDRAW2DLABELS_EXPORT SoDraw2DLabels : public SoView2DExtension
{
  //! Implements the runtime type system interface of this new node.
  SO_NODE_HEADER(SoDraw2DLabels);

public:
  //! Constructor
  SoDraw2DLabels();

  //! Initializes this class (called on dll initialization).
  static void  initClass();

  //@{! Fields
    SoSFString _CSOFld;
  //@}


  //! Handling of events occurring in the viewer.
  virtual bool evalEvent(SoView2D *view2d, View2DSliceList *list,
    View2DEvent* event, View2DEventPhase phase);

  //! Virtual method called by the \c SoView2D node when an image is rendered.
  virtual void draw(View2DSliceList *list, View2DSlice *slice, int z);

  int addLabel(kBasics::POINT &anchorPoint, kBasics::POINT &labelPoint, kBasics::POINT &lineEndPoint, int groupType, string &labelText, int borderDistance, int optFactor, bool encircle, float encircleSize, SbVec3f textColor, SbVec3f boxColor, float boxTransparency, SbVec3f borderColor, SbVec3f lineColor, string &objID);
  void clearLabels();

  void setFontSize(const int iSize);
  void setInnerBorder(const int iValue);
  void getTextDimension(const string sText, int &width, int &height, int optFactor);
  void setSendCSO(const bool bValue);

  static const int GROUPTYPE_NONE;
  static const int GROUPTYPE_PARENT;
  static const int GROUPTYPE_CHILD;

protected:
  //! Protected destructor
  virtual            ~SoDraw2DLabels();



private:

  // your own member variables...

  //! Helper variable to store last selected point where a cross shall be drawn
  //! during mouse interaction. Default is the invalid position (-1,-1,-1).
  SbVec3f _lastSelectedPos;

  struct LABEL {
    kBasics::POINT anchorPoint;
    kBasics::POINTF anchorPointDev;
    kBasics::POINT labelPoint;
    kBasics::POINTF labelPointDev;
    kBasics::POINT lineEndPoint;
    kBasics::POINTF lineEndPointDev;
    kBasics::POINT labelSize;
    int groupType;
    string labelText;
    bool encircle;
    float encircleSize;
    SbVec3f textColor;
    SbVec3f boxColor;
    SbVec3f borderColor;
    SbVec3f lineColor;
    float boxTransparency;
    string objID;
  };

  std::vector<LABEL> _labels;

  int _fontSize;
  int _innerBorder;
  View2DFont* _font;
  bool _fontValid;
  View2DSliceList *_dsl;
  View2DSlice *_dslice;
  bool _setCSO;
  string CSO;


  void _circle(float x, float y, const float r);
  void _circleSolid(float x, float y, const float r);


};

#endif // __SODRAW2DLABELS_H


