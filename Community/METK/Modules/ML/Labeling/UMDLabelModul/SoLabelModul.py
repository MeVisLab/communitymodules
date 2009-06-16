# Python module import


def preObject(fieldP=0):
   curObj = ctx.field("curObject").value
   if curObj > 0:
      ctx.field("curObject").value = curObj -1
   return

def sucObject(fieldP=0):
   curObj = ctx.field("curObject").value 
   maxObj = ctx.field("numberShapes").value
   if curObj < maxObj:
     ctx.field("curObject").value=curObj = curObj +1
   return

