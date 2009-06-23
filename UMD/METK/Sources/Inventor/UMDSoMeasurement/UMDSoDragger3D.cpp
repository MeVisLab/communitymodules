// #############################
// ####
// ####  SoDragger3D.cpp
// ####
// #############################

#include "SoDragger3D.h"

SO_KIT_SOURCE(SoDragger3D);


void SoDragger3D::initClass() {
    // initialisation class
    SO_KIT_INIT_CLASS(SoDragger3D, SoDragger, "SoDragger");
}


SoDragger3D::SoDragger3D() {
    SO_KIT_CONSTRUCTOR(SoDragger3D);
    initDragger3D();
}


SoDragger3D::~SoDragger3D() {
    if (_translSensor) { delete _translSensor; _translSensor = NULL; }
}


void SoDragger3D::initDragger3D() {    
    // scene graph of SoDragger has 'geomSeparator' that holds
    // following nodes
  
    // syntax of SO_KIT_ADD_CATALOG_ENTRY:
    // (name,className,nullByDefault,parentName,rightSiblingName, isPublicPart)
  
    // start with scaling an then two separators, one
    // for each dragger
  
    SO_KIT_ADD_CATALOG_ENTRY(dragScale, SoScale, TRUE, geomSeparator, \x0, TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(drag2DSep, SoSeparator, TRUE,
                             geomSeparator, \x0 , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(drag2DTrafo, SoTransform, TRUE,
                             drag2DSep, \x0 , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(drag2D, SoTranslate2Dragger, TRUE,
                             drag2DSep, \x0 , TRUE);
  
    SO_KIT_ADD_CATALOG_ENTRY(drag1DSep, SoSeparator, TRUE,
                             geomSeparator, \x0 , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(drag1DTrafo, SoTransform, TRUE,
                             drag1DSep, \x0 , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(drag1D, SoTranslate1Dragger, TRUE,
                             drag1DSep, \x0 , TRUE);
  
    // position field of dragger
    SO_KIT_ADD_FIELD(translation,(0.0, 0.0, 0.0));
  
    // create list of parts
    SO_KIT_INIT_INSTANCE();
  
    // add callbacks for new sensor
    _translSensor = new SoFieldSensor(posChangedCB, this);
    _translSensor->attach(&translation);

    // and value change
    addValueChangedCallback(&SoDragger3D::valueChangedCB);
  
    // rotate 1d dragger by 90 degrees 
    SoTransform *trafo1D = SO_GET_ANY_PART(this, "drag1DTrafo", SoTransform);
    trafo1D->rotation.setValue(SbVec3f(0, 1, 0), 1.57f); 
  
    // fetch draggers 
    SoTranslate1Dragger* drag1 = SO_GET_ANY_PART(this, "drag1D", SoTranslate1Dragger);
    SoTranslate2Dragger* drag2 = SO_GET_ANY_PART(this, "drag2D", SoTranslate2Dragger);
  
    // registration of child draggers. This enables a smart handling
    // of the combined dragger
    if (drag1) {
        registerChildDragger(drag1);
    }
    if (drag2) {
        registerChildDragger(drag2);
    }

    // our first node to scale the dragger
    _dScale = SO_GET_ANY_PART(this, "dragScale", SoScale);
}


//! scale dragger by \c scale
void SoDragger3D::setScale(float scale) {   
    _dScale->scaleFactor.setValue(scale,scale,scale);    
}


//! callback for applying a change of dragger's position 
void SoDragger3D::posChangedCB(void* userData, SoSensor*) {
    SoDragger3D* myDrag = (SoDragger3D*) userData;
    SbMatrix motMat = myDrag->getMotionMatrix();
    myDrag->workFieldsIntoTransform(motMat);
    myDrag->setMotionMatrix(motMat);
}

//! adapt changes of the dragger position (analog to Toolmaker, chapter 7)
void SoDragger3D::valueChangedCB(void*, SoDragger *myDrag) {
    SoDragger3D *myself = (SoDragger3D*) myDrag;
    SbMatrix motMat = myself->getMotionMatrix();
    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform(trans, rot, scale, scaleOrient);
    myself->_translSensor->detach();
    if (myself->translation.getValue() != trans) {
        myself->translation = trans;
    }
    myself->_translSensor->attach(&(myself->translation));
}
