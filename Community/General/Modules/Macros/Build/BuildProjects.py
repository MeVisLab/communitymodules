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
import string;
import codecs

def init():
  ctx.field("directory").value = MLABPackageManager.packageByIdentifier("ISI/General").path().replace("/General", "")
  directoryChanged();
  ctx.field("devenvExecutable").value = MLABFileManager.cleanupPath(MLAB.getenv("VS" + MLAB.compilerInfo()[2:MLAB.compilerInfo().find("_")] + "0COMNTOOLS").replace("Tools\\", "IDE\\devenv.exe"))
  ctx.field("projectFiles").value = "Create";
  ctx.field("debug").value = "Build";
  ctx.field("release").value = "Build";

def directoryChanged():
  ctx.field("proFiles.update").touch();
  return;
  
def runBatch():
  ctx.field("buildDebug.executable").value = ctx.field("devenvExecutable").value;
  ctx.field("buildRelease.executable").value = ctx.field("devenvExecutable").value;
  buildResults = [];
  noBatchFile = [];
  noProjectFile = [];
  ctx.field("progress").value =  0.0;
  for i in range(0, ctx.field("projectIter.numValues").value, 1):
    ctx.field("projectIter.curIndex").value = i;
    projFile = ctx.field("projectIter.curValue").value; projFile = projFile.replace('.pro', '.vcproj');
    basename = ctx.field("FileInformation.basename").value;
    if ctx.field("projectFiles").value == "Recreate":
      if MLABFileManager.exists(ctx.field("StringUtils2.result").value): #project batch file
        ctx.field("callBAT.call").touch();
      else:
        print "Error on project " + basename + ": Could not find batch file " + ctx.field("StringUtils2.result").value;
        noBatchFile += [basename];
    elif ctx.field("projectFiles").value == "Create":
      if not MLABFileManager.exists(projFile):
        #print "proj file does not exist: call bat file"
        if MLABFileManager.exists(ctx.field("StringUtils2.result").value): #project batch file
          ctx.field("callBAT.call").touch();
        else:
          print "Error on project " + basename + ": Could not find batch file " + ctx.field("StringUtils2.result").value;
      elif MLABFileManager.getLastModifiedDate(ctx.field("projectIter.curValue").value) > MLABFileManager.getLastModifiedDate(projFile):
        #print "pro file is newer: call bat file"
        ctx.field("callBAT.call").touch();
    if MLABFileManager.exists(projFile): #visual studio project file
      if ctx.field("debug").value <> "Skip":
        ctx.field("StringUtils3.string2").value = ctx.field("debug").value;
        ctx.field("buildDebug.call").touch();
        if ctx.field("debug").value <> "Clear":
          ret = checkBuildSuccessful("Debug");
          buildResults += [ret];
      if ctx.field("release").value <> "Skip":
        ctx.field("StringUtils.string2").value = ctx.field("release").value;
        ctx.field("buildRelease.call").touch();
        if ctx.field("release").value <> "Clear":
          ret = checkBuildSuccessful("Release");
          buildResults += [ret];
    else:
      print "Error on project " + basename + ": Could not find vcproj file " + projFile;
      noProjectFile += [basename];
    ctx.field("progress").value = (i+1.0)/ctx.field("projectIter.numValues").value;
  #print buildResults;
  debugWithErrors = [];
  releaseWithErrors = [];
  debugWithWarnings = [];
  releaseWithWarnings = [];
  for b in buildResults:
    if len(b)>0:
      if b[1] == 'Debug' and b[2] > 0:
        debugWithErrors += [b[0]];
      elif b[1] == 'Release' and b[2] > 0:
        releaseWithErrors += [b[0]];
      if b[1] == 'Debug' and b[3] > 0:
        debugWithWarnings += [b[0]];
      elif b[1] == 'Release' and b[3] > 0:
        releaseWithWarnings += [b[0]];
  print "Build batch completed";
  if ctx.field("projectFiles").value == "Create" or ctx.field("projectFiles").value == "Recreate":
    print "--- Project files ----";
    print "The following projects had no batch file (", len(noBatchFile), "):";
    if len(noBatchFile)==0:
      print "<none>";
    else:
      for p in noBatchFile:
        print p;
    print "------------------------\n";
  if ctx.field("debug").value == "Build" or ctx.field("debug").value == "Rebuild" or ctx.field("release").value == "Build" or ctx.field("release").value == "Rebuild":
    print "-- VCProject files ---";
    print "The following projects had no vcproj file (", len(noProjectFile), "):";
    if len(noProjectFile)==0:
      print "<none>";
    else:
      for p in noProjectFile:
        print p;
    print "------------------------\n";
  if ctx.field("debug").value == "Build" or ctx.field("debug").value == "Rebuild":
    print "------- Debug --------";
    print "The following projects had errors (", len(debugWithErrors), "):";
    if len(debugWithErrors)==0:
      print "<none>";
    else:
      for p in debugWithErrors:
        print p;
    print "The following projects had warnings (", len(debugWithWarnings), "):";
    if len(debugWithWarnings)==0:
      print "<none>";
    else:
      for p in debugWithWarnings:
        print p;
    print "------------------------\n";
  if ctx.field("release").value == "Build" or ctx.field("release").value == "Rebuild":
    print "------- Release --------";
    print "The following projects had errors (", len(releaseWithErrors), "):";
    if len(releaseWithErrors)==0:
      print "<none>";
    else:
      for p in releaseWithErrors:
        print p;
    print "The following projects had warnings (", len(releaseWithWarnings), "):";
    if len(releaseWithWarnings)==0:
      print "<none>";
    else:
      for p in releaseWithWarnings:
        print p;
    print "------------------------\n";
  return;
  
def checkBuildSuccessful(buildtype):
  path = ctx.field("FileInformation.dirname").value;
  basename = ctx.field("FileInformation.basename").value;
  buildlogFN = path + "/tmp/" + buildtype + "/obj/buildlog.htm";
  try: buildlog = codecs.open(buildlogFN, 'r', 'utf-16');
  except:
    print "Error on project " + basename + ": Could not open buildlog for " + buildtype + " build";
    return [basename, buildtype, -1, -1];
  ret = [];
  for l in buildlog.readlines():
    words = l.split();
    if len(words) > 0 and str(words[0]) == str(basename):
      print int(words[2]), "errors, ", int(words[4]), " warnings";
      ret = [basename, buildtype, int(words[2]), int(words[4])];
  buildlog.close();
  return ret;