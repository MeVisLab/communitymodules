// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
// Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of BIGR nor the names of its contributors 
//       may be used to endorse or promote products derived from this software 
//       without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------------
//! The ML module class CSOImageStatistics.
/*!
// \file    mlCSOImageStatistics.cpp
// \author  Reinhard Hameeteman
// \date    2008-01-30
//
// Calculate image statistics at CSO position on a image
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlCSOImageStatistics.h"
#include "MainAxisPCA.h"
#include "mlRotationField.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CSOImageStatistics, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CSOImageStatistics::CSOImageStatistics (void)
  : BaseOp(1, 0)
{
  ML_TRACE_IN("CSOImageStatistics::CSOImageStatistics()");

  handleNotificationOff();

  FieldContainer* fieldC = getFieldContainer();

  m_InCSOList  = NULL;
  m_OutCSOList = NULL;
  m_OutCurveList = new CurveList;
  m_OutMarkerList = new XMarkerList;

  (f_InputCSOList    = fieldC->addBase("inCSOList")) ->setBaseValue(NULL);
  (f_OutputCSOList   = fieldC->addBase("outCSOList"))->setBaseValue(NULL);

  f_OutCurveList = fieldC->addBase( "outCurveList" );
  f_OutCurveList->setBaseValue( m_OutCurveList );

  (f_InputCSOString  = fieldC->addString("inputCSOString"))->setStringValue("");

  f_AddDescription = fieldC->addBool("addDescription");
  f_AddDescription->setBoolValue( false );

  f_DescriptionString = fieldC->addString("descriptionString");
  f_DescriptionString->setStringValue( "Sum:%sum;Average:%average");

  f_OutputCurve = fieldC->addBool("outputCurve");
  f_OutputCurve->setBoolValue( false );
  
  f_OutMarkerList = fieldC->addBase("outCoordinateList");
  f_OutMarkerList->setBaseValue( m_OutMarkerList );

  (f_WorkDirectlyOnInputList = fieldC->addBool("workDirectlyOnInputList"))->setBoolValue(false);

  (f_ListenToFinishingNotifications = fieldC->addBool("listenToFinishingNotifications"))->setBoolValue(true);
  (f_ListenToSelectionNotifications = fieldC->addBool("listenToSelectionNotifications"))->setBoolValue(true);

  static const char *updateModes[2] = {"Off", "AutoUpdate"};
  (f_UpdateMode = getFieldContainer()->addEnum("updateMode", updateModes, 2))->setEnumValue(0);  

  f_Apply = fieldC->addNotify("apply");

  m_IsInNotificationCB = false;
  m_IsNotifyingMyself  = false;

  f_UseAllPointsInsideCSO = getFieldContainer()->addBool("useAllPointsInsideCSO");
  f_UseAllPointsInsideCSO->setBoolValue( false );

  f_OutputAverage = fieldC->addBool("averageOutput");
  f_OutputAverage->setBoolValue( false );

  f_OutputSum = fieldC->addBool("sumOutput");
  f_OutputSum->setBoolValue( false );

  f_Resolution = fieldC->addFloat("resolution");
  f_Resolution->setFloatValue( -1.0f );

  f_UseVoxelSize = fieldC->addBool("useVoxelSize");
  f_UseVoxelSize->setBoolValue( true );

  f_Statistics = fieldC->addString("statistics");
  f_Statistics->setStringValue( "Id;Voxels;Sum;Average" );

  f_TotalSum = fieldC->addDouble( "totalSum" );
  f_TotalSum->setDoubleValue( 0.0 );

  f_TotalAverage = fieldC->addDouble( "totalAverage" );
  f_TotalAverage->setDoubleValue( 0.0 );

  f_TotalMin = fieldC->addDouble( "totalMin" );
  f_TotalMin->setDoubleValue( 0.0 );

  f_TotalMax = fieldC->addDouble( "totalMax" );
  f_TotalMax->setDoubleValue( 0.0 );

  const char* funcNames[] = {"NearestNeighbor", "Trilinear" , "TrilinearSkipBorder"};
  f_InterpolationMode = getFieldContainer()->addEnum("interpolation",funcNames,3);
  f_InterpolationMode->setEnumValue(1);

  const char* memNames[] = {"Paged", "Global" , "VirtualVolume"};
  f_MemoryAccessMode = getFieldContainer()->addEnum("memoryAccess",memNames,3);
  f_MemoryAccessMode->setEnumValue(MEMIMAGE);


  // Activate maximum support of all input voxel data types having all operations.
  // setVoxelDataTypeSupport(BaseOp::FULLY_OPERATIONAL);

  handleNotificationOn();
}

CSOImageStatistics::~CSOImageStatistics()
{
  ML_TRACE_IN("CSOImageStatistics::~CSOImageStatistics()");

  // Remove calback function
  if (m_InCSOList != NULL){
    m_InCSOList->removeNotificationObserver(CsoListNotifyObserverCB, this);
  }
  if (!f_WorkDirectlyOnInputList->getBoolValue()){
    ML_DELETE(m_OutCSOList);
  }

  // Remove any existing curves
  while ( !m_OutCurveList->getCurveList().empty() ) {
    delete m_OutCurveList->getCurveList().back();
    m_OutCurveList->getCurveList().pop_back();
  }
  delete m_OutCurveList;

  if ( m_OutMarkerList ) { 
    m_OutMarkerList->clear();
    delete m_OutMarkerList; 
  }
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void CSOImageStatistics::handleNotification (Field *field)
{
  ML_TRACE_IN("CSOImageStatistics::handleNotification()");

  bool inputImageValid = false;
  PagedImg *inImg = getNonDummyUpdatedInImg(0);
  if ( inImg != NULL ) { 
    inputImageValid = true;
  }

  if ( field == f_InputCSOList ){
    if ( f_UpdateMode->getEnumValue() != MODE_OFF  && 
         inputImageValid  ){
      ProcessCSOList(true);
    }
  } else 
  if ( (field == f_InputCSOString) ||
       (field == f_AddDescription) ||
       (field == f_UseAllPointsInsideCSO) ){
    if ( f_UpdateMode->getEnumValue() != MODE_OFF  && 
         inputImageValid ){
      ProcessCSOList(true);
    }
  } else
  if (field == f_WorkDirectlyOnInputList){
    if (f_UpdateMode->getEnumValue() != MODE_OFF){
      bool hasChangedToOn = f_WorkDirectlyOnInputList->getBoolValue();
      if (m_InCSOList != NULL){
        if (hasChangedToOn){
          ML_DELETE(m_OutCSOList);
          m_OutCSOList = m_InCSOList;
        } else {                
          ML_CHECK_NEW(m_OutCSOList, CSOList(*m_InCSOList));
        }
      } else { // no input, nothing to put out 
        if (hasChangedToOn){
          if (m_OutCSOList != NULL){
            ML_DELETE(m_OutCSOList);
          }
        }
        m_OutCSOList = NULL;
      }   
      f_OutputCSOList->setBaseValue(m_OutCSOList);
      if ( inputImageValid ){
        ProcessCSOList(false);
      }
    }
  } else 
  if (inputImageValid ||
     ( field == f_Apply ||
       field == f_AddDescription ||
       field == f_DescriptionString ||
       field == f_UseAllPointsInsideCSO ||
       field == f_Resolution ||
       field == f_UseVoxelSize ) ){
    ProcessCSOList(true);
  } else 
  if ( field == f_OutputAverage || field == f_OutputSum || field == f_OutputCurve ){
    this->SetCurves();
  }
}


void CSOImageStatistics::SetupInternalCSOList()
{
  ML_TRACE_IN("void CSOImageStatistics::SetupInternalCSOList()");

  bool workDirectlyOnInputCSOList = f_WorkDirectlyOnInputList->getBoolValue();

  if ( !m_IsInNotificationCB ) { 
    CSOList::removeNotificationObserverFromAllCSOLists(CsoListNotifyObserverCB, this); 
  }

  if (BASE_IS_A((static_cast<Base*>(f_InputCSOList->getBaseValue())), CSOList)){
    m_InCSOList = static_cast<CSOList*>(f_InputCSOList->getBaseValue());
  } else {
    m_InCSOList = NULL;
    if (workDirectlyOnInputCSOList){
      m_OutCSOList = NULL;
    } else {
      ML_DELETE(m_OutCSOList);
      m_OutCSOList = NULL;
    }
  }

  if (m_InCSOList != NULL){
    if (!workDirectlyOnInputCSOList){ // make copy            
      ML_DELETE(m_OutCSOList);
      ML_CHECK_NEW(m_OutCSOList, CSOList(*m_InCSOList));
    } else { // use pointer
      m_OutCSOList = m_InCSOList;
    }            

    if ( !m_IsInNotificationCB ){ 
      m_InCSOList->addNotificationObserver(CsoListNotifyObserverCB, this); 
    }
  }        
  f_OutputCSOList->setBaseValue(m_OutCSOList);    
}


void CSOImageStatistics::ParseInputCSOString()
{
  ML_TRACE_IN("void CSOImageStatistics::_parseInputCSOString()");

  m_SelectedCSOIds.clear();

  if (m_OutCSOList != NULL){
    std::string inputString = f_InputCSOString->getStringValue();
    if (inputString.length() > 0){        
      int spaceSeparatorIndex = static_cast<int>(inputString.find(" "));

      while (spaceSeparatorIndex != static_cast<int>(std::string::npos)){
        std::string currentItem = inputString.substr(0, spaceSeparatorIndex);       
        inputString = inputString.substr(spaceSeparatorIndex+1, inputString.length()-spaceSeparatorIndex);
        spaceSeparatorIndex = static_cast<int>(inputString.find(" "));
        AddCSOId(currentItem);
      }
      AddCSOId(inputString); // adds the rest of the string (last element)
    }
  }
}


void CSOImageStatistics::AddCSOId(const std::string& idString)
{
  ML_TRACE_IN_TC("void CSOImageStats::_addCSOId(const std::string& idString)");

  if (m_OutCSOList != NULL){        
    int currentId = 0;
    std::stringstream strstream;
    strstream << idString;
    strstream >> currentId;
    if ( currentId == -1 ) {
      m_SelectedCSOIds.clear();
      unsigned int nCSO = m_OutCSOList->numCSO();
      for ( unsigned int iCSO=0; iCSO < nCSO; ++iCSO ){
        m_SelectedCSOIds.push_back( m_OutCSOList->getCSOAt( iCSO )->getId() );
      }
    } else {
      CSO* testCSOPointer = m_OutCSOList->getCSOById(currentId);
      if (testCSOPointer != NULL){
        m_SelectedCSOIds.push_back(currentId);
      }
    }
  }
}


void CSOImageStatistics::ProcessCSOList(bool shouldSetupInternalCSOList)
{
  ML_TRACE_IN("void CSOImageStatistics::ProcessCSOList(bool shouldSetupInternalCSOList)");

  f_Statistics->setStringValue( "" );
  std::stringstream outputString;
  outputString << "Id;Voxels;Sum;Average" << std::endl;

  m_SumSeries.clear();
  m_AverageSeries.clear();
  m_OutMarkerList->clear();

  if (shouldSetupInternalCSOList){
    SetupInternalCSOList();
  }

  if (m_OutCSOList != NULL){

    ParseInputCSOString();

    int notificationFlag = 0;

    if (m_SelectedCSOIds.size() > 0){

      double totalMin = ML_DOUBLE_MAX;
      double totalMax = ML_DOUBLE_MIN;
      double totalAverage = 0.0;
      double totalSum = 0.0;
      double totalArea = 0.0;
      for (unsigned int i = 0; i < m_SelectedCSOIds.size(); i++){
        const unsigned int currentId = m_SelectedCSOIds[i];
        CSO* currentCSO = m_OutCSOList->getCSOById(currentId);
        if ( !currentCSO->isClosed() ){ continue; }
        double sum = 0.0;
        double average = 0.0;
        double minimum = ML_DOUBLE_MAX;
        double maximum = ML_DOUBLE_MIN;
        unsigned int iD = currentCSO->getId();
        size_t voxelCount;
        this->GetStatistics( currentCSO, voxelCount, sum, average, minimum, maximum  );

        totalSum += sum;
        double currentArea = 0.0;
        if (f_UseAllPointsInsideCSO->getBoolValue() ){
          currentArea = currentCSO->getArea();
        } else {
          currentArea = currentCSO->getLength();
        }
        totalAverage += average*currentArea;
        totalArea += currentArea;
        totalMin = ML_MIN(totalMin,minimum);
        totalMax = ML_MAX(totalMax,maximum);

        outputString << iD << ";" << voxelCount << ";" << sum << ";" << average  << std::endl; 

        m_SumSeries.push_back( sum );
        m_AverageSeries.push_back( average );

        if ( f_AddDescription->getBoolValue() ) {
          std::string descriptionString = f_DescriptionString->getStringValue();
          std::stringstream ss;
          ss << sum;
          descriptionString = ReplaceString(descriptionString, "%sum", ss.str() );
          ss.str("");
          ss << average;
          descriptionString = ReplaceString(descriptionString, "%average", ss.str() );
          ss.str("");
          currentCSO->setDescription( descriptionString );
        }
      } //iD list
      if ( totalArea ){
        totalAverage /= totalArea;
      } else {
        totalAverage = 0.0;
      }
      f_TotalSum->setDoubleValue( totalSum );
      f_TotalAverage->setDoubleValue( totalAverage );
      f_TotalMin->setDoubleValue( totalMin );
      f_TotalMax->setDoubleValue( totalMax );

      // Set Notification flags
      notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED;
      notificationFlag |= CSOList::NOTIFICATION_GROUP_FINISHED;
      f_Statistics->setStringValue( outputString.str() );
    }

    if (f_WorkDirectlyOnInputList->getBoolValue() == true){        
      m_IsNotifyingMyself = true;
      m_InCSOList->notifyObservers(notificationFlag);
      m_IsNotifyingMyself = false;
    } else { 
      // output a copy, notify its observers
      m_OutCSOList->notifyObservers(notificationFlag);
    }
    f_OutMarkerList->notifyAttachments();
    this->SetCurves();
  }
}


void CSOImageStatistics::GetStatistics( CSO* cso, 
                                        size_t &voxelCount, 
                                        double &sum, 
                                        double &average, 
                                        double &minimum, 
                                        double &maximum )
{
  if ( cso && getNonDummyUpdatedInImg(0) && cso->isInPlane() ){
    
    // Create a MPRLight module
    MLErrorCode libErr = MLLoadLibrary("MLResample1");
    mlModule* mpr = NULL;
    mpr = MLCreateModuleFromName("MPR");
    if ( mpr == NULL || libErr != 1 ) {
      mlDebug("Error creating module");
      return;
    }

    std::vector< vec3 > pathPoints;
    cso->fillPathPointCoordinatesFlattened( pathPoints );
    MainAxisPCA maPCA;
    maPCA.computeMainAxis( pathPoints );

    // Set FoV
    const double size = maPCA.getLargestExtension();
    FloatField* fOVField = static_cast< FloatField* >( MLModuleGetField (mpr, "fieldOfView") );
    fOVField->setFloatValue( size );

    // Set output size
    const float resolution = f_Resolution->getFloatValue();
    int outputSize;
    if ( resolution < 0 ){
      vec3 voxelSize = getNonDummyUpdatedInImg(0)->getVoxelSize();
      double minVoxelSize = ML_MIN(ML_MIN(voxelSize[0],voxelSize[1]),voxelSize[2]);
      outputSize = static_cast<int>( ceil(size/minVoxelSize) );
    } else {
      outputSize = static_cast<int>( ceil(size/resolution) );
    }
    IntField* outputSizeField = static_cast<IntField*>( MLModuleGetField( mpr,"outputSize" ) );
    outputSizeField->setIntValue( outputSize );

    // Set Translation
    Vec3fField *translationField = static_cast<Vec3fField*>( MLModuleGetField(mpr,"translation") );
    translationField->setVec3fValue( maPCA.getMidPoint() );

    // Set Rotation
    // this is the same as SbRotation(SbVec3f(0,1,0),SbVec3f(csoNormal[0],csoNormal[1],csoNormal[2]) );
    const vec3 csoNormal= cso->getPlaneNormal();
    vec3 vy = -csoNormal;
    vec3 vz(0,1,0);
    vz -= (vz.dot(vy))*(vy);
    vy.normalize();
    vz.normalize();
    vec3 vx = vy.cross(vz);
    Rotation rot( mat4(vx, vy, vz, vec3(0,0,0)) );
    RotationField *rotation = static_cast<RotationField*>( MLModuleGetField(mpr,"rotation") );
    rotation->setRotationValue( rot );

    // Set Interpolation mode
    EnumField *interpolationMode = static_cast<EnumField*>( MLModuleGetField(mpr,"interpolation"));
    interpolationMode->setEnumValue( f_InterpolationMode->getEnumValue() );

    // Set Memory access mode
    EnumField *memoryAccessMode = static_cast<EnumField*>( MLModuleGetField(mpr,"memoryAccess"));
    memoryAccessMode->setEnumValue( f_MemoryAccessMode->getEnumValue() );

    mlField* mprInput    = MLModuleGetField(mpr,"input0");
    BaseOp*  inputModule = getInOp(0);
    MLint    inputNr     = getInOpIndex( 0 );
    OutputConnectorField *connectedOutput = inputModule->getOutField( inputNr );
    MLint32 connetionResult = MLFieldConnectFrom(mprInput,(mlField*)connectedOutput);

    if ( connetionResult != 1 ) {
      mlDebug("Error connecting mpr module");
      return;
    }
    MLFieldTouch( mprInput ); 
    MLFieldTouch( connectedOutput );

    // Get the CSO voxel bounding box
    mlField* mprOutput = MLModuleGetField(mpr,"output0");
    float vMatrix[16];
    MLImageGetWorldToVoxelMatrix(mprOutput,vMatrix);
    mat4 wTvMatrix = mat4(vMatrix);
    CSOBoundingBox csoBB = cso->getVoxelBoundingBox( wTvMatrix );
    const MLint t = cso->getTimePointIndex();
    const MLint x1 = static_cast< MLint >(csoBB.v1[0]);
    const MLint y1 = static_cast< MLint >(csoBB.v1[1]);
    const MLint z1 = static_cast< MLint >(csoBB.v1[2]);
    const MLint x2 = static_cast< MLint >(csoBB.v2[0]);
    const MLint y2 = static_cast< MLint >(csoBB.v2[1]);
    const MLint z2 = static_cast< MLint >(csoBB.v2[2]);
    SubImgBox csoVoxelBox = SubImgBox( Vector(x1,y1,z1,0,t,0),
                                       Vector(x2,y2,z2,0,t,0) );

    // Allocate memory block and fill it with the mpr image.
    void* tile = NULL;
    MLErrorCode err = getTile( static_cast<BaseOp*>(mpr),
                               0, 
                               csoVoxelBox, 
                               MLdoubleType, 
                               &tile );
    double* inputTile = reinterpret_cast< double* >(tile);

    sum = 0.0;
    average = 0.0;
    voxelCount = 0;
    minimum = ML_DOUBLE_MAX;
    maximum = ML_DOUBLE_MIN;

    // Remove data on error to avoid memory leaks.
    if (inputTile ) {
      if (err != ML_RESULT_OK) {
        freeTile(inputTile);
        inputTile = NULL;
      } else {

        std::vector< vec3 > contourPoints;

        // Convert pathPoints to voxel coordinates
        ConvertCoorinateList( pathPoints, mprOutput);
        if ( f_UseAllPointsInsideCSO->getBoolValue() ){
          // Fill contourPoints with all points inside the contour
          GetPointsInsideContour( pathPoints, contourPoints, csoVoxelBox);
        } else {
          // Use just the pathPoints
          contourPoints.swap( pathPoints );
        }

        voxelCount = contourPoints.size();
        const Vector strideVector =  csoVoxelBox.getExt().getStrides();
        const int currentCSOIndex = cso->getCSOList()->getCSOIndex( cso );
        for ( unsigned int iPos = 0; iPos < voxelCount; ++iPos){
          const MLint x0 = static_cast< MLint >(contourPoints[iPos][0]);
          const MLint y0 = static_cast< MLint >(contourPoints[iPos][1]);
          const MLint z0 = static_cast< MLint >(contourPoints[iPos][2]);
          const Vector currentPos( x0,y0,z0,0,0,0 );
          const Vector deltaPos = currentPos-csoVoxelBox.v1;
          const MLint offset = deltaPos.dot( strideVector);
          double* currentValue = inputTile+offset;
          sum += *currentValue;
          minimum = ML_MIN(minimum, *currentValue );
          maximum = ML_MAX(maximum, *currentValue );
          float x,y,z;
          MLImageMapVoxelToWorld(mprOutput, currentPos[0]+0.5,currentPos[1]+0.5,currentPos[2]+0.5,&x,&y,&z);
          XMarker currentMarker( vec3(x,y,z) );
          currentMarker.type = currentCSOIndex;
          m_OutMarkerList->push_back( currentMarker );
        }


        const double csoArea = cso->getArea();
        const double csoLength = cso->getLength();
        if (  csoLength != 0 && csoArea != 0){
          if ( f_UseVoxelSize->getBoolValue() ){
            average = sum/csoArea;
          } else {
            average = sum/csoLength;
          }
        } else {
           average = 0;
        }
      }
      freeTile( inputTile );
    }
  }
}


void CSOImageStatistics::SetCurves()
{
  // Remove any existing curves
  while ( !m_OutCurveList->getCurveList().empty() ) {
    delete m_OutCurveList->getCurveList().back();
    m_OutCurveList->getCurveList().pop_back();
  }

  // Set Curve output
  if ( f_OutputCurve->getBoolValue() ) {
    if ( f_OutputSum->getBoolValue() ) {
      CurveData *outputCurve = new CurveData;
      outputCurve->setY( m_SumSeries.size(), &m_SumSeries[0], 1 );
      m_OutCurveList->getCurveList().push_back( outputCurve );
    }
    if ( f_OutputAverage->getBoolValue() ) {
      CurveData *outputCurve = new CurveData;
      outputCurve->setY( m_AverageSeries.size(), &m_AverageSeries[0], 1 );
      m_OutCurveList->getCurveList().push_back( outputCurve );
    }
  }
  f_OutCurveList->notifyAttachments();
}


void CSOImageStatistics::ConvertCoorinateList( std::vector< vec3 > &coordinateList, mlField* image)
{
  std::vector< vec3 >::iterator it = coordinateList.begin();
  for (;it != coordinateList.end(); ++ it ) {
    float x0 = (*it)[0];
    float y0 = (*it)[1];
    float z0 = (*it)[2];
    float x1,y1,z1;
    MLImageMapWorldToVoxel(image, x0,y0,z0,&x1,&y1,&z1);
    *it = vec3(x1,y1,z1);
  }
}


void CSOImageStatistics::GetPointsInsideContour( std::vector< vec3 > const &pathPoints, 
                                            std::vector< vec3 >       &contourPoints,
                                            SubImgBox const           &box)
{
  contourPoints.clear();
  // Traverse all voxels in box
  Vector p = box.v1;
  for (p.t = box.v1.t;  p.t <= box.v2.t; ++p.t) {
    for (p.z = box.v1.z;  p.z <= box.v2.z; ++p.z) {
      for (p.y = box.v1.y;  p.y <= box.v2.y; ++p.y) {
        for (p.x = box.v1.x;  p.x <= box.v2.x; ++p.x) {
          vec3 currentPos = vec3(p.x,p.y,p.z);
          // Since all points should be in a axis aligned plane we can use
          // CSOMath::isPointInsidePolygon
          if ( CSOMath::isPointInsidePolygon(pathPoints, currentPos ) ) {
            contourPoints.push_back( currentPos );
          }
        }
      }
    }
  }
}



//----------------------------------------------------------------------------------
//! Replaces all occurrences of a substring within the input-string by another substring
//! Taken from MLMiscModules
//----------------------------------------------------------------------------------
std::string CSOImageStatistics::ReplaceString(std::string sourceString, std::string findString, std::string replaceString )
{
  ML_TRACE_IN("CSOImageStatistics::ReplaceString()")

  std::string result;
  const char *pSource     = MLStrDup(sourceString.c_str(), ML_RETURN_NULL);

  if (pSource)
  {   
    const char  *pFound=pSource;
    const size_t iLenFind=findString.size();
    size_t       iPosLast=0;

    // Search only non empty strings.
    if (iLenFind>0)
    {
      std::string strResult="";
      do 
      {
        // Search first occurrence of pFind in rest of string given by pFound, returns non NULL if found.
        pFound=strstr( pFound, findString.c_str() );

        if (pFound)
        {
          // Determine index position of pFound in pSource string. pFound 
          // is always larger or equal pSource.
          const size_t iPosFound = pFound-pSource;

          // Add original string fragment before found substring and replaced string to the result.
          strResult += sourceString.substr(iPosLast, (iPosFound-iPosLast)) + replaceString ;

          pFound    += iLenFind;
          iPosLast  =  iPosFound+iLenFind;
        }
        else
        {
          // Nothing found any more: Move rest of original string to the result string.
          strResult += sourceString.substr(iPosLast, (strlen(pSource)-iPosLast));
        }
      } while (pFound);

      // Set result string.
      result = strResult;
    }
    else
    {
      // Nothing found at all; use original string as result.
      result = sourceString;
    }
  }
  else
  {
    ML_PRINT_FATAL_ERROR("CSOImageStatistics::ReplaceString()", ML_NO_MEMORY, "Ignoring function call!");
  }

  MLFree(const_cast<char*>(pSource));
  pSource=NULL;
  return result;
}

void CSOImageStatistics::CsoListNotifyObserverCB(void* userData, int notificationFlag)
{
  ML_TRACE_IN("CSOImageStatistics::CsoListNotifyObserverCB");

  CSOImageStatistics *thisp = static_cast<CSOImageStatistics*>(userData);

  thisp->m_IsInNotificationCB = true;

  if (!thisp->m_IsNotifyingMyself){
    if (thisp->f_UpdateMode->getEnumValue() != MODE_OFF){
      if ((notificationFlag & CSOList::NOTIFICATION_CSO_FINISHED) ||
        (notificationFlag & CSOList::NOTIFICATION_GROUP_FINISHED))
      {
        if (thisp->f_ListenToFinishingNotifications->getBoolValue() == true){            
          thisp->ProcessCSOList(true);
        }
      }
      if ((notificationFlag & CSOList::NOTIFICATION_CSO_SELECTION) ||
        (notificationFlag & CSOList::NOTIFICATION_GROUP_SELECTION))
      {
        if (thisp->f_ListenToSelectionNotifications->getBoolValue() == true){
          thisp->ProcessCSOList(true);
        }
      }
    }
  }
  thisp->m_IsInNotificationCB = false;
}

ML_END_NAMESPACE

