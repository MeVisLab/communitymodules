# Python module import
from mevis import *


def init(againstTypeError = 0):
   global liste
   liste = []
   return


def pushListe(againstTypeError = 0):
   liste.append(ctx.field("input").value)
   ctx.field("isEmpty").value = False
   ctx.field("numEntries").value = len(liste)
   return


def popListe(againstTypeError = 0):
   if len(liste) > 0:
      if ctx.field("listType").value == "FIFO":
         ctx.field("output").setStringValue(liste.pop())
      else:
         ctx.field("output").setStringValue(liste.pop(0))
   if len(liste) == 0:
      ctx.field("isEmpty").value = True
   ctx.field("numEntries").value = len(liste)
   return
