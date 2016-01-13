# Python module import
from ObjMgr import *
from VisDefinitions import *
from os import *
from os.path import *

# Global class instances
global fileIterator
global fileIteratorCounter
global fileIteratorLen

def init():
   global fileIterator
   global fileIteratorLen
   global fileIteratorCounter
   fileIterator = 0
   fileIteratorLen = 0
   fileIteratorCounter = 0
   return


def convert(args):
   global fileIterator
   global fileIteratorLen
   fileIterator = iter(listdir(ctx.field("directory").value))
   fileIteratorLen = len(listdir(ctx.field("directory").value))
   #print "len:" + str(fileIteratorLen)
   fileIteratorCounter = 0
   nextFile(0)
   return
   

def nextFile(args):
   global fileIteratorCounter,fileIterator
   #print "nextFile   saveStatus="+ctx.field("ImgSave.status").value
   stop=False
   while stop==False:
      fileName = fileIterator.next()
      #print "filename = " + fileName
      fileIteratorCounter = fileIteratorCounter + 1  
      if fileIteratorCounter>=fileIteratorLen:
         stop=True
      if (fileName==""):
         stop=True
      else:        
         (fileBaseName, fileExtension) = splitext(fileName)
         if fileExtension==".tif":
            stop=True               
      
   #print "directory = " + ctx.field("directory").value      
   if fileName!="" and fileIteratorCounter<fileIteratorLen:
      #print "load "+ctx.field("directory").value + "/" + fileName      
      ctx.field("ImgLoad.filename").value = ctx.field("directory").value + "/" + fileName      
      ctx.field("ImgLoad.load").touch()
   else:
      print "ready"
   return
   
def saveFile(args):
   #print "saveFile   ImgLoad.status=" + ctx.field("ImgLoad.status").value
   global fileIteratorCounter,fileIterator
   if ctx.field("ImgLoad.status").value == "File open":
      ctx.field("ImgLoad.status").value = ""
      fileNameOld = ctx.field("ImgLoad.filename").value
      #print "fileNameOld="+fileNameOld
      (pathOld,fileOld) = split(fileNameOld)
      pathNew = pathOld + "/converted/"
      if not exists(pathNew):
         mkdir(pathNew)
      ctx.field("ImgSave.filename").value = pathNew + fileOld
      print "saving " + ctx.field("ImgSave.filename").value
      ctx.field("ImgSave.save").touch()
   return
   

def fileSaved(args):
   #print "fileSaved   status = " + ctx.field("ImgSave.status").value 
   if ctx.field("ImgSave.status").value == "Image saved":
      ctx.field("ImgSave.status").value = ""
      #print "saved"
      nextFile(0)
   return