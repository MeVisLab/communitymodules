#----------------------------------------------------------------------------------
#
# Copyright (c) 2011, Image Sciences Institute, UMC Utrecht.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of ISI nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ISI BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from mevis import *

import os.path
import os
import glob
import string

def initialize():
  ctx.field("itkImageFileReader1.close").touch();
  ctx.field("itkImageFileReader.close").touch();
  ctx.field("itkImageFileReader2.close").touch();
  ctx.field("itkImageFileReader3.close").touch();
  
def runElastix():
  # save the input files
  dir = ctx.field("temporaryDirectory").value;
  MLABFileManager.mkdir(dir);
  fixed = dir + "fixed.mhd";
  moving = dir + "moving.mhd";
  fixedmask = dir + "fixedmask.mhd";
  movingmask = dir + "movingmask.mhd";
  #ctx.field("mhdSave.filename").value = fixed;
  #ctx.field("mhdSave1.filename").value = moving;
  #ctx.field("mhdSave2.filename").value = fixedmask;
  #ctx.field("mhdSave3.filename").value = movingmask;
  ctx.field("itkImageFileWriter.unresolvedFileName").value = fixed;
  ctx.field("itkImageFileWriter1.unresolvedFileName").value = moving;
  ctx.field("itkImageFileWriter2.unresolvedFileName").value = fixedmask;
  ctx.field("itkImageFileWriter3.unresolvedFileName").value = movingmask;
  incomplete  = False;
  if ctx.field("fixedImage").isConnected() :
    print "Writing " + fixed;
    #ctx.field("mhdSave.write").touch();
    ctx.field("itkImageFileWriter.save").touch();
    incomplete = True ;    
  if ctx.field("movingImage").isConnected() :
    print "Writing " + moving;
    #ctx.field("mhdSave1.write").touch();
    ctx.field("itkImageFileWriter1.save").touch();
    incomplete = True ;
  if ctx.field("fixedMask").isConnected() :
    print "Writing " + fixedmask;
    #ctx.field("mhdSave2.write").touch();
    ctx.field("itkImageFileWriter2.save").touch();
  if ctx.field("movingMask").isConnected() :
    print "Writing " + movingmask;
    #ctx.field("mhdSave3.write").touch();
    ctx.field("itkImageFileWriter3.save").touch();
  
  # set the filenames correctly in the CallElastix module
  if ctx.field("fixedMask").isConnected() :
    ctx.field("CallElastix.fixedMask").value = fixedmask;
  else :
    ctx.field("CallElastix.fixedMask").value = "";
  if ctx.field("movingMask").isConnected() :
    ctx.field("CallElastix.movingMask").value = movingmask;
  else :
    ctx.field("CallElastix.movingMask").value = "";
  ctx.field("CallElastix.movingImage").value = moving;
  ctx.field("CallElastix.fixedImage").value = fixed;
  ctx.field("CallElastix.outputDirectory").value = dir;
  
  #close the itkImageFileReader 
  ctx.field("itkImageFileReader1.close").touch();
  ctx.field("itkImageFileReader.close").touch();
  ctx.field("itkImageFileReader2.close").touch();
  ctx.field("itkImageFileReader3.close").touch();
  
  # touch the call button
  ctx.field("CallElastix.callElastix").touch();
  ctx.field("status").value = ctx.field("CallElastix.status").value;
  if ctx.field("succeeded").value:  
    # set the correct result filename in the mhdLoad
    if len(ctx.field("CallElastix.parameterFile3").value) > 0:
      result = dir + "result.2.mhd";
    elif len(ctx.field("CallElastix.parameterFile2").value) > 0:
      result = dir + "result.1.mhd";
    else :
      result = dir + "result.0.mhd";
    #result = dir + "/result.mhd";
    if (MLABFileManager.exists(result)):
      ctx.field("itkImageFileReader1.unresolvedFileName").value = result; 
    
    deffield = dir + "deformationField.mhd";
    jacfield = dir + "spatialJacobian.mhd";
    jacMatfield = dir + "fullSpatialJacobian.mhd";
    if (MLABFileManager.exists(deffield)):
      if ctx.field("createDeformationField").value:
        #ctx.field("mhdVectorLoad.mhdLoad.filename").value = deffield;
        ctx.field("itkImageFileReader.unresolvedFileName").value = deffield;
    if (MLABFileManager.exists(jacfield)):
      if ctx.field("createJacobianImage").value:
        ctx.field("itkImageFileReader2.unresolvedFileName").value = jacfield;
    if (MLABFileManager.exists(jacMatfield)):
      if ctx.field("createJacobianMatrixImage").value:
        ctx.field("itkImageFileReader3.unresolvedFileName").value = jacMatfield;
      
  # delete the input files
  if ctx.field("removeIntermediateFiles").value :
    removeMhd(fixed);
    removeMhd(moving);
    if ctx.field("movingMask").isConnected() :
      removeMhd(movingmask);
    if ctx.field("fixedMask").isConnected() :
      removeMhd(fixedmask);
  return;
  
def removeMhd(name):
  if MLABFileManager.exists(name):
    os.remove(name);
  s = name.replace(".mhd", ".raw");
  if MLABFileManager.exists(s):
    os.remove(s);
  else:
    s = name.replace(".mhd", ".zraw");
    if MLABFileManager.exists(s):
      os.remove(s);
  return;
  
def checkTemporaryDirectory():
  if str(ctx.field("temporaryDirectory").value).find(' ') != -1:
    print "Error: path contains spaces!";
  if (len(ctx.field("temporaryDirectory").value) > 0 and ctx.field("temporaryDirectory").value[-1] != '/'):
    ctx.field("temporaryDirectory").value += '/';