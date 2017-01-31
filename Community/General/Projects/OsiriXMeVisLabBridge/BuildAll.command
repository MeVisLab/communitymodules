#! /bin/bash

RUNDIR=`dirname $0`

SOURCESDIR="$RUNDIR/Sources"
DOCSDIR="$RUNDIR/../../Documentation"
INSTALLDESCDIR="$RUNDIR/../../Configuration/Installers/AddOns/OsiriXMeVisLabBridgeAddOn"

if [ -d "$HOME/Applications/MeVisLab.app" ]; then
  MEVISLABAPP="$HOME/Applications/MeVisLab.app"
else
  MEVISLABAPP="/Applications/MeVisLab.app"
fi

PROJGEN="$MEVISLABAPP/Contents/Support/MeVisLabProjectGenerator.app/Contents/MacOS/MeVisLabProjectGenerator -no-xcode"
NUMJOBS=`/usr/sbin/sysctl -n hw.ncpu`

cd "$SOURCESDIR"

PROJECTS="\
MLOsiriXServices \
MLOsiriXBridge \
OsiriXPlugIn \
MLABOsiriXBridgeControl \
"

set -x

# build all binaries
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

# the following scripts require MLAB_ROOT
export MLAB_ROOT="$MEVISLABAPP/Contents/Packages"

# build html documentation
if [ -f "$MEVISLABAPP/Contents/Packages/MeVis/BuildSystem/BuildTools/DocTools/Scripts/generateDocs.py" ]; then
  cd "$DOCSDIR" && \
  "$MEVISLABAPP/Contents/Packages/MeVis/ThirdParty/bin/MeVisPython" -u "$MEVISLABAPP/Contents/Packages/MeVis/BuildSystem/BuildTools/DocTools/Scripts/generateDocs.py" -o "$DOCSDIR/Publish" "$DOCSDIR/Sources/OsiriXMeVisLabBridge/OsiriXMeVisLabBridge.mldoc" || \
  { read -p "Press any key to close window"; exit; }

  # build add-on installer
  if [[ -f "$MEVISLABAPP/Contents/Packages/MeVis/BuildSystem/BuildTools/Scripts/buildInstaller.py" && -f "$INSTALLDESCDIR/OsiriXMeVisLabBridgeAddOn.mlinstall" ]]; then
    cd "$INSTALLDESCDIR" && \
    "$MEVISLABAPP/Contents/Packages/MeVis/ThirdParty/bin/MeVisPython" -u "$MEVISLABAPP/Contents/Packages/MeVis/BuildSystem/BuildTools/Scripts/buildInstaller.py" "$INSTALLDESCDIR/OsiriXMeVisLabBridgeAddOn.mlinstall" "$RUNDIR" || \
    { read -p "Press any key to close window"; exit; }
  fi
fi
