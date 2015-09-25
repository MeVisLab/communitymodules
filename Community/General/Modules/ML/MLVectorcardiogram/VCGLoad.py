#----------------------------------------------------------------------------------
#! ML module VCGLoad scripting
#/*!
# \file    VCGLoad.py
# \author  Axel Newe
# \date    2015-09-25
#
# Loads a vectorcardiogram (VCG) from file.
# */
#----------------------------------------------------------------------------------

from mevis import *

def InitModule():
  
  if (not ctx.field("autoLoad").value):
    ctx.field("numBeats").value = 1

  return
