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
#  |   Description:
#  |   Python methods for FieldWriter.script
#  | 
#  |
#  |   Author(s)      : Reinhard Hameeteman
#  |   Creation date  : 02/2007
#  |
#  _______________________________________________________________________

from mevis import *

def FieldDroped( object ) :
  ctx.field('selectedFields').value +=  ',' + object.fullName()
  return
  
def DeleteFields():
  listView = ctx.control('selectedFieldsView')
  newString = 'Module.Field'
  itemId = listView.firstItem().id()
  lastId = listView.lastItem().id()
  while ( itemId <= lastId ) :
    item = listView.itemForId( itemId )
    if ( not listView.isSelected( item ) ):
      newString += ',' + '.'.join( item.texts() )
    itemId += 1
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
    
def SaveFields( arg ):
  fileName = ctx.field('fileName').value
  network = ctx.parent()
  writeHeader = ctx.field('writeHeader').value
  if ( fileName == '' ):
    fileName = MLABFileDialog.getSaveFileName(ctx.localPath(),"", "Save statistics");
    ctx.field('fileName').value = fileName
  if ( fileName <> '' ) :
    file = None
    if ( ctx.field('append').value == True ) :
      # Check if file exists, and set writeheader
      # We only want to write the header the first time
      if ( MLABFileManager.exists( fileName ) ):
        writeHeader = False
      file = open(fileName, 'a')
    else :
      file = open(fileName, 'w')
    if ( writeHeader == True ) :
      header = ctx.field('selectedFields').value
      header = header.lstrip('Module.Field,')
      header = header.replace(',' , ctx.field('separationCharacter').stringValue() ) 
      header += '\n'
      file.write( header )
    fieldList = ctx.field('selectedFields').value.split(',')
    file.write( GetValues() )
    #if ( ctx.field('append').value == True ):
    #  file.write( '\n' )
    file.close()
  return 

def GetValues( arg=None):
  selectedFields = ctx.field('selectedFields').stringValue()[13:]
  outputString = ''
  if len(selectedFields) > 0:
    fieldList = selectedFields.split(',')
    separationCharacter = ''
    if (ctx.field('separationCharacter').stringValue() == '\\n'):
      separationCharacter =  '\n'
    else:
      separationCharacter =  ctx.field('separationCharacter').stringValue()
    num = len(fieldList)
    nw = ctx.parent()
    for iField in fieldList :
      value  = nw.field( iField ).stringValue()
      outputString += value
      outputString += separationCharacter
    outputString = outputString[:-len( separationCharacter )]
    outputString +=  '\n'
  return outputString

def GetOutput( arg = None ):
  selectedFields = ctx.field('selectedFields').stringValue()[13:]
  outputString = ''
  if ( ctx.field("writeHeader").boolValue() and not ctx.field("append").boolValue() ) :
    outputString = selectedFields + '\n' 
  outputString += GetValues()
  ctx.field('outputString').value = outputString 


#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBALMoAKeDufSkjPLfaCUd7Ij4IgEsndoDH9mP+jpEXKnAczgkSCgtNyNEMyiLur8xV1zEN7f71aeTOVWVntbzpucCARE=:lD+X/cPXp4xBkg/BH8EhyVWLOXzUCvL/ccrRKrYcyKMt2wR4QiXj1OCsqQukghRS1dwd5fRaB39vHgPZUYpdyA==
#//# owner: EMC
#//# date: 2010-03-03T13:57:05
#//# hash: jaOyUudq5VM0RhaS8GuH++82FVmv0N0Koj/cTW3Ih0bm4NW4v4i1eD9/hl2qMXzolmsqBcWSGE/EBDJKmMy5Fg==
#//# MeVis end
