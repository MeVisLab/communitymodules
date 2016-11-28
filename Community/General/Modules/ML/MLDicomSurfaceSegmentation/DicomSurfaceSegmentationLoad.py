#----------------------------------------------------------------------------------
# DicomSurfaceSegmentationLoad module definitions.
#
# \file    DicomSurfaceSegmentationLoad.py
# \author  Axel Newe
# \date    2016-08-24
#----------------------------------------------------------------------------------

from mevis import *


def WakeupModule():
  
  if not (ctx.field("autoApply").value):
    ctx.field("unload").touch()
    
  return


def InitWindow():

  ctx.field("numPointSets").touch()
  ctx.field("numLineSets").touch()
  ctx.field("numMeshes").touch()
  
  return
