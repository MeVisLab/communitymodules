from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import * #for MLABFileDialog

# Global class instances
_cls_info = None

def init():
   # set default local path
   ctx.field("modulePath").value = normcase(relPath(ctx.field("modulePathTemp").value+'/','../../'))
   return


def loadScriptFile(againstTError = 0):   
   ctx.field("ScriptArea").setStringValue("")

   scriptFile = MLABFileDialog.getOpenFileName("","Script Files (*.txt)", "Open Case File");   
      
   print scriptFile
   if (isfile(scriptFile)==1):
      f = file(scriptFile,'r')
      for line in f:
         ctx.field("ScriptArea").setStringValue(ctx.field("ScriptArea").value+line)
   else:
      print "File not found"
   return
   

# frag mich mal keiner was dieser mist hier mit der url soll ... 
# aber das war die einzige möglichkeit, aus einem absoluten pfad (c:/aaa/bbb/ccc/) und einer relativen Angabe wie "../" etwas zu zaubern   
def relPath(orgpath, relative):
   (scheme, netloc, path, query, fragment) = urlsplit(urljoin("http://netloc/"+orgpath, relative),"http://",0)   
   return lstrip(path,"/")