#ifndef SO_LABEL_OBJECT
#define SO_LABEL_OBJECT

#include "LabelingSystem.h"

#include "XVEnterScope.h"
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>

#include "XVLeaveScope.h"

#include <vector>

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

class SoLabelObject : public SoGroup
{
  SO_NODE_HEADER(SoLabelObject);

public:
  static void initClass();
  SoLabelObject();
  virtual ~SoLabelObject();
  void setLabelText(const std::string& in);
  std::string getLabelText();
  std::string metkObjId;

    bool  on;
  bool  drawIfInvis;
  bool  drawIfInteraction;
  int   colorID;              //ID-Wert!
    bool  objectIsBackground;  //ID=0
  bool  validTextBoxSize;   // wichtig, wenn sich die Fontsize geändert hat, damit beim nächsten Render-durchgang die Textsize neu berechnet werden kann

  SbVec2s labelPosition;
  SbVec2s labelDimensions;

  //int   priority;       //objekte mit hoher priorität werden zuerst gelabelt

    bool  isFixed;
  bool  isLabeled;

  SbVec2s refLineNewPos;
  int   extNewOrbitIndex;

    SbVec3f centerBB;
  SbVec3f worldCenterBB;
    SbVec3f screenCenterBB;

  int     rows;

    float getFontSize(void){ return fontsize;}
    void setFontSize(float fs ){ fontsize=fs;}
  float getLinespacing(void){ return linespacing; }
  void setLinespacing(float ls ){ linespacing=ls; }
  int getInsetSpace(void) { return insetSpace; }
  void setInsetSpace(int is) { insetSpace=is; }


    void setLabelTextColor(float r, float g, float b );
    void setLabelTextColor(float r, float g, float b, float a);
  void setLabelTextColor( SbVec3f color);
    SbVec3f getLabelTextColor(void);
  void getLabelTextColor(float &r, float &g, float &b);
  void getLabelTextColor(float &r, float &g, float &b, float &a);
    void setLabelTextColorAlpha( float a );
  float getLabelTextColorAlpha(void);

  void setRefLineColor(float r, float g, float b );
    void setRefLineColor(float r, float g, float b, float a);
  void setRefLineColor( SbVec3f color);
    SbVec3f getRefLineColor(void);
  void getRefLineColor(float &r, float &g, float &b);
  void getRefLineColor(float &r, float &g, float &b, float &a);
    void setRefLineColorAlpha( float a );
  float getRefLineColorAlpha(void);

  void setRefLineHaloColor(float r, float g, float b );
  void setRefLineHaloColor( SbVec3f color);
    SbVec3f getRefLineHaloColor(void);
  void getRefLineHaloColor(float &r, float &g, float &b);

    void setArrowColor(float r, float g, float b );
  void setArrowColor( SbVec3f color);
    SbVec3f getArrowColor(void);
  void getArrowColor(float &r, float &g, float &b);
  void setArrowColorHalo(float r, float g, float b );
  void setArrowColorHalo( SbVec3f color);
    SbVec3f getArrowColorHalo(void);
  void getArrowColorHalo(float &r, float &g, float &b);

    void setRefLineHalo( bool halo) { isRefLineHalo=halo; };
  bool getRefLineHalo( void ) { return isRefLineHalo; };

  void setRefLineWidth( float lw) { refLineWidth=lw; };
  float getRefLineWidth( void ) { return refLineWidth; };

  void setInvisibleObjectRefLineWidth( float lw) { ioRefLineWidth=lw; };
  float getInvisibleObjectRefLineWidth( void ) { return ioRefLineWidth; };
  void setArrowSize( float lw) { arrowSize=lw; };
  float getArrowSize( void ) { return arrowSize; };
  void setArrowThickness( float lw) { arrowThickness=lw; };
  float getArrowThickness( void ) { return arrowThickness; };

    void setViewportDimensions(SbVec2s viewplane); //flag
    void setViewportDimensions(short viewplaneX, short viewplaneY); //flag

    void setVisibility(bool state);  //flag
    bool getVisibility();            //get

  void setColor(float r, float g, float b);
  SbVec3f getColor();
    void setBackgroundColor(float r, float g, float b);
  SbColor getBackgroundColor();

    void setTransparency(float _transparency) { labelTextColorAlpha=_transparency; };
  float getTransparency(void) { return labelTextColorAlpha; };

  void    setPosition(int x, int y); //flag
  SbVec2s getPosition();

  void    setTextSize(int x, int y); //flag
  void    setTextSize( SbVec2f _textSize); //flag
  SbVec2s getTextSize();  //flag

   void setJustificationLeft( void ) { isLeft=true;  }; //flag
   void setJustificationRight( void ) { isLeft=false; };//flag
   bool isJustificationLeft( void ) { return (isLeft?true:false); } //flag
   bool isJustificationRight( void ) { return (!isLeft?true:false); } //flag

   struct labelStateStruct {
      bool visible;
    bool isObjectVisible;
      bool onConvexHull;
      bool alreadyProcessed;
   } labelState;

  struct refline {
    SbVec2s object;
    SbVec2s label;
    int orbitIndex;
  std::string anchorType;
  std::string comment1,comment2,comment3,comment4,comment5,comment6,comment7;
  } referenceLine;

  void printInfo();

private:

  SbVec2s viewplane; //flag
  SbColor backgroundColor;
  float refLineColorAlpha;
  SbVec3f refLineColor;
  SbVec3f refLineHaloColor;
  SbVec3f arrowColor;
  SbVec3f arrowColorHalo;
  bool isRefLineHalo;
  float refLineWidth;
  float ioRefLineWidth; //breite der linien für unsichtbare Objekte, wenn Liniendarstellung
  float arrowThickness; //breite der linien für unsichtbare Objekte, wenn Pfeildarstellung
  float arrowSize;     //Länge des Pfeils (darstellung unsichtbare Objekte)
  SbVec3f labelTextColor;
  float labelTextColorAlpha;
  float fontsize;
  float linespacing;
  int insetSpace;
  bool isVisible;
  bool stateChanged;
  bool isLeft;
  std::string labelText;
protected:
  virtual void GLRender(SoGLRenderAction * action);
  //bool isInit;
};
#endif SO_LABEL_OBJECT
