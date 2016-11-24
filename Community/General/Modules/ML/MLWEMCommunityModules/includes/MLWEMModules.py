from mevis import *

def setMouseCursor():
  if ctx.field("isProcessing").boolValue():
    MLAB.setWaitCursor()
  else:
    MLAB.removeWaitCursor()
  return