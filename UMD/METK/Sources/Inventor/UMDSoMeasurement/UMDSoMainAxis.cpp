// #############################
// ####
// ####  UMDSoMainAxis.cpp
// ####
// #############################


#include "UMDMainAxis.h"
#include "UMDSoDistanceLine.h"
#include "UMDSoMainAxis.h"


SO_NODE_SOURCE(UMDSoMainAxis);

void UMDSoMainAxis::initClass() { 
   // muss zur Initialisierung der Klasse einmal explizit aufgerufen werden
   SO_NODE_INIT_CLASS(UMDSoMainAxis, UMDSoAutoMeasureTool, "UMDSoAutoMeasureTool");
}


UMDSoMainAxis::UMDSoMainAxis() {
   SO_NODE_CONSTRUCTOR(UMDSoMainAxis);
   
   SO_NODE_ADD_FIELD(xAxis, (0, 0, 0));
   SO_NODE_ADD_FIELD(yAxis, (0, 0, 0));
   SO_NODE_ADD_FIELD(zAxis, (0, 0, 0));
   
   SO_NODE_ADD_FIELD(xDiameter, (0));
   SO_NODE_ADD_FIELD(yDiameter, (0));
   SO_NODE_ADD_FIELD(zDiameter, (0));
   SO_NODE_ADD_FIELD(maxDiameter, (0));
   
   SO_NODE_ADD_FIELD(baryCenter, (0, 0, 0));
   SO_NODE_ADD_FIELD(midPoint, (0, 0, 0));
   
   SO_NODE_ADD_FIELD(displayBoundingBox, (FALSE));
   SO_NODE_ADD_FIELD(lineWidth, (1));
   
   // choice of the mainaxis origin
#ifdef ILAB5
   SO_NODE_ADD_FIELD(axisOrigin,(ORIGIN_MIDPOINT));
   SO_NODE_DEFINE_ENUM_VALUE(MainAxis_AxisOrigin, ORIGIN_BARYCENTER);
   SO_NODE_DEFINE_ENUM_VALUE(MainAxis_AxisOrigin, ORIGIN_MIDPOINT  );    
#else
   SO_NODE_ADD_FIELD(axisOrigin,(midpoint));
   SO_NODE_DEFINE_ENUM_VALUE(MainAxis_AxisOrigin, barycenter);
   SO_NODE_DEFINE_ENUM_VALUE(MainAxis_AxisOrigin, midpoint);
#endif
   SO_NODE_SET_SF_ENUM_TYPE(axisOrigin, MainAxis_AxisOrigin);
   
   // choice of the distance line unit
   SO_NODE_ADD_FIELD(unit,(mm));
   SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, mm);
   SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, cm);
   SO_NODE_DEFINE_ENUM_VALUE(AutoMeasureTool_Unit, m);
   SO_NODE_SET_SF_ENUM_TYPE(unit, AutoMeasureTool_Unit);
   
   SO_NODE_ADD_FIELD(displayMainAxis, (TRUE));
   SO_NODE_ADD_FIELD(largestValueOnly, (FALSE));
   
   _displayBoundingBoxSensor = new SoFieldSensor(outputChangedCB, this);
   _displayBoundingBoxSensor->attach(&displayBoundingBox);
   _displayMainAxisSensor = new SoFieldSensor(outputChangedCB, this);
   _displayMainAxisSensor->attach(&displayMainAxis);
   _largestValueOnlySensor = new SoFieldSensor(outputChangedCB, this);
   _largestValueOnlySensor->attach(&largestValueOnly);
   _axisOriginSensor = new SoFieldSensor(outputChangedCB, this);
   _axisOriginSensor->attach(&axisOrigin);
   
   _firstInit = true;
}


UMDSoMainAxis::~UMDSoMainAxis() {
   // ein bischen aufraeumen
   if (_displayBoundingBoxSensor) { delete _displayBoundingBoxSensor; _displayBoundingBoxSensor = NULL; }
   if (_displayMainAxisSensor)    { delete _displayMainAxisSensor;    _displayMainAxisSensor = NULL; }
   if (_largestValueOnlySensor)   { delete _largestValueOnlySensor;   _largestValueOnlySensor = NULL; }
   if (_axisOriginSensor)   { delete _axisOriginSensor;   _axisOriginSensor = NULL; }
}


bool UMDSoMainAxis::_calculate() {
   // Eingang hat sich geändert
   
   // Inhalt einhängen, wenn vorhanden
   if (inObject1Node.getValue() != NULL) {
      
      // Punktmenge besorgen
      float* pointSet;
      long size;
      getPointSet(_inputSep1, pointSet, size);
      
      if (size > 0) {
         // Hauptachsen berechnen
         UMDMainAxis* mainAxis = new UMDMainAxis;
         mainAxis->computeMainAxis(pointSet, size);
         float *xAxisTmp, *yAxisTmp, *zAxisTmp;
         float xExt, yExt, zExt;
         mainAxis->getMainAxis(xAxisTmp, yAxisTmp, zAxisTmp);
         mainAxis->getExtension(xExt, yExt, zExt);
         xAxis.setValue(xAxisTmp);
         yAxis.setValue(yAxisTmp);
         zAxis.setValue(zAxisTmp);
         
         xDiameter.setValue(xExt);
         yDiameter.setValue(yExt);
         zDiameter.setValue(zExt);
         
         SbVec3f temp;
         mainAxis->getBaryCenter(&temp[0], &temp[1], &temp[2]);
         baryCenter = temp;
         
         mainAxis->getMidPoint(&temp[0], &temp[1], &temp[2]);
         midPoint = temp;
         
         maxDiameter.setValue(mainAxis->getLargestExtension());
         
         delete[] pointSet;
         delete   mainAxis;
         delete[] xAxisTmp;
         delete[] yAxisTmp;
         delete[] zAxisTmp;
         return true;
      }
   }
   return false;
}


void UMDSoMainAxis::display() {
   SbVec3f origin;
   if (axisOrigin.getValue() == barycenter)
      origin = baryCenter.getValue();
   else
      origin = midPoint.getValue();
   
   if (displayMainAxis.getValue() == TRUE) {
      if (largestValueOnly.getValue() == TRUE) {
         UMDSoDistanceLine* tempDist;
         if(xDiameter.getValue() > yDiameter.getValue() && 
            xDiameter.getValue() > zDiameter.getValue())
            tempDist = insertDistanceLine(origin, stretchVector(xAxis.getValue(), xDiameter.getValue()));
         else if (yDiameter.getValue() > xDiameter.getValue() &&
            yDiameter.getValue() > zDiameter.getValue())
            tempDist = insertDistanceLine(origin, stretchVector(yAxis.getValue(), yDiameter.getValue()));
         else
            tempDist = insertDistanceLine(origin, stretchVector(zAxis.getValue(), zDiameter.getValue()));
         
         _toolSep->addChild(tempDist);
         tempDist->color.connectFrom(&color);
         tempDist->toolNameFlag.connectFrom(&displayName);
         tempDist->toolName.connectFrom(&toolName);
      }
      else {
         // nun werden noch die drei Abstandslinien dem Gruppen-Knoten
         // angehaengt
         UMDSoDistanceLine* tempDist1 = insertDistanceLine(origin, stretchVector(xAxis.getValue(), xDiameter.getValue()));
         _toolSep->addChild(tempDist1);
         tempDist1->toolNameFlag.connectFrom(&displayName);
         tempDist1->toolName.connectFrom(&toolName);
         tempDist1->color.connectFrom(&color);
         UMDSoDistanceLine* tempDist2 = insertDistanceLine(origin, stretchVector(yAxis.getValue(), yDiameter.getValue()));
         _toolSep->addChild(tempDist2);
         tempDist2->color.connectFrom(&tempDist1->color);
         UMDSoDistanceLine* tempDist3 = insertDistanceLine(origin, stretchVector(zAxis.getValue(), zDiameter.getValue()));
         _toolSep->addChild(tempDist3);
         tempDist3->color.connectFrom(&tempDist1->color);
      }
   }
   
   // wenn die BoundingBox angezeigt werden soll, wird hier die Punktmenge
   // als BoundingBox in den Ausgabegraphen eingehängt
   if (displayBoundingBox.getValue() == TRUE) {
      // Knoten zum einstellen der Liniendicke der BoundingBox
      SoDrawStyle* drawStyle = new SoDrawStyle;
      _toolSep->addChild(drawStyle);
      drawStyle->lineWidth.connectFrom(&lineWidth);
      
      // Farbe der BoundingBox
      SoMaterial* mat = new SoMaterial;
      _toolSep->addChild(mat);
      mat->diffuseColor.connectFrom(&color);
      _toolSep->addChild(insertBoundingBox(midPoint.getValue(), 
         stretchVector(xAxis.getValue(), xDiameter.getValue()), 
         stretchVector(yAxis.getValue(), yDiameter.getValue()), 
         stretchVector(zAxis.getValue(), zDiameter.getValue())));
   }
}


SoIndexedLineSet* UMDSoMainAxis::insertBoundingBox(SbVec3f midPoint, SbVec3f xAxis, SbVec3f yAxis, SbVec3f zAxis) {
   
   SbVec3f halfXAxis(xAxis / 2);
   SbVec3f halfYAxis(yAxis / 2);
   SbVec3f halfZAxis(zAxis / 2);
   
   SoMFVec3f* vertices = new SoMFVec3f;
   vertices->set1Value(0, midPoint - halfXAxis - halfYAxis - halfZAxis);
   vertices->set1Value(1, midPoint - halfXAxis - halfYAxis + halfZAxis);
   vertices->set1Value(2, midPoint - halfXAxis + halfYAxis - halfZAxis);
   vertices->set1Value(3, midPoint - halfXAxis + halfYAxis + halfZAxis);
   vertices->set1Value(4, midPoint + halfXAxis - halfYAxis - halfZAxis);
   vertices->set1Value(5, midPoint + halfXAxis - halfYAxis + halfZAxis);
   vertices->set1Value(6, midPoint + halfXAxis + halfYAxis - halfZAxis);
   vertices->set1Value(7, midPoint + halfXAxis + halfYAxis + halfZAxis);
   
   int vertexNum[24] = {0, 1, 3, 2, 0, -1, 
      4, 5, 7, 6, 4, -1,
      0, 4, -1,
      1, 5, -1,
      2, 6, -1,
      3, 7, -1};
   
   SoVertexProperty* vertexProp = new SoVertexProperty;
   vertexProp->vertex = *vertices;
   SoIndexedLineSet* lineSet = new SoIndexedLineSet;
   lineSet->ref();
   lineSet->vertexProperty = vertexProp;
   lineSet->coordIndex.setValues(0, 24, vertexNum);
   
   lineSet->unrefNoDelete();
   return lineSet;
} //insertBoundingBox


UMDSoDistanceLine* UMDSoMainAxis::insertDistanceLine(SbVec3f midPoint, SbVec3f axis) { 
   UMDSoDistanceLine *line = new UMDSoDistanceLine;
   line->ref();
   
   if (_firstInit) {
      _firstInit = false;
      color.setValue(line->color.getValue());
   }
   line->startPos.setValue(midPoint - axis / 2);
   line->endPos.setValue(midPoint + axis / 2);
   line->color.connectFrom(&color);
   line->enableText.connectFrom(&displayToolMeasure);
   line->unitFlag.connectFrom(&displayUnit);
   line->unit.connectFrom(&unit);
   line->minDistance.setValue(10000);
   line->unrefNoDelete();
   return line;
} //insertDistanceLine


SbVec3f UMDSoMainAxis::stretchVector(const SbVec3f vector, const float length) {
   SbVec3f strVec;
   
   // Summe der Quadrate des Richtungsvektors
   float sumVec = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
   
   // Falls diese Summe Null ist -> Abbruch
   if (sumVec == 0) {
      strVec[0] = strVec[1] = strVec[2] = 0;
      return strVec;
   }
   
   // Berechnung des Endpunktes durch Umstellung der Formel
   // |aVector| = sqrt(aVector[0]^2 + aVector[1]^2 + aVector[2]^2):
   float factor = sqrtf((length * length) / sumVec);
   
   strVec[0] = vector[0] * factor;
   strVec[1] = vector[1] * factor;
   strVec[2] = vector[2] * factor;
   return strVec;
}
