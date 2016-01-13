# Python module import
from ObjMgr import *
from linear import *
 

def calcBBString(fieldP = 0):
   ctx.field("BoundingBox_String").value = str(ctx.field("BoundingBox_min").value[0]) + ' ' + str(ctx.field("BoundingBox_min").value[1]) + ' ' + str(ctx.field("BoundingBox_min").value[2]) + ' 0 0 0 ' + str(ctx.field("BoundingBox_max").value[0]) + ' ' + str(ctx.field("BoundingBox_max").value[1]) + ' ' + str(ctx.field("BoundingBox_max").value[2]) + ' 0 0 0'   
   return   