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
#----------------------------------------------------------------------------------

from mevis import *
import string

def updateTransformationFile():
  if (ctx.field("workingDirectory").value[-1] != '/'):
    ctx.field("workingDirectory").value += '/';
  if (ctx.field("autoUpdateTransformationFile").value):
    ctx.field("FileDirectory.update").touch();
    files = ctx.field("FileDirectory.fileList").value;
    files = files.split('\n');    
    ctx.field("transformationFile").value = files[-1];    
    
def updateModule():
  ctx.field("StringUtils2.string2").value = ctx.field("transformationFile").value;
  args = ctx.field("StringUtils2.result").value;
  if ctx.field("createDeformationField").value:
    args += " -def all";
  if ctx.field("createJacobianField").value:
    args += " -jac all";
  if ctx.field("createJacobianMatrixField").value:
    args += " -jacmat all";
  
  ctx.field("CallApplication.arguments").value = args;
  
  ctx.field("itkImageFileWriter.save").touch();
    
  ctx.field("itkImageFileReader.close").touch();
  ctx.field("itkImageFileReader1.close").touch();
  ctx.field("itkImageFileReader2.close").touch();
  ctx.field("itkImageFileReader3.close").touch();
  
  MLABFileManager.mkdir(ctx.field("workingDirectory").value + "Transformix/");
  ctx.field("CallApplication.call").touch();
  
  ctx.field("itkImageFileReader.unresolvedFileName").value = ctx.field("workingDirectory").value + "Transformix/result.mhd";
  if ctx.field("createDeformationField").value:
    ctx.field("itkImageFileReader1.unresolvedFileName").value = ctx.field("workingDirectory").value + "Transformix/deformationField.mhd";  
  if ctx.field("createJacobianField").value:
    ctx.field("itkImageFileReader2.unresolvedFileName").value = ctx.field("workingDirectory").value + "Transformix/spatialJacobian.mhd";  
  if ctx.field("createJacobianMatrixField").value:
    ctx.field("itkImageFileReader3.unresolvedFileName").value = ctx.field("workingDirectory").value + "Transformix/fullSpatialJacobian.mhd";  
    
def deleteFiles():
  ctx.field("itkImageFileReader.close").touch();
  ctx.field("itkImageFileReader1.close").touch();
  ctx.field("itkImageFileReader2.close").touch();
  ctx.field("itkImageFileReader3.close").touch();
  if (MLABFileManager.exists( ctx.field("itkImageFileReader.fileName").value )):
    deleteMhd(ctx.field("itkImageFileReader.fileName").value);
  if (MLABFileManager.exists( ctx.field("itkImageFileReader1.fileName").value )):
    deleteMhd(ctx.field("itkImageFileReader1.fileName").value);
  if (MLABFileManager.exists( ctx.field("itkImageFileReader2.fileName").value )):
    deleteMhd(ctx.field("itkImageFileReader2.fileName").value);
  if (MLABFileManager.exists( ctx.field("itkImageFileReader3.fileName").value )):
    deleteMhd(ctx.field("itkImageFileReader3.fileName").value);
  MLABFileManager.rmdir(ctx.field("workingDirectory").value + "Transformix/");

def deleteMhd(fn):
  mhd = fn;
  raw = mhd.replace(".mhd", ".raw");
  zraw = mhd.replace(".mhd", ".zraw");
  MLABFileManager.remove( mhd );
  MLABFileManager.remove( raw );  
  MLABFileManager.remove( zraw );
  