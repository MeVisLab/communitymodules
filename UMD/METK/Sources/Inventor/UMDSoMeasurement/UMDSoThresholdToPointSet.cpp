          // #############################
// ####
// ####  UMDSoThresholdToPointSet.cpp
// ####
// #############################

#include "UMDSoThresholdToPointSet.h"


SO_NODE_SOURCE(UMDSoThresholdToPointSet)


void UMDSoThresholdToPointSet::initClass() {
  // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
  SO_NODE_INIT_CLASS(UMDSoThresholdToPointSet, SoSeparator, "SoSeparator");
}

UMDSoThresholdToPointSet::UMDSoThresholdToPointSet() {
  SO_NODE_CONSTRUCTOR(UMDSoThresholdToPointSet);

  SO_NODE_ADD_FIELD(inImage, ()); // input image

  // Threshhold
  SO_NODE_ADD_FIELD(minValue, (0));
  SO_NODE_ADD_FIELD(maxValue, (1));

  // Materialbinding OVERALL (false) or PER_VERTEX (true)
  SO_NODE_ADD_FIELD(setColor, (FALSE));

  _minValueSens = new SoFieldSensor(_fieldChangedCB, this);
  _minValueSens->attach(&minValue);
  _maxValueSens = new SoFieldSensor(_fieldChangedCB, this);
  _maxValueSens->attach(&maxValue);
  _imageSens = new SoFieldSensor(_fieldChangedCB, this);
  _imageSens->attach(&inImage);
  _setColorSens = new SoFieldSensor(_fieldChangedCB, this);
  _setColorSens->attach(&setColor);

  _pointSet = new SoPointSet;
  addChild(_pointSet);

  vertexProperty = new SoVertexProperty;
  _pointSet->vertexProperty = vertexProperty;
  vertexProperty->ref();

  calculate();
}


UMDSoThresholdToPointSet::~UMDSoThresholdToPointSet() {
  if (_minValueSens) { delete _minValueSens; _minValueSens = NULL; }
  if (_maxValueSens) { delete _maxValueSens; _maxValueSens = NULL; }
  if (_imageSens) { delete _imageSens; _imageSens = NULL; }
  if (_setColorSens) { delete _setColorSens; _setColorSens = NULL; }
  if (vertexProperty) {vertexProperty->unref(); vertexProperty = NULL; }
}


void UMDSoThresholdToPointSet::_fieldChangedCB(void* userData, SoSensor*) {
  // Call the member function.
  ((UMDSoThresholdToPointSet*)userData)->calculate();
}


void UMDSoThresholdToPointSet::calculate() {

  // is there a valid image input
  if (inImage.isValid()) {

    vertexProperty->vertex.deleteValues(0, -1);
    vertexProperty->orderedRGBA.deleteValues(0, -1);

    int imageSizeX, imageSizeY, imageSizeZ;
    inImage.getSize(imageSizeX, imageSizeY, imageSizeZ);

    // generating a buffer for faster processing
    float vecBuffer[BUFFER_SIZE][3];
    uint32_t colBuffer[BUFFER_SIZE];

    // needed for grayscale converting
    long imageMax = (long)inImage.getMaxValue();
    long grayValue = 0;


    // space for image deposit
    void* imageData = malloc(imageSizeX*imageSizeY*imageSizeZ*MLSizeOf(MLfloatType));

    // vertexproperty to store the vectors
    if (setColor.getValue() == TRUE)
      vertexProperty->materialBinding = SoVertexProperty::PER_VERTEX;
    else
      vertexProperty->materialBinding = SoVertexProperty::OVERALL;

    long counterVertex = 0;
    long counterBuffer = 0;
    SbVec3f tmpVec;
    // checking the image in individual layers
    for (int counterZ = 0; counterZ < imageSizeZ; counterZ++) {
      inImage.getTile3D(imageData, MLfloatType, 0, 0, counterZ, imageSizeX, imageSizeY, 1);
      float* value= (float*) imageData;

      for (int counterY = 0; counterY < imageSizeY; counterY++) {
        for (int counterX = 0; counterX < imageSizeX; counterX++) {
          if ((*value > minValue.getValue()) && (*value < maxValue.getValue())) {

            // store the vector
            tmpVec.setValue(counterX, counterY, counterZ);
            inImage.mapVoxelToWorld(tmpVec, tmpVec);
            vecBuffer[counterBuffer][0] = tmpVec[0];
            vecBuffer[counterBuffer][1] = tmpVec[1];
            vecBuffer[counterBuffer][2] = tmpVec[2];

            // store the color
            if (setColor.getValue() == TRUE) {
              grayValue = (long)((255 * (*value)) / imageMax);
              colBuffer[counterBuffer] = (grayValue << 24) | (grayValue << 16) | (grayValue << 8) | 255;
            }
            counterBuffer++;

            // flush buffer when full
            if (counterBuffer == BUFFER_SIZE) {
              vertexProperty->vertex.setValues(counterVertex, BUFFER_SIZE, vecBuffer);
              if (setColor.getValue() == TRUE)
                vertexProperty->orderedRGBA.setValues(counterVertex, BUFFER_SIZE, colBuffer);
              counterVertex += BUFFER_SIZE;
              counterBuffer = 0;
            }
          } // if
          value++;
        } // counterY
      } // counterX
    } // counterZ

    // flush buffer
    vertexProperty->vertex.setValues(counterVertex, counterBuffer, vecBuffer);
    if (setColor.getValue() == TRUE)
      vertexProperty->orderedRGBA.setValues(counterVertex, counterBuffer, colBuffer);

    // clean up memory
    if (imageData) { free(imageData); imageData = NULL; }
  } // image.isValid()
  else {
    // clear pointset, when no valid image is available
    vertexProperty->vertex.deleteValues(0, -1);
    vertexProperty->orderedRGBA.deleteValues(0, -1);
  }

}

