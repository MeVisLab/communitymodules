def init():
   switchOnViewerStatus(0)
   return

def switchOnViewerStatus(againstTError = 0):
   if (ctx.field("ProtocolViewerStatus").boolValue()):
      ctx.field("SoSwitch1.whichChild").value = 0
      ctx.field("SoSwitch.whichChild").value = 1
   else:   
      ctx.field("SoSwitch1.whichChild").value = -1
      ctx.field("SoSwitch.whichChild").value = 0     
   return