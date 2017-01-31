# **InsertLicense** code
#  _______________________________________________________________________
#  __________________  F R A U N H O F E R   M E V I S  __________________
# 
#  |
#  |
#  |   Description:
#  |   ...
#  |
#  |   Author(s)      : Felix Ritter
#  |   Creation date  : 02/2010
#  |
#  __________________  F R A U N H O F E R   M E V I S  __________________
#  _______________________________________________________________________

# Python module import
from __future__ import print_function
from mevis import *

def init():
  return

def seriesDropped(info):
  ctx.field('import.inputFileList').value = '\n'.join(info['series'])
  MLAB.processInventorQueue()
  ctx.field('view2d.startSlice').value = info['curImage']
  ctx.control('bridge').setWindowFilePath(info['series'][0])
  ctx.field('lut.greyCenter').value = info['curImageWL']
  ctx.field('lut.greyWidth').value = info['curImageWW']
  return
