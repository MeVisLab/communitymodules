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
#! Macro module UMDAnonymizer
#/*!
# \file    UMDAnonymizer.py
# \author  Konrad Mühler
# \date    2008-03-03
#
# 
#*/
#----------------------------------------------------------------------------------

from mevis import *

dateiliste = []

def updateDir(againstTypeError = 0):
   # alle Werte zurücksetzen
   ctx.field("liste").setValue("")
   ctx.field("toggleString").setValue("")
   
   manager = MLABFileManager
   dateiliste[:] = manager.contentOfDirectory(ctx.field("directory").value, "*.dcm")
   combinedFiles = "Filename"
   for eintrag in dateiliste:
      combinedFiles = combinedFiles + "?" + eintrag
   ctx.field("liste").setValue(combinedFiles)
   return


def start(againstTypeError = 0):
   for counter in range(len(dateiliste)):
      if ctx.field("toggleString").value[counter] == "1":
         filename = ctx.field("directory").value + "/" + dateiliste[counter]
         print "filename="+filename
         ctx.field("ImgLoad.filename").value = filename         
         ctx.field("FileInformation.dirname").value=ctx.field("FileInformation.dirname").value+"anonym/"         
      ctx.field("ImgSave.save").touch()
   return


def invertSelection(againstTypeError = 0):
   toggle = ""
   for counter in range(len(ctx.field("toggleString").value)):
      if ctx.field("toggleString").value[counter] == "1":
         toggle = toggle + "0"
      else:
         toggle = toggle + "1"
   ctx.field("toggleString").value = toggle
   return