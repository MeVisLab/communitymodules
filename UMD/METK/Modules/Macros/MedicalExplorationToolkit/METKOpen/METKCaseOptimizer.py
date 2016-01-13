# Python module import
from os import *
from os.path import *

def getUnusedFiles(fieldP=0):
   myPath = dirname(ctx.field("fileName").value)
   print myPath
   
   for fileinfolder in listdir(myPath):
      #print fileinfolder
      #print splitext(fileinfolder)[1]
      if (splitext(fileinfolder)[1]==".dcm"):
         if (checkExistence(fileinfolder)==0):
            print "This file is not used:"
            print fileinfolder
            
            print "Delete " + myPath+"/"+fileinfolder
            remove(myPath+"/"+fileinfolder)
            print "Delete " + myPath+"/"+splitext(fileinfolder)[0]+".tif"
            remove(myPath+"/"+splitext(fileinfolder)[0]+".tif")                        
         else:
            print "USED: "+fileinfolder                         
   return


def checkExistence(filename):
   f = file(ctx.field("fileName").value,'r')
   line = f.readline()
   for line in f:
      if (line.find(filename)!=-1):
         return 1
      line = f.readline()      
   return 0