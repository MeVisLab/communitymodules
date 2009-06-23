//  
// Created by Florian Link (MeVis eGmbH)
//

#include "XVBasics.h"
#include "UMDMeasurementInit.h"

#include "UMDSoArrow.h"
#include "UMDSoCake.h"
//#include "UMDSoDragger3D.h"

#include "UMDSoMeasureText.h"

#include "UMDSoMeasureTool.h"
#include "UMDSoAngleLines.h"
#include "UMDSoDistanceLine.h"
#include "UMDSoRuler.h"
#include "UMDSoVolEstimation.h"

#include "SoVerticalRuler.h"

#include "UMDSoShapeToPointSet.h"
#include "UMDSoThresholdToPointSet.h"

#include "UMDSoAutoMeasureTool.h"
#include "UMDSoMainAxis.h"
#include "UMDSoMaximumExtension.h"
#include "UMDSoAngleToObjects.h"
#include "UMDSoMinimalDistance.h"

// __USING_NAMESPACE_MEVIS_IV

void UMDSoMeasurementInit()
{
   UMDSoArrow::initClass();
   UMDSoCake::initClass();
   //SoDragger3D::initClass();
   UMDSoMeasureText::initClass();

   UMDSoAutoMeasureTool::initClass();
   UMDSoMeasureTool::initClass();
   // die einzelnen Werkzeuge müssen nach SoMeasureTool initialisiert
   // werden, sonst generiert ILab keinen Ausgang am Modul
   UMDSoAngleLines::initClass();
   UMDSoDistanceLine::initClass();
   UMDSoRuler::initClass();
   UMDSoVolEstimation::initClass();

   SoVerticalRuler::initClass();

   UMDSoShapeToPointSet::initClass();
   UMDSoThresholdToPointSet::initClass();

   UMDSoMainAxis::initClass();
   UMDSoMaximumExtension::initClass();
   UMDSoAngleToObjects::initClass();
   UMDSoMinimalDistance::initClass();
}

XV_INIT_LIBRARY(UMDSoMeasurementInit)
