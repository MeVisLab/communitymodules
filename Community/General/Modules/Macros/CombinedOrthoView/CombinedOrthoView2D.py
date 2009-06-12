
from mevis import MLAB

def mainFrameResizeEventSlot(event):
  mainFrameSize = ctx.control("mainFrame").size()  
  
  freeFloat = ctx.control("freeFloatFrame")
  soRenderArea = ctx.control("soRenderAreaFrame")
  soRenderArea1 = ctx.control("soRenderArea1Frame")

  freeFloat.setControlPos(soRenderArea, 0, 0)
  freeFloat.setControlSize(soRenderArea, mainFrameSize)
    
  position = ctx.field("position").value
  margin = ctx.field("margin").value
  hwidth = int(mainFrameSize.width()/2)
  hheight = int(mainFrameSize.height()/2)
  
  if position == "TopLeft":
    freeFloat.setControlPos(soRenderArea1, margin + 1, margin + 1)
  elif position == "BottomRight":
    freeFloat.setControlPos(soRenderArea1, hwidth + margin, hheight + margin)
    
  freeFloat.setControlSize(soRenderArea1, hwidth - 2 * margin, hheight - 2 * margin)

  return

def update():
  mainFrameResizeEventSlot(None)