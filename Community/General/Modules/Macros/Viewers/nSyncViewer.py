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
#! Macro module nSyncViewer
#!
# \file    nSyncViewer.py
# \author  Reinhard Hameeteman
# \date    2007-09-12
#
# 
#
#----------------------------------------------------------------------------------

from mevis import *
from math import sqrt,floor,ceil
from PythonQt import *

size = QSize(0,0)
MaxInputs = 20
m_titleString = "input0,input1"
m_showTitleString = "[False,False]"
lockUpdate = False
m_win = None

#layoutTable = {1:[1,1],2:[1,2],3:[1,3],4:[2,2],5:[2,3]}

viewerUnit = '\
Box { \
  expandY = Yes\
  %title% \
  Viewer OrthoView2D%nr%.self { border = No type = SoRenderArea expandY = Yes name = Viewer%nr%} \
  EventFilter { \
    filter = KeyPress \
    control = Viewer%nr% \
    command = @@py:if args[0][\'key\']==\'F\' : ctx.module(\'OrthoView2D%nr%\').showWindow(\'Settings\')@@ \
  } \
} \
'
dummyUnit = '\
Viewer Dummy.self { \
  type = SoRenderArea \
  backgroundColor = lightgray \
  border = no viewing = No \
}\
'


def Init( arg = None ) :
  '''Because the numberOfInputs is presistent the GUI needs updating when the module is copied'''
  ctx.addMacroModuleFromString('Window{ Vertical {expandY = yes expandX = yes Vertical { Label { title = "" } } Box {pw = 400 ph = 400}}}')
  ctx.field('OrthoView2D0.ext.borderOn').value = False
  ctx.field('OrthoView2D0.ext.annotVerticalRuler').value = False
  NumberOfInputChanged()
  return


def NumberOfInputChanged( field = None ) :
  '''Show / hide the appropriate input fields and connect the right viewers
     Creat the right modules and connect 
  '''
  global MaxInputs
  #MLAB.log("NumberOfInputChanged()")
  nInputs = ctx.field('numberOfInputs').value
  # turn off viewers
  ctx.field('In0.noBypass').value = True
  ctx.field('invInput0').disconnectOutputs()
  ctx.field( 'OrthoView2D0.'+ ctx.field('singleInvInput').stringValue() ).connectFrom(ctx.field('invInput0') )
  
  # create the appropriate modules
  for iInput in range(1,nInputs) :
    viewerModuleName = 'OrthoView2D'+str(iInput)
    reformatModuleName = 'Reformat'+str(iInput)
    if ( not ctx.hasModule( viewerModuleName ) ) : 
      newViewerModule = ctx.addModule( 'OrthoView2D' )
      newViewerModule.field('instanceName').value = viewerModuleName
    if ( not ctx.hasModule( reformatModuleName ) ) : 
      newReformatModule = ctx.addModule( 'Reformat' )
      newReformatModule.field('instanceName').value = reformatModuleName
    # Show the connectors and connect the Inventor inputs
    ctx.field( 'input'+str(iInput) ).setHidden( False )
    ctx.field('invInput' + str(iInput)).disconnectOutputs()
    ctx.field( 'OrthoView2D'+ str( iInput ) + '.' + ctx.field('singleInvInput').stringValue() ).connectFrom(ctx.field('invInput' + str(iInput)))
    
    # Sync several fields
    ctx.field( viewerModuleName+'.worldPosition' ).connectFrom( ctx.field('OrthoView2D'+str(iInput-1)+'.worldPosition' ))
    ctx.field( viewerModuleName+'.pos.blendOnto' ).connectFrom( ctx.field('OrthoView2D'+str(iInput-1)+'.pos.blendOnto' ))
    ctx.field( viewerModuleName+'.ext.annoCoords').connectFrom( ctx.field('OrthoView2D'+str(iInput-1)+'.ext.annoCoords'))
    ctx.field( viewerModuleName+'.layout').value = 'LAYOUT_AXIAL'
    ctx.field( viewerModuleName+'.ext.borderOn').value = False 
    ctx.field( viewerModuleName+'.ext.annotVerticalRuler').value = False 
  ctx.field( 'SyncPos.vector0' ).connectFrom( ctx.field( 'OrthoView2D'+ str( nInputs-1 ) + '.worldPosition' ) )
  ctx.field( 'OrthoView2D0.worldPosition' ).connectFrom( ctx.field( 'SyncPos.vector1' ) )
    
  # remove unused modules and hide connections
  for iInput in range(nInputs,MaxInputs):
    ctx.field( 'input'+str(iInput) ).disconnect()
    ctx.field( 'input'+str(iInput) ).setHidden( True )
    ctx.field( 'invInput'+str(iInput) ).disconnect()
    ctx.field( 'invInput'+str(iInput) ).setHidden( True )
    mod = ctx.module( 'OrthoView2D'+ str( iInput ) )
    if mod is not None :
      ctx.network().removeModule( mod )
    mod = ctx.module( 'Reformat'+ str( iInput )  )
    if mod is not None :
      ctx.network().removeModule( mod )
  ctx.field('In0.noBypass').value = False
  if ( ctx.hasControl('ViewerFrame') ) :
    UpdateViewerGUI()
  if ( ctx.hasControl('SettingsFrame') ) :
    UpdateSettingsGUI()
  ctx.updateLayout()
  ShowInventorInputs()
  # Reformat if desired
  ctx.field('referenceGrid').setMaxValue( nInputs-1 )
  ApplyReformat()
  SyncZoom()
  SyncLUT()
  SyncTimepoints()
  return


def ShowInventorInputs():
  hide = not ctx.field('showInventorInputs').boolValue()
  nInputs = ctx.field('numberOfInputs').value
  for iInput in range(20) :
    if (iInput >= nInputs ): hide = True
    ctx.field( 'invInput'+str(iInput) ).setHidden( hide )
  ctx.updateLayout()
  return


def ApplyReformat():
  '''Connect the viewer inputs to the right module'''
  #MLAB.log("ApplyMergeRegions()")
  ctx.field('In0.noBypass').value = True
  nInputs = ctx.field('numberOfInputs').value
  for i in range(nInputs) :
    input = 'input' + str(i)
    if ctx.field('reformat').value :
      ctx.field( 'OrthoView2D'+ str( i ) + '.image').connectFrom(ctx.field('Reformat'+str(i)+'.output0') )
      ctx.field( 'Reformat'+str(i)+'.input1').connectFrom( ctx.field('Switch.output0') )
      ctx.field( 'Reformat'+str(i)+'.input0').connectFrom( ctx.field('In' + str(i) + '.output0') )
    else :
      ctx.field( 'OrthoView2D'+ str( i ) + '.image').connectFrom(ctx.field('In' + str(i) + '.output0'))
  if ctx.field('reformat').value :
    ctx.field('Switch.currentInput').value = ctx.field("referenceGrid").value
  ctx.field('In0.noBypass').value = False
  return
  

def SyncZoom( arg = None ) :
  '''Synchronise the zooming of the used viewers'''
  #MLAB.log('SyncZoom()')
  syncZoom = ctx.field('syncZoom').value
  nInputs = ctx.field('numberOfInputs').value
  for i in range(1,nInputs) :
    if ( syncZoom ) :
      ctx.field( 'OrthoView2D'+ str( i )+'.view.sliceZoom' ).connectFrom( ctx.field('OrthoView2D'+ str( i-1 ) + '.view.sliceZoom') )
    else :
      ctx.field( 'OrthoView2D'+ str( i ) + '.view.sliceZoom').disconnectAll()
  if syncZoom :
    ctx.field( 'SyncZoom.float0' ).connectFrom(ctx.field('OrthoView2D'+ str( nInputs-1 ) + '.view.sliceZoom'))
    ctx.field( 'OrthoView2D0.view.sliceZoom').connectFrom(  ctx.field('SyncZoom.float1') )
  else :
    ctx.field( 'SyncZoom.float0' ).disconnectAll()
    ctx.field( 'OrthoView2D0.view.sliceZoom').disconnectAll()
  return


def UnZoom():
  nInputs = ctx.field('numberOfInputs').value
  for i in range(nInputs) :
    ctx.field( 'OrthoView2D'+ str( i )+'.view.unzoom'   ).touch()
  return

def SyncPanning( arg = None ) :
  '''Synchronise the panning of the used viewers'''
  #MLAB.log('SyncZoom()')
  syncZoom = ctx.field('syncPanning').value
  nInputs = ctx.field('numberOfInputs').value
  for i in range(1,nInputs) :
    if ( syncZoom ) :
      for j in range(1,4) :
        ctx.field( 'OrthoView2D'+ str( i )+'.view.viewingCenter' + str( j ) ).connectFrom( ctx.field('OrthoView2D'+ str( i-1 ) + '.view.viewingCenter' + str( j )) )
    else :
      for j in range(1,4) :
        ctx.field( 'OrthoView2D'+ str( i ) + '.view.viewingCenter' + str( j ) ).disconnectAll()
  if syncZoom :
    for j in range(1,4) :
      ctx.field( 'SyncPanning' + str( j  ) + '.vector0' ).connectFrom(ctx.field('OrthoView2D'+ str( nInputs-1 ) + '.view.viewingCenter' + str( j ) ))
      ctx.field( 'OrthoView2D0.view.viewingCenter' + str( j )).connectFrom(  ctx.field('SyncPanning' +  str( j ) +'.vector1') )
  else :
    for j in range(1,4) :
      ctx.field( 'SyncPanning' + str( j ) + '.vector0' ).disconnectAll()
      ctx.field( 'OrthoView2D0.view.viewingCenter' + str( j ) ).disconnectAll()
  return


def SyncLUT( arg = None ) :
  '''Synchronise the LUTs of the used viewers'''
  syncLUT = ctx.field('syncLUT').value
  nInputs = ctx.field('numberOfInputs').value
  for i in range(1, nInputs ) :
    if ( syncLUT ) :
      ctx.field( 'OrthoView2D'+str(i)+'.greyCenter' ).connectFrom( ctx.field('OrthoView2D'+str(i-1)+'.greyCenter' ))
      ctx.field( 'OrthoView2D'+str(i)+'.greyWidth'  ).connectFrom( ctx.field('OrthoView2D'+str(i-1)+'.greyWidth'  ))
    else :
      ctx.field( 'OrthoView2D'+str(i)+'.greyCenter').disconnectAll()
      ctx.field( 'OrthoView2D'+str(i)+'.greyWidth' ).disconnectAll()
  if syncLUT :
    ctx.field( 'SyncLUTCenter.float0'    ).connectFrom( ctx.field('OrthoView2D'+str(nInputs-1)+'.greyCenter' ))
    ctx.field( 'OrthoView2D0.greyCenter' ).connectFrom( ctx.field('SyncLUTCenter.float1'  ))
    ctx.field( 'SyncLUTWidth.float0'     ).connectFrom( ctx.field('OrthoView2D'+str(nInputs-1)+'.greyWidth'  ))
    ctx.field( 'OrthoView2D0.greyWidth'  ).connectFrom( ctx.field('SyncLUTWidth.float1' ))
  else :
    ctx.field( 'SyncLUTCenter.float0'    ).disconnectAll()
    ctx.field( 'OrthoView2D0.greyCenter' ).disconnectAll()
    ctx.field( 'SyncLUTWidth.float0'     ).disconnectAll()
    ctx.field( 'OrthoView2D0.greyWidth'  ).disconnectAll()
  return
  

def SyncTimepoints( arg = None ) :
  '''Synchronise the timepoints of the used viewers'''
  syncTimepoints = ctx.field('syncTimepoints').value
  nInputs = ctx.field('numberOfInputs').value
  for i in range(1,nInputs) :
    if ( syncTimepoints ) :
      ctx.field( 'OrthoView2D'+ str( i )+'.view.timePoint' ).connectFrom( ctx.field('OrthoView2D'+str(i-1)+'.view.timePoint') )
    else :
      ctx.field( 'OrthoView2D'+ str( i ) + '.view.timePoint').disconnectAll()
  if ( syncTimepoints ) :
    ctx.field( 'SyncTimepoints.float0' ).connectFrom( ctx.field('OrthoView2D'+str(nInputs-1)+'.view.timePoint') )
    ctx.field( 'OrthoView2D0.view.timePoint').connectFrom(  ctx.field('SyncTimepoints.float1') )
  else :
    ctx.field( 'SyncTimepoints.float0' ).disconnectAll()
    ctx.field( 'OrthoView2D0.view.timePoint').disconnectAll()
  return


def UpdateViewerGUI():
  global viewerUnit, dummyUnit, m_win
  if ( not ctx.hasControl('ViewerFrame') ) : return
  layoutTable = eval( ctx.field("layout").value )
  nInputs = ctx.field('numberOfInputs').value
  #showTitles = eval( ctx.field("showTitleString").value )
  #titles = ctx.field("titleString").value.split(',')
  rows = 1
  cols = 1
  if nInputs in layoutTable :
    rows = layoutTable[nInputs][0]
    cols = layoutTable[nInputs][1]
  else :
    rows = int( round(sqrt(nInputs)) )
    cols = int( ceil(sqrt(nInputs)) )
  ctx.control('mainWindow').setMinWidth(  min(cols*200,1000) )
  ctx.control('mainWindow').setMinHeight( min(rows*200,800) )
  iUnit = 0
  viewerGUI = 'Table { '
  for row in range(rows) :
    viewerGUI += 'Row{'
    for col in range(cols) :
      if iUnit < nInputs :
        unit = viewerUnit.replace('%nr%',str(iUnit) )
        '''
        if iUnit < len(showTitles) and iUnit < len(titles):
          if showTitles[iUnit] :
            unit = unit.replace('%title%', 'Label { title = ' + titles[iUnit] + '}' )
          else :
            unit = unit.replace('%title%', '' )
        else :
        '''
        unit = unit.replace('%title%', '' )
        viewerGUI += unit
      else:
        viewerGUI += dummyUnit
      iUnit +=1
    viewerGUI += '}'
  viewerGUI += '}'
  ctx.control('ViewerFrame').setContentString( viewerGUI )
  m_win = ctx.window()
  ctx.updateLayout()
  return


'''
def UpdateSettingsGUI():
  global lockUpdate
  lockUpdate = True
  MLAB.log("UpdateSettingsGUI")
  nInputs = ctx.field('numberOfInputs').value
  titles = ctx.field('titleString').value.split(',')
  showTitles = eval( ctx.field("showTitleString").value )
  titlesCtrl = ctx.control("titlesListCtrl")
  titlesCtrl.clearItems()
  for iTitle in range( nInputs ) :
    title = ''
    if iTitle < len(titles) and titles != ['']:
      title = titles[iTitle]
    else :
      title = 'Input'+str(iTitle)
    row = titlesCtrl.insertCheckBoxItem();
    row.setTexts(['','Input'+str(iTitle),title]);
    showTitle = False 
    if iTitle < len( showTitles ) :
      showTitle = showTitles[iTitle]
    row.setOn( showTitle )
  lockUpdate = False 
  #UpdateTitleString()
  #UpdateShowTitleString()
  return


def SetNewTitle():
  global lockUpdate
  titlesCtrl = ctx.control("titlesListCtrl")
  selectedTitle = titlesCtrl.currentItem() 
  lockUpdate = True
  currentText = selectedTitle.setText(2, ctx.field("newTitle").value )
  lockUpdate = False
  UpdateTitleStrings()
  return

def UpdateTitleStrings():
  MLAB.log("UpdateTitleStrings")
  global lockUpdate
  if ( not lockUpdate ) :
    nInputs = ctx.field('numberOfInputs').value
    titlesCtrl = ctx.control("titlesListCtrl")
    titleString = ''
    showTitleString = '['
    item = titlesCtrl.firstItem()
    while item :
      titleString += item.text(2) + ','
      showTitleString += str( item.isOn() ) + ','
      item = item.nextSibling()
    titleString = titleString.strip(',')
    showTitleString = showTitleString.strip(',')
    showTitleString += ']'
    ctx.field("showTitleString").value = showTitleString
    ctx.field("titleString").value = titleString 
    UpdateTitles()
  return

  
def UpdateTitles():
  global m_showTitleString, m_titleString, lockUpdate, m_win
  if lockUpdate : return
  MLAB.log("UpdateTitles")
  showTitleString = ctx.field("showTitleString").value
  titleString = ctx.field("titleString").value
  if ( m_titleString != titleString or m_showTitleString != showTitleString ) :
    m_titleString = titleString 
    m_showTitleString = showTitleString 
    #win = ctx.showWindow("Viewer")
    if m_win :
      print "update frame"
      m_win.close()
      m_win.show()
    #ctx.updateLayout()
    #ctx.closeAllWindows()
    #ctx.reshowClosedWindows()
  return

'''
def WindowResize( arg = None ) :
  '''On window resize, update the layout'''
  #MLAB.log('WindowResize()')
  #ctx.control('mainWindow').updateFrame()
  return 

def Hide() : 
  global size
  #size = ctx.control("mainWindow").size()
  return

def Show() : 
  global size
  #ctx.control("mainWindow").resize( size )
  return

def ToggleAnnotation( arg = None ):
  '''Toggle all annotations on and off'''
  #MLAB.log('ToggleAnnotation()')
  nInputs = ctx.field('numberOfInputs').value
  currentValue = ctx.field('OrthoView2D0.annotationSizeMode').value
  values = list( ctx.field('OrthoView2D0.annotationSizeMode').items() )
  newValue = ''
  if ( currentValue == values[-1] ) :
    newValue = values[0]
  else :
    newValue = values[ values.index( currentValue)+1 ]
  for  i in range( nInputs )  :
    ctx.field('OrthoView2D' + str(i) + '.annotationSizeMode').value = newValue 
  return

# MeVis signature v1
# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
# owner: MeVis Internal Developer
# date: 2007-07-05T03:39:16
# hash: Tzy7AGzs3RTVPEX8HYNWb67yw48RjE3KzJBfPjHXAcvDneKgDMcD#dT2Tvl6AZmx/gsbhkHVMhWl3KW+V/IXA==
# MeVis end

