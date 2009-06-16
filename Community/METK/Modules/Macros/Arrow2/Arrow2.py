from math import sqrt

def init():
   ctx.field('Material.ambientColor').setStringValue('0.2 0.2 0.2')
   ctx.field('Material.emissiveColor').setStringValue('0 0 0')
   ctx.field('Material.specularColor').setStringValue('0 0 0')
   return
   
   
def calcArrow(arg=0):
   startP = ctx.field("startPoint").value
   endP = ctx.field("endPoint").value   
   arrowVec = (startP[0]-endP[0],startP[1]-endP[1],startP[2]-endP[2])
   length = sqrt(arrowVec[0]*arrowVec[0]+arrowVec[1]*arrowVec[1]+arrowVec[2]*arrowVec[2])
   
   #Scale length of vector to get space at start and end
   factor = ctx.field("lengthFactor").value
   newLength = length * factor
   
   diffLength = length-newLength
   
   #startP = (startP[0]+arrowVec[0]*(diffLength/2)/length,startP[1]+arrowVec[1]*(diffLength/2)/length,startP[2]+arrowVec[2]*(diffLength/2)/length)   
   #endP = (startP[0]-arrowVec[0]*ctx.field("lengthFactor").value,startP[1]-arrowVec[1]*ctx.field("lengthFactor").value,startP[2]-arrowVec[2]*ctx.field("lengthFactor").value)

   startP = (startP[0]-arrowVec[0]*(1-factor)/2,startP[1]-arrowVec[1]*(1-factor)/2,startP[2]-arrowVec[2]*(1-factor)/2)
   endP = (startP[0]-arrowVec[0]*factor,startP[1]-arrowVec[1]*factor,startP[2]-arrowVec[2]*factor)
   
   
   ctx.field("UMDSoArrow.position").value = startP
   ctx.field("UMDSoArrow.length").value = newLength
   ctx.field("UMDSoArrow.rotationRef").value = endP
   return