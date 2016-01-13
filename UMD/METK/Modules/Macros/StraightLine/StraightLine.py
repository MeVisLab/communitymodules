from math import *

def init():
   ctx.field('Material.ambientColor').setStringValue('0.2 0.2 0.2')
   ctx.field('Material.emissiveColor').setStringValue('0 0 0')
   ctx.field('Material.specularColor').setStringValue('0 0 0')
   return
   
   
def calcLine(arg=0):
   startP = ctx.field("startPoint").value
   endP = ctx.field("endPoint").value   
   #dirVec = (startP[0]-endP[0],startP[1]-endP[1],startP[2]-endP[2])
   dirVec = (-startP[0]+endP[0],-startP[1]+endP[1],-startP[2]+endP[2])
   length = vecLength(dirVec)

   ctx.field("SoCylinder.height").value = length
   ctx.field("SoTranslation1.translation").setValue(0,length/2,0)
      
   yAxis=(0,1,0)
   cp1 = crossProduct(yAxis,dirVec)
   #print "cp1="+str(cp1[0])+","+str(cp1[1])+","+str(cp1[2])
   
   #alpha=arccos(a*b/|a|*|b|)
   alpha = acos(dotProduct(yAxis,dirVec)/( vecLength(yAxis)*vecLength(dirVec) ))   
   ctx.field("SoRotation1.rotation").setValue(cp1[0],cp1[1],cp1[2],alpha)
   ctx.field("SoTranslation2.translation").setValue(startP[0],startP[1],startP[2])
      
   return
   
   
   
def crossProduct(a,b):
   result = (a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0])
   return result
   
def dotProduct(a,b):
   result = a[0]*b[0]+a[1]*b[1]+a[2]*b[2]
   return result


def vecLength(a):
   result = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])
   return result