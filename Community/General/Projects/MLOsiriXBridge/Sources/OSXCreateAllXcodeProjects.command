#! /bin/bash

set -x

PROJECTS="\
MLOsiriXServices/MLOsiriXServices \
MLOsiriXBridgeModule/MLOsiriXBridge \
MLABOsiriXBridgeControl/MLABOsiriXBridgeControl \
"

cd `dirname $0`

for f in $PROJECTS
do
  rm -vfr ${f}.xcodeproj
  rm -vfr ${f}_debug.xcodeproj
  rm -vfr "`dirname ${f}`/tmp"
  open -W -b de.mevis.MeVisLab.ProjectGenerator ${f}.pro
done
