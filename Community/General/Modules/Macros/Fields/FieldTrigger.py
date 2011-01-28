#----------------------------------------------------------------------------------
# 
# Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
# Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of BIGR nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------
# 
#  |
#  |   
#  |
#  |   Description:
#  |   Python methods for FieldTrigger.script
#  | 
#  |
#  |   Author(s)      : Reinhard Hameeteman
#  |   Creation date  : 05/2007
#  |
#----------------------------------------------------------------------------------

# Python module import
from mevis import *

def WindowWakeup():
  BuildModuleTreeView()
  return

def UpdateModuleList():
  '''Get a list of all modules in the network and filter out this module'''
  network = ctx.parent()
  moduleList = filter( lambda x: ( x != ctx.field('instanceName').value ),network.modules() )
  ctx.field('moduleList').value = ','.join(moduleList)
  return

def BuildModuleTreeView( field = None ):
  UpdateModuleList()
  fieldTreeView = ctx.control("fieldListBox");
  fieldTreeView.clearItems();

  # insert dummy object
  rootItem = fieldTreeView.insertItem("")
  
  fieldTreeView.setSorting(0, True); #sort by Module
  fieldTreeView.setRootIsDecorated( True )

  modules = ctx.field("moduleList").value.split(',')
  network = ctx.parent()
  for module in modules  :
    moduleItem = fieldTreeView.insertItem(rootItem, module);
    moduleItem.setText(0, module)
    fieldTreeView.ensureItemVisible(moduleItem);
    # Prevent error when this is the only module
    if network.hasModule( module ) :
      currentModule = network.module( module )
  
      # insert fields as child of modules
      fields = currentModule.parameters()
      for iField in fields :
        if ( currentModule.field( iField ).getType() == 'Trigger' or 
             ctx.field('allFields').boolValue() ) :
          fieldItem = fieldTreeView.insertItem(moduleItem, moduleItem, iField)
          fieldItem.setText(0, iField)

  # remove the dummy
  fieldTreeView.removeItem( rootItem )
  return

def AddFields( item = None ):
  fieldTreeView = ctx.control("fieldListBox");
  selectedFieldsTreeView = ctx.control("selectedFieldsView");
  selectedFields = ctx.field('selectedFields') 
  
  sfs = selectedFields.value.split(',');
  sfids = selectedFieldsTreeView.selectedItemIds();
  insertpos = 0;
  if len(sfids) > 0:
    insertpos = sfids[0];
  else:
    if len(sfs) > 0:
      insertpos = len(sfs) - 1;
      
  output = ''
  for i in range(0,len(sfs)):
    output += sfs[i] + ",";
    if i == insertpos:
      for  field in fieldTreeView.selectedItemIds() :
        selectedItem = fieldTreeView.itemForId( field )
        fieldName = str( selectedItem.text(0) )
        if selectedItem.parent():
          moduleName = str( selectedItem.parent().text(0) )
          output += moduleName + '.' + fieldName + ","
     
  selectedFields.value = output.rstrip(",");
  return output;

def FieldDroped( object ) :
  fieldName =  object.fullName()
  # Don't allow this module to be triggered
  if ( fieldName.find( ctx.field('instanceName').value ) == -1  and
       ( ctx.field( 'parent:'+fieldName ).getType() == 'Trigger' or
         ctx.field('allFields').boolValue() ) ) :
    ctx.field('selectedFields').value +=  ',' + fieldName
  return
  
def DeleteFields( field = None ):
  listView = ctx.control('selectedFieldsView')
  newString = ctx.field('selectedFields').value 
  for iId in listView.selectedItemIds() :
    item = listView.itemForId( iId )
    module = item.text(0)
    field  = item.text(1)
    fieldString = ',' + module + '.' + field
    newString = newString.replace(fieldString,'')
  ctx.field('selectedFields').value = newString
  return
  
def KeyPressed( event=None ):
  if event['key']=='Up' and event['shiftKey'] :
    MoveItemUp()
  elif  event['key']=='Down' and event['shiftKey'] :
    MoveItemDown()
  return

def MoveItemUp() :
  selectedFields = ctx.field("selectedFields").stringValue().split(',')
  listView = ctx.control('selectedFieldsView')
  currentItem = '.'.join( listView.currentItem().texts() )
  currentItemIdx = selectedFields.index( currentItem )
  if currentItemIdx > 1 :
    selectedFields.remove( currentItem )
    selectedFields.insert( currentItemIdx-1, currentItem )
  ctx.field("selectedFields").value = ','.join( selectedFields )
  return

def MoveItemDown() :
  selectedFields = ctx.field("selectedFields").stringValue().split(',')
  listView = ctx.control('selectedFieldsView')
  currentItem = '.'.join( listView.currentItem().texts() )
  currentItemIdx = selectedFields.index( currentItem )
  if currentItemIdx < len( selectedFields )-1 :
    selectedFields.remove( currentItem )
    selectedFields.insert( currentItemIdx+1, currentItem )
  ctx.field("selectedFields").value = ','.join( selectedFields )
  return
    
def TriggerFields( arg ):
  network = ctx.parent()
  fieldList = ctx.field('selectedFields').value.split(',')
  sleep = ctx.field("sleep").intValue()
  eachIter = ctx.field("sleepEachStep").boolValue()
  for iRepetition in range( ctx.field('repetition').value ):
    ctx.field("currentIteration").value=iRepetition
    for iField in fieldList[1:] :
      if (MLAB.shouldStop() ) :
        break
      ctx.field('parent:'+iField).touch()
      MLAB.processEvents()
      MLAB.processInventorQueue()
      if eachIter : MLAB.sleep(sleep)
    MLAB.processEvents()
    MLAB.processInventorQueue()
    MLAB.sleep(sleep)
  return

#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBALMoAKeDufSkjPLfaCUd7Ij4IgEsndoDH9mP+jpEXKnAczgkSCgtNyNEMyiLur8xV1zEN7f71aeTOVWVntbzpucCARE=:lD+X/cPXp4xBkg/BH8EhyVWLOXzUCvL/ccrRKrYcyKMt2wR4QiXj1OCsqQukghRS1dwd5fRaB39vHgPZUYpdyA==
#//# owner: EMC
#//# date: 2010-03-03T14:51:40
#//# hash: coVMpF1OpLnh/Aq1AXmPuWl4n/kpEa2aLF1QJq5AOncdkqez6UhJXMrSPyy97eIAOYYnAAagI3NevWdPsmaM6w==
#//# MeVis end
