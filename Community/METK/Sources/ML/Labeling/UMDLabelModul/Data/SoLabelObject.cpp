#include "SoLabelObject.h"

#include "XVEnterScope.h"
#include <Inventor/elements/SoCacheElement.h>
#if defined(SGI_VERSION) && SGI_VERSION >= 20200
# include <Inventor/SoTrace.h>
#endif
#ifndef SO_TRACE_IN
# define SO_TRACE_IN(__classAndMethod___)
#endif
#include "XVLeaveScope.h"



SO_NODE_SOURCE(SoLabelObject);

void SoLabelObject::initClass()
{
  SO_NODE_INIT_CLASS(SoLabelObject, SoGroup, "Group");
}

SoLabelObject::SoLabelObject()
{ 
  SO_TRACE_IN("SoLabelObject::SoLabelObject");
  
  // Execute inventor internal stuff for node construction.
  SO_NODE_CONSTRUCTOR(SoLabelObject);
  	
  metkObjId = "";
  colorID = 0; 
  //priority = 1;
  drawIfInvis = false;
  drawIfInteraction = false;
  isFixed = false;
  //isInit = false;
  isLeft = false;
  isVisible = false;  
  labelText="";
  objectIsBackground = false;  
  on = true;
  rows=1;

  labelPosition.setValue(0.0, 0.0);
  refLineWidth=1.0;
  ioRefLineWidth=1.0;
  arrowThickness=1.0;
  arrowSize=10.0;
  isRefLineHalo=true;
  referenceLine.label.setValue( 0.0, 0.0 ); 
  referenceLine.object.setValue( 0.0, 0.0 ); 
  referenceLine.orbitIndex = 0; 
  refLineNewPos.setValue(0.0, 0.0);
  extNewOrbitIndex=0;
  refLineColor.setValue( 0.0, 0.0, 0.0 );
  refLineColorAlpha= 1.0;
  arrowColor.setValue( 0.0, 0.0, 0.0);
  arrowColorHalo.setValue( 1.0, 1.0, 1.0 );

  stateChanged = true;
  labelTextColor[0] = 0.0; labelTextColor[1] = 0.0; labelTextColor[2] = 0.0;  //MB
  labelTextColorAlpha = 0.0;  
  fontsize=12.0f;
  linespacing=1;
  insetSpace=5;
  validTextBoxSize = false;
  viewplane[0]=0;viewplane[1]=0;//MB: init
  
  backgroundColor[0] = 1.0; backgroundColor[1] = 1.0; backgroundColor[2] = 1.0;
  isLabeled=true;
  
  labelState.alreadyProcessed=false;
  labelState.isObjectVisible=false;
  labelState.onConvexHull=false;
  labelState.visible=false;

  centerBB.setValue(0.0, 0.0, 0.0);
  worldCenterBB.setValue(0.0, 0.0, 0.0);
  screenCenterBB.setValue(0.0, 0.0, 0.0);
}

SoLabelObject::~SoLabelObject() {
}


void SoLabelObject::GLRender(SoGLRenderAction *action)
{
	//nothing to do
	SoGroup::GLRender(action);
}


void SoLabelObject::setViewportDimensions(SbVec2s viewplane) { 
  this->viewplane = viewplane;
}

void SoLabelObject::setViewportDimensions(short viewplaneX, short viewplaneY) { 
  this->viewplane[0] = viewplaneX;
  this->viewplane[1] = viewplaneY;
}

void SoLabelObject::setVisibility(bool state) {  
	isVisible = state;
}

void SoLabelObject::setLabelTextColor(float r, float g, float b ) {
   labelTextColor.setValue(r,g,b);
}
void SoLabelObject::setLabelTextColor( SbVec3f color) {
   labelTextColor.setValue( color.getValue() );
}
void SoLabelObject::setLabelTextColor(float r, float g, float b, float a) {
   labelTextColor.setValue(r,g,b);
   labelTextColorAlpha =a;
}
SbVec3f SoLabelObject::getLabelTextColor() {
   return labelTextColor.getValue();
}
void SoLabelObject::getLabelTextColor(float &r, float &g, float &b) {
	r=labelTextColor.getValue()[0];
	g=labelTextColor.getValue()[1];
	b=labelTextColor.getValue()[2];
}
void SoLabelObject::getLabelTextColor(float &r, float &g, float &b, float &a){
    r=labelTextColor.getValue()[0];
	g=labelTextColor.getValue()[1];
	b=labelTextColor.getValue()[2];
	a=labelTextColorAlpha;
}
void  SoLabelObject::setLabelTextColorAlpha( float a ) {
	labelTextColorAlpha=a;
}
float SoLabelObject::getLabelTextColorAlpha() {
	return labelTextColorAlpha;
}


void SoLabelObject::setRefLineColor(float r, float g, float b ) {
   refLineColor.setValue(r,g,b);
}
void SoLabelObject::setRefLineColor( SbVec3f color) {
   refLineColor.setValue( color.getValue() );
}
void SoLabelObject::setRefLineColor(float r, float g, float b, float a) {
   refLineColor.setValue(r,g,b);
   refLineColorAlpha =a;
}
SbVec3f SoLabelObject::getRefLineColor() {
   return refLineColor.getValue();
}
void SoLabelObject::getRefLineColor(float &r, float &g, float &b) {
	r=refLineColor.getValue()[0];
	g=refLineColor.getValue()[1];
	b=refLineColor.getValue()[2];
}
void SoLabelObject::getRefLineColor(float &r, float &g, float &b, float &a){
    r=refLineColor.getValue()[0];
	g=refLineColor.getValue()[1];
	b=refLineColor.getValue()[2];
	a=refLineColorAlpha;
}
void  SoLabelObject::setRefLineColorAlpha( float a ) {
	refLineColorAlpha=a;
}
float SoLabelObject::getRefLineColorAlpha() {
	return refLineColorAlpha;
}

void SoLabelObject::setRefLineHaloColor(float r, float g, float b ){
  refLineHaloColor.setValue(r,g,b);
}
void SoLabelObject::setRefLineHaloColor( SbVec3f color) {
  refLineHaloColor.setValue( color.getValue() );
}
SbVec3f SoLabelObject::getRefLineHaloColor(void) {
  return refLineHaloColor.getValue();
}
void SoLabelObject::getRefLineHaloColor(float &r, float &g, float &b){
    r=refLineHaloColor.getValue()[0];
	g=refLineHaloColor.getValue()[1];
	b=refLineHaloColor.getValue()[2];
}


void SoLabelObject::setArrowColor(float r, float g, float b ) {
    arrowColor.setValue(r,g,b);
}
void SoLabelObject::setArrowColor( SbVec3f color) {
	arrowColor.setValue( color.getValue() );
}
SbVec3f SoLabelObject::getArrowColor(void) {
	return arrowColor;
}

void SoLabelObject::getArrowColor(float &r, float &g, float &b) {
	r=arrowColor.getValue()[0];
	g=arrowColor.getValue()[1];
	b=arrowColor.getValue()[2];
}
void SoLabelObject::setArrowColorHalo(float r, float g, float b ) {
	arrowColorHalo.setValue(r,g,b);
}

void SoLabelObject::setArrowColorHalo( SbVec3f color) {
	arrowColorHalo.setValue( color.getValue() );
}

SbVec3f SoLabelObject::getArrowColorHalo(void) {
	return arrowColorHalo;
}
void SoLabelObject::getArrowColorHalo(float &r, float &g, float &b) {
	r=arrowColorHalo.getValue()[0];
	g=arrowColorHalo.getValue()[1];
	b=arrowColorHalo.getValue()[2];
}



bool SoLabelObject::getVisibility() {
  return isVisible;
}


void SoLabelObject::setColor(float r, float g, float b) {
  labelTextColor.setValue(r, g, b);   
}

void SoLabelObject::setBackgroundColor(float r, float g, float b) {
  backgroundColor[0] = r;
  backgroundColor[1] = g;
  backgroundColor[2] = b;
}
SbColor SoLabelObject::getBackgroundColor() {
	return backgroundColor;
}


SbVec3f SoLabelObject::getColor() {
	return labelTextColor.getValue();
}


void SoLabelObject::setPosition(int x, int y) { 
  labelPosition.setValue(x, y);
  stateChanged = true; 
}

SbVec2s SoLabelObject::getPosition() {
  return labelPosition;
}

void SoLabelObject::setLabelText(const std::string& in){
	labelText=in;
	rows=1;
	for(unsigned int i=0;i<labelText.size();i++){
		if(labelText[i]=='\n'){
			rows++;
		}
	}
}

std::string SoLabelObject::getLabelText(){
	return labelText;
}

void SoLabelObject::setTextSize(int x, int y) {
  labelDimensions[0] = x;
  labelDimensions[1] = y;
}

void SoLabelObject::setTextSize( SbVec2f _size ) {
  labelDimensions[0] = _size.getValue()[0];
  labelDimensions[1] = _size.getValue()[1];
}

SbVec2s SoLabelObject::getTextSize() {
  return labelDimensions;
}

void SoLabelObject::printInfo()
{
	std::cout << "obj=" << metkObjId;
	//std::cout << "  labelText=" << labelText;
	std::cout << "  isVisible=" << isVisible;
	std::cout << "  on=" << on;
	std::cout << "  isLabeled=" << isLabeled;
	std::cout << "  isLeft=" << isLeft;
	std::cout << "  objectIsBackground=" << objectIsBackground;
	std::cout << "  colorID=" << colorID;
	std::cout << "  drawIfInvis=" << drawIfInvis;
	std::cout << "  drawIfInteraction=" << drawIfInteraction;
	std::cout << "  validTextBoxSize=" << validTextBoxSize;
	std::cout << "  labelPosition=" << labelPosition[0] << "," << labelPosition[1];
	std::cout << "  labelDimensions=" << labelDimensions[0] << "," << labelDimensions[1];
	//std::cout << "  priority=" << priority;
	std::cout << "  isFixed=" << isFixed;
	std::cout << "  extNewOrbitIndex=" << extNewOrbitIndex;
	std::cout << "  refLineNewPos=" << refLineNewPos[0] << "," << refLineNewPos[1];
	std::cout << "  centerBB=" << centerBB[0] << "," << centerBB[1];
	std::cout << "  screenCenterBB=" << screenCenterBB[0] << "," << screenCenterBB[1];
	std::cout << "  rows=" << rows;
	std::cout << "  lS.isObjectVisible=" << labelState.isObjectVisible;
	std::cout << "  lS.visible=" << labelState.visible;
	std::cout << "  lS.onConvexHull=" << labelState.onConvexHull;
	std::cout << "  lS.alreadyProcessed=" << labelState.alreadyProcessed;
	std::cout << "  rL.object=" << referenceLine.object[0] << "," << referenceLine.object[1];
	std::cout << "  rL.label=" << referenceLine.label[0] << "," << referenceLine.label[1];
	std::cout << "  rL.orbitIndex=" << referenceLine.orbitIndex;
	std::cout << "  rL.anchorType=" << referenceLine.anchorType;
	std::cout << "  rL.comment1=" << referenceLine.comment1;
	std::cout << "  rL.comment2=" << referenceLine.comment2;
	std::cout << "  rL.comment3=" << referenceLine.comment3;
	std::cout << "  rL.comment4=" << referenceLine.comment4;
	std::cout << "  rL.comment5=" << referenceLine.comment5;
	std::cout << "  rL.comment6=" << referenceLine.comment6;
	std::cout << "  rL.comment7=" << referenceLine.comment7;

	std::cout << std::endl;
}