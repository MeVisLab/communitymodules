# Python module import
# from ObjMgr import *
# from HNOdefinitions import *

# Global class instances
# _cls_info= None


def init():
   # global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   # _cls_info= ObjInfo(ctx.module("info"))

   ctx.field("Corner1").value = (0.0,0.0,0.0)
   ctx.field("Corner2").value = (1.0,1.0,1.0)
   return


## Documentation for a method.
#  @param self The object pointer.
def setBox(againstTError = 0):   
   corner1 = ctx.field("Corner1").value
   corner2 = ctx.field("Corner2").value
   ctx.field("SoTransform.translation").value = ((corner2[0] - corner1[0])/2.0 + corner1[0], (corner2[1] - corner1[1])/2.0 + corner1[1], (corner2[2] - corner1[2])/2.0 + corner1[2])
   ctx.field("SoCube.width").value = abs(corner2[0] - corner1[0])
   ctx.field("SoCube.height").value = abs(corner2[1] - corner1[1])
   ctx.field("SoCube.depth").value = abs(corner2[2] - corner1[2])

   return