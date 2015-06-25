#! /bin/bash

cd `dirname $0`

if [ -d "$HOME/Applications/MeVisLab.app" ]; then
  PROJGEN="$HOME/Applications/MeVisLab.app/Contents/Support/MeVisLabProjectGenerator.app/Contents/MacOS/MeVisLabProjectGenerator -no-xcode"
else
  PROJGEN="/Applications/MeVisLab.app/Contents/Support/MeVisLabProjectGenerator.app/Contents/MacOS/MeVisLabProjectGenerator -no-xcode"
fi
NUMJOBS=`/usr/sbin/sysctl -n hw.ncpu`

PROJECTS="\
MLOsiriXServices \
MLOsiriXBridge \
OsiriXPlugIn \
MLABOsiriXBridgeControl \
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
