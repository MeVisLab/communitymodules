

#-----------------------------------------------------------------------------
#
# \file    SoThresholdToPointSet.py
# \author  Alexander Broersen
# \date    11/2016
#
#-----------------------------------------------------------------------------

def init():
  # int color
  ctx.field("colorFlag").setBoolValue(False)
  ctx.field("setColor").setBoolValue(True)

def listenColorFlag(field):
  flag = field.value
  ctx.field("setColor").setBoolValue(not flag)
