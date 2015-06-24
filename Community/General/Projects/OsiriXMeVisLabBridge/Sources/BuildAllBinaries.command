#! /bin/bash

cd `dirname $0`

PROJGEN="$HOME/Applications/MeVisLab.app/Contents/Support/MeVisLabProjectGenerator.app/Contents/MacOS/MeVisLabProjectGenerator -no-xcode"
NUMJOBS=`/usr/sbin/sysctl -n hw.ncpu`

PROJECTS="\
MLOsiriXServices \
MLOsiriXBridge \
MLABOsiriXBridgeControl \
OsiriXPlugIn \
"

set -x

for f in $PROJECTS
do
  pushd ${f}
  $PROJGEN ${f}.pro && \
  make -f Makefile.${f} clean && \
  make -j $NUMJOBS -f Makefile.${f} && \
  make -f Makefile.${f}_debug clean && \
  make -j $NUMJOBS -f Makefile.${f}_debug || \
  { read -p "Press any key to close window"; exit; }
  popd
done
