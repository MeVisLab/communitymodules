#----------------------------------------------------------------------------------
# ML module SaveU3D definition
#
# \file    DEPRECATED_SaveU3D.py
# \author  Axel Newe
# \date    2016-10-01
#----------------------------------------------------------------------------------

from mevis import *


def InitModule():
  
  ctx.field("status").value               = "Idle."
  ctx.field("U3DSave.progress").value     = 0
  ctx.field("U3DAssemble.progress").value = 0
  ctx.field("Calculator.exp1").touch()  # Trigger calculation of progress
  
  return


def AddNewSpecification(callingField = None):
  
  newSpecificationObjectType = ctx.field("U3DSpecificationGenerator.newSpecificationType").value
  newSpecification           = ctx.field("U3DSpecificationGenerator.newSpecification").value
  
  if (newSpecificationObjectType == "OBJECTTYPE_POINTCLOUD"):
    oldSpecification = ctx.field("pointCloudSpecification").value
    ctx.field("pointCloudSpecification").value = oldSpecification + newSpecification
    
  elif (newSpecificationObjectType == "OBJECTTYPE_LINESET_FROM_MARKERS"):
    oldSpecification = ctx.field("lineSetSpecification").value
    ctx.field("lineSetSpecification").value = oldSpecification + newSpecification
    
  elif (newSpecificationObjectType == "OBJECTTYPE_LINESET_FROM_FIBERS"):
    oldSpecification = ctx.field("lineSetSpecification").value
    ctx.field("lineSetSpecification").value = oldSpecification + newSpecification
    
  elif (newSpecificationObjectType == "OBJECTTYPE_MESH"):
    oldSpecification = ctx.field("meshSpecification").value
    ctx.field("meshSpecification").value = oldSpecification + newSpecification

  elif (newSpecificationObjectType == "OBJECTTYPE_METADATA"):
    oldSpecification = ctx.field("metaDataSpecification").value
    ctx.field("metaDataSpecification").value = oldSpecification + newSpecification

  return


def Save(callingField = None):

  ctx.field("status").connectFrom(ctx.field("U3DAssemble.status"))
  ctx.field("progress").connectFrom(ctx.field("U3DAssemble.progress"))  
  ctx.field("U3DAssemble.apply").touch()
  
  numGeometryGenerators = ctx.field("U3DInfo.numPointSets").value + ctx.field("U3DInfo.numLineSets").value + ctx.field("U3DInfo.numMeshes").value
  
  if (numGeometryGenerators > 0):
  
    ctx.field("status").connectFrom(ctx.field("U3DSave.status"))
    ctx.field("progress").connectFrom(ctx.field("U3DSave.progress"))
    
  ctx.field("U3DSave.apply").touch()
  
    
  



  return



