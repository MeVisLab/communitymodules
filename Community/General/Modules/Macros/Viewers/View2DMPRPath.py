#----------------------------------------------------------------------------------
# 
# Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
# Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of BIGR nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------
 

from mevis import *

def SetPathInput( field = None ):
  if ( field.getMLBaseType() == 'XMarkerList') :
    ctx.field("MPRPath.frames").connectFrom( ctx.field("PathToKeyFrame.outputKeys") )
    ctx.field("PathToKeyFrame.inputKeys").connectFrom( ctx.field("path") )
  else :
    ctx.field("MPRPath.frames").connectFrom( ctx.field("path") )
    ctx.field("PathToKeyFrame.inputKeys").disconnectAll()
  return
    
def ShowOutputs( field ):
  flag = not field.boolValue()
  ctx.field("mprSliceImage").setHidden(flag)
  ctx.field("stack").setHidden(flag)
  ctx.field("cmpr").setHidden(flag)
  ctx.field("self").setHidden(flag)
  ctx.field("scene").setHidden(flag)
  ctx.updateLayout();
  return 

def inventorInputOnChanged(field):
  flag = not field.boolValue();
  ctx.field("inInvPreLUT").setHidden(flag);
  ctx.field("inInvPostLUT").setHidden(flag);
  ctx.field("inInvPostViewer").setHidden(flag);
  ctx.updateLayout();
  return 

def keyPressed(event) :
  if event['key'] == 'S':
    ctx.showWindow('Settings')
  #ctx.control("label2").setTitle("\"" + event.key + "\" pressed<br>ascii: " + event.ascii + "<br>ctrl "+event.ctrlKey + "<br>alt "+event.altKey + "<br>ctrl "+event.shiftKey);
  return 

def ChangeView( viewContent ):
  switch = ctx.field("Switch.currentInput")
  if viewContent.value == "MPRSlice" : switch.value = 0
  elif viewContent.value == "CMPR" : switch.value = 1
  else : switch.value = 2
  return 


#//# MeVis signature v1
#//# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBALMoAKeDufSkjPLfaCUd7Ij4IgEsndoDH9mP+jpEXKnAczgkSCgtNyNEMyiLur8xV1zEN7f71aeTOVWVntbzpucCARE=:lD+X/cPXp4xBkg/BH8EhyVWLOXzUCvL/ccrRKrYcyKMt2wR4QiXj1OCsqQukghRS1dwd5fRaB39vHgPZUYpdyA==
#//# owner: EMC
#//# date: 2010-03-02T15:34:50
#//# hash: cB3d1viYgaLpMFXKrl1NMTfRtOUmI1VY08ACQ4xD22tvNL/V6CmMFZzxnIfedkAlRoxho4FE6lW1ulQnf0Qomw==
#//# MeVis end
