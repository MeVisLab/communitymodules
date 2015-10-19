from mevis import *

_windowCtrl = ""

def initWindowSlot( control ):
  global _windowCtrl
  _windowCtrl = control
  #ctx.field("METKViewer3D.METKPicking.mustBePickable").value=0
  return
  
def mainFrameResizeEventSlot( event ):
  updateGeometry()
  return


def updateGeometry():
  if not _windowCtrl:
    return

  # Current frame size of enclosing frame
  mainFrameSize     = _windowCtrl.control( "mainFrame" ).size()  
  
  # FreeFloat where to display in render area + its panel
  freeFloatFrameCtrl = _windowCtrl.control( "freeFloatFrame" )

  # Render area
  renderAreaFrameCtrl = _windowCtrl.control( "renderAreaFrame" )

  # Render area panel
  renderAreaPanelFrameCtrl = ctx.control( "renderAreaPanelFrame" )

  # Place render area within FreeFloat frame
  freeFloatFrameCtrl.setControlPos ( renderAreaFrameCtrl, 0, 0 )
  freeFloatFrameCtrl.setControlSize( renderAreaFrameCtrl, mainFrameSize )
    
  # Place render area panel in FreeFloat frame
  panelSize = renderAreaPanelFrameCtrl.size()
  freeFloatFrameCtrl.setControlPos ( renderAreaPanelFrameCtrl, mainFrameSize.width() -  panelSize.width() - 2, mainFrameSize.height() - panelSize.height() - 2 )
  renderAreaPanelFrameCtrl.setVisible( ctx.field( "showFenster" ).value  )
  
  return
  

def showFensterSlot( field ):
  updateGeometry()
  return
  
  
def viewAllSmall(field=0):
   ctx.field("smallViewer.viewAll").touch()
   return