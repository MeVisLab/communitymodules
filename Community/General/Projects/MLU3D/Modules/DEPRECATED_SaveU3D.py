from mevis import *

def InitModule():
  
  ctx.field("status").value               = "Idle."
  ctx.field("U3DSave.progress").value     = 0
  ctx.field("U3DAssemble.progress").value = 0
  ctx.field("Calculator.exp1").touch()  # Trigger progress calculation
  
  return

def AddNewSpecification(callingField = None):
  
  newSpecObjectType = ctx.field("U3DSpecificationGenerator.newSpecType").value
  newSpec           = ctx.field("U3DSpecificationGenerator.newSpec").value
  
  if (newSpecObjectType == "OBJECTTYPE_POINTCLOUD"):
    oldSpec = ctx.field("pointCloudSpecification").value
    ctx.field("pointCloudSpecification").value = oldSpec + newSpec
    
  elif (newSpecObjectType == "OBJECTTYPE_LINESET_FROM_MARKERS"):
    oldSpec = ctx.field("lineSetSpecification").value
    ctx.field("lineSetSpecification").value = oldSpec + newSpec
    
  elif (newSpecObjectType == "OBJECTTYPE_LINESET_FROM_FIBERS"):
    oldSpec = ctx.field("lineSetSpecification").value
    ctx.field("lineSetSpecification").value = oldSpec + newSpec
    
  elif (newSpecObjectType == "OBJECTTYPE_MESH"):
    oldSpec = ctx.field("meshSpecification").value
    ctx.field("meshSpecification").value = oldSpec + newSpec

  elif (newSpecObjectType == "OBJECTTYPE_METADATA"):
    oldSpec = ctx.field("metaDataSpecification").value
    ctx.field("metaDataSpecification").value = oldSpec + newSpec

  return

def Save(callingField = None):

  ctx.field("status").connectFrom(ctx.field("U3DAssemble.status"))
  ctx.field("progress").connectFrom(ctx.field("U3DAssemble.progress"))  
  ctx.field("U3DAssemble.apply").touch()
  
  numGen = ctx.field("U3DInfo.numPointSets").value + ctx.field("U3DInfo.numLineSets").value + ctx.field("U3DInfo.numMeshes").value
  
  if (numGen > 0):
  
    ctx.field("status").connectFrom(ctx.field("U3DSave.status"))
    ctx.field("progress").connectFrom(ctx.field("U3DSave.progress"))
    
  ctx.field("U3DSave.apply").touch()

  return



