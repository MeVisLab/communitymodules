# -----------------------------------------------------------------------
# 
# (c) Copyright 2001-2008, MeVis Research GmbH, Bremen, Germany
# ALL RIGHTS RESERVED
# 
# THIS FILE CONTAINS CONFIDENTIAL AND PROPRIETARY INFORMATION OF MEVIS 
# RESEARCH GMBH. ANY DUPLICATION, MODIFICATION, DISTRIBUTION, OR 
# DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY PROHIBITED 
# WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS RESEARCH GMBH.
# 
#----------------------------------------------------------------------------------
#! Macro module UMDCampathMacro
#/*!
# \file    UMDCampathMacro.py
# \author  Marcus Wolff
# \date    2008-02-26
#
# Campathdemonstrator
#*/
#----------------------------------------------------------------------------------

from mevis import *
from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import *
from VisDefinitions import *

_maxInt = 16777215

_cls_info = None
_mainWin = None

def init():
   global _cls_info
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)   
   return


def clearScriptArea(field = 0):
  ctx.field("UMDAnimation2.ScriptArea").setStringValue("")
  return;

def loadScriptFile(againstTError = 0):   
   clearScriptArea()
   scriptFile = MLABFileDialog.getOpenFileName("","Script Files (*.txt)", "Open Case File");   
   ctx.field("LastScriptFile").value = scriptFile
   loadScriptFileString(scriptFile)
   return;
   

def loadScriptFileString(scriptFile):
   if (isfile(scriptFile)==1):
      f = file(scriptFile,'r')
      for line in f:
         ctx.field("UMDAnimation2.ScriptArea").setStringValue(ctx.field("UMDAnimation2.ScriptArea").value+line)
   else:
      print "File not found"
   return


def cleanUpDebugPath(field = 0):
  ctx.field("DebugCameraPath.cleanup").touch()
  return


def openCase(field = 0):   
   #ctx.field("METKScriptInit.scriptFile").value = ctx.field("baseScriptPath").value + "/" + ctx.field("caseType").value + "/init-script.txt"
   
   ctx.field("METKManager.cleanup").touch()
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
     ctx.field("LastXMLFile").value = caseFile
     ctx.field("METKManager.fileName").value = caseFile
     ctx.field("METKManager.load").touch()
     _mainWin.setTitle("UMDCampathDemonstrator | " + caseFile)
   return


# frag mich mal keiner was dieser mist hier mit der url soll ... 
# aber das war die einzige möglichkeit, aus einem absoluten pfad (c:/aaa/bbb/ccc/) und einer relativen Angabe wie "../" etwas zu zaubern   
def relPath(orgpath, relative):
   (scheme, netloc, path, query, fragment) = urlsplit(urljoin("http://netloc/"+orgpath, relative),"http://",0)   
   return lstrip(path,"/")


def loadUsedXMLFile(field = 0):
  ctx.field("METKManager.fileName").value=ctx.field("LastXMLFile").value
  ctx.field("METKManager.load").touch()
  return


def loadUsedScriptFile(field = 0):
  clearScriptArea()
  loadScriptFileString(ctx.field("LastScriptFile").value)
  return

def loadUsedFiles(field = 0):
  loadUsedScriptFile()
  loadUsedXMLFile()
  return

def initWindow(args=0):
   global _mainWin
   _mainWin = ctx.window() 
   _mainWin.setTitle("UMDCampathDemonstrator")
   return

def setSphereField(fieldP = 0):
   ctx.field("METKCalcCamPath.showField").value = int(ctx.field("sphereField").value)   
   ctx.field("SoExaminerViewer1.viewAll").touch()
   return

def startAnimation(args=0):
   ctx.field("UMDAnimation2.ScriptAreaExecute").touch()
   setSphereField()
   ctx.field("SoExaminerViewer1.viewAll").touch()
   return

    
# MeVis signature v1
# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
# owner: MeVis Internal Developer
# date: 2008-01-30T03:14:08
# hash: J07FZP7/dcyEDqdn1UaEY59P+BkNJHcXRegtWMUdij9hj9oI9kz9OYDT9Kr/k0yMSYmZj4wbqdz6CaUOgd3vow==
# MeVis end

