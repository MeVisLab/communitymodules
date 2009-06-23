from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import * #for MLABFileDialog


# Global class instances
_cls_info = None


def init():
   # set default local path   
   ctx.field("fldLocalPath").value = ctx.field("modulePathTemp").value   
   #ctx.field("fldFilename").touch()
   return