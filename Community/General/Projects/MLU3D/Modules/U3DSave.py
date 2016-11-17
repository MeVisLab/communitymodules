#----------------------------------------------------------------------------------
# ML module U3DSave definition
#
# \file    U3DSave.py
# \author  Axel Newe
# \date    2016-10-01
#----------------------------------------------------------------------------------

from mevis import *


def InitModule():
  
  ctx.field("status").value             = "Idle."
  ctx.field("networkPanelStatus").value = "Idle."
  ctx.field("progress").value           = 0
  ctx.field("success").value            = False
  
  return
