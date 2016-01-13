# Python module import
from mevis import *

dateiliste = []

def init(againstTypeError = 0):
   return


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
         ctx.field("GenerateIVFile.filename").value = filename
   ctx.field("GenerateIVFile.start").touch()
   return


def updateProgress(againstTypeError = 0):
   if len(dateiliste):
      ctx.field("progress").value = 1.0 - ((1.0 * ctx.field("GenerateIVFile.Queue.numEntries").value) / (1.0 * len(dateiliste)))
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