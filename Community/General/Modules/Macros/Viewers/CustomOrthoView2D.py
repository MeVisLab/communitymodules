

#-----------------------------------------------------------------------------
#
# \file    OrthoView2D.py
# \author  Alexander Broersen
# \date    11/2016
#
#-----------------------------------------------------------------------------

def inventorInputOnChanged(field):
  flag = not field.boolValue()
  ctx.field("inInvPreLUT").setHidden(flag)
  ctx.field("inInvPostLUT").setHidden(flag)
  ctx.field("inInvPostViewer").setHidden(flag)
  ctx.updateLayout()

def toggleSynchPosition():
  synchPosOn = ctx.field("synchronizePosition").boolValue()
  if synchPosOn:
    ctx.field("view.worldPosition").connectFrom(ctx.field("pos.worldPosition"))
  else:
    ctx.field("view.worldPosition").disconnect()
