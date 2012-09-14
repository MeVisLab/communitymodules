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
//! The ML module class CSOSetProperties.
/*!
// \file    mlCSOSetProperties.cpp
// \author  Reinhard Hameeteman / Olaf Konrad
// \date    2007-11-06
//
// Set the properties of all cso in a list to a CSO value
*/
//----------------------------------------------------------------------------------


// Local includes
#include "mlCSOSetProperties.h"

// Small hack to determine whether we have MeVisLab2.2
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  #define IS_MEVISLAB_2_2_OR_HIGHER
#endif

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(CSOSetProperties, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
CSOSetProperties::CSOSetProperties (void)
  : BaseOp(0, 0)
{
  ML_TRACE_IN("CSOSetProperties::CSOSetProperties()")

  handleNotificationOff();
  FieldContainer* fieldC = getFieldContainer();
  ML_CHECK(fieldC);

  m_InputCSOList = NULL;
  f_InputCSOList = fieldC->addBase("inputCSOList");
  f_InputCSOList->setBaseValue(m_InputCSOList);

  m_OutputCSOList = NULL;
  ML_CHECK_NEW(m_OutputCSOList, CSOList());

  (f_OutputCSOList = fieldC->addBase("outputCSOList"))->setBaseValue(m_OutputCSOList);

  (f_InputCSOString  = fieldC->addString("inputCSOString"))->setStringValue("");

  const char *inputMode[2] = {"CSO", "Group"};
  (f_InputMode = fieldC->addEnum("inputMode",inputMode,2))->setEnumValue( CSOINPUT );
  (f_InputGroupString  = fieldC->addString("inputGroupString"))->setStringValue("");

  (f_WorkDirectlyOnInputList = fieldC->addBool("workDirectlyOnInputList"))->setBoolValue(false);

  (f_ListenToFinishingNotifications = fieldC->addBool("listenToFinishingNotifications"))->setBoolValue(true);
  (f_ListenToSelectionNotifications = fieldC->addBool("listenToSelectionNotifications"))->setBoolValue(true);

  (f_PassOnNotifications = fieldC->addBool("passOnNotifications"))->setBoolValue( false );

  (f_AutoUpdate = fieldC->addBool( "autoUpdate"))->setBoolValue( false );
  f_Update = fieldC->addNotify("update");

  f_GetPorperties = fieldC->addNotify("getProperties");

  m_IsInNotificationCB = false;
  m_IsNotifyingMyself  = false;

  // CSO Properties fields
  f_CSOComputeNormal   = fieldC->addBool  ("CSOComputeNormal");
  f_CSOLabel           = fieldC->addString("CSOLabel");
  f_CSODescription     = fieldC->addString("CSODescription");
  f_CSOCreatorId       = fieldC->addInt   ("CSOCreatorId");
  f_CSOIsSelected      = fieldC->addBool  ("CSOIsSelected");
  f_CSOClearSelection  = fieldC->addBool  ("CSOClearSelection");
  f_CSOLineWidth       = fieldC->addFloat ("CSOLineWidth");
  f_CSOLineStyle       = fieldC->addEnum  ("CSOLineStyle", CSOList::sLineStyleModeStrings, 4);
  f_CSOColor           = fieldC->addColor ("CSOColor");
  f_CSOAlpha           = fieldC->addFloat ("CSOAlpha");
  f_CSOTimePointIndex  = fieldC->addInt  ("CSOTimePointIndex");
  f_CSOVoxelWriteMode  = fieldC->addEnum ("CSOVoxelWriteMode", CSOList::sVoxelWriteModeStrings, 4);
  f_CSOVoxelWriteValue = fieldC->addFloat("CSOVoxelWriteValue");
  f_CSOShowState       = fieldC->addBool ("CSOShowState");
  f_CSOVoxelizeState   = fieldC->addBool ("CSOVoxelizeState");
  f_CSOEditableState   = fieldC->addBool ("CSOEditableState");
#if defined(IS_MEVISLAB_2_2_OR_HIGHER)
  f_CSOMarkerMode      = fieldC->addEnum ("CSOMarkerMode", CSOList::sSeedPointStyleStrings, 3);
#else
  f_CSOMarkerMode      = fieldC->addEnum ("CSOMarkerMode", CSOList::sMarkerModeStrings, 3);
#endif
  f_CSOMarkerColor     = fieldC->addColor("CSOMarkerColor");
  f_CSOMarkerAlpha     = fieldC->addFloat("CSOMarkerAlpha");
  f_CSOMarkerSize      = fieldC->addFloat("CSOMarkerSize");

   // Group properties
  f_GroupLabel             = fieldC->addString("GroupLabel");
  f_GroupDescription       = fieldC->addString("GroupDescription");
  f_GroupIsSelected        = fieldC->addBool  ("GroupIsSelected");
  f_GroupClearSelection    = fieldC->addBool  ("GroupClearSelection");
  f_GroupLineWidth         = fieldC->addFloat ("GroupLineWidth");
  f_GroupLineStyle         = fieldC->addEnum  ("GroupLineStyle", CSOList::sLineStyleModeStrings, 4);
  f_GroupColor             = fieldC->addColor ("GroupColor");
  f_GroupAlpha             = fieldC->addFloat ("GroupAlpha");
  f_GroupTimePointIndex    = fieldC->addInt   ("GroupTimePointIndex");
  f_GroupVoxelWriteMode    = fieldC->addEnum  ("GroupVoxelWriteMode", CSOList::sVoxelWriteModeStrings, 4);
  f_GroupVoxelWriteValue   = fieldC->addFloat ("GroupVoxelWriteValue");
  f_GroupShowState         = fieldC->addBool  ("GroupShowState");
  f_GroupVoxelizeState     = fieldC->addBool  ("GroupVoxelizeState");
  f_GroupEditableState     = fieldC->addBool  ("GroupEditableState");
#if defined(IS_MEVISLAB_2_2_OR_HIGHER)
  f_GroupMarkerMode        = fieldC->addEnum  ("GroupMarkerMode", CSOList::sSeedPointStyleStrings, 3);
#else
  f_GroupMarkerMode        = fieldC->addEnum  ("GroupMarkerMode", CSOList::sMarkerModeStrings, 3);
#endif
  f_GroupMarkerColor       = fieldC->addColor ("GroupMarkerColor");
  f_GroupMarkerAlpha       = fieldC->addFloat ("GroupMarkerAlpha");
  f_GroupMarkerSize        = fieldC->addFloat ("GroupMarkerSize");
  f_GroupUseVisuals        = fieldC->addBool  ("GroupUseVisuals");
  f_GroupUseShowState      = fieldC->addBool  ("GroupUseShowState");
  f_GroupUseVoxelizeState  = fieldC->addBool  ("GroupUseVoxelizeState");
  f_GroupUseEditableState  = fieldC->addBool  ("GroupUseEditableState");
  f_GroupUseVoxelWrite     = fieldC->addBool  ("GroupUseVoxelWrite");
  f_GroupUseTimePointIndex = fieldC->addBool  ("GroupUseTimePointIndex");
  f_GroupUseMarkerSettings = fieldC->addBool  ("GroupUseMarkerSettings");
  f_GroupMaximumNumCSOs          = fieldC->addInt   ("GroupMaximumNumCSOs");
  f_GroupOverflowHandling        = fieldC->addEnum  ("GroupOverflowHandling",        CSOGroupRules::sOverflowHandlingStrings, NUM_OVERFLOW_MODES );
  f_GroupRemoveFromGroupHandling = fieldC->addEnum  ("GroupRemoveFromGroupHandling", CSOGroupRules::sRemoveHandlingStrings,   NUM_REMOVE_MODES   );
  f_GroupDeleteGroupCSOHandling  = fieldC->addEnum  ("GroupDeleteGroupCSOHandling",  CSOGroupRules::sRemoveHandlingStrings,   NUM_REMOVE_MODES   );

  // Set defaults
  f_CSOComputeNormal->setBoolValue( false );
  f_CSOLabel->setStringValue("");
  f_CSODescription->setStringValue("");
  f_CSOCreatorId->setIntValue( 0 );
  f_CSOIsSelected->setBoolValue( false );
  f_CSOClearSelection->setBoolValue( false );
  f_CSOLineWidth->setFloatValue(1.0f);
  f_CSOLineStyle->setEnumValue(LINE_STYLE_SOLID);
  f_CSOColor->setColorValue(1,1,1);
  f_CSOAlpha->setFloatValue(1);
  f_CSOAlpha->setFloatMinValue(0);
  f_CSOAlpha->setFloatMaxValue(1);
  f_CSOTimePointIndex->setIntValue(0);
  f_CSOVoxelWriteMode->setEnumValue(VOXEL_VALUE_MODULE);
  f_CSOVoxelWriteValue->setFloatValue(1024);
  f_CSOShowState->setBoolValue(true);
  f_CSOVoxelizeState->setBoolValue(true);
  f_CSOEditableState->setBoolValue(true);
  f_CSOMarkerMode->setEnumValue(MARKER_DISPLAY_MODE_RECT);
  f_CSOMarkerColor->setColorValue(0,1,0);
  f_CSOMarkerAlpha->setFloatValue(1);
  f_CSOMarkerSize->setFloatValue(3);

  f_GroupLabel->setStringValue("");
  f_GroupDescription->setStringValue("");
  f_GroupIsSelected->setBoolValue( false );
  f_GroupClearSelection->setBoolValue( false );
  f_GroupLineWidth->setFloatValue(1.0f);
  f_GroupLineStyle->setEnumValue(LINE_STYLE_SOLID);
  f_GroupColor->setColorValue(1,1,1);
  f_GroupAlpha->setFloatValue(1);
  f_GroupAlpha->setFloatMinValue(0);
  f_GroupAlpha->setFloatMaxValue(1);
  f_GroupTimePointIndex->setIntValue(0);
  f_GroupVoxelWriteMode->setEnumValue(VOXEL_VALUE_MODULE);
  f_GroupVoxelWriteValue->setFloatValue(1024);
  f_GroupShowState->setBoolValue(true);
  f_GroupVoxelizeState->setBoolValue(true);
  f_GroupEditableState->setBoolValue(true);
  f_GroupMarkerMode->setEnumValue(MARKER_DISPLAY_MODE_RECT);
  f_GroupMarkerColor->setColorValue(0,1,0);
  f_GroupMarkerAlpha->setFloatValue(1);
  f_GroupMarkerSize->setFloatValue(3);
  f_GroupUseVisuals->setBoolValue(false);
  f_GroupUseShowState->setBoolValue(false);
  f_GroupUseVoxelizeState->setBoolValue(false);
  f_GroupUseEditableState->setBoolValue(false);
  f_GroupUseVoxelWrite->setBoolValue(false);
  f_GroupUseTimePointIndex ->setBoolValue(false);
  f_GroupUseMarkerSettings ->setBoolValue(false);
  f_GroupMaximumNumCSOs->setIntValue(0);
  f_GroupOverflowHandling->setEnumValue( CSOGroupRules::getDefaultOverflowHandling() );
  f_GroupRemoveFromGroupHandling->setEnumValue( CSOGroupRules::getDefaultRemoveFromGroupHandling() );
  f_GroupDeleteGroupCSOHandling->setEnumValue( CSOGroupRules::getDefaultDeleteGroupCSOHandling() );


  //! Property selection
  (f_SetCSOComputeNormal   = fieldC->addBool( "setCSOComputeNormal"   ))->setBoolValue( false );
  (f_SetCSOLabel           = fieldC->addBool( "setCSOLabel"           ))->setBoolValue( false );
  (f_SetCSODescription     = fieldC->addBool( "setCSODescription"     ))->setBoolValue( false );
  (f_SetCSOCreatorId       = fieldC->addBool( "setCSOCreatorId"       ))->setBoolValue( false );
  (f_SetCSOIsSelected      = fieldC->addBool( "setCSOIsSelected"      ))->setBoolValue( false );
  (f_SetCSOLineWidth       = fieldC->addBool( "setCSOLineWidth"       ))->setBoolValue( false );
  (f_SetCSOLineStyle       = fieldC->addBool( "setCSOLineStyle"       ))->setBoolValue( false );
  (f_SetCSOColor           = fieldC->addBool( "setCSOColor"           ))->setBoolValue( false );
  (f_SetCSOAlpha           = fieldC->addBool( "setCSOAlpha"           ))->setBoolValue( false );
  (f_SetCSOTimePointIndex  = fieldC->addBool( "setCSOTimePointIndex"  ))->setBoolValue( false );
  (f_SetCSOVoxelWriteMode  = fieldC->addBool( "setCSOVoxelWriteMode"  ))->setBoolValue( false );
  (f_SetCSOVoxelWriteValue = fieldC->addBool( "setCSOVoxelWriteValue" ))->setBoolValue( false );
  (f_SetCSOShowState       = fieldC->addBool( "setCSOShowState"       ))->setBoolValue( false );
  (f_SetCSOVoxelizeState   = fieldC->addBool( "setCSOVoxelizeState"   ))->setBoolValue( false );
  (f_SetCSOEditableState   = fieldC->addBool( "setCSOEditableState"   ))->setBoolValue( false );
  (f_SetCSOMarkerMode      = fieldC->addBool( "setCSOMarkerMode"      ))->setBoolValue( false );
  (f_SetCSOMarkerColor     = fieldC->addBool( "setCSOMarkerColor"     ))->setBoolValue( false );
  (f_SetCSOMarkerAlpha     = fieldC->addBool( "setCSOMarkerAlpha"     ))->setBoolValue( false );
  (f_SetCSOMarkerSize      = fieldC->addBool( "setCSOMarkerSize"      ))->setBoolValue( false );

  (f_SetGroupLabel           = fieldC->addBool( "setGroupLabel"           ))->setBoolValue( false );
  (f_SetGroupDescription     = fieldC->addBool( "setGroupDescription"     ))->setBoolValue( false );
  (f_SetGroupIsSelected      = fieldC->addBool( "setGroupIsSelected"      ))->setBoolValue( false );
  (f_SetGroupLineWidth       = fieldC->addBool( "setGroupLineWidth"       ))->setBoolValue( false );
  (f_SetGroupLineStyle       = fieldC->addBool( "setGroupLineStyle"       ))->setBoolValue( false );
  (f_SetGroupColor           = fieldC->addBool( "setGroupColor"           ))->setBoolValue( false );
  (f_SetGroupAlpha           = fieldC->addBool( "setGroupAlpha"           ))->setBoolValue( false );
  (f_SetGroupTimePointIndex  = fieldC->addBool( "setGroupTimePointIndex"  ))->setBoolValue( false );
  (f_SetGroupVoxelWriteMode  = fieldC->addBool( "setGroupVoxelWriteMode"  ))->setBoolValue( false );
  (f_SetGroupVoxelWriteValue = fieldC->addBool( "setGroupVoxelWriteValue" ))->setBoolValue( false );
  (f_SetGroupShowState       = fieldC->addBool( "setGroupShowState"       ))->setBoolValue( false );
  (f_SetGroupVoxelizeState   = fieldC->addBool( "setGroupVoxelizeState"   ))->setBoolValue( false );
  (f_SetGroupEditableState   = fieldC->addBool( "setGroupEditableState"   ))->setBoolValue( false );
  (f_SetGroupMarkerMode      = fieldC->addBool( "setGroupMarkerMode"      ))->setBoolValue( false );
  (f_SetGroupMarkerColor     = fieldC->addBool( "setGroupMarkerColor"     ))->setBoolValue( false );
  (f_SetGroupMarkerAlpha     = fieldC->addBool( "setGroupMarkerAlpha"     ))->setBoolValue( false );
  (f_SetGroupMarkerSize      = fieldC->addBool( "setGroupMarkerSize"      ))->setBoolValue( false );
  (f_SetGroupUseVisuals      = fieldC->addBool( "setGroupUseVisuals"      ))->setBoolValue( false );
  (f_SetGroupUseShowState    = fieldC->addBool( "setGroupUseShowState"    ))->setBoolValue( false );
  (f_SetGroupUseVoxelizeState        = fieldC->addBool( "setGroupUseVoxelizeState"        ))->setBoolValue( false );
  (f_SetGroupUseEditableState        = fieldC->addBool( "setGroupUseEditableState"        ))->setBoolValue( false );
  (f_SetGroupUseTimePointIndex       = fieldC->addBool( "setGroupUseTimePointIndex"       ))->setBoolValue( false );
  (f_SetGroupUseVoxelWrite           = fieldC->addBool( "setGroupUseVoxelWrite"           ))->setBoolValue( false );
  (f_SetGroupUseMarkerSettings       = fieldC->addBool( "setGroupUseMarkerSettings"       ))->setBoolValue( false );
  (f_SetGroupMaximumNumCSOs          = fieldC->addBool( "setGroupMaximumNumCSOs"          ))->setBoolValue( false );
  (f_SetGroupOverflowHandling        = fieldC->addBool( "setGroupOverflowHandling"        ))->setBoolValue( false );
  (f_SetGroupRemoveFromGroupHandling = fieldC->addBool( "setGroupRemoveFromGroupHandling" ))->setBoolValue( false );
  (f_SetGroupDeleteGroupCSOHandling  = fieldC->addBool( "setGroupDeleteGroupCSOHandling"  ))->setBoolValue( false );

  handleNotificationOn();

}
CSOSetProperties::~CSOSetProperties()
{
  ML_TRACE_IN("CSOSetProperties::~CSOSetProperties()");

  if (m_InputCSOList != NULL){
    m_InputCSOList->removeNotificationObserver(CSOListNotifyObserverCB, this);
  }
  if (!f_WorkDirectlyOnInputList->getBoolValue()){
#if MEVISLAB_VERSION < 203
    ML_DELETE(m_OutputCSOList);
#endif
    m_OutputCSOList = NULL;
  }
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void CSOSetProperties::handleNotification (Field *field)
{
  ML_TRACE_IN("CSOSetProperties::handleNotification()");

  // f_InputCSOList
  if ( field == f_InputCSOList ){
    if ( f_AutoUpdate->getBoolValue() ){
      SetProperties(true);
    }
  } else

  // f_InputCSOString
  if (field == f_InputCSOString){
    if ( f_AutoUpdate->getBoolValue() ){
      SetProperties(true);
    }
  } else

  //f_WorkDirectlyOnInputList
  if (field == f_WorkDirectlyOnInputList){
    if ( f_AutoUpdate->getBoolValue() ){
      bool hasChangedToOn = f_WorkDirectlyOnInputList->getBoolValue();
      if (m_InputCSOList != NULL){
        if (hasChangedToOn){
#if MEVISLAB_VERSION < 203
          ML_DELETE(m_OutputCSOList);
#endif
          m_OutputCSOList = NULL;
          m_OutputCSOList = m_InputCSOList;
        } else {
          ML_CHECK_NEW(m_OutputCSOList, CSOList(*m_InputCSOList));
        }
      } else { // no input, nothing to put out
        if (hasChangedToOn){
          if (m_OutputCSOList != NULL){
#if MEVISLAB_VERSION < 203
            ML_DELETE(m_OutputCSOList);
#endif
            m_OutputCSOList = NULL;
          }
        }
        m_OutputCSOList = NULL;
      }
      f_OutputCSOList->setBaseValue(m_OutputCSOList);
      SetProperties(false);
    }
  } else

  // f_Update
  if (field == f_Update || f_AutoUpdate->getBoolValue() ){
    SetProperties(true);
  }

  if (field == f_GetPorperties ){
    GetProperties();
  }
}


void CSOSetProperties::SetupInternalCSOList()
{
  ML_TRACE_IN("void CSOSetProperties::_setupInternalCSOList()");

  bool workDirectlyOnInputCSOList = f_WorkDirectlyOnInputList->getBoolValue();

  if (!m_IsInNotificationCB) { CSOList::removeNotificationObserverFromAllCSOLists(CSOListNotifyObserverCB, this); }

  if (BASE_IS_A((static_cast<Base*>(f_InputCSOList->getBaseValue())), CSOList)){
    m_InputCSOList = static_cast<CSOList*>(f_InputCSOList->getBaseValue());
  } else {
    m_InputCSOList = NULL;
    if (workDirectlyOnInputCSOList){
      m_OutputCSOList = NULL;
    } else {
#if MEVISLAB_VERSION < 203
      ML_DELETE(m_OutputCSOList);
#endif
      m_OutputCSOList = NULL;
    }
  }
  if (m_InputCSOList != NULL){
    if (!workDirectlyOnInputCSOList){ // make copy
#if MEVISLAB_VERSION < 203
      ML_DELETE(m_OutputCSOList);
#endif
      m_OutputCSOList = NULL;
      ML_CHECK_NEW(m_OutputCSOList, CSOList(*m_InputCSOList));
    } else { // use pointer
      m_OutputCSOList = m_InputCSOList;
    }
    if (!m_IsInNotificationCB) { m_InputCSOList->addNotificationObserver(CSOListNotifyObserverCB, this); }
  }
  f_OutputCSOList->setBaseValue(m_OutputCSOList);
}



void CSOSetProperties::ParseInputCSOString()
{
  ML_TRACE_IN("void CSOSetProperties::_parseInputCSOString()");

  m_selectedCSOIds.clear();

  if (m_OutputCSOList != NULL){
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


void CSOSetProperties::ParseInputGroupString()
{
  ML_TRACE_IN("void CSOSetProperties::_parseInputCSOString()");

  m_selectedGroupIds.clear();

  if (m_OutputCSOList != NULL){
    std::string inputString = f_InputGroupString->getStringValue();
    if (inputString.length() > 0){
      int spaceSeparatorIndex = static_cast<int>(inputString.find(" "));

      while (spaceSeparatorIndex != static_cast<int>(std::string::npos)){
        std::string currentItem = inputString.substr(0, spaceSeparatorIndex);
        inputString = inputString.substr(spaceSeparatorIndex+1, inputString.length()-spaceSeparatorIndex);
        spaceSeparatorIndex = static_cast<int>(inputString.find(" "));
        AddGroupId(currentItem);
      }
      AddGroupId(inputString); // adds the rest of the string (last element)
    }
  }
}



void CSOSetProperties::AddCSOId(const std::string& idString)
{
  ML_TRACE_IN_TC("void CSOSetProperties::AddCSOId(const std::string& idString)");

  if (m_OutputCSOList != NULL){
    int currentId = 0;
    std::stringstream strstream;
    strstream << idString;
    strstream >> currentId;
    if ( currentId == -1 ) {
      m_selectedCSOIds.clear();
      unsigned int nCSO = m_OutputCSOList->numCSO();
      for ( unsigned int iCSO=0; iCSO < nCSO; ++iCSO ){
        m_selectedCSOIds.push_back( m_OutputCSOList->getCSOAt( iCSO )->getId() );
      }
    } else {
      CSO* testCSOPointer = m_OutputCSOList->getCSOById(currentId);
      if (testCSOPointer != NULL){
        m_selectedCSOIds.push_back(currentId);
      }
    }
  }
}


void CSOSetProperties::AddGroupId(const std::string& idString)
{
  ML_TRACE_IN_TC("void CSOSetProperties::AddGroupId(const std::string& idString)");

  if (m_OutputCSOList != NULL){
    int currentId = 0;
    std::stringstream strstream;
    strstream << idString;
    strstream >> currentId;
    if ( currentId == -1 ) {
      m_selectedGroupIds.clear();
      unsigned int nGroups = m_OutputCSOList->numGroups();
      for ( unsigned int iGroup=0; iGroup < nGroups; ++iGroup ){
        m_selectedGroupIds.push_back( m_OutputCSOList->getGroupAt( iGroup )->getId() );
      }
    } else {
      CSOGroup* testGroupPointer = m_OutputCSOList->getGroupById(currentId);
      if (testGroupPointer != NULL){
        m_selectedGroupIds.push_back(currentId);
      }
    }
  }
}

void CSOSetProperties::SetProperties(bool shouldSetupInternalCSOList)
{
  this->SetProperties(shouldSetupInternalCSOList, 0 );
}

void CSOSetProperties::SetProperties(bool shouldSetupInternalCSOList, int notificationFlag)
{
  if (shouldSetupInternalCSOList){
    SetupInternalCSOList();
  }
  if (m_OutputCSOList != NULL){

    switch ( f_InputMode->getEnumValue() ){
      case CSOINPUT:
      {
        ParseInputCSOString();
        if ( f_CSOClearSelection->getBoolValue() && f_SetCSOIsSelected->getBoolValue() ){
          m_OutputCSOList->clearSelectedCSOs();
          notificationFlag |= CSOList::NOTIFICATION_CSO_SELECTION;
        }

        for (unsigned int i = 0; i < m_selectedCSOIds.size(); i++){
          const unsigned int currentId = m_selectedCSOIds[i];
          CSO* currentCSO = m_OutputCSOList->getCSOById(currentId);

          // Common
          if ( f_SetCSOLabel->getBoolValue()          ) { currentCSO->setLabel(          f_CSOLabel->getStringValue()       ); notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED; }
          if ( f_SetCSODescription->getBoolValue()    ) { currentCSO->setDescription(    f_CSODescription->getStringValue() ); notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED; }
          if ( f_SetCSOCreatorId->getBoolValue()      ) { currentCSO->setCreatorId(      static_cast<int>(f_CSOCreatorId->getIntValue())      ); notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED; }
          if ( f_SetCSOTimePointIndex->getBoolValue() ) { currentCSO->setTimePointIndex( static_cast<int>(f_CSOTimePointIndex->getIntValue()) ); notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED; }

          if ( f_SetCSOIsSelected->getBoolValue()      ) {
            CSOList *currentList = currentCSO->getCSOList();
            if (currentList->isSelected(currentCSO) != f_CSOIsSelected->getBoolValue() ){
              notificationFlag |= CSOList::NOTIFICATION_CSO_SELECTION;
            }
            if ( f_CSOIsSelected->getBoolValue() ){
              currentCSO->getCSOList()->addSelected(currentCSO);
            } else {
              currentCSO->getCSOList()->removeFromSelection( currentCSO );
            }
          }
          if ( f_SetCSOShowState->getBoolValue()     ) { currentCSO->setShowState(     f_CSOShowState->getBoolValue()     ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOVoxelizeState->getBoolValue() ) { currentCSO->setVoxelizeState( f_CSOVoxelizeState->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOEditableState->getBoolValue() ) { currentCSO->setEditableState( f_CSOEditableState->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          if ( f_SetCSOComputeNormal->getBoolValue() &&
            f_CSOComputeNormal->getBoolValue()    ){
              currentCSO->pathChanged();
              currentCSO->invalidatePlaneNormal();
              currentCSO->computePlaneNormal();
              notificationFlag |= CSOList::NOTIFICATION_CSO_FINISHED;
          }

          // PathPoints
          if ( f_SetCSOLineStyle->getBoolValue() ) { currentCSO->setLineStyle( static_cast<CSOLineStyle>(f_CSOLineStyle->getEnumValue()) ); notificationFlag |= CSOList::NOTIFICATION_REPAINT;     }
          if ( f_SetCSOLineWidth->getBoolValue() ) { currentCSO->setLineWidth( f_CSOLineWidth->getFloatValue()       ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOColor->getBoolValue()     ) { currentCSO->setColor(     f_CSOColor->getVec3fValue()           ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOAlpha->getBoolValue()     ) { currentCSO->setAlpha(     f_CSOAlpha->getFloatValue()           ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // Markers
          if ( f_SetCSOMarkerMode->getBoolValue()  ) { currentCSO->setMarkerMode(  f_CSOMarkerMode->getEnumValue()   ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOMarkerColor->getBoolValue() ) { currentCSO->setMarkerColor( f_CSOMarkerColor->getVec3fValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOMarkerAlpha->getBoolValue() ) { currentCSO->setMarkerAlpha( f_CSOMarkerAlpha->getFloatValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOMarkerSize->getBoolValue()  ) { currentCSO->setMarkerSize(  f_CSOMarkerSize->getFloatValue()  ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // Voxelize
          if ( f_SetCSOVoxelWriteMode->getBoolValue()  ) { currentCSO->setVoxelWriteMode(  f_CSOVoxelWriteMode->getEnumValue()   ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetCSOVoxelWriteValue->getBoolValue() ) { currentCSO->setVoxelWriteValue( f_CSOVoxelWriteValue->getFloatValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
        }// m_selectedCSOIds loop
        break;
      } //CSOINPUT

      case GROUPINPUT:
      {
        ParseInputGroupString();
        if (f_GroupClearSelection->getBoolValue() && f_SetGroupIsSelected->getBoolValue() ){
          m_OutputCSOList->clearSelectedGroups();
          notificationFlag |= CSOList::NOTIFICATION_GROUP_SELECTION;
        }

        for (unsigned int i = 0; i < m_selectedGroupIds.size(); i++){

          //Common
          const unsigned int currentId = m_selectedGroupIds[i];
          CSOGroup* currentGroup = m_OutputCSOList->getGroupById(currentId);

          if ( f_SetGroupLabel->getBoolValue()       ) { currentGroup->setLabel(       f_GroupLabel->getStringValue()       ); notificationFlag |= CSOList::NOTIFICATION_GROUP_FINISHED; }
          if ( f_SetGroupDescription->getBoolValue() ) { currentGroup->setDescription( f_GroupDescription->getStringValue() ); notificationFlag |= CSOList::NOTIFICATION_GROUP_FINISHED; }

          if ( f_SetGroupIsSelected->getBoolValue()      ) {
            CSOList *currentList = currentGroup->getCSOList();
            if (currentList->isSelected(currentGroup) != f_GroupIsSelected->getBoolValue() ){
              notificationFlag |= CSOList::NOTIFICATION_GROUP_SELECTION;
            }
            if ( f_GroupIsSelected->getBoolValue() ){
              currentList->addSelected(currentGroup);
            } else {
              currentList->removeFromSelection( currentGroup );
            }
          }

          if ( f_SetGroupUseTimePointIndex->getBoolValue() ){ currentGroup->setUseTimePointIndex( f_GroupUseTimePointIndex->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupTimePointIndex->getBoolValue()    ){ currentGroup->setTimePointIndex(    static_cast<int>(f_GroupTimePointIndex->getIntValue())     ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          if ( f_SetGroupUseShowState->getBoolValue()     ) { currentGroup->setUseShowState(     f_GroupUseShowState->getBoolValue()     ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupShowState->getBoolValue()        ) { currentGroup->setShowState(        f_GroupShowState->getBoolValue()        ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupUseVoxelizeState->getBoolValue() ) { currentGroup->setUseVoxelizeState( f_GroupUseVoxelizeState->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupVoxelizeState->getBoolValue()    ) { currentGroup->setVoxelizeState(    f_GroupVoxelizeState->getBoolValue()    ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupUseEditableState->getBoolValue() ) { currentGroup->setUseEditableState( f_GroupUseEditableState->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupEditableState->getBoolValue()    ) { currentGroup->setEditableState(    f_GroupEditableState->getBoolValue()    ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // PathPoints
          if ( f_SetGroupUseVisuals->getBoolValue() ) { currentGroup->setUseVisuals( f_GroupUseVisuals->getBoolValue()       ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupLineStyle->getBoolValue()  ) { currentGroup->setLineStyle(  static_cast<CSOLineStyle>(f_GroupLineStyle->getEnumValue())        ); notificationFlag |= CSOList::NOTIFICATION_GROUP_FINISHED; }
          if ( f_SetGroupLineWidth->getBoolValue()  ) { currentGroup->setLineWidth(  f_GroupLineWidth->getFloatValue()       ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupColor->getBoolValue()      ) { currentGroup->setColor(      f_GroupColor->getVec3fValue()           ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupAlpha->getBoolValue()      ) { currentGroup->setAlpha(      f_GroupAlpha->getFloatValue()           ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // Markers
          if ( f_SetGroupUseMarkerSettings->getBoolValue() ) { currentGroup->setUseMarkerSettings( f_GroupUseMarkerSettings->getBoolValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupMarkerMode->getBoolValue()        ) { currentGroup->setMarkerMode(        f_GroupMarkerMode->getEnumValue()        ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupMarkerSize->getBoolValue()        ) { currentGroup->setMarkerSize(        f_GroupMarkerSize->getFloatValue()       ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupMarkerColor->getBoolValue()       ) { currentGroup->setMarkerColor(       f_GroupMarkerColor->getVec3fValue()      ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupMarkerAlpha->getBoolValue()       ) { currentGroup->setMarkerAlpha(       f_GroupMarkerAlpha->getFloatValue()      ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // Voxelize
          if ( f_SetGroupUseVoxelWrite->getBoolValue()   ) { currentGroup->setUseVoxelWrite(   f_GroupUseVoxelWrite->getBoolValue()    ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupVoxelWriteMode->getBoolValue()  ) { currentGroup->setVoxelWriteMode(  f_GroupVoxelWriteMode->getEnumValue()   ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }
          if ( f_SetGroupVoxelWriteValue->getBoolValue() ) { currentGroup->setVoxelWriteValue( f_GroupVoxelWriteValue->getFloatValue() ); notificationFlag |= CSOList::NOTIFICATION_REPAINT; }

          // Rules
          if ( f_SetGroupMaximumNumCSOs->getBoolValue() ) {
            currentGroup->getRules().setNumMaximumCSOs( static_cast<int>(f_GroupMaximumNumCSOs->getIntValue()) );
            currentGroup->getRules().applyTo(*currentGroup);
            notificationFlag |= (CSOList::NOTIFICATION_CSO_FINISHED | CSOList::NOTIFICATION_GROUP_FINISHED);
          }
          if ( f_SetGroupOverflowHandling->getBoolValue() ) {
            currentGroup->getRules().setOverflowHandling( static_cast<CSOGroupOverflowHandling>( f_GroupOverflowHandling->getEnumValue()) );
            currentGroup->getRules().applyTo(*currentGroup);
            notificationFlag |= (CSOList::NOTIFICATION_CSO_FINISHED | CSOList::NOTIFICATION_GROUP_FINISHED);
          }
          if ( f_SetGroupRemoveFromGroupHandling->getBoolValue() ) {
            currentGroup->getRules().setRemoveFromGroupHandling( static_cast<CSORemoveHandling>(f_GroupRemoveFromGroupHandling->getEnumValue()) );
            currentGroup->getRules().applyTo(*currentGroup);
            notificationFlag |= (CSOList::NOTIFICATION_CSO_FINISHED | CSOList::NOTIFICATION_GROUP_FINISHED);
          }
          if ( f_SetGroupDeleteGroupCSOHandling->getBoolValue() ) {
            currentGroup->getRules().setDeleteGroupCSOHandling( static_cast<CSORemoveHandling>(f_GroupDeleteGroupCSOHandling->getEnumValue()) );
            currentGroup->getRules().applyDeletionRule(*currentGroup);
            notificationFlag |= (CSOList::NOTIFICATION_CSO_FINISHED | CSOList::NOTIFICATION_GROUP_FINISHED);
          }
        }
        break;
      } //GROUPINPUT
      default:
        break;
    }

    // Send notifications
    if (f_WorkDirectlyOnInputList->getBoolValue() == true){
      m_IsNotifyingMyself = true;
      m_InputCSOList->notifyObservers(notificationFlag);
      m_IsNotifyingMyself = false;
    } else {
      m_OutputCSOList->notifyObservers(notificationFlag);
    }
  }
}

void CSOSetProperties::GetProperties()
{
  CSO* cso = m_InputCSOList->getSelectedCSOAt(0);

  if ( !cso ) {
    cso = m_InputCSOList->getCSOAt(0);
  }

  if (cso) {
    // Common
   f_CSOLabel->setStringValue(       cso->getLabel()         );
   f_CSODescription->setStringValue( cso->getDescription()   );
   f_CSOCreatorId->setIntValue(      cso->getCreatorId()     );
   f_CSOTimePointIndex->setIntValue( cso->getTimePointIndex());
   f_CSOShowState->setBoolValue(     cso->getShowState()     );
   f_CSOVoxelizeState->setBoolValue( cso->getVoxelizeState() );
   f_CSOEditableState->setBoolValue( cso->getEditableState() );

    // PathPoints
    f_CSOLineStyle->setEnumValue(    cso->getLineStyle() );
    f_CSOLineWidth->setFloatValue(   cso->getLineWidth() );
    f_CSOColor->setVec3fValue(       cso->getColor()     );
    f_CSOAlpha->setFloatValue(       cso->getAlpha()     );

    // Markers
    f_CSOMarkerMode->setEnumValue(   cso->getMarkerMode()  );
    f_CSOMarkerColor->setVec3fValue( cso->getMarkerColor() );
    f_CSOMarkerAlpha->setFloatValue( cso->getMarkerAlpha() );
    f_CSOMarkerSize->setFloatValue(  cso->getMarkerSize()  );

    // Voxelize
    f_CSOVoxelWriteMode->setEnumValue(   cso->getVoxelWriteMode()  );
    f_CSOVoxelWriteValue->setFloatValue( cso->getVoxelWriteValue() );
  }
}

void CSOSetProperties::CSOListNotifyObserverCB(void* userData, int notificationFlag)
{
  ML_TRACE_IN("CSOSetProperties::_csoListNotifyObserverCB");

  CSOSetProperties* thisp = static_cast<CSOSetProperties*>(userData);

  thisp->m_IsInNotificationCB = true;
  int flags = (( thisp->f_PassOnNotifications->getBoolValue() &&
               !thisp->f_WorkDirectlyOnInputList->getBoolValue() ) ? notificationFlag : 0);

  if (!thisp->m_IsNotifyingMyself){
    if ( thisp->f_AutoUpdate->getBoolValue() ){
      if ( thisp->f_ListenToFinishingNotifications->getBoolValue()     &&
           ((notificationFlag & CSOList::NOTIFICATION_CSO_FINISHED)    ||
            (notificationFlag & CSOList::NOTIFICATION_GROUP_FINISHED)) ){
        thisp->SetProperties( true, flags );
      }
      if ( thisp->f_ListenToSelectionNotifications->getBoolValue()      &&
           ((notificationFlag & CSOList::NOTIFICATION_CSO_SELECTION)    ||
            (notificationFlag & CSOList::NOTIFICATION_GROUP_SELECTION)) ){
        thisp->SetProperties( true, flags );
      }
    }
  }
  thisp->m_IsInNotificationCB = false;
}

ML_END_NAMESPACE

