
from mevis import MLAB

def mainFrameResizeEventSlot(event):
  mainFrameSize = ctx.control("mainFrame").size()  
  
  freeFloat = ctx.control("freeFloatFrame")
  soRenderArea = ctx.control("soRenderAreaFrame")
  soExaminerViewer = ctx.control("soExaminerViewerFrame")

  freeFloat.setControlPos(soRenderArea, 0, 0)
  freeFloat.setControlSize(soRenderArea, mainFrameSize)
    
  position = ctx.field("position").value
  margin = ctx.field("margin").value
  hwidth = int(mainFrameSize.width()/2)
  hheight = int(mainFrameSize.height()/2)
  
  if position == "TopLeft":
    freeFloat.setControlPos(soExaminerViewer, margin + 1, margin + 1)
  elif position == "BottomRight":
    freeFloat.setControlPos(soExaminerViewer, hwidth + margin, hheight + margin)
    
  freeFloat.setControlSize(soExaminerViewer, hwidth - 2 * margin, hheight - 2 * margin)

  return