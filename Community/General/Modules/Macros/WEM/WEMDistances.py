#----------------------------------------------------------------------------------
#! Macro module WEMDistances
#/*!
# \file    WEMDistances.py
# \author  Reinhard
# \date    2011-10-03
#
# Calculate some distance measures on WEMs
# */
#----------------------------------------------------------------------------------

from mevis import *

def Update():
  ctx.field("symmetricMin").value  = ( ctx.field("min01").value  + ctx.field("min10").value  )/2
  ctx.field("symmetricMean").value = ( ctx.field("mean01").value + ctx.field("mean10").value )/2
  ctx.field("symmetricMax").value  = ( ctx.field("max01").value  + ctx.field("max10").value  )/2
  ctx.field("hausdorff").value     = max( ctx.field("max01").value, ctx.field("max10").value  )
  return

#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBANEfsmYse2e1dRhkQ9AQbreCq9uxwzWLoGom13MNYmyfwoJqQOEXljLFAgw2eEjaT12G4CdqKWhRxh9ANP6n7GMCARE=:VI/mB8bT4u+mRtf/ru8yUQi8BzpaS3UeL2x62YxsUYnVqCWuLrVNLiukIIjnJMKQXlc8ezmgOIcVAV7pgvgKpQ==
#//# owner: MeVis
#//# date: 2011-09-03T10:18:33
#//# hash: weWl3qiFU5ceUmbFn9p9xqu/Ob8VqQnw2g9FJNivuXAw3W2CYRmKmSePsDHcRghebwpnlQjwl9LhzloxgzMANQ==
#//# MeVis end